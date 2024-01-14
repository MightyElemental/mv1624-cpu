#include <cstdint>
#include "cpu.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

extern u32 inst_register;
extern u8  mem_bus;

extern bool ir_en;
extern bool clear;

u8 byte_count;

/**
 * @brief Read bytes from the memory bus into the instruction register
 * 
 */
void clk_ir_reg() {
    if (clear) {
        inst_register = 0;
        byte_count = 0;
        return;
    }
    if (!ir_en) return; // If IR write is not enabled, do nothing

    inst_register |= mem_bus << ((3 - byte_count)*8);

    byte_count = (byte_count+1) % 4;
}