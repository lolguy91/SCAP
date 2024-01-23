#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Instruction structure
struct Instruction {
    uint8_t reg1 : 2;
    uint8_t reg2 : 2;
    uint8_t opcode : 4;
    uint16_t addr;
};

// Register structure
struct Register {
    uint8_t value;
};

struct Register registers[4] = {
    {0x00}, // A
    {0x10}  // B
};

// Token structure
struct Token {
    char value[20];
};

FILE *input_file, *output_file;

// Convert a string to uppercase
void str_toupper(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

// Tokenize a line into an array of tokens
int tokenize_line(char *line, struct Token *tokens, int max_tokens) {
    char *token = strtok(line, " \t\r\n");
    int token_count = 0;

    while (token != NULL && token_count < max_tokens) {
        strncpy(tokens[token_count].value, token, sizeof(tokens[token_count].value) - 1);
        tokens[token_count].value[sizeof(tokens[token_count].value) - 1] = '\0';
        token_count++;
        token = strtok(NULL, " \t\r\n");
    }

    return token_count;
}

// Parse tokens to generate the instruction
void parse_instruction(struct Token *tokens, int token_count, struct Instruction *instruction) {
    // Reset instruction fields
    memset(instruction, 0, sizeof(struct Instruction));

    // Set opcode based on the mnemonic
    char *mnemonic = tokens[0].value;
    str_toupper(mnemonic);  // Convert mnemonic to uppercase

    if (strcmp(mnemonic, "LD") == 0) {
        instruction->opcode = 0x0;
    } else if (strcmp(mnemonic, "ST") == 0) {
        instruction->opcode = 0x1;
    } else if (strcmp(mnemonic, "MV") == 0) {
        instruction->opcode = 0x2;
    } else if (strcmp(mnemonic, "ADD") == 0) {
        instruction->opcode = 0x3;
    } else if (strcmp(mnemonic, "SUB") == 0) {
        instruction->opcode = 0x4;
    } else if (strcmp(mnemonic, "SL") == 0) {
        instruction->opcode = 0x5;
    } else if (strcmp(mnemonic, "PUSH") == 0) {
        instruction->opcode = 0x6;
    } else if (strcmp(mnemonic, "POP") == 0) {
        instruction->opcode = 0x7;
    } else if (strcmp(mnemonic, "JMP") == 0) {
        instruction->opcode = 0x8;
    } else if (strcmp(mnemonic, "JZ") == 0) {
        instruction->opcode = 0x9;
    } else if (strcmp(mnemonic, "JNZ") == 0) {
        instruction->opcode = 0xA;
    } else if (strcmp(mnemonic, "JC") == 0) {
        instruction->opcode = 0xB;
    } else if (strcmp(mnemonic, "JNC") == 0) {
        instruction->opcode = 0xC;
    } else if (strcmp(mnemonic, "CALL") == 0) {
        instruction->opcode = 0xD;
    } else if (strcmp(mnemonic, "RET") == 0) {
        instruction->opcode = 0xE;
    } else if (strcmp(mnemonic, "NOP") == 0) {
        instruction->opcode = 0xF;
    } else {
        printf("[SCAP AS] Error: Unknown mnemonic %s\n", mnemonic);
        exit(1);
    }

    // Set register and address fields based on the opcode
    switch (instruction->opcode) {
        case 0x0: case 0x1: case 0x2:
            if (token_count >= 4) {
                instruction->reg1 = atoi(&tokens[1].value[1]);
                sscanf(tokens[2].value, "0x%hx", &instruction->addr);
            }
            break;

        case 0x3: case 0x4:
            if (token_count >= 4) {
                instruction->reg1 = atoi(&tokens[1].value[1]);
                instruction->reg2 = atoi(&tokens[2].value[1]);
                sscanf(tokens[3].value, "0x%hx", &instruction->addr);
            }
            break;

        case 0x5:
            if (token_count >= 2) {
                instruction->reg1 = atoi(&tokens[1].value[1]);
            }
            break;

        case 0x6: case 0x7:
            if (token_count >= 2) {
                instruction->reg1 = atoi(&tokens[1].value[1]);
            }
            break;

        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD:
            if (token_count >= 2) {
                sscanf(tokens[1].value, "0x%hx", &instruction->addr);
            }
            break;
        case 0xE: case 0xF:
            break;
        default:
            printf("[SCAP AS] Error: Unsupported opcode %s\n", mnemonic);
            exit(1);
    }
    //switch byte order of addr to big endian if neeeded
    #ifdef BIG_ENDIAN
        instruction->addr = __builtin_bswap16(instruction->addr);
    #endif
    // Log decoded values
    #ifdef BIG_ENDIAN
    //print addr switched
    printf("[SCAP AS] Decoded: BIN=0x%01x%01x%01x%02x%02x\n",
           instruction->opcode, instruction->reg1,
           instruction->reg2, instruction->addr & 0xff, instruction->addr >> 8);
    #else
    printf("[SCAP AS] Decoded: BIN=0x%01x%01x%01x%04x\n",
           instruction->opcode, instruction->reg1,
           instruction->reg2, instruction->addr);
    #endif
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("[SCAP AS] Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    input_file = fopen(argv[1], "r");
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

    output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        perror("[SCAP AS] Error opening output file");
        fclose(input_file);
        return 1;
    }

    char line[100];
    struct Token tokens[10]; // Assuming a maximum of 10 tokens per line
    struct Instruction instruction;

    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Ignore comments and empty lines
        if (line[0] == ';' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        // Tokenize the line
        int token_count = tokenize_line(line, tokens, sizeof(tokens) / sizeof(tokens[0]));

        // Parse tokens to generate the instruction
        if (token_count > 0) {
            parse_instruction(tokens, token_count, &instruction);
            fwrite(&instruction, sizeof(struct Instruction), 1, output_file);
        }
    }

    printf("[SCAP AS] Assembly completed. Output written to %s\n", output_filename);

    fclose(input_file);
    fclose(output_file);

    return 0;
}

