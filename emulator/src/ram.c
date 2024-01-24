#include <bus.h>
#include <stdlib.h>

char *ram;
uint16_t ram_base;

uint8_t ram_read(uint16_t addr) { return ram[addr - ram_base]; }
void ram_write(uint16_t addr, uint8_t data) { ram[addr - ram_base] = data; }

void ram_init(uint16_t base, uint16_t size) {
    ram = (char *)malloc(size);
    ram_base = base;
    add_device(base, size, ram_read, ram_write);
}