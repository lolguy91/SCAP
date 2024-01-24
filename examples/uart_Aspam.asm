;Simple UART example
    ldi A 0x41    ;load the the letter A into A
    st A 0xFF00    ;print the letter to UART (0xFF00)
    jmp 0x0000     ;jump back to the beginning(spammy profit :D)

