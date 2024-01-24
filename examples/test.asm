jmp $main
 

disable_keyboard:
    ld A 0x00
    st A 0xFF00  

main:
    jmp $disable_keyboard