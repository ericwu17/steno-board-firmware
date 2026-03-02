#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

void convert_uint_to_3_byte_LE(uint32_t n, uint8_t* p) {
    p[0] = n & 0xFF;
    p[1] = (n >> 8) & 0xFF;
    p[2] = (n >> 16) & 0xFF;
}

uint32_t convert_3_byte_LE_to_uint(const uint8_t* p) {
    return p[0] | (p[1] << 8) | (p[2] << 16);
}

void convert_uint_to_2_byte_LE(uint32_t n, uint8_t* p) {
    p[0] = n & 0xFF;
    p[1] = (n >> 8) & 0xFF;
}

uint32_t convert_2_byte_LE_to_uint(const uint8_t* p) {
    return p[0] | (p[1] << 8);
}




#endif
