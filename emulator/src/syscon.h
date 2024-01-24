#ifndef SYSCON_H
#define SYSCON_H

#include <stdint.h>
#include <bus.h>

void syscon_init(uint16_t base);
extern bool should_run;

#endif