jmp $main
;data section
    db 0x00
    db 0x48
    db 0x65
    db 0x6C
    db 0x6C
    db 0x6F
    db 0x2C
    db 0x20
    db 0x57
    db 0x6F
    db 0x72
    db 0x6C
hw: ;goes backwards
    db 0x64 
spl: db $hw
zero: db 00
;main function
main:
    ld SPL $spl
    ld SPH $zero
print:
    pop A
    ld B $zero 
    sub A B
    jz end
    st a 0xFF00
    jmp print     ;jump back to the beginning

end:
    jmp end

