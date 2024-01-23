; Simple test.asm for SCAP assembler

; MV instruction - Move immediate value 1 to register A
MV A, 0x0001

; ADD instruction - Add register A and immediate value 1, store result in register B
ADD B, A, 0x0001

; Halt the program (optional)
NOP
