jmp $main
;NOTE: the string is stored in reverse order
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

hw: db 0x48 ;the first letter of the string
    

;main function
main:
    ;initialize stack pointer and B
    ldi B 0x00
    ldi SPL $hw
    ldi SPH 0x00
print:
    pop A ;pop off a letter
    st A 0xFF00 ;print it to UART
    sub A B ;compare with NULL
    jnz $print ;jump back to print if character is not NULL
end:
    ldi A 0x69 ;load shutdown commmand into A
    st A 0xFFEE  ;send it to the Syscon

