#include <iostream>
#include <cstdint>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

using namespace std;

void printBinary32(u32 value) {
    for (int i = 31; i >= 0; --i) {
        // Use bitwise AND to check the value of each bit
        std::cout << ((value >> i) & 1);
        if (i%4==0) cout << "_";
    }
    cout << endl;
}

void printBinary16(u16 value) {
    for (int i = 15; i >= 0; --i) {
        // Use bitwise AND to check the value of each bit
        cout << ((value >> i) & 1);
        if (i%4==0) cout << "_";
    }
    cout << endl;
}

void printBinary8(u8 value) {
    for (int i = 7; i >= 0; --i) {
        // Use bitwise AND to check the value of each bit
        cout << ((value >> i) & 1);
        if (i%4==0) cout << "_";
    }
    cout << endl;
}

/**
 * @brief Prints color-formatted text in red if false, green if true
 * 
 * @param val 
 * @param str 
 */
void print_color_string(bool val, string str, string suffix) {
    cout << "\033" << (val ? "[30;42m" : "[37;41m") << str << "\033[0m" << suffix;
}