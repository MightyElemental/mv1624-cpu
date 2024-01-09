#include <iostream>
#include <cstdint>
#include "cpu.h"

#define u8 uint8_t
#define u16 uint16_t

uint8_t stage; // The current stage of the multi-cycle instruction

// State of the CPU
extern bool zero;
extern bool carry;
extern bool overflow;
extern bool pos;
extern bool neg;

extern bool reg_en; // enable register write
extern bool avx_reg_en; // enable avx register write

extern bool alu_ctl3;
extern bool alu_ctl2;
extern bool alu_ctl1;
extern bool alu_ctl0;


// Basic Instructions (uses operand)
bool halt; // 0
bool move; // 1
bool add; // 2
bool sub; // 3
bool bitwiseAnd; // 4
bool load; // 5
bool store; // 6
bool addm; // 7
bool subm; // 8
bool compare; // 9
bool bitwiseOr; // 10
bool bitwiseXor; // 11

bool reg_instructions; // 14
bool vtx_instructions; // 15

// Register Instructions
// 1110iiii
// rr--rr--
bool reg_load;    //  load value into register

bool reg_add_opr; //  Add value from operand
bool reg_add_mem; //  Add value from mem with address stored in D
bool reg_add_reg; //  Add value from reg

bool reg_sub_opr; //  Sub value from operand
bool reg_sub_mem; //  Sub value from mem with address stored in D
bool reg_sub_reg; //  Sub value from reg

bool reg_and_reg; //  Bitwise AND two registers
bool reg_or_reg;  //  Bitwise OR two registers
bool reg_xor_reg; //  Bitwise XOR two reigsters

bool reg_lshift;  //  Left shift register
bool reg_rshift;  //  Right shift register

// Vector Instructions
bool vec_load;   // 1. load values into vector register



void printBinary(u16 value) {
    for (int i = 15; i >= 0; --i) {
        // Use bitwise AND to check the value of each bit
        std::cout << ((value >> i) & 1);
        if (i%4==0) std::cout << "_";
    }
    std::cout << std::endl;
}

void decode_1h_instruction(u16 instruction) {
    // bool y00 = (instruction & (1 << 15)) != 0;
    // bool y01 = (instruction & (1 << 14)) != 0;
    // bool y02 = (instruction & (1 << 13)) != 0;
    // bool y03 = (instruction & (1 << 12)) != 0;

    u8 nib1 = instruction >> 12;
    u8 nib2 = (instruction >> 8) & 0x0F;

    std::cout << "inst: ";
    printBinary(instruction);

    // std::cout << "y0-3? " << y00 << y01 << y02 << y03 << std::endl;

    halt             =  nib1 == 0;

    reg_instructions =  nib1 == 14;
    vtx_instructions =  nib1 == 15;

    //bool y04 = (instruction & (1 << 11)) != 0;
    //bool y05 = (instruction & (1 << 10)) != 0;
    //bool y06 = (instruction & (1 <<  9)) != 0;
    //bool y07 = (instruction & (1 <<  8)) != 0;

    reg_add_mem = reg_instructions && (nib2 == 0);
    reg_add_reg = reg_instructions && (nib2 == 1);
    reg_sub_mem = reg_instructions && (nib2 == 2);
    reg_sub_reg = reg_instructions && (nib2 == 3);

    // bool y08 = (instruction & (1 << 7)) != 0;
    // bool y09 = (instruction & (1 << 6)) != 0;
    // bool y10 = (instruction & (1 << 5)) != 0;
    // bool y11 = (instruction & (1 << 4)) != 0;

    // bool y12 = (instruction & (1 << 3)) != 0;
    // bool y13 = (instruction & (1 << 2)) != 0;
    // bool y14 = (instruction & (1 << 1)) != 0;
    // bool y15 = (instruction & (1 << 0)) != 0;
}

void set_microcode_flags(u16 instruction) {
    decode_1h_instruction(instruction);



    std::cout << "HLT? " << (halt?"Yes":"No") << std::endl;
    std::cout << "VTX? " << (vtx_instructions?"Yes":"No") << std::endl;
    std::cout << "REG? " << (reg_instructions?"Yes":"No") << std::endl;
}

