#include <iostream>
#include <cstdint>
#include "cpu.h"

static const int memory_size = 1024*64;
uint8_t memory[memory_size] = {
    0b1111'0000, 0b1111'1111,
    0b1100'0000, 0b0000'0000,
    0b1110'0000, 0b0000'0000,
};

int main() {
    std::cout << "hello world\n";

    init_cpu(memory_size);

    bool halted = false;

    while(!halted && program_counter < memory_size) {
        halted = cycle();
    }

    return 0;
}