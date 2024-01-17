#include <cstdint>
#include "cpu.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

extern u16 program_counter;

extern bool pc_en;       // enable PC write
extern bool pc_byte_adv; // advance the program counter by a single byte instead of two

void clk_pc() {
    // Increment PC by 2 if write enable is active, or by 1 if pc_byte_adv is enabled
    program_counter += pc_en ? (2 - (pc_byte_adv?1:0)) : 0;
}