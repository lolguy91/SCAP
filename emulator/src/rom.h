#ifndef ROM_H
#define ROM_H

#include <stdlib.h>
#include <stdint.h>
#include <bus.h>

void rom_init(uint16_t base,uint16_t size,FILE *rom);
#endif