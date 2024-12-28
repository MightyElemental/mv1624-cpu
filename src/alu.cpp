#include <cstdint>
#include "cpu.h"
// #include "utils.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

/*
    For simplicity sake, calculations will use standard C operations
    as opposed to writing out circuit logic.
*/

extern bool zero;
extern bool carry;
extern bool overflow;
extern bool pos;
extern bool neg;

extern bool alu_ctl3; // mutex3
extern bool alu_ctl2; // mutex2
extern bool alu_ctl1; // mutex1
extern bool alu_ctl0; // modifier

extern u16 alu_in_a; // reg_y_dat
extern u16 alu_in_b;
extern u16 reg_dat_in;

/**
 * @brief Adds or subs a to/from b depending on the modifier value.
 * 
 * @return u16
 */
u16 alu_adder() {
    u16 res = alu_ctl0 ? (alu_in_a - alu_in_b) : (alu_in_a + alu_in_b);
    zero = res == 0;

    return res;
}

/**
 * @brief Performs a bitwise and operation on a and b
 * 
 * @return u16 
 */
u16 alu_and() {
    return alu_in_a & alu_in_b;
}

/**
 * @brief Performs a bitwise or operation on a and b
 * 
 * @return u16 
 */
u16 alu_or() {
    return alu_in_a | alu_in_b;
}

/**
 * @brief Performs a bitwise xor operation on a and b
 * 
 * @return u16 
 */
u16 alu_xor() {
    return alu_in_a ^ alu_in_b;
}

/**
 * @brief Performs a left/right shift on a depending on modifier value
 * 
 * @return u16
 */
u16 alu_shift() {
    return alu_ctl0 ? alu_in_a << 1 : alu_in_a >> 1;
}

/**
 * @brief Performs an integer multiply on a and b
 * 
 * @return u16 
 */
u16 alu_mult() {
    return alu_in_a * alu_in_b;
}

void execute_alu() {
    u8 mutex = alu_ctl3 << 3 | alu_ctl2 << 2 | alu_ctl1 << 1 | alu_ctl0;

    // printBinary16(alu_in_a);
    // printBinary16(alu_in_b);

    switch(mutex) {
        case 0: // 0000 passthrough
            reg_dat_in = alu_in_b;
            break;
        case 1: // 0001 bitwise AND
            reg_dat_in = alu_and();
            break;
        case 2: // 0010 bitwise OR
            reg_dat_in = alu_or();
            break;
        case 3: // 0011 bitwise XOR
            reg_dat_in = alu_xor();
            break;
        case 4: // 0100 add
        case 5: // 0101 sub
            reg_dat_in = alu_adder();
            break;
        case 6: // 0110 right shift
        case 7: // 0111 left shift
            reg_dat_in = alu_shift();
            break;
        case 8: // 1000 multiply
            reg_dat_in = alu_mult();
            break;
        default:
            reg_dat_in = alu_in_b;
    }
}