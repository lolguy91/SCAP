#include "SCAPCore.h"
#include <stdio.h>
#include "bus.h"
//registers
uint8_t A;
uint8_t B;
uint16_t PC;
uint16_t SP;
uint8_t flags;

//fetch and decode stuff
uint32_t instruction;
uint8_t opcode;
uint8_t reg1;
uint8_t reg2;
uint16_t addr;

bool scap_fetch(){
    uint8_t low = bus_read(PC);
    uint8_t mid = bus_read(PC+1);
    uint8_t high = bus_read(PC+2);
    instruction = (high << 16) | (mid << 8) | low;
    PC += 3;
    return true;
}
bool scap_decode(){
    opcode = (instruction >> 4) & 0xF;
    reg1 = (instruction >> 0) & 0x3;
    reg2 = (instruction >> 2) & 0x3;
    addr = (instruction >> 8) & 0xFFFF;
    return true;
}

bool write_reg(uint8_t reg, uint8_t data){
    switch(reg){
        case 0:// A register
            A = data;
            return true;
        case 1:// B register
            B = data;
            return true;
        case 2: // SP upper
            SP &= 0xFF00;
            SP |= data;
            return true;
        case 3:// SP lower
            SP &= 0x00FF;
            SP |= (data << 8);
            return true;
    }
    return false;
}

uint8_t read_reg(uint8_t reg){
    switch(reg){
        case 0:// A register
            return A;
        case 1:// B register
            return B;
        case 2: // SP upper
            return SP >> 8;
        case 3:// SP lower
            return SP & 0xFF;
    }
    return 0;
}

bool scap_execute(){
    uint8_t a = 0;
    uint8_t b = 0;
    uint8_t result = 0;
    switch(opcode){
        case 0x0: // LD
            return write_reg(reg1, bus_read(addr));
        case 0x1://  ST
            bus_write(addr, read_reg(reg1));
            return true;
        case 0x2:// MV
            return write_reg(reg1, read_reg(reg2));
        case 0x3://ADD
            a = read_reg(reg1);
            b = read_reg(reg2);
            result = a + b;
            if(result == 0){
                flags |= 0x1;
            }
            if((uint32_t)a + (uint32_t)b > 0xFF){
                flags |= 0x2;
            }
            return write_reg(reg1, result);
        case 0x4://SUB
            a = read_reg(reg1);
            b = read_reg(reg2);
            result = a - b;
            if(result == 0){
                flags |= 0x1;
            }
            if((int32_t)a + (int32_t)b < 0){
                flags |= 0x2;
            }
            return write_reg(reg1, result);
        case 0x5://SL
            return write_reg(reg1, read_reg(reg1) << read_reg(reg2));
        case 0x6://PUSH
            SP++;
            bus_write(SP, read_reg(reg1));
            return true;
        case 0x7://POP
            write_reg(reg1, bus_read(SP));
            SP--;
            return true;
        case 0x8://JMP
            PC = addr;
            return true;
        case 0x9://JZ
            if(flags & 0x1){
                PC = addr;
            }
            return true;
        case 0xA://JNZ
            if(!(flags & 0x1)){
                PC = addr;
            }
            return true;
        case 0xB://JC
            if(flags & 0x2){
                PC = addr;
            }
            return true;
        case 0xC://JNC
            if(!(flags & 0x2)){
                PC = addr;
            }
            return true;
        case 0xD://CALL
            SP++;
            bus_write(SP, PC & 0xFF);
            SP++;
            bus_write(SP, (PC >> 8) & 0xFF);
            PC = addr;
            return true;
        case 0xE://RET
            PC = (bus_read(SP) << 8) | bus_read(SP-1);
            SP -= 2;
            return true;
        case 0xF: // NOP
            return true;
    }
    return false;
}

bool step_scap(){
    #ifdef _DEBUG
        printf("PC: 0x%04X\n", PC);
        printf("SP: 0x%04X\n", SP);
        printf("A: 0x%02X\n", A);
        printf("B: 0x%02X\n", B);
        printf("flags: 0x%02X\n", flags);
    #endif

    bool f = scap_fetch();
    if (!f) return false;
    bool d = scap_decode();
    if (!d) return false;
    bool e = scap_execute();
    
    #ifdef _DEBUG
        printf("opcode: 0x%02X\n", opcode);
        printf("addr: 0x%04X\n", addr);
        printf("reg1: %d\n", reg1);
        printf("reg2: %d\n", reg2);
    #endif
    return e;
}
void scap_init(){
    A = 0;
    B = 0;
    PC = 0;
    SP = 0xFF;
    flags = 0;
}
void scap_interrupt(){
    SP++;
    bus_write(SP, PC & 0xFF);
    SP++;
    bus_write(SP, (PC >> 8) & 0xFF);
    PC = (bus_read(0xFFFF) << 8) | bus_read(0xFFFE);
}

