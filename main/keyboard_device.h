#ifndef KEYBOARD_DEVICE_H
#define KEYBOARD_DEVICE_H

#include <stdbool.h>


void init_keyboard();
bool is_connected();

void send_char(char c);
void send_string(char* s);
void send_n_deletes(int n);


#endif
