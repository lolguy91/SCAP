#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <bus.h>

void uart_init(uint16_t base);
void uart_uncapture();

#endif