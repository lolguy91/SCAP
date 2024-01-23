; test.asm - Test file for SCAP assembler

; LDIREG,ADDR
ldi A, 0x0101

; ST OREG,ADDR
st B, 0x0202

; MV IREG,OREG
mv C, B

; ADD REG1,REG2
add A, B

; SUB REG1,REG2
sub C, A

; SL REG1
sl B

; PUSH REG1
push A

; POP REG1
pop B

; JMP ADDR
jmp 0x0303

; JZ ADDR
jz 0x0404

; JNZ ADDR
jnz 0x0505

; JC ADDR
jc 0x0606

; JNC ADDR
jnc 0x0707

; CALL ADDR
call 0x0808

; RET
ret

; NOP
nop
