jmp $main

dump_signal: db 0x70

main:
    ld A $dump_signal
    st A 0xFFEE