#include <cstdint>

#include "cpu.h"

#define u8 uint8_t
#define u16 uint16_t
#define i8 int8_t
#define i16 int16_t

extern bool clear;      // clear any stored memory
extern bool reg_en;     // register write enable
extern bool reg_selx0;  // register select x1
extern bool reg_selx1;  // register select x2
extern bool reg_sely0;  // register select y1
extern bool reg_sely1;  // register select y2

extern u16 reg_dat_in;  // the databus into the register block
extern u16 reg_x_dat;   // register data x line
extern u16 reg_y_dat;   // register data y line

u16 registers[4];

void write_regx() {
    if (clear) return;   // if clear is high, ignore write
    if (!reg_en) return; // if the write enable line is not high, don't write

    u8 val = (reg_selx0) | (reg_selx1 << 1);
    registers[val] = reg_dat_in;
}

void clear_registers() {
    if (!clear) return; // if clear not high, don't clear
    
    for (u8 i = 0; i < 4; i++) {
        registers[i] = 0;
    }
}

void set_regx_bus() {
    u8 val = (reg_selx0) | (reg_selx1 << 1);
    reg_x_dat = registers[val];
}

void set_regy_bus() {
    u8 val = (reg_sely0) | (reg_sely1 << 1);
    reg_y_dat = registers[val];
}