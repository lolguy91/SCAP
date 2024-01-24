;Simple Register dump example
    ldi B 0x69 ;load an example value into B
    ldi SPH 0x96 ;load an example value into SPH
    ldi SPL 0xAB ;load an example value into SPL
    ldi A 0x70 ;load the regdump command into A
    st A 0xFFEE ;send it to the Syscon
    ldi A 0x69 ;load shutdown commmand into A
    st A 0xFFEE  ;send it to the Syscon