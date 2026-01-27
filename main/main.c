#include "keyboard_device.h"
#include "esp_log.h"
#include "kbd_gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void app_main(void) {
    init_keyboard();
    init_kbd_gpio();

    while(1) {
        vTaskDelay(20 / portTICK_PERIOD_MS);
        // if (is_connected()) {
        //     ESP_LOGI("XXX", "CONNECTED");

        //     vTaskDelay(3000 / portTICK_PERIOD_MS);
        //     send_string("Hello Rila.");
        //     vTaskDelay(1000 / portTICK_PERIOD_MS);
        //     send_n_deletes(5);
        //     send_string("Eric.");
        //     vTaskDelay(1000 / portTICK_PERIOD_MS);
        //     send_n_deletes(5 + 6);
        // }
        int n = read_switches();
        ESP_LOGI("XXX", "SWITCHES %i", n);
        set_led_1(n & 1);
        set_led_2((n >> 1) & 1);
    }
}
