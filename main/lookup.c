#include <stdint.h>
#include "lookup.h"
#include "sections.h"
#include "utils.h"

#define KEY_SECTION_ENTRY_LENGTH_BYTES 6

uint32_t find_stroke_offset_in_val_section(uint32_t stroke) {
    int low = 0; 
    int high = key_section_size / KEY_SECTION_ENTRY_LENGTH_BYTES;

    while (low < high) {
        const int mid = (low + high) / 2;
        const int mid_idx = mid * KEY_SECTION_ENTRY_LENGTH_BYTES;
        const int mid_key = convert_3_byte_LE_to_uint(&key_section[mid_idx]);

        if (mid_key == stroke) {
            // found
            const int stroke_offset = convert_3_byte_LE_to_uint(&key_section[mid_idx+3]);
            return stroke_offset;
        } else if (mid_key < stroke) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    
    // not found
    return STROKE_NOT_FOUND;
}