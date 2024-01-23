#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

struct IO {
    unsigned int reg1 : 1;
    unsigned int reg2 : 1;
    unsigned int addr : 16;
};

struct Instruction {
    unsigned int opcode : 4;
    struct IO io;
};

void assemble_instruction(char* mnemonic, char* reg1, char* reg2, char* addr, struct Instruction* instruction) {
    if (strcasecmp(mnemonic, "ld") == 0) {
        instruction->opcode = 0x0;
    } else if (strcasecmp(mnemonic, "st") == 0) {
        instruction->opcode = 0x1;
    } else if (strcasecmp(mnemonic, "mv") == 0) {
        instruction->opcode = 0x2;
    } else if (strcasecmp(mnemonic, "add") == 0) {
        instruction->opcode = 0x3;
    } else if (strcasecmp(mnemonic, "sub") == 0) {
        instruction->opcode = 0x4;
    } else if (strcasecmp(mnemonic, "sl") == 0) {
        instruction->opcode = 0x5;
    } else if (strcasecmp(mnemonic, "push") == 0) {
        instruction->opcode = 0x6;
    } else if (strcasecmp(mnemonic, "pop") == 0) {
        instruction->opcode = 0x7;
    } else if (strcasecmp(mnemonic, "jmp") == 0) {
        instruction->opcode = 0x8;
    } else if (strcasecmp(mnemonic, "jz") == 0) {
        instruction->opcode = 0x9;
    } else if (strcasecmp(mnemonic, "jnz") == 0) {
        instruction->opcode = 0xA;
    } else if (strcasecmp(mnemonic, "jc") == 0) {
        instruction->opcode = 0xB;
    } else if (strcasecmp(mnemonic, "jnc") == 0) {
        instruction->opcode = 0xC;
    } else if (strcasecmp(mnemonic, "call") == 0) {
        instruction->opcode = 0xD;
    } else if (strcasecmp(mnemonic, "ret") == 0) {
        instruction->opcode = 0xE;
    } else if (strcasecmp(mnemonic, "nop") == 0) {
        instruction->opcode = 0xF;
    } else {
        printf("[SCAP AS] Error: Unknown mnemonic %s\n", mnemonic);
        return;
    }

    unsigned int temp_addr;
    if (sscanf(addr, "0x%x", &temp_addr) != 1) {
        printf("[SCAP AS] Error: Invalid address format %s\n", addr);
        return;
    }
    instruction->io.addr = temp_addr;

    printf("[SCAP AS] Assembled: %X%X%X%X\n", instruction->opcode, instruction->io.reg1, instruction->io.reg2, instruction->io.addr);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("[SCAP AS] Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("[SCAP AS] Error opening input file");
        return 1;
    }

    // Construct the output file name
    char output_filename[256];
    char *last_dot = strrchr(argv[1], '.');
    if (last_dot != NULL) {
        *last_dot = '\0'; // Remove the old extension
    }
    snprintf(output_filename, sizeof(output_filename), "%s.bin", argv[1]);

    FILE *output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        perror("[SCAP AS] Error opening output file");
        fclose(input_file);
        return 1;
    }

    char mnemonic[10], reg1[2], reg2[2], addr[8];
    struct Instruction instruction;

    while (fscanf(input_file, "%s %[^,], %[^,], %s", mnemonic, reg1, reg2, addr) == 4)  {
        assemble_instruction(mnemonic, reg1, reg2, addr, &instruction);
        fwrite(&instruction, sizeof(struct Instruction), 1, output_file);
    }

    printf("[SCAP AS] Assembly completed. Output written to %s\n", output_filename);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
