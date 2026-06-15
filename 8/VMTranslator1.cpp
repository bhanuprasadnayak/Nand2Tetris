#include <bits/stdc++.h>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

ofstream outputFile;
string currentFileName;
int labelCounter = 0;

void writeArithmetic(const string &command) {
    if (command == "add" || command == "sub" || command == "and" || command == "or") {
        outputFile << "@SP\nAM=M-1\nD=M\nA=A-1\n";
        if (command == "add") outputFile << "M=M+D\n";
        if (command == "sub") outputFile << "M=M-D\n";
        if (command == "and") outputFile << "M=M&D\n";
        if (command == "or")  outputFile << "M=M|D\n";
    } else if (command == "neg" || command == "not") {
        outputFile << "@SP\nA=M-1\n";
        outputFile << (command == "neg" ? "M=-M\n" : "M=!M\n");
    } else {
        string L_true = "T" + to_string(labelCounter);
        string L_end = "E" + to_string(labelCounter++);
        outputFile << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@" << L_true << "\n";
        if (command == "eq") outputFile << "D;JEQ\n";
        if (command == "gt") outputFile << "D;JGT\n";
        if (command == "lt") outputFile << "D;JLT\n";
        outputFile << "@SP\nA=M-1\nM=0\n@" << L_end << "\n0;JMP\n";
        outputFile << "(" << L_true << ")\n@SP\nA=M-1\nM=-1\n(" << L_end << ")\n";
    }
}

void writePush(const string &segment, int index) {
    if (segment == "constant") outputFile << "@" << index << "\nD=A\n";
    else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
        string base = segment == "local" ? "LCL" : segment == "argument" ? "ARG" : segment == "this" ? "THIS" : "THAT";
        outputFile << "@" << index << "\nD=A\n@" << base << "\nA=M+D\nD=M\n";
    } else if (segment == "temp") outputFile << "@" << 5 + index << "\nD=M\n";
    else if (segment == "pointer") outputFile << (index ? "@THAT\nD=M\n" : "@THIS\nD=M\n");
    else outputFile << "@" << currentFileName << "." << index << "\nD=M\n";
    outputFile << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
}

void writePop(const string &segment, int index) {
    if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
        string base = segment == "local" ? "LCL" : segment == "argument" ? "ARG" : segment == "this" ? "THIS" : "THAT";
        outputFile << "@" << index << "\nD=A\n@" << base << "\nD=M+D\n@R13\nM=D\n";
        outputFile << "@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    } else if (segment == "temp") outputFile << "@SP\nAM=M-1\nD=M\n@" << 5 + index << "\nM=D\n";
    else if (segment == "pointer") outputFile << "@SP\nAM=M-1\nD=M\n" << (index ? "@THAT\nM=D\n" : "@THIS\nM=D\n");
    else outputFile << "@SP\nAM=M-1\nD=M\n@" << currentFileName << "." << index << "\nM=D\n";
}

void writeLabel(const string &label) { outputFile << "(" << currentFileName << "$" << label << ")\n"; }
void writeGoto(const string &label) { outputFile << "@" << currentFileName << "$" << label << "\n0;JMP\n"; }
void writeIfGoto(const string &label) { outputFile << "@SP\nAM=M-1\nD=M\n@" << currentFileName << "$" << label << "\nD;JNE\n"; }

void writeFunction(const string &name, int nVars) {
    outputFile << "(" << name << ")\n";
    for (int i = 0; i < nVars; i++) writePush("constant", 0);
}

void writeCall(const string &name, int nArgs) {
    string returnLabel = "RET" + to_string(labelCounter++);
    outputFile << "@" << returnLabel << "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    for (string seg : {"LCL", "ARG", "THIS", "THAT"})
        outputFile << "@" << seg << "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    outputFile << "@SP\nD=M\n@" << nArgs + 5 << "\nD=D-A\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n";
    outputFile << "@" << name << "\n0;JMP\n(" << returnLabel << ")\n";
}

void writeReturn() {
    outputFile << "@LCL\nD=M\n@R13\nM=D\n@5\nA=D-A\nD=M\n@R14\nM=D\n";
    outputFile << "@SP\nAM=M-1\nD=M\n@ARG\nA=M\nM=D\nD=A+1\n@SP\nM=D\n";
    for (string seg : {"THAT", "THIS", "ARG", "LCL"}) {
        outputFile << "@R13\nAM=M-1\nD=M\n@" << seg << "\nM=D\n";
    }
    outputFile << "@R14\nA=M\n0;JMP\n";
}

void writeBootstrap() {
    outputFile << "@256\nD=A\n@SP\nM=D\n";
    writeCall("Sys.init", 0);
}

void translateVMFile(const string &filePath) {
    currentFileName = fs::path(filePath).stem().string();
    ifstream input(filePath);
    string line, cmd, arg1; 
    int arg2;

    while (getline(input, line)) {
        if (line.find("//") != string::npos) line = line.substr(0, line.find("//"));
        stringstream ss(line);
        if (!(ss >> cmd)) continue;

        if (cmd == "push") { ss >> arg1 >> arg2; writePush(arg1, arg2); }
        else if (cmd == "pop") { ss >> arg1 >> arg2; writePop(arg1, arg2); }
        else if (cmd == "label") { ss >> arg1; writeLabel(arg1); }
        else if (cmd == "goto") { ss >> arg1; writeGoto(arg1); }
        else if (cmd == "if-goto") { ss >> arg1; writeIfGoto(arg1); }
        else if (cmd == "function") { ss >> arg1 >> arg2; writeFunction(arg1, arg2); }
        else if (cmd == "call") { ss >> arg1 >> arg2; writeCall(arg1, arg2); }
        else if (cmd == "return") writeReturn();
        else writeArithmetic(cmd);
    }
}

int main(int argc, char *argv[]) {
    string folderPath = argv[1];
    string outputAsmFile = folderPath + (folderPath.back() == '/' ? "" : "/") + ".asm";
    outputFile.open(outputAsmFile);
    writeBootstrap();

    for (auto &entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".vm") translateVMFile(entry.path().string());
    }
}
