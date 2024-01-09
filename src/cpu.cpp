#include <cstdint>
#include <iostream>
#include "decoder.h"

uint16_t program_counter = 0;

extern uint8_t memory[];
extern bool halt;

// Registers
uint16_t reg_a;
uint16_t reg_b;
uint16_t reg_c;
uint16_t reg_d;

// Vector Registers
uint16_t avx_reg_a[4];
uint16_t avx_reg_b[4];
uint16_t avx_reg_c[4];
uint16_t avx_reg_d[4];

// Flags
bool zero;
bool carry;
bool overflow;
bool pos;
bool neg;

bool reg_en;
bool avx_reg_en;


/**
 * @brief Actions to take when the system clock ticks
 * 
 * @return true when the program has halted
 * @return false otherwise
 */
bool cycle() {
    std::cout << "\npc: " << (program_counter) << std::endl;
    uint8_t upper = memory[program_counter++];
    uint8_t lower = memory[program_counter++];
    uint16_t inst = (upper << 8) | lower;

    set_microcode_flags(inst);

    return halt;
}