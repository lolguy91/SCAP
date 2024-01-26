#include "SCAPCore.h"
#include "bus.h"
#include <stdio.h>

#define FLAGS_CARRY (1 << 0)
#define FLAGS_OVERFLOW (1 << 1)
#define FLAGS_ZERO (1 << 3)
#define FLAGS_NEGATIVE (1 << 4)
#define FLAGS_TEST_PASS (1 << 5)

// global state and shit
uint8_t pipeline_stage;
uint8_t tmp1,tmp2;

// registers
uint8_t GPregs[10];
uint16_t PC;
uint16_t SP;
uint16_t flags;

// instruction bytes
uint8_t ib1,ib2,ib3,ib4;

// decoded instruction(doesnt always get fully decoded)
uint8_t class,opcode;
uint8_t reg1,reg2;
uint16_t addr;
uint8_t imm,class_specific;

// execute results
uint8_t result1,result2;

bool debug;

bool write_reg(uint8_t reg, uint8_t data) {
    if (reg < 10) {
        GPregs[reg] = data;
        return true;
    }else{
        switch (reg) {
            case 10: // PCH
                PC = (PC & 0xFF00) | (data << 8);
                return true;
            case 11: // PCL
                PC = (PC & 0x00FF) | (data << 0);   
                return true;
            case 12: // SPH
                SP = (SP & 0xFF00) | (data << 8);
                return true;
            case 13: // SPL
                SP = (SP & 0x00FF) | (data << 0);
                return true;
            case 14: // FLAGSH
                flags = (flags & 0xFF00) | (data << 8);
                return true;
            case 15: // FLAGSL
                flags = (flags & 0x00FF) | (data << 0);
                return true;
        }
    }   
    return false;
}

uint8_t read_reg(uint8_t reg) {
    if (reg < 10) {
        return GPregs[reg];
    } else{
        switch (reg) {
            case 10: // PCH
                return (PC >> 8) & 0xFF;
            case 11: // PCL
                return PC & 0xFF;
            case 12: // SPH
                return (SP >> 8) & 0xFF;
            case 13: // SPL
                return SP & 0xFF;
            case 14: // FLAGSH
                return (flags >> 8) & 0xFF;
            case 15: // FLAGSL
                return flags & 0xFF;
        }
    }
    return 0xFF;
}

bool scap_fetch() {
    // stages 1-4 fetch their respective bytes
    ib1 = bus_read(PC);
    return true;
}

bool scap_decode() {
    // stages 1-4 fetch their respective bytes
    ib2 = bus_read(PC + 1);
    
    // the upper 4 bits of ib1 are the class
    // the lower 4 bits of ib1 are the opcode
    class = ib1 >> 8;
    opcode = ib1 & 0xF;

    // debugging is a good thing :-)
    if (debug) {
        printf("class: 0x%02X\r\n", class);
        printf("opcode: 0x%02X\r\n", opcode);
    }

    // only return true if the class ID is used
    if (class >= 0 && class <= 6 || class == 0xF) {
        return true;
    }
    return false;
}

