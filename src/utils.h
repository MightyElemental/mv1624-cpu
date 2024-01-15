#include <cstdint>
#include <iostream>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

void printBinary32(u32 value);
void printBinary16(u16 value);
void printBinary8(u8 value);
void print_color_string(bool val, std::string str, std::string suffix);