#include <bus.h>
#include <uart.h>
#include <stdio.h>
#include <stdlib.h>
#include <SCAPCore.h>

uint16_t syscon_base;
bool should_run;
uint8_t syscon_read(uint16_t addr){
    if (addr != syscon_base){
        return 0;
    }
    //TODO: return smth
    return 0;
}
void syscon_write(uint16_t addr, uint8_t data){
    if (addr != syscon_base){
        return;
    }
    if(data == 0x69){
        should_run = false;
    } else if (data == 0x70) {
        printf("Register Dump:\n\r");
        printf("====================\n\r");
        printf("Program Counter (PC):   0x%04X\n\r", PC);
        printf("Stack Pointer (SP):     0x%04X\n\r", SP);
        printf("A:                      0x%02X\n\r", A);
        printf("B:                      0x%02X\n\r", B);
        printf("Flags:                  0x%02X\n\r", flags);
        printf("====================\n\r");

    }
}
void syscon_check_for_killsignal(){
    while(should_run){
        if(uart_check_for_killsignal()){
            printf("\n[SCAP EMULATOR] Terminated by user\r\n");
            should_run = false;
            uart_uncapture();
            exit(0);
        }
    }
}
void syscon_init(uint16_t base){
    syscon_base = base;
    should_run = true;
    add_device(base,1,syscon_read, syscon_write);
}