#include <cstdint>
#include <iostream>
#include "decoder.h"
#include "alu.h"
#include "registers.h"
#include "inst_register.h"
#include "prog_counter.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

u16 program_counter = 0;
u32 inst_register = 0;
u8  mem_bus;

u16 addr_select;

extern u8 memory[];
extern bool halt;     // whether the CPU is halted
extern bool execute;

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

bool stage0;     // current stage of the decoder 0
bool stage1;     // current stage of the decoder 1

bool clear;      // clear any stored memory
bool ir_en;      // instruction register write enable
bool pc_en;      // program counter write enable
bool vec_reg_en; // vector register write enable
bool ram_en;     // memory write enable

bool addr_sel0;  // memory address source select 0
bool addr_sel1;  // memory address source select 1
u16  op_mem_addr;// memory address from operand
u16  mem_addr;   // selected memory address

bool reg_en;     // register write enable
bool reg_selx0;  // register select1
bool reg_selx1;  // register select2
bool reg_sely0;  // register select y1
bool reg_sely1;  // register select y2
u16  reg_dat_in; // the databus into the register block
u16  reg_x_dat;  // register data x line
u16  reg_y_dat;  // register data y line

bool dat_sel0;
bool dat_sel1;

void init_cpu(int memsize) {
    spointer = memsize-1;
    ir_en  = true;
}

void write_mem() {
    if (!ram_en) return; // if ram enable is not high, don't write memory

    memory[mem_addr] = reg_x_dat;
}

void set_mem_source() {
    u8 mem_src = ((addr_sel0&1)) | ((addr_sel1&1) << 1);

    /*
    0 - program counter
    1 - from a register
    2 - from an operand
    */

    switch (mem_src) {
        case 0:
            mem_addr = program_counter;
            break;
        case 1:
            mem_addr = reg_y_dat;
            break;
        case 2:
            mem_addr = op_mem_addr;
    }
}

void clk_mem() {
    mem_bus = memory[mem_addr];
}

/**
 * @brief Actions to take when the system clock ticks
 * 
 * @return true when the program has halted
 * @return false otherwise
 */
bool cycle() {
    std::cout << "pc: " << (program_counter) << std::endl;

    /*
        Basic steps
        1. load first byte
        2. partial decode
        3. load remaining required bytes
        4. full decode
        5. set data busses (registers & other data input busses)
        6. calculate via ALU
        7. write to register
        8. write to ram
    */

    set_mem_source();
    clk_mem();

    std::cout << "addr: " << mem_addr << ", mem: " << +mem_bus << std::endl;

    // TODO: Fix issue where instruction register is not updating on the same cycle

    clk_ir_reg();

    set_microcode_flags();

    clk_pc();

    set_regx_bus();
    set_regy_bus();

    execute_alu();

    clk_registers();
    write_mem();

    return halt && execute;
}