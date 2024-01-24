#ifndef SCAPCORE_H
#define SCAPCORE_H

#include <stdbool.h>

extern uint8_t A;
extern uint8_t B;
extern uint16_t PC;
extern uint16_t SP;
extern uint8_t flags;

bool step_scap();
void scap_init(bool debug_mode);
void scap_interrupt();

#endif // SCAPCORE_H
