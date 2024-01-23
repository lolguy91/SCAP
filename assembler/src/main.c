#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct Instruction {
    uint8_t opcode : 4;
    uint8_t reg1 : 2;
    uint8_t reg2 : 2;
    uint16_t addr;
};

void assemble_instruction(char* mnemonic, char* reg1, char* reg2, char* addr, struct Instruction* instruction) {
    // Convert mnemonic to uppercase for case-insensitive comparison
    for (int i = 0; mnemonic[i]; i++) {
        mnemonic[i] = toupper(mnemonic[i]);
    }

    // Initialize instruction fields
    memset(instruction, 0, sizeof(struct Instruction));

    // Set opcode based on the mnemonic
    if (strcmp(mnemonic, "LDI") == 0 || strcmp(mnemonic, "ldi") == 0) {
        instruction->opcode = 0x0;
    } else if (strcmp(mnemonic, "ST") == 0 || strcmp(mnemonic, "st") == 0) {
        instruction->opcode = 0x1;
    } else if (strcmp(mnemonic, "MV") == 0 || strcmp(mnemonic, "mv") == 0) {
        instruction->opcode = 0x2;
    } else if (strcmp(mnemonic, "ADD") == 0 || strcmp(mnemonic, "add") == 0) {
        instruction->opcode = 0x3;
    } else if (strcmp(mnemonic, "SUB") == 0 || strcmp(mnemonic, "sub") == 0) {
        instruction->opcode = 0x4;
    } else if (strcmp(mnemonic, "SL") == 0 || strcmp(mnemonic, "sl") == 0) {
        instruction->opcode = 0x5;
    } else if (strcmp(mnemonic, "PUSH") == 0 || strcmp(mnemonic, "push") == 0) {
        instruction->opcode = 0x6;
    } else if (strcmp(mnemonic, "POP") == 0 || strcmp(mnemonic, "pop") == 0) {
        instruction->opcode = 0x7;
    } else if (strcmp(mnemonic, "JMP") == 0 || strcmp(mnemonic, "jmp") == 0) {
        instruction->opcode = 0x8;
    } else if (strcmp(mnemonic, "JZ") == 0 || strcmp(mnemonic, "jz") == 0) {
        instruction->opcode = 0x9;
    } else if (strcmp(mnemonic, "JNZ") == 0 || strcmp(mnemonic, "jnz") == 0) {
        instruction->opcode = 0xA;
    } else if (strcmp(mnemonic, "JC") == 0 || strcmp(mnemonic, "jc") == 0) {
        instruction->opcode = 0xB;
    } else if (strcmp(mnemonic, "JNC") == 0 || strcmp(mnemonic, "jnc") == 0) {
        instruction->opcode = 0xC;
    } else if (strcmp(mnemonic, "CALL") == 0 || strcmp(mnemonic, "call") == 0) {
        instruction->opcode = 0xD;
    } else if (strcmp(mnemonic, "RET") == 0 || strcmp(mnemonic, "ret") == 0) {
        instruction->opcode = 0xE;
    } else if (strcmp(mnemonic, "NOP") == 0 || strcmp(mnemonic, "nop") == 0) {
        instruction->opcode = 0xF;
    } else {
        printf("[SCAP AS] Error: Unknown mnemonic %s\n", mnemonic);
        return;
    }

    // Set register and address fields
    if (reg1) instruction->reg1 = atoi(&reg1[1]);
    if (reg2) instruction->reg2 = atoi(&reg2[1]);
    if (addr) sscanf(addr, "0x%hx", &instruction->addr);

    // Log decoded values
    printf("[SCAP AS] Decoded: HEX=%04X, BIN=%04d%02d%02d%04d%08d\n",
       instruction->addr, instruction->opcode, instruction->reg1,
       instruction->reg2, (instruction->addr & 0xF000) >> 12, instruction->addr & 0xFFF);    
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

    char line[100];
    char mnemonic[10], reg1[3], reg2[3], addr[8];
    struct Instruction instruction;

    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Ignore comments and empty lines
        if (line[0] == ';' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        // Reset fields before assembling
        memset(&instruction, 0, sizeof(struct Instruction));
        memset(mnemonic, 0, sizeof(mnemonic));
        memset(reg1, 0, sizeof(reg1));
        memset(reg2, 0, sizeof(reg2));
        memset(addr, 0, sizeof(addr));

        // Assemble instruction based on the line
        if (sscanf(line, "%s %[^,], %[^,], %s", mnemonic, reg1, reg2, addr) >= 2) {
            assemble_instruction(mnemonic, reg1, reg2, addr, &instruction);
            fwrite(&instruction, sizeof(struct Instruction), 1, output_file);
        } else if (sscanf(line, "%s", mnemonic) == 1) {
            assemble_instruction(mnemonic, NULL, NULL, NULL, &instruction);
            fwrite(&instruction, sizeof(struct Instruction), 1, output_file);
        }
    }

    printf("[SCAP AS] Assembly completed. Output written to %s\n", output_filename);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
