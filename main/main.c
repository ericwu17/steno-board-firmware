#include "keyboard_device.h"
#include "esp_log.h"
#include "kbd_gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "key_section.h"
#include "str_section.h"
#include "val_section.h"

TaskHandle_t stroke_reader_task_hdl;
TaskHandle_t stroke_handler_task_hdl;

// temporary function to avoid optimizations
void count_zeros() {
    int count = 0;
    for (int i = 0; i < key_section_size; i ++) {
        if (key_section[i] == 0) {
            count += 1;
        }
    }
    for (int i = 0; i < str_section_size; i ++) {
        if (str_section[i] == 0) {
            count += 1;
        }
    }
    for (int i = 0; i < val_section_size; i ++) {
        if (val_section[i] == 0) {
            count += 1;
        }
    }

    ESP_LOGI("XXX", "ZERO COUNT IS %x", count);
}

void app_main(void) {
    init_keyboard();
    init_kbd_gpio();

    xTaskCreate(stroke_reader_task, "stroke_reader_task", 3 * 1024, NULL, configMAX_PRIORITIES - 3,
                &stroke_reader_task_hdl);
    xTaskCreate(stroke_handler_task, "stroke_handler_task", 3 * 1024, NULL, configMAX_PRIORITIES - 3,
                &stroke_handler_task_hdl);

    int led1state = 0;
    while(1) {
        vTaskDelay(20 / portTICK_PERIOD_MS);

        vTaskDelay(1000/portTICK_PERIOD_MS);
        set_led_1(led1state);
        led1state = !led1state;

        count_zeros();

    }
}



