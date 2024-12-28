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

extern u32 inst_register;

/* Categories
0b00xx'xxxx - No Operand Instructions
0b01xx'xxxx - ALU Instructions
0b10xx'xxxx - ?
0b11xx'xxxx - Vector Instructions
*/

// Basic Instructions
bool sbinst;            // Single Byte Instructions
bool nop;               // 0x3F
bool halt_inst;         // 0x00




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

bool alu_or_opr;        // Bitwise OR register with operand
bool alu_or_mem_ind;    // Bitwise OR register with memory with address stored in reg
bool alu_or_mem_dir;    // Bitwise OR register with memory with addr in operand
bool alu_or_reg;        // Bitwise OR register with register

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
bool vec_instructions;  // 0b11xx'xxxx

bool vec_load;          // load values into vector register





void decode_1h_instruction() {
    // bool y00 = (instruction & (1 << 15)) != 0;
    // bool y01 = (instruction & (1 << 14)) != 0;
    // bool y02 = (instruction & (1 << 13)) != 0;
    // bool y03 = (instruction & (1 << 12)) != 0;

    u8 byte1 = (inst_register >> 24);

    // std::cout << "y0-3? " << y00 << y01 << y02 << y03 << std::endl;

    sbinst              = (byte1&0b1100'0000) == 0b0000'0000; // 00xx_xxxx
    nop                 = (byte1) == 0x3F;
    halt_inst           = (byte1) == 0x00;

    // alu instructions

    alu_instructions    = (byte1&0b1100'0000) == 0b0100'0000; // 01xx_xxxx

    reg_load_opr        = (byte1)             == 0b0100'0000; // 0100_0000
    reg_load_mem_ind    = (byte1)             == 0b0100'0001; // 0100_0001
    reg_load_mem_dir    = (byte1)             == 0b0100'0010; // 0100_0010
    reg_load_reg        = (byte1)             == 0b0100'0011; // 0100_0011

    alu_and_opr         = (byte1)             == 0b0100'0100; 
    alu_and_mem_ind     = (byte1)             == 0b0100'0101; 
    alu_and_mem_dir     = (byte1)             == 0b0100'0110; 
    alu_and_reg         = (byte1)             == 0b0100'0111; 

    alu_or_opr          = (byte1)             == 0b0100'1000; 
    alu_or_mem_ind      = (byte1)             == 0b0100'1001; 
    alu_or_mem_dir      = (byte1)             == 0b0100'1010; 
    alu_or_reg          = (byte1)             == 0b0100'1011; 

    alu_xor_opr         = (byte1)             == 0b0100'1100; 
    alu_xor_mem_ind     = (byte1)             == 0b0100'1101; 
    alu_xor_mem_dir     = (byte1)             == 0b0100'1110; 
    alu_xor_reg         = (byte1)             == 0b0100'1111; 

    alu_add_opr         = (byte1)             == 0b0101'0000; // 0101_0000
    alu_add_mem_ind     = (byte1)             == 0b0101'0001; // 0101_0001
    alu_add_mem_dir     = (byte1)             == 0b0101'0010; // 0101_0010
    alu_add_reg         = (byte1)             == 0b0101'0011; // 0101_0011

    alu_sub_opr         = (byte1)             == 0b0101'0100; 
    alu_sub_mem_ind     = (byte1)             == 0b0101'0101; 
    alu_sub_mem_dir     = (byte1)             == 0b0101'0110; 
    alu_sub_reg         = (byte1)             == 0b0101'0111; 

    alu_rshift          = (byte1)             == 0b0101'1011; 
    alu_lshift          = (byte1)             == 0b0101'1111; 

    alu_mul_opr         = (byte1)             == 0b0110'0000; 
    alu_mul_mem_ind     = (byte1)             == 0b0110'0001; 
    alu_mul_mem_dir     = (byte1)             == 0b0110'0010; 
    alu_mul_reg         = (byte1)             == 0b0110'0011; 

    // vector instructions
    vec_instructions    = (byte1&0b1100'0000) == 0b1100'0000; // 11xx_xxxx
}

/**
 * Set the flags for the microcode based on the current stage and instruction
 */
void set_microcode_flags() {
    // Update ring counter for CPU step
    fetch = ring_count == 0;
    decode = ring_count == 1;
    execute = ring_count == 2;

    // Produce one-hot encodings for instructions
    decode_1h_instruction();

    // Determine if the full instruction needs to be loaded
    // Instructions like NOP, HALT, and certain ALU instructions may not require full loading
    load_full_inst = !(halt_inst || nop || 
                       alu_add_reg || alu_sub_reg || 
                       alu_and_reg || alu_or_reg || 
                       alu_xor_reg || alu_mul_reg || 
                       reg_load_reg);

    // Determine if the current stage is done
    stage_done = (fetch && substage >= (load_full_inst ? 1 : 0)) || decode || execute;

    // Advance program counter by a single byte instead of two
    pc_byte_adv = (substage == 1 || decode) && (sbinst);
    
    // Enable signals based on the current stage and instruction
    ir_en = (fetch && !stage_done) || (execute && stage_done);
    pc_en = decode || (fetch && !stage_done);
    reg_en = execute && (alu_instructions);

    // Extract bytes from the instruction register
    u8 byte1 = (inst_register >> 24);
    u8 byte2 = (inst_register >> 16);

    // ALU Control Signals
    // These determine which operation the ALU should perform
    // Mapping ALU operations to control signals
    alu_ctl3 = (byte1 & 0b0010'0000) >> 5;
    alu_ctl2 = (byte1 & 0b0001'0000) >> 4;
    alu_ctl1 = (byte1 & 0b0000'1000) >> 3;
    alu_ctl0 = (byte1 & 0b0000'0100) >> 2;

    // Write and Read Register Select
    reg_selx0 = (byte2 & 0b0000'0001);
    reg_selx1 = (byte2 & 0b0000'0010) >> 1;
    reg_sely0 = (byte2 & 0b0000'0100) >> 2;
    reg_sely1 = (byte2 & 0b0000'1000) >> 3;

    /*
    Data Select Signals:
    +----------+----------+------------+
    | dat_sel1 | dat_sel0 | source     |
    +----------+----------+------------+
    |     0    |     0    | operand    |
    |     0    |     1    | memory     |
    |     1    |     0    | register y |
    |     1    |     1    |      ?     |
    +----------+----------+------------+
    */

    // Determines the source of data for operations
    // The last two bits of byte1 determine the source of data
    dat_sel0 = (byte1 & 0x03) == 1 || (byte1 & 0x03) == 2;
    dat_sel1 = (byte1 & 0x03) == 3;

    // Halt flag is set if HALT instruction is being executed
    halt_flag = halt_inst && execute;

    /*
    Address Source Selection:
    +-----------+-----------+-----------------+
    | addr_sel1 | addr_sel0 | Addr Source     |
    +-----------+-----------+-----------------+
    |     0     |     0     | program counter |
    |     0     |     1     | register y      |
    |     1     |     0     | operand         |
    |     1     |     1     |     UNUSED      |
    +-----------+-----------+-----------------+
    */

    // Set address select signals based on the instruction
    addr_sel0 = reg_load_mem_ind;
    addr_sel1 = reg_load_mem_dir;

    // Debugging Information
    std::cout << "inst: ";
    printBinary32(inst_register);

    std::string step = (ring_count == 0 ? "\033[97;46mFetch\033[0m" : 
                        (ring_count == 1 ? "\033[97;45mDecode\033[0m" : "\033[97;44mExecute\033[0m"));

    std::cout 
        << "loadfull: " << (load_full_inst ? "Yes" : "No") 
        << ", stage: " << step 
        << ", done? " << (stage_done ? "Yes" : "No")
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
    std::cout << std::endl;
    print_color_string(reg_load_opr, "reg_load_opr", " ");
    print_color_string(alu_add_opr, "alu_add_opr", " ");
    print_color_string(alu_add_reg, "alu_add_reg", " ");
    print_color_string(alu_sub_opr, "alu_sub_opr", " ");
    print_color_string(alu_sub_reg, "alu_sub_reg", " ");
    print_color_string(alu_and_opr, "alu_and_opr", " ");
    print_color_string(alu_and_reg, "alu_and_reg", " ");
    print_color_string(alu_or_opr, "alu_or_opr", " ");
    std::cout << std::endl;
    print_color_string(alu_or_reg, "alu_or_reg", " ");
    print_color_string(alu_xor_opr, "alu_xor_opr", " ");
    print_color_string(alu_xor_reg, "alu_xor_reg", " ");
    print_color_string(alu_mul_opr, "alu_mul_opr", " ");
    print_color_string(alu_mul_reg, "alu_mul_reg", " ");
    print_color_string(alu_lshift, "alu_lshift", " ");
    std::cout << std::endl;

    // Update substage: reset if stage is done or if in decode stage
    substage = stage_done ? 0 : substage + 1;
    substage = decode ? 0 : substage; // No substages in decode

    // Advance to next step if stages are complete
    ring_count = stage_done ? (ring_count + 1) % 3 : ring_count;

    // Update decoder stages based on the current substage
    stage0 = (substage) & 1;
    stage1 = ((substage) >> 1) & 1;
}

