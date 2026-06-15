#include <bits/stdc++.h>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

struct Token { string kind, value; };

bool isSym(char c) {
    return string("{}()[].,;+-*/&|<>=~").find(c) != string::npos;
}

bool isKW(const string &w) {
    static const unordered_set<string> keywords = {
        "class","constructor","function","method","field","static","var",
        "int","char","boolean","void","true","false","null","this",
        "let","do","if","else","while","return"
    };
    return keywords.count(w);
}

vector<Token> lexer(const string &path) {
    ifstream in(path);
    string src((istreambuf_iterator<char>(in)), {});
    vector<string> raw;
    string tmp;
    bool inStr = false, inBlock = false;

    for (size_t i = 0; i < src.size(); ++i) {
        if (!inBlock && i + 1 < src.size() && src[i] == '/' && src[i + 1] == '*') {
            inBlock = true; i++; continue;
        }
        if (inBlock && i + 1 < src.size() && src[i] == '*' && src[i + 1] == '/') {
            inBlock = false; i++; continue;
        }
        if (inBlock) continue;
        if (!inBlock && i + 1 < src.size() && src[i] == '/' && src[i + 1] == '/') {
            while (i < src.size() && src[i] != '\n') i++;
            continue;
        }
        char c = src[i];
        if (inStr) {
            tmp += c;
            if (c == '"') { raw.push_back(tmp); tmp.clear(); inStr = false; }
        } else if (isspace(c)) {
            if (!tmp.empty()) { raw.push_back(tmp); tmp.clear(); }
        } else if (isSym(c)) {
            if (!tmp.empty()) { raw.push_back(tmp); tmp.clear(); }
            raw.push_back(string(1, c));
        } else if (c == '"') {
            if (!tmp.empty()) { raw.push_back(tmp); tmp.clear(); }
            tmp = "\""; inStr = true;
        } else tmp += c;
    }
    if (!tmp.empty()) raw.push_back(tmp);

    vector<Token> tokens;
    for (auto &r : raw) {
        Token t;
        if (isKW(r)) t.kind = "keyword";
        else if (r.size() == 1 && isSym(r[0])) t.kind = "symbol";
        else if (isdigit(r[0])) t.kind = "int";
        else if (r[0] == '"') { t.kind = "string"; t.value = r.substr(1, r.size() - 2); tokens.push_back(t); continue; }
        else t.kind = "ident";
        t.value = r;
        tokens.push_back(t);
    }
    return tokens;
}

struct Symbol { string type, segment; int index; };

struct SymTab {
    unordered_map<string, Symbol> classVars, subVars;
    int statCount = 0, fieldCount = 0, argCount = 0, varCount = 0;

    void newSubroutine() { subVars.clear(); argCount = varCount = 0; }

    void define(const string &n, const string &t, const string &k) {
        if (k == "static") classVars[n] = {t, k, statCount++};
        else if (k == "field") classVars[n] = {t, k, fieldCount++};
        else if (k == "arg") subVars[n] = {t, k, argCount++};
        else if (k == "var") subVars[n] = {t, k, varCount++};
    }

    bool exists(const string &n) const { return subVars.count(n) || classVars.count(n); }

    Symbol get(const string &n) const { return subVars.count(n) ? subVars.at(n) : classVars.at(n); }

    int count(const string &k) const {
        if (k == "field") return fieldCount;
        if (k == "var") return varCount;
        return 0;
    }
};

string segName(const string &k) {
    if (k == "static") return "static";
    if (k == "field") return "this";
    if (k == "arg") return "argument";
    if (k == "var") return "local";
    return "";
}

struct VMWriter {
    ofstream out;
    VMWriter(const string &file) { out.open(file); }

    void functionDef(const string &n, int l) { out << "function " << n << " " << l << "\n"; }
    void push(const string &s, int i) { out << "push " << s << " " << i << "\n"; }
    void pop(const string &s, int i) { out << "pop " << s << " " << i << "\n"; }
    void command(const string &c) { out << c << "\n"; }
    void call(const string &n, int a) { out << "call " << n << " " << a << "\n"; }
    void ret() { out << "return\n"; }
    void label(const string &l) { out << "label " << l << "\n"; }
    void go(const string &l) { out << "goto " << l << "\n"; }
    void ifgo(const string &l) { out << "if-goto " << l << "\n"; }
};

