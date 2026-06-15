#include <bits/stdc++.h>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

bool checkSymbol(char c) {
    static const string symbols = "{}()[].,;+-*/&|<>=~";
    return symbols.find(c) != string::npos;
}

bool checkKeyword(const string &word) {
    static const vector<string> keywordList = {
        "class","constructor","function","method","field","static","var",
        "int","char","boolean","void","true","false","null","this",
        "let","do","if","else","while","return"
    };
    return find(keywordList.begin(), keywordList.end(), word) != keywordList.end();
}

string xmlSafe(const string &s) {
    if (s == "<") return "&lt;";
    if (s == ">") return "&gt;";
    if (s == "&") return "&amp;";
    return s;
}

string getTokenCategory(const string &tok) {
    if (checkKeyword(tok)) return "keyword";
    if (tok.size() == 1 && checkSymbol(tok[0])) return "symbol";
    if (isdigit(tok[0])) return "integerConstant";
    if (tok[0] == '"') return "stringConstant";
    return "identifier";
}

vector<string> tokenizeSource(const string &code) {
    vector<string> tokens;
    string buffer;
    bool inString = false, blockComment = false;

    for (size_t i = 0; i < code.size(); ++i) {
        char c = code[i];
        if (!blockComment && i + 1 < code.size() && c == '/' && code[i + 1] == '*') {
            blockComment = true;
            ++i;
            continue;
        }
        if (blockComment && i + 1 < code.size() && c == '*' && code[i + 1] == '/') {
            blockComment = false;
            ++i;
            continue;
        }
        if (blockComment) continue;
        if (!inString && i + 1 < code.size() && c == '/' && code[i + 1] == '/') {
            while (i < code.size() && code[i] != '\n') ++i;
            continue;
        }
        if (inString) {
            buffer += c;
            if (c == '"') {
                tokens.push_back(buffer);
                buffer.clear();
                inString = false;
            }
            continue;
        }
        if (isspace(c)) {
            if (!buffer.empty()) {
                tokens.push_back(buffer);
                buffer.clear();
            }
            continue;
        }
        if (c == '"') {
            if (!buffer.empty()) {
                tokens.push_back(buffer);
                buffer.clear();
            }
            buffer += c;
            inString = true;
            continue;
        }
        if (checkSymbol(c)) {
            if (!buffer.empty()) {
                tokens.push_back(buffer);
                buffer.clear();
            }
            tokens.push_back(string(1, c));
            continue;
        }
        buffer += c;
    }

    if (!buffer.empty()) tokens.push_back(buffer);
    return tokens;
}

void writeXMLTokens(const vector<string> &tokens, const string &outPath) {
    ofstream out(outPath);
    out << "<tokens>\n";
    for (auto &tok : tokens) {
        string category = getTokenCategory(tok);
        string value = tok;
        if (category == "stringConstant") {
            value = value.substr(1, value.size() - 2);
        }
        out << "  <" << category << "> " << xmlSafe(value) << " </" << category << ">\n";
    }
    out << "</tokens>\n";
}

void handleFile(const string &path) {
    ifstream input(path);
    if (!input.is_open()) {
        cerr << "Error: cannot open file " << path << "\n";
        return;
    }

    string source((istreambuf_iterator<char>(input)), {});
    auto tokens = tokenizeSource(source);

    fs::path p(path);
    string outFile = (p.parent_path() / ("Tokenized_" + p.stem().string() + ".xml")).string();
    writeXMLTokens(tokens, outFile);

    cout << "Token XML generated: " << outFile << "\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: ./tokenizer <directory>\n";
        return 1;
    }

    string directory = argv[1];
    for (auto &entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".jack") {
            handleFile(entry.path().string());
        }
    }

    return 0;
}
s
