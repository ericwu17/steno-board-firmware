#include <stdint.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"


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

static const int stroke_queue_length = 5;
static QueueHandle_t stroke_queue;

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

    stroke_queue = xQueueCreate(stroke_queue_length, sizeof(uint32_t));
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

uint32_t read_keyboard_state() {
    uint32_t res = 0;
    vTaskDelay(3);
    gpio_set_level(ROW1, 0);
    {
        res |= (!gpio_get_level(COL01) << 0);  // Z
        res |= (!gpio_get_level(COL02) << 2);  // T
        res |= (!gpio_get_level(COL03) << 4);  // P
        res |= (!gpio_get_level(COL04) << 6);  // H
        res |= (!gpio_get_level(COL05) << 10); // *
        res |= (!gpio_get_level(COL06) << 14); // F
        res |= (!gpio_get_level(COL07) << 16); // P
        res |= (!gpio_get_level(COL08) << 18); // L
        res |= (!gpio_get_level(COL09) << 20); // T
        res |= (!gpio_get_level(COL10) << 22); // D
    }
    gpio_set_level(ROW1, 1);
    vTaskDelay(3);
    gpio_set_level(ROW2, 0);
    {
        res |= (!gpio_get_level(COL01) << 1);  // S
        res |= (!gpio_get_level(COL02) << 3);  // K
        res |= (!gpio_get_level(COL03) << 5);  // W
        res |= (!gpio_get_level(COL04) << 7);  // R
        res |= (!gpio_get_level(COL05) << 10); //*
        res |= (!gpio_get_level(COL06) << 15); // R
        res |= (!gpio_get_level(COL07) << 17); // B
        res |= (!gpio_get_level(COL08) << 19); // G
        res |= (!gpio_get_level(COL09) << 21); // S
        res |= (!gpio_get_level(COL10) << 23); // Z
    }
    gpio_set_level(ROW2, 1);
    vTaskDelay(3);
    gpio_set_level(ROW3, 0);
    {
        res |= (!gpio_get_level(COL03) << 8);  // A
        res |= (!gpio_get_level(COL04) << 9);  // O
        res |= (!gpio_get_level(COL05) << 11); // #
        res |= (!gpio_get_level(COL06) << 12); // E
        res |= (!gpio_get_level(COL07) << 13); // U
    }
    gpio_set_level(ROW3, 1);


    return res;
}

void stroke_reader_task(void *pvParameters) {
    while (1) {
        uint32_t curr_stroke = 0;

        while (1) {
            uint32_t curr_keyboard_state = read_keyboard_state();
            curr_stroke |= curr_keyboard_state;

            if (curr_keyboard_state == 0 && curr_stroke != 0) {
                // a stroke has just ended
                xQueueSend(stroke_queue, &curr_stroke, 0);
                break;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

void stroke_handler_task(void *pvParameters) {
    while (1) {
        uint32_t stroke = -1;
        if (xQueueReceive(stroke_queue, &stroke, 5000 / portTICK_PERIOD_MS) == pdTRUE) {
            ESP_LOGI("XXX", "GOT A STROKE %x", stroke);
        }
    }
}