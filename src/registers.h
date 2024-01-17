#include <cstdint>
#define u16 uint16_t

void clk_registers();
void set_regx_bus();
void set_regy_bus();
void print_registers();

extern u16 registers[4];