#include <stdint.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "keyboard_device.h"


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

static uint8_t *CURR_KBD_STATE = {0};

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

void read_keyboard_state() {
    // puts output in global variable CURR_KBD_STATE

    // For now, we are only using the first 3 bytes.
    // Byte 1:    S1- T- K- P- W- H- R- A-
    // Byte 2:    O- *1 -E -U -F -R -P -B
    // Byte 3:    -L -G -T -S -D -Z #1 S2-
    // Byte 4:    *2 *3 *4 #2 #3 #4 #5 #6
    CURR_KBD_STATE[0] = 0;
    CURR_KBD_STATE[1] = 0;
    CURR_KBD_STATE[2] = 0;
    CURR_KBD_STATE[3] = 0;

    vTaskDelay(3);
    gpio_set_level(ROW1, 0);
    {
        CURR_KBD_STATE[0] |= (!gpio_get_level(COL01) << 7);  // S1-
        CURR_KBD_STATE[0] |= (!gpio_get_level(COL02) << 6);  // T-
        CURR_KBD_STATE[0] |= (!gpio_get_level(COL03) << 4);  // P-
        CURR_KBD_STATE[0] |= (!gpio_get_level(COL04) << 2);  // H-
        CURR_KBD_STATE[1] |= (!gpio_get_level(COL05) << 6);  // *1
        CURR_KBD_STATE[1] |= (!gpio_get_level(COL06) << 3);  // -F
        CURR_KBD_STATE[1] |= (!gpio_get_level(COL07) << 1);  // -P
        CURR_KBD_STATE[2] |= (!gpio_get_level(COL08) << 7);  // -L
        CURR_KBD_STATE[2] |= (!gpio_get_level(COL09) << 5);  // -T
        CURR_KBD_STATE[2] |= (!gpio_get_level(COL10) << 3);  // -D
    }
    gpio_set_level(ROW1, 1);
    vTaskDelay(3);
    gpio_set_level(ROW2, 0);
    {
        CURR_KBD_STATE[2] |= (!gpio_get_level(COL01) << 0);  // S2-
        CURR_KBD_STATE[0] |= (!gpio_get_level(COL02) << 5);  // K-
        CURR_KBD_STATE[0] |= (!gpio_get_level(COL03) << 3);  // W-
        CURR_KBD_STATE[0] |= (!gpio_get_level(COL04) << 1);  // R-
        CURR_KBD_STATE[3] |= (!gpio_get_level(COL05) << 7);  // *2
        CURR_KBD_STATE[1] |= (!gpio_get_level(COL06) << 2);  // -R
        CURR_KBD_STATE[1] |= (!gpio_get_level(COL07) << 0);  // -B
        CURR_KBD_STATE[2] |= (!gpio_get_level(COL08) << 6);  // -G
        CURR_KBD_STATE[2] |= (!gpio_get_level(COL09) << 4);  // -S
        CURR_KBD_STATE[2] |= (!gpio_get_level(COL10) << 2);  // -Z
    }
    gpio_set_level(ROW2, 1);
    vTaskDelay(3);
    gpio_set_level(ROW3, 0);
    {
        CURR_KBD_STATE[0] |= (!gpio_get_level(COL03) << 0);  // A-
        CURR_KBD_STATE[1] |= (!gpio_get_level(COL04) << 7);  // O-
        CURR_KBD_STATE[2] |= (!gpio_get_level(COL05) << 1); // #1
        CURR_KBD_STATE[1] |= (!gpio_get_level(COL06) << 5); // -E
        CURR_KBD_STATE[1] |= (!gpio_get_level(COL07) << 4); // -U
    }
    gpio_set_level(ROW3, 1);
}

void stroke_reader_task(void *pvParameters) {
    uint64_t prev_state = 0;

    while (1) {
        read_keyboard_state();
        uint64_t curr_state = *((uint64_t*) (CURR_KBD_STATE));

        if (curr_state != prev_state) {
            send_state(&CURR_KBD_STATE[0]);
        }
    
        prev_state = curr_state;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
