#include <bus.h>
#include <stdlib.h>
#include <stdio.h>

uint16_t uart_base;
uint8_t uart_read(uint16_t addr){
    if (addr != uart_base){
        return 0;
    }
    uint8_t data;
    scanf("%c",&data);
    return data;
}
void uart_write(uint16_t addr, uint8_t data){
    //if (addr != uart_base){
    //    return;
    //}
    printf("%c",data);
}

void uart_init(uint16_t base){
    uart_base = base;
    add_device(base,1,uart_read, uart_write);
}