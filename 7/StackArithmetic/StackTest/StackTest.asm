// push constant 17
@17
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 17
@17
D=A
@SP
AM=M+1
A=A-1
M=D

// eq
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel0
D;JEQ
@SP
A=M-1
M=0
(CompLabel0)

// push constant 17
@17
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 16
@16
D=A
@SP
AM=M+1
A=A-1
M=D

// eq
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel1
D;JEQ
@SP
A=M-1
M=0
(CompLabel1)

// push constant 16
@16
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 17
@17
D=A
@SP
AM=M+1
A=A-1
M=D

// eq
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel2
D;JEQ
@SP
A=M-1
M=0
(CompLabel2)

// push constant 892
@892
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 891
@891
D=A
@SP
AM=M+1
A=A-1
M=D

// lt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel3
D;JLT
@SP
A=M-1
M=0
(CompLabel3)

// push constant 891
@891
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 892
@892
D=A
@SP
AM=M+1
A=A-1
M=D

// lt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel4
D;JLT
@SP
A=M-1
M=0
(CompLabel4)

// push constant 891
@891
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 891
@891
D=A
@SP
AM=M+1
A=A-1
M=D

// lt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel5
D;JLT
@SP
A=M-1
M=0
(CompLabel5)

// push constant 32767
@32767
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 32766
@32766
D=A
@SP
AM=M+1
A=A-1
M=D

// gt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel6
D;JGT
@SP
A=M-1
M=0
(CompLabel6)

// push constant 32766
@32766
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 32767
@32767
D=A
@SP
AM=M+1
A=A-1
M=D

// gt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel7
D;JGT
@SP
A=M-1
M=0
(CompLabel7)

// push constant 32766
@32766
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 32766
@32766
D=A
@SP
AM=M+1
A=A-1
M=D

// gt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
M=-1
@CompLabel8
D;JGT
@SP
A=M-1
M=0
(CompLabel8)

// push constant 57
@57
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 31
@31
D=A
@SP
AM=M+1
A=A-1
M=D

// push constant 53
@53
D=A
@SP
AM=M+1
A=A-1
M=D

// add
@SP
AM=M-1
D=M
@SP
A=M-1
M=D+M

// push constant 112
@112
D=A
@SP
AM=M+1
A=A-1
M=D

// sub
@SP
AM=M-1
D=M
@SP
A=M-1
M=M-D

// neg
@SP
A=M-1
M=-M

// and
@SP
AM=M-1
D=M
@SP
A=M-1
M=D&M

// push constant 82
@82
D=A
@SP
AM=M+1
A=A-1
M=D

// or
@SP
AM=M-1
D=M
@SP
A=M-1
M=D|M

// not
@SP
A=M-1
M=!M

