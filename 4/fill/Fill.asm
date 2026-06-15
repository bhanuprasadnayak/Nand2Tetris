// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

// === Infinite loop start ===
(LOOP)
    @KBD
    D=M          // D = keyboard input

    @FILL_BLACK
    D;JNE        // If D != 0 (key pressed), jump to FILL_BLACK

    @FILL_WHITE  // Otherwise, fill screen white
    0;JMP


// === Fill screen with black pixels ===
(FILL_BLACK)
    @SCREEN
    D=A
    @addr
    M=D          // addr = SCREEN base (16384)

(BLACK_LOOP)
    @addr
    A=M
    M=-1         // Set RAM[addr] = -1 (all 1s → black)

    @addr
    M=M+1        // addr++

    @addr
    D=M
    @24576
    D=D-A        // If addr == 24576 → done

    @LOOP
    D;JEQ

    @BLACK_LOOP
    0;JMP


// === Fill screen with white pixels ===
(FILL_WHITE)
    @SCREEN
    D=A
    @addr
    M=D          // addr = SCREEN base

(WHITE_LOOP)
    @addr
    A=M
    M=0          // Set RAM[addr] = 0 (white)

    @addr
    M=M+1        // addr++

    @addr
    D=M
    @24576
    D=D-A        // Check if addr == 24576

    @LOOP
    D;JEQ

    @WHITE_LOOP
    0;JMP
