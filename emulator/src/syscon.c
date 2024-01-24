#include <bus.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

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
    }
}
void syscon_init(uint16_t base){
    syscon_base = base;
    should_run = true;
    add_device(base,1,syscon_read, syscon_write);
}