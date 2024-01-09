#include <iostream>
#include <cstdint>
#include "cpu.h"

#define u8 uint8_t
#define u16 uint16_t

/*
    For simplicity sake, calculations will use standard C operations
    as opposed to writing out circuit logic.
*/

extern bool zero;
extern bool carry;
extern bool overflow;
extern bool pos;
extern bool neg;

bool alu_ctl3; // mutex3
bool alu_ctl2; // mutex2
bool alu_ctl1; // mutex1
bool alu_ctl0; // modifier

u16 alu_in_a;
u16 alu_in_b;

/**
 * @brief Adds or subs a to/from b depending on the modifier value.
 * 
 * @return u16
 */
u16 alu_adder() {
    return alu_ctl0 ? (alu_in_a - alu_in_b) : (alu_in_a + alu_in_b);
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

u16 execute_alu() {
    u8 mutex = (alu_ctl3&1) << 2;
    mutex |= (alu_ctl2&1) << 1;
    mutex |= (alu_ctl1&1);

    switch(mutex) {
        case 1:
            return alu_in_b; // passthrough
        case 2:
            return alu_or();
        case 3:
            return alu_shift();
        case 4:
            return alu_and();
        case 5:
            return alu_xor();
        case 6:

        case 7:

        case 0:
        default:
            return alu_adder();
    }

    //std::cout << "mutex: " << +mutex << std::endl;

    return alu_in_b;
}