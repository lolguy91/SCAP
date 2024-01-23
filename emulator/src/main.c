#include <stdio.h>
#include <stdlib.h>
#include <bus.h>
#include <rom.h>
#include <ram.h>
#include <SCAPCore.h>

int main(int argc, char *argv[]) {
    FILE *rom;

    if(argc != 2) {
        printf("Usage: %s <rom>\n", argv[0]);
        return 1;
    }

    rom = fopen(argv[1], "rb");

    if (rom == NULL) {
        printf("Error opening %s\n", argv[1]);
        return 1;
    }

    rom_init(0x0000, 0xFF, rom);
    ram_init(0x00FF, 0x8000);
    scap_init();

    while (1) {
        if (!step_scap()) break;
        #ifdef _DEBUG
            //wait for a keypress
            char c;
            scanf("%c", &c);
        #endif
    }

    return 0;
}