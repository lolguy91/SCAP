#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Check if correct number of command-line arguments is provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <binary_input> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the output file for writing
    FILE *outputFile = fopen(argv[2], "wb");
    if (outputFile == NULL) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    // Parse the binary input and write the corresponding bytes to the output
    // file
    int bit;
    unsigned char byte = 0;
    int bitCount = 0;

    for (int i = 0; argv[1][i] != '\0'; i++) {
        bit = argv[1][i];

        if (bit == '0' || bit == '1') {
            byte = (byte << 1) | (bit - '0');
            bitCount++;

            if (bitCount == 8) {
                fputc(byte, outputFile);
                byte = 0;
                bitCount = 0;
            }
        }
    }

    // If there are remaining bits, write the last byte
    if (bitCount > 0) {
        byte = byte << (8 - bitCount);
        fputc(byte, outputFile);
    }

    // Close the output file
    fclose(outputFile);

    printf("Binary data written to %s successfully.\n", argv[2]);

    return EXIT_SUCCESS;
}
