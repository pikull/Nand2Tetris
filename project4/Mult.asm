// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

    @i
    M=0
    @R2
    M=0

// END if RAM[0] = 0
    @R0
    D=M
    @END
    D;JEQ

(LOOP)
// RAM[2] = RAM[2] + RAM[1]
    @R1
    D=M
    @R2
    M=D+M

// i++
    @i
    M=M+1

// LOOP if i < RAM[0]
    D=M
    @R0
    D=D-M
    @LOOP
    D;JLT

(END)
    @END
    0;JMP
