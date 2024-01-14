#include <iostream>
#include <cstdint>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

void printBinary32(u32 value) {
    for (int i = 31; i >= 0; --i) {
        // Use bitwise AND to check the value of each bit
        std::cout << ((value >> i) & 1);
        if (i%4==0) std::cout << "_";
    }
    std::cout << std::endl;
}

void printBinary16(u16 value) {
    for (int i = 15; i >= 0; --i) {
        // Use bitwise AND to check the value of each bit
        std::cout << ((value >> i) & 1);
        if (i%4==0) std::cout << "_";
    }
    std::cout << std::endl;
}