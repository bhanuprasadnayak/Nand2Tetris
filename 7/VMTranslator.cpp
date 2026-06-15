#include <bits/stdc++.h>
using namespace std;

ofstream out;
string fname; int lbl=0;

void arith(string c){
    if(c=="add"||c=="sub"||c=="and"||c=="or"){
        out<<"@SP\nAM=M-1\nD=M\nA=A-1\n";
        if(c=="add") out<<"M=M+D\n";
        if(c=="sub") out<<"M=M-D\n";
        if(c=="and") out<<"M=M&D\n";
        if(c=="or")  out<<"M=M|D\n";
    } 
    else if(c=="neg"||c=="not"){
        out<<"@SP\nA=M-1\n";
        if(c=="neg") out<<"M=-M\n"; 
        else out<<"M=!M\n";
    } 
    else {
        string L1="TRUE"+to_string(lbl),L2="END"+to_string(lbl++);
        out<<"@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@"<<L1<<"\n";
        if(c=="eq") out<<"D;JEQ\n"; 
        if(c=="gt") out<<"D;JGT\n";
        if(c=="lt") out<<"D;JLT\n";
        out<<"@SP\nA=M-1\nM=0\n@"<<L2<<"\n0;JMP\n("<<L1<<")\n@SP\nA=M-1\nM=-1\n("<<L2<<")\n";
    }
}

void push(string seg,int i){
    if(seg=="constant") out<<"@"<<i<<"\nD=A\n";
    else if(seg=="local"||seg=="argument"||seg=="this"||seg=="that"){
        string b=(seg=="local"?"LCL":seg=="argument"?"ARG":seg=="this"?"THIS":"THAT");
        out<<"@"<<i<<"\nD=A\n@"<<b<<"\nA=M+D\nD=M\n";
    } 
    else if(seg=="temp") out<<"@"<<5+i<<"\nD=M\n";
    else if(seg=="pointer") out<<(i==0?"@THIS\nD=M\n":"@THAT\nD=M\n");
    else out<<"@"<<fname<<"."<<i<<"\nD=M\n";
    out<<"@SP\nA=M\nM=D\n@SP\nM=M+1\n";
}

void pop(string seg,int i){
    if(seg=="local"||seg=="argument"||seg=="this"||seg=="that"){
        string b=(seg=="local"?"LCL":seg=="argument"?"ARG":seg=="this"?"THIS":"THAT");
        out<<"@"<<i<<"\nD=A\n@"<<b<<"\nD=M+D\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    } 
    else if(seg=="temp") out<<"@SP\nAM=M-1\nD=M\n@"<<5+i<<"\nM=D\n";
    else if(seg=="pointer") out<<"@SP\nAM=M-1\nD=M\n"<<(i==0?"@THIS\nM=D\n":"@THAT\nM=D\n");
    else out<<"@SP\nAM=M-1\nD=M\n@"<<fname<<"."<<i<<"\nM=D\n";
}

int main(int argc,char*argv[]){
    string inF=argv[1],outF=inF.substr(0,inF.find(".vm"))+".asm"; out.open(outF);
    fname=inF.substr(inF.find_last_of("/\\")+1); fname=fname.substr(0,fname.find(".vm"));
    ifstream in(inF); string line,cmd,a1; int a2;
    while(getline(in,line)){
        if(line.find("//")!=string::npos) line=line.substr(0,line.find("//"));
        stringstream ss(line); 
        if(!(ss>>cmd)) continue;
        if(cmd=="push"){ss>>a1>>a2; push(a1,a2);}
        else if(cmd=="pop"){ss>>a1>>a2; pop(a1,a2);}
        else arith(cmd);
    }
}

