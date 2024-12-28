#include <cstdint>
#include "cpu.h"
#include "registers.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

const u16 memory_size = 1024*64 - 1;
u8 memory[memory_size] = {
    // reg_load_opr, register B, value 2245
    0b0100'0000, 0b0000'0001, 0b0000'1000, 0b1100'0101,
    // nop
    0x3F,
    // reg_load_opr, register A = value 1
    0b0100'0000, 0b0000'0000, 0b0000'0000, 0b0000'0001,
    // alu_add_opr, register B += value 2074
    0b0101'0000, 0b0000'0001, 0b0000'1000, 0b0001'1010,
    // alu_add_reg, register A += register A
    0b0101'0011, 0b0000'0000,
    // alu_add_reg, register C += register B
    0b0101'0011, 0b0000'0110,
    // alu_add_reg, register C += register C
    0b0101'0011, 0b0000'1010,
    // alu_sub_reg, register C -= register A
    0b0101'0111, 0b0000'0010,
    // alu_sub_opr, register B -= value 1234
    0b0101'0100, 0b0000'0001, 0b0000'0100, 0b1101'0010,
    // alu_lshift, register A <<= 1
    0b0101'1111, 0b0000'0000,
    // alu_mul_opr, register A *= 3
    0b0110'0000, 0b0000'0000, 0b0000'0000, 0b0000'0011,
    // halt
    0x00,
    //0b1010'1010, 0b1010'1010 // data
    //0b1110'0000, 0b0000'0000,
};

int main() {

    init_cpu(memory_size);

    bool halted = false;

    while(!halted && program_counter < memory_size) {
        halted = cycle();
    }

    print_registers();

    return 0;
}