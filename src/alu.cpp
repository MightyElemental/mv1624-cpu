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

    return static_cast<u16>(res);
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
    u8 mutex = (alu_ctl3&1) << 2;
    mutex   |= (alu_ctl2&1) << 1;
    mutex   |= (alu_ctl1&1);

    // printBinary16(alu_in_a);
    // printBinary16(alu_in_b);

    switch(mutex) {
        case 0:
            reg_dat_in = alu_in_b; // passthrough
        case 2:
            reg_dat_in = alu_or();
        case 3:
            reg_dat_in = alu_shift();
        case 4:
            reg_dat_in = alu_and();
        case 5:
            reg_dat_in = alu_xor();
        case 6:
            reg_dat_in = alu_mult();
        case 7:

        case 1:
        default:
            reg_dat_in = alu_adder();
    }
}