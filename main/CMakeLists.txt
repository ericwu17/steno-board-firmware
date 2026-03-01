set(srcs
         "esp_hid_gap.c")
set(include_dirs ".")

idf_component_register(SRCS "kbd_gpio.c" "main.c" "keyboard_device.c" "lookup.c" "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       REQUIRES esp_hid
                       PRIV_REQUIRES nvs_flash driver)
