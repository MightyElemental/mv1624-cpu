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

extern bool stage0;     // current stage of the decoder 0
extern bool stage1;     // current stage of the decoder 1

extern bool vec_reg_en; // enable vector register write
extern bool ir_en;      // instruction register write enable
extern bool pc_en;      // program counter write enable
extern bool ram_en;     // memory write enable

extern bool alu_ctl3;
extern bool alu_ctl2;
extern bool alu_ctl1;
extern bool alu_ctl0;

extern bool dat_sel0;
extern bool dat_sel1;

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

    u8 byte1 = (inst_register >> 24);

    // std::cout << "y0-3? " << y00 << y01 << y02 << y03 << std::endl;

    halt                = (byte1) == 0;
    reg_load_opr        = (byte1&0b1111'1100) == 0b1000'0000; // 1000_00rr
    reg_add_opr         = (byte1&0b1111'1100) == 0b1000'0100; // 1000_01rr

    vtx_instructions    = nib1 == 15;

}

void set_microcode_flags() {
    // update ring counter for CPU step
    fetch = ring_count == 0;
    decode = ring_count == 1;
    execute = ring_count == 2;
    
    // Produce one-hot encodings for instructions
    decode_1h_instruction();

    bytes_to_load = inst_register >> 30;
    // bitwise comparison = not (a xor b)
    stage_done = (fetch && substage >= bytes_to_load) || decode || execute;

    ir_en = (fetch && !stage_done) || (execute && stage_done);
    pc_en = decode || (fetch && !stage_done);
    reg_en = execute && (reg_load_mem_dir || reg_load_opr || reg_add_opr);

    u8 byte1 = (inst_register >> 24);

    alu_ctl3 = false;
    alu_ctl2 = false;
    alu_ctl1 = reg_add_opr;
    alu_ctl0 = false;

    // write+read register select
    reg_selx0 = byte1&1;
    reg_selx1 = (byte1>>1)&1;
    // read register select
    reg_sely0 = (reg_load_opr && byte1&1);
    reg_sely1 = (reg_load_opr && (byte1>>1)&1);

    dat_sel0 = false;
    dat_sel1 = false;

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
    //addr_sel1 = reg_load_mem_dir;

    //op_mem_addr = static_cast<u32>(inst_register);

    //if (execute) {

        std::cout << "inst: ";
        printBinary32(inst_register);

        std::string step = (ring_count == 0 ? "\033[97;46mFetch\033[0m" : 
        (ring_count == 1 ? "\033[97;45mDecode\033[0m" : "\033[97;44mExecute\033[0m"));

        std::cout 
        << "bytes2load: " << +bytes_to_load 
        << ", stage: " << step 
        << ", done? " << (stage_done?"Yes":"No")
        << ", substage: " << +substage
        << std::endl;

        print_color_string(ir_en, "ir_en", " ");
        print_color_string(pc_en, "pc_en", " ");
        print_color_string(reg_en, "reg_en", " ");
        std::cout << std::endl;

        print_color_string(reg_load_opr, "reg_load_opr", " ");
        print_color_string(reg_add_opr, "reg_add_opr", " ");
        std::cout << std::endl;

        // std::cout << "HLT? " << (halt?"Yes":"No") << std::endl;
    //}
    //std::cout << "VTX? " << (vtx_instructions?"Yes":"No") << std::endl;

    

    substage = stage_done ? 0 : substage+1; // do not increment substage if done
    substage = decode ? 0 : substage;       // no substages in decode

    // go to next step if stages are complete
    ring_count = stage_done ? (ring_count+1) % 3 : ring_count;

    stage0 = (substage) & 1;
    stage1 = ((substage)>>1) & 1;
}

