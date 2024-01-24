#ifndef SYSCON_H
#define SYSCON_H

#include <stdint.h>
#include <bus.h>

void syscon_init(uint16_t base);
void syscon_check_for_killsignal();


extern bool should_run;

#endif