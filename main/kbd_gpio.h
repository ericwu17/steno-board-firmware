#ifndef KBD_GPIO_H
#define KBD_GPIO_H

void init_kbd_gpio();
int read_switches();

void set_led_1(int level);
void set_led_2(int level);

#endif