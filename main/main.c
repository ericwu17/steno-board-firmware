#include "keyboard_device.h"
#include "esp_log.h"
#include "kbd_gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t stroke_reader_task_hdl;
TaskHandle_t stroke_handler_task_hdl;

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
    }
}
