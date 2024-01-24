#include "SCAPCore.h"
#include "bus.h"
#include <stdio.h>

// registers
uint8_t A;
uint8_t B;
uint16_t PC;
uint16_t SP;
uint8_t flags;

// fetch and decode stuff
uint32_t instruction;
uint8_t opcode;
uint8_t reg1;
uint8_t reg2;
uint16_t addr;
uint8_t imm;

bool debug;

bool scap_fetch() {
    // Instruction on SCAP are 4 bytes long
    uint8_t low = bus_read(PC);
    uint8_t mid = bus_read(PC + 1);
    uint8_t high = bus_read(PC + 2);
    uint8_t even_higher = bus_read(PC + 3);
    instruction = (even_higher << 24) | (high << 16) | (mid << 8) | low;
    PC += 4;
    return true;
}

bool scap_decode() {
    // On SCAP, the first 4 bits are the opcode
    // the next 2 bits are the first register
    // the next 2 bits are the second register
    // the next 16 bits are the address
    // and the las 8 bits are the immediate value.
    // Everything except the opcode is optional.
    // The address is in Little Endian.

    opcode = (instruction >> 4) & 0xF;
    reg1 = (instruction >> 0) & 0x3;
    reg2 = (instruction >> 2) & 0x3;
    addr = (instruction >> 8) & 0xFFFF;
    imm = (instruction >> 24) & 0xFF;
    return true;
}

bool write_reg(uint8_t reg, uint8_t data) {
    // of the 6 builtin registers, only 4 are accessable
    // because I only have 2 bits for a register number
    switch (reg) {
    case 0: // A register
        A = data;
        return true;
    case 1: // B register
        B = data;
        return true;
    case 2: // SP upper
        SP &= 0xFF00;
        SP |= data;
        return true;
    case 3: // SP lower
        SP &= 0x00FF;
        SP |= (data << 8);
        return true;
    }
    return false;
}

uint8_t read_reg(uint8_t reg) {
    // of the 6 builtin registers, only 4 are accessable
    // because I only have 2 bits for a register number
    switch (reg) {
    case 0: // A register
        return A;
    case 1: // B register
        return B;
    case 2: // SP upper
        return SP >> 8;
    case 3: // SP lower
        return SP & 0xFF;
    }
    return 0;
}

bool scap_execute() {
    // NOTE:these values are only used by ADD and SUB
    uint8_t a = 0;
    uint8_t b = 0;
    uint8_t result = 0;

    switch (opcode) {
    case 0x0: // LD
        return write_reg(reg1, bus_read(addr));
    case 0x1: // ST
        bus_write(addr, read_reg(reg1));
        return true;
    case 0x2: // MV
        return write_reg(reg1, read_reg(reg2));
    case 0x3: // ADD
        a = read_reg(reg1);
        b = read_reg(reg2);
        result = a + b;
        flags = 0;
        if (result == 0) {
            flags |= 0x1;
        }
        return write_reg(reg1, result);
    case 0x4: // SUB
        a = read_reg(reg1);
        b = read_reg(reg2);
        result = a - b;
        flags = 0;
        if (result == 0) {
            flags |= 0x1;
        }
        return write_reg(reg1, result);
    case 0x5: // SL
        return write_reg(reg1, read_reg(reg1) << read_reg(reg1));
    case 0x6: // PUSH
        SP++;
        bus_write(SP, read_reg(reg1));
        return true;
    case 0x7: // POP
        write_reg(reg1, bus_read(SP));
        SP--;
        return true;
    case 0x8: // JMP
        PC = addr;
        return true;
    case 0x9: // JZ
        if (flags & 0x1) {
            PC = addr;
        }
        return true;
    case 0xA: // JNZ

        if (!(flags & 0x1)) {
            PC = addr;
        }
        return true;
    case 0xB: // LDP
        return write_reg(reg1,
                         bus_read(bus_read(addr + 1) << 8) | bus_read(addr));
    case 0xC: // LDI
        return write_reg(reg1, imm);
    case 0xD: // CALL
        SP++;
        bus_write(SP, PC & 0xFF);
        SP++;
        bus_write(SP, (PC >> 8) & 0xFF);
        PC = addr;
        return true;
    case 0xE: // RET
        PC = (bus_read(SP) << 8) | bus_read(SP - 1);
        SP -= 2;
        return true;
    case 0xF: // NOP
        return true;
    }
    return false;
}

bool scap_step() {
    if (debug) {
        printf("PC: 0x%04X\r\n", PC);
        printf("SP: 0x%04X\r\n", SP);
        printf("A: 0x%02X\r\n", A);
        printf("B: 0x%02X\r\n", B);
        printf("flags: 0x%02X\r\n", flags);
    }

    /*bool f =*/scap_fetch();
    // if (!f)               Useless, since fetch can't fail
    //    return false;
    /*bool d =*/scap_decode();
    // if (!d)               Useless, since decode can't fail
    //    return false;
    bool e = scap_execute();
    if (debug) {
        printf("========\r\n");
        printf("opcode: 0x%02X\r\n", opcode);
        printf("addr: 0x%04X\r\n", addr);
        printf("reg1: %d\r\n", reg1);
        printf("reg2: %d\r\n", reg2);
        printf("imm: 0x%02X\r\n", imm);
    }
    // If execute fails,we return false
    return e;
}

void scap_init(bool debug_mode) {
    // Reset registers to default values defined by the specification
    A = 0;
    B = 0;
    PC = 0;
    SP = 0xFF;
    flags = 0;

    debug = debug_mode;
}

void scap_interrupt() {
    SP++;
    bus_write(SP, PC & 0xFF);
    SP++;
    bus_write(SP, (PC >> 8) & 0xFF);
    PC = (bus_read(0xFFFF) << 8) | bus_read(0xFFFE);
}
