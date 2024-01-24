#ifndef RAM_H
#define RAM_H

#include <bus.h>
#include <stdint.h>

//\brief Load RAM into memory
//\param base Base address of RAM
//\param size Size of RAM
void ram_init(uint16_t base, uint16_t size);

#endif