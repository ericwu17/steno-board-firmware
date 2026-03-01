#ifndef LOOKUP_H
#define LOOKUP_H
#include <stdint.h>

#define STROKE_NOT_FOUND 0xFFFFFFFF

// returns an offset into val_section if found, otherwise returns 0xffffffff.
uint32_t find_stroke_offset_in_val_section(uint32_t stroke);

#endif