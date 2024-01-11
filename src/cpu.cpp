#include <cstdint>
#include <iostream>
#include "decoder.h"
#include "alu.h"
#include "registers.h"

#define u8 uint8_t
#define u16 uint16_t

u16 program_counter = 0;

u16 addr_select;

extern u8 memory[];
extern bool halt;

// Stack pointer
u16 spointer; // stored at the top of memory and moves down

// Vector Registers
u16 avx_reg_a[4];
u16 avx_reg_b[4];
u16 avx_reg_c[4];
u16 avx_reg_d[4];

// Flags
bool zero;
bool carry;
bool overflow;
bool pos;
bool neg;

bool clear;      // clear any stored memory
bool ir_en;      // instruction register write enable
bool pc_en;      // program counter write enable
bool avx_reg_en; // vector register write enable
bool ram_en;     // memory write enable

bool reg_en;     // register write enable
bool reg_selx0;  // register select1
bool reg_selx1;  // register select2
bool reg_sely0;  // register select y1
bool reg_sely1;  // register select y2
u16  reg_dat_in; // the databus into the register block
u16  reg_x_dat;  // register data x line
u16  reg_y_dat;  // register data y line

void init_cpu(int memsize) {
    spointer = memsize-1;
}

/**
 * @brief Actions to take when the system clock ticks
 * 
 * @return true when the program has halted
 * @return false otherwise
 */
bool cycle() {
    std::cout << "\npc: " << (program_counter) << std::endl;
    u8 upper = memory[program_counter++];
    u8 lower = memory[program_counter++];
    u16 inst = (upper << 8) | lower;

    set_microcode_flags(inst);

    set_regx_bus();
    set_regy_bus();

    execute_alu();

    write_regx();
    clear_registers();

    return halt;
}