struct Compiler {
    vector<Token> tok;
    size_t pos = 0;
    SymTab st;
    VMWriter *vm;
    string cls;
    int labelCount = 0;

    Compiler(vector<Token> t, VMWriter *v) : tok(std::move(t)), vm(v) {}

    Token cur() { return pos < tok.size() ? tok[pos] : Token(); }
    Token next() { return pos < tok.size() ? tok[pos++] : Token(); }
    bool match(const string &s) { return pos < tok.size() && tok[pos].value == s; }
    void need(const string &s) {
        if (!match(s)) cerr << "Expected '" << s << "', found '" << (pos < tok.size() ? tok[pos].value : "<EOF>") << "'\n";
        else pos++;
    }

    bool isType() {
        return pos < tok.size() && (tok[pos].kind == "ident" ||
            (tok[pos].kind == "keyword" && (tok[pos].value == "int" || tok[pos].value == "char" || tok[pos].value == "boolean")));
    }

    void compileClass() {
        need("class");
        cls = next().value;
        need("{");
        while (match("static") || match("field")) classVarDec();
        while (match("constructor") || match("function") || match("method")) subroutine();
        need("}");
    }

    void classVarDec() {
        string kind = next().value, type = next().value, name = next().value;
        st.define(name, type, kind);
        while (match(",")) { next(); st.define(next().value, type, kind); }
        need(";");
    }

    void subroutine() {
        string subKind = next().value;
        if (match("void") || isType()) next();
        string name = next().value;
        st.newSubroutine();
        if (subKind == "method") st.define("this", cls, "arg");
        need("("); parameterList(); need(")");
        need("{");
        while (match("var")) varDec();
        vm->functionDef(cls + "." + name, st.count("var"));
        if (subKind == "constructor") {
            vm->push("constant", st.count("field"));
            vm->call("Memory.alloc", 1);
            vm->pop("pointer", 0);
        } else if (subKind == "method") {
            vm->push("argument", 0);
            vm->pop("pointer", 0);
        }
        statements();
        need("}");
    }

    void parameterList() {
        if (isType()) {
            string type = next().value, name = next().value;
            st.define(name, type, "arg");
            while (match(",")) { next(); st.define(next().value, next().value, "arg"); }
        }
    }

    void varDec() {
        need("var");
        string type = next().value, name = next().value;
        st.define(name, type, "var");
        while (match(",")) { next(); st.define(next().value, type, "var"); }
        need(";");
    }

    void statements() {
        while (true) {
            if (match("let")) letStmt();
            else if (match("if")) ifStmt();
            else if (match("while")) whileStmt();
            else if (match("do")) doStmt();
            else if (match("return")) returnStmt();
            else break;
        }
    }

    void doStmt() {
        need("do"); subCall(); need(";"); vm->pop("temp", 0);
    }

    void letStmt() {
        need("let");
        string name = next().value;
        bool arr = false;
        if (match("[")) {
            next(); expr(); need("]");
            Symbol s = st.get(name);
            vm->push(segName(s.segment), s.index);
            vm->command("add");
            arr = true;
        }
        need("="); expr(); need(";");
        if (arr) {
            vm->pop("temp", 0);
            vm->pop("pointer", 1);
            vm->push("temp", 0);
            vm->pop("that", 0);
        } else {
            Symbol s = st.get(name);
            vm->pop(segName(s.segment), s.index);
        }
    }

    void whileStmt() {
        need("while");
        int a = labelCount++, b = labelCount++;
        vm->label("WHILE_EXP" + to_string(a));
        need("("); expr(); need(")");
        vm->command("not");
        vm->ifgo("WHILE_END" + to_string(b));
        need("{"); statements(); need("}");
        vm->go("WHILE_EXP" + to_string(a));
        vm->label("WHILE_END" + to_string(b));
    }

