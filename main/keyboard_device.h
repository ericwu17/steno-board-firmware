#ifndef KEYBOARD_DEVICE_H
#define KEYBOARD_DEVICE_H

#include <stdbool.h>
#include <stdint.h>


void init_keyboard();
bool is_connected();

void send_state(uint8_t *buffer);

void stroke_reader_task(void *pvParameters);


#endif
