#include <cstdint>
// #include <iostream>
#include "cpu.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

extern u32 inst_register;
extern u16  mem_bus;

extern bool ir_en;
extern bool clear;

extern bool stage0;     // current stage of the decoder 0
extern bool stage1;     // current stage of the decoder 1

// masks used to delete IR data
u32 masks[2] = {
    0x00'00'00'00,
    0xFF'FF'00'00,
};

/**
 * @brief Read bytes from the memory bus into the instruction register
 * 
 */
void clk_ir_reg() {
    if (clear) {
        inst_register = 0;
        return;
    }
    // std::cout << "     \033" << (ir_en ? "[30;42m" : "[37;41m") << +byte_count << ", clear: " << (clear?"YES":"NO") << ", ir_en: " << (ir_en?"YES":"NO") << "\033[0m" << std::endl;
    if (!ir_en) return; // If IR write is not enabled, do nothing

    inst_register &= masks[stage0&1];
    inst_register |= mem_bus << ((1 - (stage0&1))*16);
}