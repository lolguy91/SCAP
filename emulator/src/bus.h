#ifndef BUS_H
#define BUS_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t (*BusReadFunc)(uint16_t);
typedef void (*BusWriteFunc)(uint16_t, uint8_t);

struct device {
    uint16_t base;
    uint16_t limit;
    BusReadFunc read;
    BusWriteFunc write;
};
bool add_device(uint16_t base, uint16_t limit, BusReadFunc read,
                BusWriteFunc write);
void bus_write(uint16_t addr, uint8_t data);
uint8_t bus_read(uint16_t addr);

#endif