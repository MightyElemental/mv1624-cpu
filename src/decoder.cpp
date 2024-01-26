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

bool load_full_inst;    // whether the other 16 bytes of the instruction are required

// State of the CPU
extern bool zero;
extern bool carry;
extern bool overflow;
extern bool pos;
extern bool neg;
extern bool halt_flag;

extern bool stage0;     // current stage of the decoder 0
extern bool stage1;     // current stage of the decoder 1

extern bool vec_reg_en; // enable vector register write
extern bool ir_en;      // instruction register write enable
extern bool pc_en;      // program counter write enable
extern bool pc_byte_adv;// advance the program counter by a single byte instead of two
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

/* Categories
0b00xx'xxxx - no operand instructions
0b01xx'xxxx - ALU instructions
*/

// Basic Instructions
bool sbinst;            // Single Byte Instructions
bool nop;               // 0x00
bool halt_inst;         // 0x3F




// ALU Instructions
bool alu_instructions;  // 0b01xx'xxxx

bool reg_load_opr;      // load value from operand into register
bool reg_load_mem_ind;  // load value from mem with address stored in reg
bool reg_load_mem_dir;  // load value from mem addr in operand
bool reg_load_reg;      // copy value from one reg to another

bool alu_and_opr;       // Bitwise AND register with operand
bool alu_and_mem_ind;   // Bitwise AND register with memory with address stored in reg
bool alu_and_mem_dir;   // Bitwise AND register with memory with addr in operand
bool alu_and_reg;       // Bitwise AND register with register

bool alu_or_opr;       // Bitwise OR register with operand
bool alu_or_mem_ind;   // Bitwise OR register with memory with address stored in reg
bool alu_or_mem_dir;   // Bitwise OR register with memory with addr in operand
bool alu_or_reg;       // Bitwise OR register with register

bool alu_xor_opr;       // Bitwise XOR register with operand
bool alu_xor_mem_ind;   // Bitwise XOR register with memory with address stored in reg
bool alu_xor_mem_dir;   // Bitwise XOR register with memory with addr in operand
bool alu_xor_reg;       // Bitwise XOR register with register

bool alu_add_opr;       // Add value from operand
bool alu_add_mem_ind;   // Add value from mem with address stored in reg
bool alu_add_mem_dir;   // Add value from mem addr in operand
bool alu_add_reg;       // Add value from reg

bool alu_sub_opr;       // Sub value from operand
bool alu_sub_mem_ind;   // Sub value from mem with address stored in reg
bool alu_sub_mem_dir;   // Sub value from mem addr in operand
bool alu_sub_reg;       // Sub value from reg

bool alu_lshift;        // Left shift register
bool alu_rshift;        // Right shift register

bool alu_mul_opr;       // Multiply value from operand
bool alu_mul_mem_ind;   // Multiply value from mem with address stored in reg
bool alu_mul_mem_dir;   // Multiply value from mem addr in operand
bool alu_mul_reg;       // Multiply value from reg

// Vector Instructions
bool vec_load;   // 1. load values into vector register





void decode_1h_instruction() {
    // bool y00 = (instruction & (1 << 15)) != 0;
    // bool y01 = (instruction & (1 << 14)) != 0;
    // bool y02 = (instruction & (1 << 13)) != 0;
    // bool y03 = (instruction & (1 << 12)) != 0;

    u8 byte1 = (inst_register >> 24);

    // std::cout << "y0-3? " << y00 << y01 << y02 << y03 << std::endl;

    sbinst              = (byte1&0b1100'0000) == 0b0000'0000; // 00xx_xxxx
    nop                 = (byte1) == 0x00;
    halt_inst           = (byte1) == 0x3F;

    alu_instructions    = (byte1&0b1100'0000) == 0b0100'0000; // 01xx_xxxx
    reg_load_opr        = (byte1)             == 0b0100'0000; // 0100_0000

    alu_add_opr         = (byte1)             == 0b0101'0000; // 0101_0000
    alu_add_mem_ind     = (byte1)             == 0b0101'0001; // 0101_0001
    alu_add_mem_dir     = (byte1)             == 0b0101'0010; // 0101_0010
    alu_add_reg         = (byte1)             == 0b0101'0011; // 0101_0011

}

void set_microcode_flags() {
    // update ring counter for CPU step
    fetch = ring_count == 0;
    decode = ring_count == 1;
    execute = ring_count == 2;
    
    // Produce one-hot encodings for instructions
    decode_1h_instruction();

    load_full_inst = !(halt_inst || nop || alu_add_reg);
    // bitwise comparison = not (a xor b)
    stage_done = (fetch && substage >= (load_full_inst&1)) || decode || execute;

    pc_byte_adv = (substage==1 || decode ) && (sbinst);
    ir_en = (fetch && !stage_done) || (execute && stage_done);
    pc_en = decode || (fetch && !stage_done);
    reg_en = execute && (reg_load_mem_dir || reg_load_opr || alu_add_opr || alu_add_reg);

    u8 byte1 = (inst_register >> 24);
    u8 byte2 = (inst_register >> 16);

    alu_ctl3 = false;
    alu_ctl2 = alu_add_opr || alu_add_reg || alu_sub_opr;
    alu_ctl1 = false;
    alu_ctl0 = alu_sub_opr;

    // write+read register select
    reg_selx0 = (byte2&1);
    reg_selx1 = (byte2>>1)&1;
    // read register select
    reg_sely0 = ((byte2>>2)&1);
    reg_sely1 = ((byte2>>3)&1);

    dat_sel0 = false;
    dat_sel1 = alu_add_reg;

    halt_flag = halt_inst && execute;

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
        << "loadfull: " << (load_full_inst?"Yes":"No") 
        << ", stage: " << step 
        << ", done? " << (stage_done?"Yes":"No")
        << ", substage: " << +substage
        << std::endl;

        print_color_string(ir_en, "ir_en", " ");
        print_color_string(pc_en, "pc_en", " ");
        print_color_string(reg_en, "reg_en", " ");
        print_color_string(pc_byte_adv, "pc_byte_adv", " ");
        std::cout << std::endl;

        print_color_string(nop, "nop", " ");
        print_color_string(halt_inst, "halt", " ");
        print_color_string(alu_instructions, "alu_instr", " ");
        print_color_string(reg_load_opr, "reg_load_opr", " ");
        print_color_string(alu_add_opr, "reg_add_opr", " ");
        print_color_string(alu_add_reg, "alu_add_reg", " ");
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