    void ifStmt() {
        need("if");
        need("("); expr(); need(")");
        int f = labelCount++, e = labelCount++;
        vm->command("not");
        vm->ifgo("IF_FALSE" + to_string(f));
        need("{"); statements(); need("}");
        if (match("else")) {
            vm->go("IF_END" + to_string(e));
            vm->label("IF_FALSE" + to_string(f));
            next(); need("{"); statements(); need("}"); vm->label("IF_END" + to_string(e));
        } else vm->label("IF_FALSE" + to_string(f));
    }

    void returnStmt() {
        need("return");
        if (!match(";")) expr();
        else vm->push("constant", 0);
        need(";"); vm->ret();
    }

    int exprList() {
        int n = 0;
        if (!match(")")) { expr(); n++; while (match(",")) { next(); expr(); n++; } }
        return n;
    }

    void subCall() {
        string name = next().value; int nargs = 0;
        if (match(".")) {
            next();
            string sub = next().value;
            need("(");
            if (st.exists(name)) {
                Symbol s = st.get(name);
                vm->push(segName(s.segment), s.index);
                nargs = 1 + exprList(); need(")");
                vm->call(s.type + "." + sub, nargs);
            } else {
                nargs = exprList(); need(")");
                vm->call(name + "." + sub, nargs);
            }
        } else {
            need("(");
            vm->push("pointer", 0);
            nargs = 1 + exprList(); need(")");
            vm->call(cls + "." + name, nargs);
        }
    }

    void expr() {
        term();
        while (match("+") || match("-") || match("*") || match("/") || match("&") || match("|") || match("<") || match(">") || match("=")) {
            string op = next().value; term();
            if (op == "+") vm->command("add");
            else if (op == "-") vm->command("sub");
            else if (op == "*") vm->call("Math.multiply", 2);
            else if (op == "/") vm->call("Math.divide", 2);
            else if (op == "&") vm->command("and");
            else if (op == "|") vm->command("or");
            else if (op == "<") vm->command("lt");
            else if (op == ">") vm->command("gt");
            else if (op == "=") vm->command("eq");
        }
    }

    void term() {
        Token x = next();

        if (x.kind == "int") { vm->push("constant", stoi(x.value)); return; }
        if (x.kind == "string") {
            vm->push("constant", x.value.size());
            vm->call("String.new", 1);
            for (char c : x.value) {
                vm->push("constant", (int)c);
                vm->call("String.appendChar", 2);
            }
            return;
        }
        if (x.kind == "keyword") {
            if (x.value == "true") { vm->push("constant", 1); vm->command("neg"); }
            else if (x.value == "false" || x.value == "null") vm->push("constant", 0);
            else if (x.value == "this") vm->push("pointer", 0);
            return;
        }
        if (x.value == "(") { expr(); need(")"); return; }
        if (x.value == "-" || x.value == "~") {
            term();
            if (x.value == "-") vm->command("neg");
            else vm->command("not");
            return;
        }
        if (x.kind == "ident") {
            string name = x.value;
            if (match("[")) {
                next(); expr(); need("]");
                Symbol s = st.get(name);
                vm->push(segName(s.segment), s.index);
                vm->command("add");
                vm->pop("pointer", 1);
                vm->push("that", 0);
                return;
            }
            if (match("(") || match(".")) { pos--; subCall(); return; }
            if (st.exists(name)) {
                Symbol s = st.get(name);
                vm->push(segName(s.segment), s.index);
            }
        }
    }
};

int main(int argc, char **argv) {
    if (argc < 2) { cerr << "Usage: JackCompiler <file|directory>\n"; return 1; }

    fs::path input(argv[1]);
    vector<fs::path> files;

    if (fs::is_directory(input)) {
        for (auto &f : fs::directory_iterator(input))
            if (f.path().extension() == ".jack") files.push_back(f.path());
    } else if (input.extension() == ".jack") files.push_back(input);

    for (auto &file : files) {
        auto tokens = lexer(file.string());
        VMWriter vm(file.stem().string() + ".vm");
        Compiler compiler(tokens, &vm);
        compiler.compileClass();
        cout << "Compiled " << file.filename().string() << " → " << file.stem().string() << ".vm\n";
    }
}

