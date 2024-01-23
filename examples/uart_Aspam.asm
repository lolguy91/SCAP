ld A $the_a ;load the the letter A stored at 0x0009 into A
st A 0xFF00 ;print the letter to UART (0xFF00)
jmp 0x0000 ;jump back to the beginning
the_a:
db 0x41 ;store the letter A
