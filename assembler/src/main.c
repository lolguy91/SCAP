#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Instruction structure
struct Instruction {
    uint8_t byte1;
    uint16_t addr;
    uint8_t imm;
};

// Register structure
struct Register {
    uint8_t value;
};

// Token structure
struct Token {
    char value[20];
};

struct tag {
    bool exists;
    char name[256];
    uint16_t addr;
};
struct tag tags[255];

FILE *input_file, *output_file;

// Convert a string to uppercase
void str_toupper(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

uint16_t get_tag_addr(char *name) {
    for (int i = 0; i < 255; i++) {
        if(tags[i].exists) {
        }
        if (tags[i].exists && strcmp(tags[i].name, name) == 0) {
            return tags[i].addr;
        }
    }
    printf("[SCAP AS] Error: Tag '%s' not found\n", name);
    exit(1);
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
    } else if (strcmp(mnemonic, "LS") == 0) {
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
    } else if (strcmp(mnemonic, "LDP") == 0) {
        opcode = 0xB;
    } else if (strcmp(mnemonic, "LDI") == 0) {
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
                if(tokens[2].value[0] == '0' && tokens[2].value[1] == 'x'){
                    sscanf(tokens[2].value, "0x%hx", &instruction->addr);
                }else if(tokens[2].value[0] == '0' && tokens[2].value[1] == 'b'){
                    sscanf(tokens[2].value, "0b%hx", &instruction->addr);
                } else if(tokens[2].value[0] == '$'){
                    instruction->addr = get_tag_addr(&tokens[2].value[1]);
                }
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

        case 0xB:
            if (token_count >= 3) {
                reg1 = regname2id(tokens[1].value);
                if(tokens[2].value[0] == '0' && tokens[2].value[1] == 'x'){
                    sscanf(tokens[2].value, "0x%hx", &instruction->addr);
                }else if(tokens[2].value[0] == '0' && tokens[2].value[1] == 'b'){
                    sscanf(tokens[2].value, "0b%hx", &instruction->addr);
                } else if(tokens[2].value[0] == '$'){
                    instruction->addr = get_tag_addr(&tokens[2].value[1]);
                }
            }else{
                printf("[SCAP AS] Error: Missing register or address\n");
                exit(1);
            }
            break;
        case 0xC:
            if (token_count >= 3) {
                reg1 = regname2id(tokens[1].value);
                if(tokens[2].value[0] == '0' && tokens[2].value[1] == 'x'){
                    sscanf(tokens[2].value, "0x%hhx", &instruction->imm);
                }else if(tokens[2].value[0] == '0' && tokens[2].value[1] == 'b'){
                    sscanf(tokens[2].value, "0b%hhx", &instruction->imm);
                } else if(tokens[2].value[0] == '$'){
                    instruction->imm = get_tag_addr(&tokens[2].value[1]) && 0xFF;
                }
            }else{
                printf("[SCAP AS] Error: Missing register or address\n");
                exit(1);
            }
            break;

            break;
        case 0x8: case 0x9: case 0xA: case 0xD:
            if (token_count >= 2) {
                if(tokens[1].value[0] == '0' && tokens[1].value[1] == 'x'){
                    sscanf(tokens[1].value, "0x%hx", &instruction->addr);
                }else if(tokens[1].value[0] == '0' && tokens[1].value[1] == 'b'){
                    sscanf(tokens[1].value, "0b%hx", &instruction->addr);
                } else if(tokens[1].value[0] == '$'){
                    instruction->addr = get_tag_addr(&tokens[1].value[1]);
                }
            } else {
                printf("[SCAP AS] Error: Missing address\n");
                exit(1);
            }
            break;
        case 0xE: case 0xF:
            break;
        case 0xFF://DB(special case)
            if(tokens[1].value[0] == '0' && tokens[1].value[1] == 'x'){
                sscanf(tokens[1].value, "0x%hx", &instruction->addr);
            }else if(tokens[1].value[0] == '0' && tokens[1].value[1] == 'b'){
                sscanf(tokens[1].value, "0b%hx", &instruction->addr);
            } else if(tokens[1].value[0] == '$'){
                instruction->addr = get_tag_addr(&tokens[1].value[1]);
            }
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
        printf("[SCAP AS] Decoded: BIN=0x%02x%02x%02x%02x\n",
               instruction->byte1, instruction->addr & 0xff,
               instruction->addr >> 8, instruction->imm);
        #else
        // Log decoded values normally
        printf("[SCAP AS] Decoded: BIN=0x%02x%04x%02x\n",
               instruction->byte1, instruction->addr, instruction->imm);
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
    uint16_t curraddr = 0;
    //get tags
    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Ignore comments and empty lines
        if (line[0] == ';' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        // Tokenize the line
        int token_count = tokenize_line(line, tokens, sizeof(tokens) / sizeof(tokens[0]));

        if(token_count >= 0){
            //detect tags
            if(tokens[0].value[strlen(tokens[0].value) - 1] == ':'){
                for(int i = 0; i < 255; i++){
                    if(tags[i].exists == false){
                        tags[i].exists = true;
                        memcpy(tags[i].name, &tokens[0].value[0], strlen(tokens[0].value) - 1);
                        tags[i].name[strlen(tokens[0].value) - 1] = '\0';
                        tags[i].addr = curraddr;
                        printf("[SCAP AS] Found tag: \"%s\", at address: 0x%04x\n", tags[i].name, tags[i].addr);
                        break;
                    }
                }
                if (token_count >= 2 && tokens[1].value[0] != ';') {
                    str_toupper(tokens[1].value);
                    if(strcmp(tokens[1].value, "DB") == 0){
                        curraddr += 1;
                    }else{
                        curraddr += 4;
                    }
                }
            }else{
                str_toupper(tokens[0].value);
                if(strcmp(tokens[0].value, "DB") == 0){
                    curraddr += 1;
                }else{
                    curraddr += 4;
                }
            }
        }
    }

    //rewind input file
    fseek(input_file, 0, SEEK_SET);

    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Ignore comments and empty lines
        if (line[0] == ';' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        // Tokenize the line
        int token_count = tokenize_line(line, tokens, sizeof(tokens) / sizeof(tokens[0]));

        // Parse tokens to generate the instruction
        if (token_count > 0) {
            //ignore tags
            if(tokens[0].value[strlen(tokens[0].value) - 1] == ':'){
                if(token_count >= 2 && tokens[1].value[0] != ';') {
                    parse_instruction(&tokens[1], token_count, &instruction);
                    if(instruction.byte1 == 0xFF) {
                        instruction.addr &= 0xFF;
                        fwrite(&instruction.addr , 1, 1, output_file);
                    }else{
                        fwrite(&instruction.byte1, 1, 1, output_file);
                        fwrite(&instruction.byte1, 1, 1, output_file);
                        fwrite(&instruction.imm, 2, 1, output_file);
                    }
                }
            } else{
                parse_instruction(tokens, token_count, &instruction);
                if(instruction.byte1 == 0xFF) {
                    instruction.addr &= 0xFF;
                    fwrite(&instruction.addr , 1, 1, output_file);
                }else{
                    fwrite(&instruction.byte1, 1, 1, output_file);
                    fwrite(&instruction.addr, 2, 1, output_file);
                    fwrite(&instruction.imm, 1, 1, output_file);
                }
            }

        }
    }

    printf("[SCAP AS] Assembly completed. Output written to %s\n", output_filename);

    fclose(input_file);
    fclose(output_file);

    return 0;
}

