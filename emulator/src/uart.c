#include <bus.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <uart.h>
#include <unistd.h>

uint16_t uart_base;
int terminal;
struct termios original_settings;
struct winsize winsize;

uint8_t uart_read(uint16_t addr) {
    if (addr != uart_base) {
        return 0;
    }
    uint8_t data;
    scanf("%c", &data);
    return data;
}
void uart_write(uint16_t addr, uint8_t data) {
    if (addr != uart_base) {
        return;
    }
    printf("%c", data);
}
bool uart_check_for_killsignal() {
    // killsignal = Ctrl + C
    char c;
    read(terminal, &c, 1);
    if (c == 3) {
        return true;
    }
    return false;
}
void uart_uncapture() {
    tcsetattr(terminal, TCSANOW, &original_settings);
    close(terminal);
}

void uart_init(uint16_t base) {
    uart_base = base;
    terminal = 0;
    if (terminal < 0) {
        perror("Error opening terminal");
        tcsetattr(terminal, TCSANOW, &original_settings);
        close(terminal);
        exit(1);
    }
    // Save current terminal settings
    tcgetattr(terminal, &original_settings);
    ioctl(terminal, TIOCGWINSZ, &winsize);

    // Configure terminal for raw mode
    struct termios new_settings = original_settings;
    new_settings.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | IXON | IXOFF);
    new_settings.c_oflag &= ~OPOST;
    new_settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(terminal, TCSANOW, &new_settings);

    // Set terminal input handler
    ioctl(terminal, TIOCSWINSZ, &winsize);
    ioctl(terminal, TCSETAF, 0);

    add_device(base, 1, uart_read, uart_write);
}