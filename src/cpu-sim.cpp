#include <iostream>
#include <cstdint>
#include "cpu.h"
#include "registers.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

const int memory_size = 1024*64;
uint8_t memory[memory_size] = {
    0b0100'0000, 0b0000'0001, 0b0000'1000, 0b1100'0101, // load - B - 2245
    0x00, // nop
    0b0100'0000, 0b0000'0000, 0b0000'0000, 0b0000'0001, // load - A - 1
    0b0101'0000, 0b0000'0001, 0b0000'1000, 0b0001'1010, // add  - B - 2074
    0b0101'0011, 0b0000'0000, // add  - A - A
    0x3F, // halt
    //0b1010'1010, 0b1010'1010 // data
    //0b1110'0000, 0b0000'0000,
};

int main() {

    u16 a = 2245;
    u16 b = 2074;
    u16 res = (a+b);

    std::cout << "test " << res << " " << res << std::endl;

    init_cpu(memory_size);

    bool halted = false;

    while(!halted && program_counter < memory_size) {
        halted = cycle();
    }

    print_registers();

    return 0;
}