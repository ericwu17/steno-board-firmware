#ifndef LOOKUP_H
#define LOOKUP_H
#include <stdint.h>
#include <stdbool.h>

#define STROKE_NOT_FOUND 0xFFFFFFFF

#define RECENT_STROKE_EVENT_NUM_STROKES 10
struct recent_stroke_event_t {
    int num_keys_written;
    int num_strokes_pressed;
    uint32_t strokes_pressed[RECENT_STROKE_EVENT_NUM_STROKES];
};

typedef struct recent_stroke_event_t recent_stroke_event_t;

#define RECENT_STROKES_QUEUE_SIZE 30
struct recent_stroke_events_t {
    int n;
    int start;
    int end;
    recent_stroke_event_t events[RECENT_STROKES_QUEUE_SIZE];
};
typedef struct recent_stroke_events_t recent_stroke_events_t;

static recent_stroke_events_t recent_events;

void do_lookup(uint32_t stroke);

void write_n_deletes(int n);

uint32_t find_stroke_offset_in_val_section(uint32_t stroke);



// helper functions

bool ends_with(uint32_t* a, int n_a, uint32_t* b, int n_b);
bool prev_strokes_match(int n_strokes, uint32_t* strokes);
bool prev_strokes_match_rec(int n_strokes, uint32_t* strokes, int n);
void recent_strokes_push_back(recent_stroke_event_t recent_event);
recent_stroke_event_t recent_strokes_pop_back();
void read_3_byte_strokes(uint32_t* dst, const uint8_t* src, int n);

void accept_stroke_with_translation(int n_strokes, uint32_t* strokes, const unsigned char* translation);
void accept_not_found_stroke(uint32_t stroke);


#endif