// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

(LOOP)
// variables for screen loops
    @i
    M=0

    @SCREEN
    D=A
    @address
    M=D // 16384 = base address of screen

// clear if no input, fill if input
    @KBD
    D=M
    @CLEAR
    D;JEQ
    @FILL
    D;JGT

(CLEAR)
// loop i < 8192 = screen size / 16
    @i
    D=M
    @address
    A=D+M // pointer = address + index
    M=0 // set to 0
// i++
    @i
    M=M+1
// restart CLEAR loop
    D=M
    @8192
    D=D-A
    @CLEAR
    D;JLT
// resume main LOOP
    @LOOP
    0;JMP

(FILL)
// loop i < 8192 = screen size / 16
    @i
    D=M
    @address
    A=D+M // pointer = address + index
    M=-1 // set to 0b1111...
// i++
    @i
    M=M+1
// restart FILL loop
    D=M
    @8192
    D=D-A
    @FILL
    D;JLT
// resume main loop
    @LOOP
    0;JMP
