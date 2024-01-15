#include <iostream>
#include <cstdint>
#include "cpu.h"
#include "registers.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

const int memory_size = 1024*64;
uint8_t memory[memory_size] = {
    //0b0000'0000, // halt
    0b1001'0010, 0b1111'1111, 0b1100'0101,
    0b0100'0000, 0b0001'0001,
    0b0011'0101,
    //0b1110'0000, 0b0000'0000,
};

int main() {

    u16 a = 0;
    u16 b = 15;
    u16 res = (a-b);

    std::cout << "test " << (a-b) << " " << res << std::endl;

    init_cpu(memory_size);

    bool halted = false;

    while(!halted && program_counter < memory_size) {
        halted = cycle();
    }

    print_registers();

    return 0;
}