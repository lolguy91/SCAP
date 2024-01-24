#ifndef UART_H
#define UART_H

#include <bus.h>
#include <stdint.h>

//\brief Initializes the UART
//\param base The base address of the UART
void uart_init(uint16_t base);

//\brief -Checks if there is a kill signal
bool uart_check_for_killsignal();

//\brief Uncaptures the terminal
void uart_uncapture();

#endif