bool scap_execute() {
    // stages 1-4 fetch their respective bytes
    ib3 = bus_read(PC + 2);
    switch (class) {
        case 0x0: // Loads, Stores and moves
            // in class 0 byte 2 is always a regparam
            reg1 = ib2 & 0xF;
            reg2 = ib2 >> 4;
            switch(opcode) {
                case 0x1: //LDI
                    imm = ib3;
                    write_reg(reg1, imm);
                    return true;
                case 0x3: //MV
                    write_reg(reg1, read_reg(reg2));
                case 0x4: //SWP
                    tmp1 = read_reg(reg1);
                    tmp2 = read_reg(reg2);
                    write_reg(reg1, tmp2);
                    write_reg(reg2, tmp1);
                    return true;
                default: // all other ones need ib4
                    return true;
            }
        case 0x1: // Arithmetic
            // in class 1 byte 2 is always a regparam
            reg1 = ib2 & 0xF;
            reg2 = ib2 >> 4;
            switch(opcode) {
                case 0x0: //ADD
                    tmp1 = read_reg(reg1); // store the old value
                    write_reg(reg1, read_reg(reg1) + read_reg(reg2));
                    if(read_reg(reg1) == 0) flags |= FLAGS_ZERO;
                    if(read_reg(reg1) < tmp1) flags |= FLAGS_CARRY;
                    return true;
                case 0x1: //SUB
                    tmp1 = read_reg(reg1); // store the old value
                    write_reg(reg1, read_reg(reg1) - read_reg(reg2));
                    if(read_reg(reg1) == 0) flags |= FLAGS_ZERO;
                    if(read_reg(reg1) > tmp1) flags |= FLAGS_OVERFLOW;
                    return true;
                case 0x2: //ADDS
                    tmp1 = read_reg(reg1); // store the old value
                    write_reg(reg1, read_reg(reg1) + read_reg(reg2));
                    if(read_reg(reg1) == 0) flags |= FLAGS_ZERO;
                    if(read_reg(reg1) & 0x80) flags |= FLAGS_NEGATIVE;
                    if(read_reg(reg1) < tmp1) flags |= FLAGS_CARRY;
                    return true;
                case 0x3: //SUBS
                    tmp1 = read_reg(reg1); // store the old value
                    write_reg(reg1, read_reg(reg1) - read_reg(reg2));
                    if(read_reg(reg1) == 0) flags |= FLAGS_ZERO;
                    if(read_reg(reg1) & 0x80) flags |= FLAGS_NEGATIVE;
                    if(read_reg(reg1) > tmp1) flags |= FLAGS_OVERFLOW;
                    return true;
                default: // there are no other ones
                    return false;
            }
        case 0x2: // Bit logic
            // in class 2 byte 2 is always a regparam
            reg1 = ib2 & 0xF;
            reg2 = ib2 >> 4;
            switch(opcode) {
                case 0x0: //SL
                    write_reg(reg1, read_reg(reg1) << read_reg(reg2));
                    return true;
                case 0x1: //SR
                    write_reg(reg1, read_reg(reg1) >> read_reg(reg2));
                    return true;
                case 0x2: //ROL
                    write_reg(reg1, (read_reg(reg1) << read_reg(reg2)) | (read_reg(reg1) >> (8 - read_reg(reg2))));
                    return true;
                case 0x3: //ROR
                    write_reg(reg1, (read_reg(reg1) >> read_reg(reg2)) | (read_reg(reg1) << (8 - read_reg(reg2))));
                    return true;
                case 0x4: //AND
                    write_reg(reg1, read_reg(reg1) & read_reg(reg2));
                    return true;
                case 0x5: //OR
                    write_reg(reg1, read_reg(reg1) | read_reg(reg2));
                    return true;
                case 0x6: //XOR
                    write_reg(reg1, read_reg(reg1) ^ read_reg(reg2));
                    return true;

                default: // there are no other ones
                    return false;
            }
        case 0x3: // Stack
            // in class 3 byte 2 is always a regparam
            reg1 = ib2 & 0xF;
            reg2 = ib2 >> 4;
            switch(opcode) {
                case 0x0: //PUSH
                    SP++;
                    result1 = read_reg(reg1);
                    return true;
                case 0x1: //POP
                    write_reg(reg1, bus_read(SP));
                    SP--;
                    return true;
                default: // there are no other ones
                    return false;
            }
        case 0x4: // Tests
            // in class 4 byte 2 is always a regparam
            reg1 = ib2 & 0xF;
            reg2 = ib2 >> 4;

            //Here, byte 3 is the test type
            class_specific = ib3; 
            switch(opcode) {
                case 0x0: //TESTU
                    switch (class_specific) {// class specific is the test type
                        case 0x0: //TEST Equality
                            if (read_reg(reg1) == read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x1: //TEST Inequality
                            if (read_reg(reg1) != read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x2: //TEST Less
                            if (read_reg(reg1) < read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x3: //TEST Greater
                            if (read_reg(reg1) > read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x4: //TEST Less or Equal
                            if (read_reg(reg1) <= read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x5: //TEST Greater or Equal
                            if (read_reg(reg1) >= read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        default: // there are no other ones
                            return false;
                    }
                    return true;
                case 0x1: // TESTS
                    switch (class_specific) {// class specific is the test type
                        case 0x0: //TESTS Equality
                            if (read_reg(reg1) == read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x1: //TESTS Inequality
                            if (read_reg(reg1) != read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x2: //TESTS Less
                            if ((int8_t)read_reg(reg1) < (int8_t)read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x3: //TESTS Greater
                            if ((int8_t)read_reg(reg1) > (int8_t)read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x4: //TESTS Less or Equal
                            if ((int8_t)read_reg(reg1) <= (int8_t)read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        case 0x5: //TESTS Greater or Equal
                            if ((int8_t)read_reg(reg1) >= (int8_t)read_reg(reg2)) flags |= FLAGS_TEST_PASS;
                            return true;
                        default: // there are no other ones
                            return false;
                    }
                default: // there are no other ones
                    return false;
            }
        case 0x5: // Branches
            addr = (ib3 << 8) | ib2;
            switch(opcode) {
                case 0x0: //JMP
                    PC = addr;
                    return true;
                case 0x1: //CNDJMP
                    if (flags & FLAGS_TEST_PASS) {
                        PC = addr;
                    }
                    return true;
                default: // there are no other ones
                    return false;
            }
        case 0x6: // Call and Return
            addr = (ib3 << 8) | ib2;
            switch(opcode) {
                case 0x0: //CALL
                    result1 = (PC) & 0xFF;
                    result2 = (PC >> 8) & 0xFF;
                    PC = addr;
                    return true;
                case 0x1: //RET
                    tmp1 = bus_read(SP - 1);
                    return true;
                default: // there are no other ones
                    return false;
            }
        case 0xF: // NOP
            return true;
        default: // there are no other ones
            return false;
    }
}

bool scap_execute2() {
    // stages 1-4 fetch their respective bytes
    ib4 = bus_read(PC + 3);
    switch (class) {
        case 0x0: // Loads, Stores and moves
            switch(opcode) {
                case 0x0: //LD
                    write_reg(reg1, bus_read((ib4 << 8) | ib3));
                    return true;
                case 0x2: //ST
                    result1 = read_reg(reg1);
                    return true;
                default: // there are no other ones
                    return false;
            }
        case 0x6: // Call and Return(Special case, see below)
            switch(opcode) {
                case 0x0: //CALL
                    bus_write(SP, result1);// we only need to do it in this pipline step because we can't do 2 memory opeations on the same cycle
                    return true;
                case 0x1: //RET
                    tmp2 = bus_read(SP);
                    SP -= 2;
                    PC = (tmp2 << 8) | tmp1;
                    return true;
                default: // there are no other ones
                    return false;
            }
        default:
            return true;
    }
}

bool scap_writeback() {
    switch (class) {
        case 0x0: // Loads, Stores and moves
            if(opcode == 0x2) {
                bus_write((ib4 << 8) | ib3, result1);
            }
            return true;
        case 0x3: // Stack
            if(opcode == 0x0) {
                bus_write(SP, result1);
            }
            return true;
        case 0x6: // Call and Return
            if(opcode == 0x0) {
                bus_write(SP + 1, result2);
                SP+= 2;
            }
            return true;
        // for instructions that don't write back, return true
        default:
            return true;
    }
    return false;

    // on SCAP, Writeback is responsible for incrementing the program counter
    PC += 4;
}


bool scap_step() {
    if (debug) {
        printf("====BRFORE====\r\n");
        printf("PC: 0x%04X\r\n", PC);
        printf("SP: 0x%04X\r\n", SP);
        printf("A: 0x%02X\r\n", A);
        printf("B: 0x%02X\r\n", B);
        printf("flags: 0x%02X\r\n", flags);
        printf("========\r\n");
    }
    // simulate all the stages of the pipeline in order
    bool e = true;
    e &= scap_fetch();
    e &= scap_decode();
    e &= scap_execute();
    e &= scap_execute2();
    e &= scap_writeback();

    if (debug) {
        printf("====AFTER====\r\n");
        printf("PC: 0x%04X\r\n", PC);
        printf("SP: 0x%04X\r\n", SP);
        printf("A: 0x%02X\r\n", A);
        printf("B: 0x%02X\r\n", B);
        printf("flags: 0x%02X\r\n", flags);
        printf("========\r\n");
    }
    // If one of the stages fails,we return false
    return e;
}

void scap_init(bool debug_mode) {
    // Reset registers to default values defined by the specification
    for (int i = 0; i < 10; i++) {
        GPregs[i] = 0;
    }
    PC = 0x0000;
    SP = 0x00FF;
    flags = 0x0000;

    // Set debug mode
    debug = debug_mode;
}

void scap_interrupt() {
    // NOTE: this is incorrect, but it works for now
    SP++;
    bus_write(SP, PC & 0xFF);
    SP++;
    bus_write(SP, (PC >> 8) & 0xFF);
    PC = (bus_read(0xFFFF) << 8) | bus_read(0xFFFE);
}
