#ifndef SYSCON_H
#define SYSCON_H

#include <bus.h>
#include <stdint.h>

//\brief Initialize the system controller
//\param base The base address of the system controller
void syscon_init(uint16_t base);

//\brief Check for a kill signal in a loop(must be used on another thread)
void syscon_check_for_killsignal();

//\brief Whether the system should run
extern bool should_run;

#endif