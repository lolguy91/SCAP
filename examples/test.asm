jmp $main
;data section
the_a: db 0x41 

;main function
main:
    ld A $the_a    ;load the the letter A into A
    st A 0xFF00    ;print the letter to UART (0xFF00)
    jmp 0x0000     ;jump back to the beginning
