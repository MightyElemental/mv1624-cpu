#include <cstdint>
#include "cpu.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

extern u16 program_counter;

extern bool pc_en;

void clk_pc() {
    // Increment PC by 1 if write enable is active
    program_counter += pc_en ? 1 : 0;
}