#include "driver/gpio.h"

#define SW1 0
#define SW2 40
#define SW3 41
#define SW4 42

#define COL01 6
#define COL02 7
#define COL03 17
#define COL04 18
#define COL05 8
#define COL06 3
#define COL07 9
#define COL08 10
#define COL09 14
#define COL10 21

#define ROW1 13
#define ROW2 12
#define ROW3 11

#define LED1 5  // Left LED (red)
#define LED2 4  // Right LED (green)

void init_kbd_gpio() {
    gpio_set_direction(SW1, GPIO_MODE_INPUT);
    gpio_set_direction(SW2, GPIO_MODE_INPUT);
    gpio_set_direction(SW3, GPIO_MODE_INPUT);
    gpio_set_direction(SW4, GPIO_MODE_INPUT);

    gpio_set_direction(ROW1, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROW2, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROW3, GPIO_MODE_OUTPUT);

    gpio_set_direction(COL01, GPIO_MODE_INPUT);
    gpio_set_direction(COL02, GPIO_MODE_INPUT);
    gpio_set_direction(COL03, GPIO_MODE_INPUT);
    gpio_set_direction(COL04, GPIO_MODE_INPUT);
    gpio_set_direction(COL05, GPIO_MODE_INPUT);
    gpio_set_direction(COL06, GPIO_MODE_INPUT);
    gpio_set_direction(COL07, GPIO_MODE_INPUT);
    gpio_set_direction(COL08, GPIO_MODE_INPUT);
    gpio_set_direction(COL09, GPIO_MODE_INPUT);
    gpio_set_direction(COL10, GPIO_MODE_INPUT);

    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);


    gpio_pullup_en(SW1);
    gpio_pullup_en(SW2);
    gpio_pullup_en(SW3);
    gpio_pullup_en(SW4);

    gpio_set_level(ROW1, 1);
    gpio_set_level(ROW2, 1);
    gpio_set_level(ROW3, 1);

    gpio_pullup_en(COL01);
    gpio_pullup_en(COL02);
    gpio_pullup_en(COL03);
    gpio_pullup_en(COL04);
    gpio_pullup_en(COL05);
    gpio_pullup_en(COL06);
    gpio_pullup_en(COL07);
    gpio_pullup_en(COL08);
    gpio_pullup_en(COL09);
    gpio_pullup_en(COL10);

    gpio_set_level(LED1, 0);
    gpio_set_level(LED2, 0);

    
}

int read_switches() {
    return (!gpio_get_level(SW1) << 3) | (!gpio_get_level(SW2) << 2) | (!gpio_get_level(SW3) << 1) | (!gpio_get_level(SW4));
}

void set_led_1(int level) {
    gpio_set_level(LED1, level);
}
void set_led_2(int level) {
    gpio_set_level(LED2, level);
}