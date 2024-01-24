#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bus.h>
#include <rom.h>
#include <ram.h>
#include <uart.h>
#include <syscon.h>
#include <pthread.h>
#include <SCAPCore.h>

int main(int argc, char *argv[]) {
    FILE *rom;
    int debug_mode = 0;

    if (argc < 2 || argc > 3) {
        printf("Usage: %s [-d] <rom>\n", argv[0]);
        return 1;
    }

    if (argc == 3 && strcmp(argv[1], "-d") == 0) {
        debug_mode = 1;
        rom = fopen(argv[2], "rb");
    } else {
        rom = fopen(argv[1], "rb");
    }

    if (rom == NULL) {
        printf("Error opening %s\n", (argc == 3) ? argv[2] : argv[1]);
        return 1;
    }

    rom_init(0x0000, 0xFF, rom);
    ram_init(0x00FF, 0x8000);
    uart_init(0xFF00);
    syscon_init(0xFFEE);
    scap_init(debug_mode);

    //spin up another thread that checks for killsignal
    pthread_t thread;
    pthread_create(&thread, NULL, (void*)syscon_check_for_killsignal, NULL);
    pthread_detach(thread);


    while (should_run) {
        if (!step_scap()) break;
        if (!debug_mode) continue;
        
        // Wait for a keypress
        char c;
        scanf("%c", &c);
    }
    uart_uncapture();

    return 0;
}
