# Steno board firmware

This is a project that implements the [Plover HID protocol](https://github.com/dnaq/plover-machine-hid) 
on a ESP32 microcontroller.

It's a IEEE SPI (student project initiative) that I completed in my final
year of college.

I originally wanted to have the keyboard device handle all the translation,
but I later decided I preferred the flexibility of using the Plover
software to load dictionaries, and handle the translation from
strokes to words.
