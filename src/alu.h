#include <cstdint>

#define u8 uint8_t
#define u16 uint16_t

extern u16 alu_in_a;
extern u16 alu_in_b;

u16 execute_alu();

u16 alu_adder();
u16 alu_and();
u16 alu_or();
u16 alu_xor();
u16 alu_shift();
u16 alu_mult();