// push constant 3030
@3030
D=A
@SP
AM=M+1
A=A-1
M=D

// pop pointer 0
@0
D=A
@3
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D

// push constant 3040
@3040
D=A
@SP
AM=M+1
A=A-1
M=D

// pop pointer 1
@1
D=A
@3
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D

// push constant 32
@32
D=A
@SP
AM=M+1
A=A-1
M=D

// pop this 2
@2
D=A
@THIS
A=M
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D

// push constant 46
@46
D=A
@SP
AM=M+1
A=A-1
M=D

// pop that 6
@6
D=A
@THAT
A=M
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D

// push pointer 0
@0
D=A
@3
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1

// push pointer 1
@1
D=A
@3
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1

// add
@SP
AM=M-1
D=M
@SP
A=M-1
M=D+M

// push this 2
@2
D=A
@THIS
A=M
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1

// sub
@SP
AM=M-1
D=M
@SP
A=M-1
M=M-D

// push that 6
@6
D=A
@THAT
A=M
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1

// add
@SP
AM=M-1
D=M
@SP
A=M-1
M=D+M

