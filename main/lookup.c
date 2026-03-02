#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "lookup.h"
#include "sections.h"
#include "utils.h"
#include "keyboard_device.h"

#define KEY_SECTION_ENTRY_LENGTH_BYTES 6

void do_lookup(uint32_t stroke) {
    int offset = find_stroke_offset_in_val_section(stroke);
    if (offset == STROKE_NOT_FOUND) {
        accept_not_found_stroke(stroke);
    } else {
        int num_entries = convert_2_byte_LE_to_uint(&val_section[offset]);
        offset += 2;

        uint32_t best_match_prev_strokes[RECENT_STROKE_EVENT_NUM_STROKES];
        int n_best_match_prev_strokes = 0;
        int best_string_offset = 0xffffffff;

        for (int entry_i = 0; entry_i < num_entries; entry_i++) {
            int n = val_section[offset];
            offset += 1;
            
            uint32_t prev_strokes[RECENT_STROKE_EVENT_NUM_STROKES];
            read_3_byte_strokes(prev_strokes, &val_section[offset], n);
            offset += 3*n;

            int _flags = val_section[offset];
            offset += 1;

            int str_offset = convert_3_byte_LE_to_uint(&val_section[offset]);
            offset += 3;

            if (prev_strokes_match(n, prev_strokes)) {
                // if the length is >= the best match, use the new match.
                if (n >= n_best_match_prev_strokes) {
                    memcpy(best_match_prev_strokes, prev_strokes, n*sizeof(uint32_t));
                    n_best_match_prev_strokes = n;
                    best_string_offset = str_offset;
                }
            }
        }

        if (best_string_offset == 0xffffffff) {
            accept_not_found_stroke(stroke);
            return;
        }

        const unsigned char* best_string = &str_section[best_string_offset];
        int popped_char_count = 0;
        if (n_best_match_prev_strokes > 0) {
            int popped_count = 0;
            while (popped_count < n_best_match_prev_strokes) {
                recent_stroke_event_t popped = recent_strokes_pop_back();
                popped_count += popped.num_strokes_pressed;
                popped_char_count += popped.num_keys_written;
            }
        }

        uint32_t strokes_pressed_for_this_entry[RECENT_STROKE_EVENT_NUM_STROKES];
        memcpy(strokes_pressed_for_this_entry, best_match_prev_strokes, n_best_match_prev_strokes*sizeof(uint32_t));
        strokes_pressed_for_this_entry[n_best_match_prev_strokes] = stroke;
        int n_strokes_pressed_for_this_entry = n_best_match_prev_strokes + 1;

        write_n_deletes(popped_char_count + 1);

        accept_stroke_with_translation(n_strokes_pressed_for_this_entry, strokes_pressed_for_this_entry, best_string);
    }
}

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

void accept_not_found_stroke(uint32_t stroke) {
    char res_string[30];
    int n_res_string = 0;
    char* steno_stroke_order = "ZSTKPWHRAO*#EUFRPBLGTSDZ";

    for (int i = 0; i < sizeof(n_res_string); i ++) {
        res_string[i] = 0;
    }
    // "ZSTKPBFR" -> 0xFF
    // "AO*EU" -> 0x3700
    bool has_no_hyphen = (stroke & 0xFF) == stroke || (stroke & 0x3700) != 0;

    for (int i = 0; i < strlen(steno_stroke_order); i++) {
        if ((stroke & (1 << i)) != 0) {
            res_string[n_res_string] = steno_stroke_order[i];
            n_res_string ++;
        }

        if (i == 11 && !has_no_hyphen) {
            res_string[n_res_string] = '-';
            n_res_string ++;
        }
    }
    
    accept_stroke_with_translation(1, &stroke, (unsigned char*) res_string);
}

void accept_stroke_with_translation(int n_strokes, uint32_t* strokes, const unsigned char* translation) {
    recent_stroke_event_t r;
    r.num_strokes_pressed = n_strokes;
    r.num_keys_written = strlen((char*) translation);
    memcpy(&r.strokes_pressed, strokes, n_strokes * sizeof(uint32_t));

    send_string((char*) translation);
    send_string(" ");
    ESP_LOGI("XXX", "STROKE WITH TRANSLATION: %s", translation);

    recent_strokes_push_back(r);
}

// pushes a recent_stroke_event_t to the end of the queue. Drops the recent_stroke_event_t
// at the start of the queue if the queue is full.
void recent_strokes_push_back(recent_stroke_event_t recent_event) {
    if (recent_events.start == recent_events.end && recent_events.n != 0) {
        // queue full, drop the event at the start of the queue.
        recent_events.n -= 1;
        recent_events.start = (recent_events.start + 1) % RECENT_STROKES_QUEUE_SIZE;
    }

    memcpy(&recent_events.events[recent_events.end], &recent_event, sizeof(recent_stroke_event_t));
    recent_events.n += 1;
    recent_events.end = (recent_events.end + 1) % RECENT_STROKES_QUEUE_SIZE;
}

bool prev_strokes_match(int n_strokes, uint32_t* strokes) {
    return prev_strokes_match_rec(n_strokes, strokes, 0);
}
bool ends_with(uint32_t* a, int n_a, uint32_t* b, int n_b) {
    // returns true if a ends with b

    if (n_a < n_b) {
        return false;
    }
    a = a + (n_a - 1);
    b = b + (n_b - 1);
    for (int i = 0; i < n_b; i++) {
        if (*a != *b) {
            return false;
        }

        a = a - 1;
        b = b - 1;
    }
    return true;
}

bool prev_strokes_match_rec(int n_strokes, uint32_t* strokes, int n) {
    if (n_strokes == 0) {
        return true;
    } else if (n >= recent_events.n) {
        return false;
    } else {
        const int idx = ((recent_events.end - 1) + RECENT_STROKES_QUEUE_SIZE - n) % RECENT_STROKES_QUEUE_SIZE;
        recent_stroke_event_t last_recent = recent_events.events[idx];
        if (ends_with(strokes, n_strokes, last_recent.strokes_pressed, last_recent.num_strokes_pressed)) {
            const int num_last_strokes_pressed = last_recent.num_strokes_pressed;
            return prev_strokes_match_rec(n_strokes - num_last_strokes_pressed, strokes, n+1);
        } else {
            return false;
        }
    }
}

void read_3_byte_strokes(uint32_t* dst, const uint8_t* src, int n) {
    for (int i = 0; i < n; i++) {
        *dst = convert_3_byte_LE_to_uint(src);

        dst += 1;
        src += 3;
    }
}

recent_stroke_event_t recent_strokes_pop_back() {
    // this function has undefined behavior is the queue is empty. Pops a recent_stroke_event_t from the back
    // of the queue.

    int last_idx = (recent_events.end + RECENT_STROKES_QUEUE_SIZE - 1) % RECENT_STROKES_QUEUE_SIZE;
    recent_stroke_event_t result = recent_events.events[last_idx];

    recent_events.end = last_idx;
    recent_events.n -= 1;
    return result;
}

void write_n_deletes(int n) {
    send_n_deletes(n);

    ESP_LOGI("XXX", "The delete key has been pressed %u times", n);
}