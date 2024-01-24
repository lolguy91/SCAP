#include <SCAPCore.h>
#include <bus.h>
#include <pthread.h>
#include <ram.h>
#include <rom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscon.h>
#include <uart.h>
#include <unistd.h>

// config
int debug_mode = 0;
int clock_rate = 1000000;
char *input_file;

int main(int argc, char *argv[]) {
    FILE *rom;

    if (argc < 2) {
        printf("Usage: %s [-d] [-s]  <rom>\n", argv[0]);
        return 1;
    }

    //Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debug_mode = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            clock_rate = atoi(argv[i + 1]);
            i++;
        } else {
            input_file = argv[i];
        }
    }

    if (input_file == NULL) {
        printf("[SCAP EM] Error: No input file provided.\n");
        return 1;
    }
    rom = fopen(input_file, "rb");
    if (rom == NULL) {
        perror("[SCAP EM] Error:Error opening ROM");
        return 1;
    }

    rom_init(0x0000, 0xFF, rom);
    ram_init(0x00FF, 0x8000);
    uart_init(0xFF00);
    syscon_init(0xFFEE);
    scap_init(debug_mode);

    // spin up another thread that checks for killsignal
    pthread_t thread;
    pthread_create(&thread, NULL, (void *)syscon_check_for_killsignal, NULL);
    pthread_detach(thread);

    while (should_run) {
        // Slow down execution to clock speed.
        usleep(1000000 / clock_rate);

        // Run the emulator.
        // TODO: add proper error handling for execute falioures
        if (!scap_step())
            break;
        if (!debug_mode)
            continue;

        // Wait for a keypress.
        char c;
        scanf("%c", &c);
    }
    uart_uncapture();

    return 0;
}
