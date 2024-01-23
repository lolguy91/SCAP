#ifndef SCAPCORE_H
#define SCAPCORE_H

#include <stdbool.h>

bool step_scap();
void scap_init(bool debug_mode);
void scap_interrupt();

#endif // SCAPCORE_H
