#include <iostream>
#include <cstdint>
#include "utils.h"
#include "cpu.h"
#include "registers.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

u8 substage;        // The current substage of the multi-cycle instruction
u8 ring_count;      // Fetch / Decode / Execute step
bool stage_done;    // Whether the current fetch/decode/execute stage is complete

bool fetch;
bool decode;
bool execute;

u8 bytes_to_load;      // How many bytes to load into the instruction register

// State of the CPU
extern bool zero;
extern bool carry;
extern bool overflow;
extern bool pos;
extern bool neg;

extern bool vec_reg_en; // enable vector register write
extern bool ir_en;      // instruction register write enable
extern bool pc_en;      // program counter write enable
extern bool ram_en;     // memory write enable

extern bool alu_ctl3;
extern bool alu_ctl2;
extern bool alu_ctl1;
extern bool alu_ctl0;

extern bool reg_en;     // register write enable
extern bool reg_selx0;  // register select x1
extern bool reg_selx1;  // register select x2
extern bool reg_sely0;  // register select y1
extern bool reg_sely1;  // register select y2

extern bool addr_sel0;  // memory address source select 0
extern bool addr_sel1;  // memory address source select 1
extern u16  op_mem_addr;// memory address from operand

extern u32 inst_register;

// Basic Instructions (uses operand)
bool halt; // 0
// bool move; // 1
// bool bitwiseAnd; // 4
// bool addm; // 7
// bool subm; // 8
// bool compare; // 9
// bool bitwiseOr; // 10
// bool bitwiseXor; // 11

bool vtx_instructions; // 15

// Register Instructions
// 1110iiii
// rr--rr--
bool reg_load_opr;      // load value from operand into register
bool reg_load_mem_dir;  // load value from mem addr in operand

bool reg_add_opr;       // Add value from operand
bool reg_add_mem_ind;   // Add value from mem with address stored in reg
bool reg_add_mem_dir;   // Add value from mem addr in operand
bool reg_add_reg;       // Add value from reg

bool reg_sub_opr;       // Sub value from operand
bool reg_sub_mem_ind;   // Sub value from mem with address stored in reg
bool reg_sub_mem_dir;   // Sub value from mem addr in operand
bool reg_sub_reg;       // Sub value from reg

bool reg_mul_opr;       // Multiply value from operand
bool reg_mul_mem_ind;   // Multiply value from mem with address stored in reg
bool reg_mul_mem_dir;   // Multiply value from mem addr in operand
bool reg_mul_reg;       // Multiply value from reg

bool reg_and_reg;       // Bitwise AND two registers
bool reg_or_reg;        // Bitwise OR two registers
bool reg_xor_reg;       // Bitwise XOR two reigsters

bool reg_lshift;        // Left shift register
bool reg_rshift;        // Right shift register

// Vector Instructions
bool vec_load;   // 1. load values into vector register





void decode_1h_instruction() {
    // bool y00 = (instruction & (1 << 15)) != 0;
    // bool y01 = (instruction & (1 << 14)) != 0;
    // bool y02 = (instruction & (1 << 13)) != 0;
    // bool y03 = (instruction & (1 << 12)) != 0;

    u8 nib1 = inst_register >> 28;
    u8 nib2 = (inst_register >> 24) & 0x0F;

    std::cout << "inst: ";
    printBinary32(inst_register);

    // std::cout << "y0-3? " << y00 << y01 << y02 << y03 << std::endl;

    halt             =  nib1 == 0;

    vtx_instructions =  nib1 == 15;

    //bool y04 = (instruction & (1 << 11)) != 0;
    //bool y05 = (instruction & (1 << 10)) != 0;
    //bool y06 = (instruction & (1 <<  9)) != 0;
    //bool y07 = (instruction & (1 <<  8)) != 0;

    reg_load_mem_dir = nib1 == 5; // TESTING - DELETE

    reg_add_opr = (nib2 == 2);
    //reg_add_mem = reg_instructions && (nib2 == 0);
    //reg_add_reg = reg_instructions && (nib2 == 1);

    //reg_sub_mem = reg_instructions && (nib2 == 2);
    //reg_sub_reg = reg_instructions && (nib2 == 3);

    // bool y08 = (instruction & (1 << 7)) != 0;
    // bool y09 = (instruction & (1 << 6)) != 0;
    // bool y10 = (instruction & (1 << 5)) != 0;
    // bool y11 = (instruction & (1 << 4)) != 0;

    // bool y12 = (instruction & (1 << 3)) != 0;
    // bool y13 = (instruction & (1 << 2)) != 0;
    // bool y14 = (instruction & (1 << 1)) != 0;
    // bool y15 = (instruction & (1 << 0)) != 0;
}

void set_microcode_flags() {
    // update ring counter for CPU step
    fetch = ring_count == 0;
    decode = ring_count == 1;
    execute = ring_count == 2;
    
    // Produce one-hot encodings for instructions
    decode_1h_instruction();

    bytes_to_load = inst_register >> 30;
    stage_done = (fetch && substage >= bytes_to_load) || decode;

    ir_en = (fetch && !stage_done);
    pc_en = decode | (fetch && !stage_done);

    reg_en = reg_load_mem_dir | reg_load_opr;

    alu_ctl3 = false;
    alu_ctl2 = false;
    alu_ctl1 = false;
    alu_ctl0 = false;

    /*
    +-----------+-----------+-----------------+
    | addr_sel1 | addr_sel0 | Addr Source     |
    +-----------+-----------+-----------------+
    |     0     |     0     | program counter |
    +-----------+-----------+-----------------+
    |     0     |     1     | register y      |
    +-----------+-----------+-----------------+
    |     1     |     0     | operand         |
    +-----------+-----------+-----------------+
    |     1     |     1     |        X        |
    +-----------+-----------+-----------------+
    */

    //addr_sel0 = 
    addr_sel1 = reg_load_mem_dir;

    op_mem_addr = static_cast<u32>(inst_register); // TODO: Make IR 32bit and dynamically load bytes

    std::cout 
    << "bytes2load: " << +bytes_to_load 
    << ", stage: " << (ring_count == 0 ? "Fetch" : (ring_count == 1 ? "Decode" : "Execute")) 
    << ", done? " << (stage_done?"Yes":"No")
    << ", substage: " << +substage
    << std::endl;

    std::cout << "HLT? " << (halt?"Yes":"No") << std::endl;
    //std::cout << "VTX? " << (vtx_instructions?"Yes":"No") << std::endl;

    if (execute) {
        exit( 0);
    }

    substage = stage_done ? 0 : substage+1; // do not increment substage if done
    substage = decode ? 0 : substage;       // no substages in decode
    ring_count = stage_done ? (ring_count+1) % 3 : ring_count;
}

