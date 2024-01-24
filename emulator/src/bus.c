#include <bus.h>

struct device devices[8];
uint8_t devices_count = 0;

void bus_write(uint16_t addr, uint8_t data) {
    for (int i = 0; i < 8; i++) {
        if (addr >= devices[i].base &&
            addr <= devices[i].base + devices[i].limit) {
            devices[i].write(addr, data);
            return;
        }
    }
}
uint8_t bus_read(uint16_t addr) {
    for (int i = 0; i < 8; i++) {
        if (addr >= devices[i].base &&
            addr <= devices[i].base + devices[i].limit) {
            return devices[i].read(addr);
        }
    }
    return 0;
}

bool add_device(uint16_t base, uint16_t limit, BusReadFunc read,
                BusWriteFunc write) {
    for (int i = devices_count; i < 8; i++) {
        if (devices[i].base == 0) {
            devices[i].base = base;
            devices[i].limit = limit;
            devices[i].read = read;
            devices[i].write = write;
            devices_count++;
            return true;
        }
    }
    return false;
}