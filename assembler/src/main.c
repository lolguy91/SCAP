#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Instruction structure
struct Instruction {
    uint8_t byte1;
    uint16_t addr;
};

// Register structure
struct Register {
    uint8_t value;
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
uint8_t regname2id(char *name) {
    if (strcmp(name, "A") == 0) {
        return 0;
    } else if (strcmp(name, "B") == 0) {
        return 1;
    } else if (strcmp(name, "SPL") == 0) {
        return 2;
    } else if (strcmp(name, "SPH") == 0) {
        return 3;
    }
    return 0;
}

// Parse tokens to generate the instruction
void parse_instruction(struct Token *tokens, int token_count, struct Instruction *instruction) {
    // Reset instruction fields
    memset(instruction, 0, sizeof(struct Instruction));
    uint8_t opcode = 0;
    uint8_t reg1 = 0;
    uint8_t reg2 = 0;
    // Set opcode based on the mnemonic
    char *mnemonic = tokens[0].value;
    str_toupper(mnemonic);  // Convert mnemonic to uppercase

    if (strcmp(mnemonic, "LD") == 0) {
        opcode = 0x0;
    } else if (strcmp(mnemonic, "ST") == 0) {
        opcode = 0x1;
    } else if (strcmp(mnemonic, "MV") == 0) {
        opcode = 0x2;
    } else if (strcmp(mnemonic, "ADD") == 0) {
        opcode = 0x3;
    } else if (strcmp(mnemonic, "SUB") == 0) {
        opcode = 0x4;
    } else if (strcmp(mnemonic, "SL") == 0) {
        opcode = 0x5;
    } else if (strcmp(mnemonic, "PUSH") == 0) {
        opcode = 0x6;
    } else if (strcmp(mnemonic, "POP") == 0) {
        opcode = 0x7;
    } else if (strcmp(mnemonic, "JMP") == 0) {
        opcode = 0x8;
    } else if (strcmp(mnemonic, "JZ") == 0) {
        opcode = 0x9;
    } else if (strcmp(mnemonic, "JNZ") == 0) {
        opcode = 0xA;
    } else if (strcmp(mnemonic, "JC") == 0) {
        opcode = 0xB;
    } else if (strcmp(mnemonic, "JNC") == 0) {
        opcode = 0xC;
    } else if (strcmp(mnemonic, "CALL") == 0) {
        opcode = 0xD;
    } else if (strcmp(mnemonic, "RET") == 0) {
        opcode = 0xE;
    } else if (strcmp(mnemonic, "NOP") == 0) {
        opcode = 0xF;
    } else if (strcmp(mnemonic, "DB") == 0) {
        opcode = 0xFF;
        reg1 = 0xFF;
        reg2 = 0xFF;
    } else {
        printf("[SCAP AS] Error: Unknown mnemonic %s\n", mnemonic);
        exit(1);
    }

    // Set register and address fields based on the opcode
    switch (opcode) {
        case 0x0: case 0x1: case 0x2:
            if (token_count >= 3) {
                reg1 = regname2id(tokens[1].value);
                sscanf(tokens[2].value, "0x%hx", &instruction->addr);
            }else{
                printf("[SCAP AS] Error: Missing register or address\n");
                exit(1);
            }
            break;

        case 0x3: case 0x4:
            if (token_count >= 3) {
                reg1 = regname2id(tokens[1].value);
                reg2 = regname2id(tokens[2].value);
            } else {
                printf("[SCAP AS] Error: Missing register \n");
                exit(1);
            }
            break;

        case 0x5:
            if (token_count >= 2) {
                reg1 = regname2id(tokens[1].value);
            } else {
                printf("[SCAP AS] Error: Missing register\n");
                exit(1);
            }
            break;

        case 0x6: case 0x7:
            if (token_count >= 2) {
                reg1 = regname2id(tokens[1].value);
            } else {
                printf("[SCAP AS] Error: Missing register\n");
                exit(1);
            }
            break;

        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD:
            if (token_count >= 2) {
                sscanf(tokens[1].value, "0x%hx", &instruction->addr);
            } else {
                printf("[SCAP AS] Error: Missing address\n");
                exit(1);
            }
            break;
        case 0xE: case 0xF:
            break;
        case 0xFF://DB(special case)
            sscanf(tokens[1].value, "0x%hx", &instruction->addr);
            break;
        default:
            printf("[SCAP AS] Error: Unsupported opcode %s\n", mnemonic);
            exit(1);
    }
    //switch byte order of addr to big endian if neeeded
    #ifdef _BIG_ENDIAN_
        if(opcode != 0xFF){
            instruction->addr = __builtin_bswap16(instruction->addr);
        }
    #endif
    instruction->byte1 = opcode << 4;
    instruction->byte1 |= reg1 << 0;
    instruction->byte1 |= reg2 << 2;
    if(opcode != 0xFF){
        #ifdef _BIG_ENDIAN_
        // Log decoded values switched
        printf("[SCAP AS] Decoded: BIN=0x%02x%02x%02x\n",
               instruction->byte1, instruction->addr & 0xff,
               instruction->addr >> 8);
        #else
        // Log decoded values normally
        printf("[SCAP AS] Decoded: BIN=0x%02x%04x\n",
               instruction->byte1, instruction->addr);
        #endif
    }else{
        // Log DB value 
        printf("[SCAP AS] DB val: 0x%02x\n",
               instruction->addr & 0xff);
    }
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
            if(instruction.byte1 == 0xFF) {
                instruction.addr &= 0xFF;
                fwrite(&instruction.addr , 1, 1, output_file);
            }else{
                fwrite(&instruction.byte1, 1, 1, output_file);
                fwrite(&instruction.addr, 2, 1, output_file);
            }
        }
    }

    printf("[SCAP AS] Assembly completed. Output written to %s\n", output_filename);

    fclose(input_file);
    fclose(output_file);

    return 0;
}

