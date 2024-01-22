#include <bus.h>
#include <stdlib.h>
#include <stdio.h>

char* rom;
uint16_t rom_base;
uint8_t rom_read(uint16_t addr){
    return rom[addr - rom_base];
}
void rom_write(uint16_t addr, uint8_t data){
    //nope
}

void rom_init(uint16_t base,uint16_t size,FILE *file){
    rom = (char*)malloc(size);
    rom_base = base;
    fread(rom, size, 1, file);

    add_device(base, size, rom_read, rom_write);
}