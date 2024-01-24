#ifndef ROM_H
#define ROM_H

#include <bus.h>
#include <stdint.h>
#include <stdlib.h>

//\brief Load ROM into memory
//\param base Base address of ROM
//\param size Size of ROM
//\param rom File pointer of ROM
void rom_init(uint16_t base, uint16_t size, FILE *rom);

#endif