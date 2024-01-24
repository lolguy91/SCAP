#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <bus.h>

void uart_init(uint16_t base);
bool uart_check_for_killsignal();
void uart_uncapture();

#endif