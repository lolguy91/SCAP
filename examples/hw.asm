jmp $main
;data section
    db 0x00
    db 0x0A
    db 0x0D
    db 0x21
    db 0x64 
    db 0x6C
    db 0x72
    db 0x6F
    db 0x57
    db 0x20
    db 0x2C
    db 0x6F
    db 0x6C
    db 0x6C
    db 0x65
hw: ;goes backwards
    db 0x48

;main function
main:
    ldi B 0x00
    ldi SPL $hw
    ldi SPH 0x00
print:
    pop A
    st A 0xFF00 
    sub A B
    jnz $print     ;jump back to print
end:
    ldi A 0x69
    st A 0xFFEE

