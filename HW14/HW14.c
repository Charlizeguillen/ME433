#include <stdio.h>
#include "pico/stdlib.h"

#define HX711_DT 16
#define HX711_SCK 17

int32_t read_hx711() {
    uint32_t raw = 0;

    // wait until data is ready
    while (gpio_get(HX711_DT) == 1) {
        tight_loop_contents();
    }

    // read 24 bits
    for (int i = 0; i < 24; i++) {
        gpio_put(HX711_SCK, 1);
        sleep_us(1);

        raw = raw << 1;
        if (gpio_get(HX711_DT)) {
            raw |= 1;
        }

        gpio_put(HX711_SCK, 0);
        sleep_us(1);
    }

    // 25th pulse sets gain to 128
    gpio_put(HX711_SCK, 1);
    sleep_us(1);
    gpio_put(HX711_SCK, 0);
    sleep_us(1);

    // sign extend 24-bit two's complement
    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }

    return (int32_t)raw;
}

int main() {
    stdio_init_all();

    gpio_init(HX711_DT);
    gpio_set_dir(HX711_DT, GPIO_IN);

    gpio_init(HX711_SCK);
    gpio_set_dir(HX711_SCK, GPIO_OUT);
    gpio_put(HX711_SCK, 0);

    sleep_ms(2000);
    printf("HX711 ready\n");

    while (true) {
        int32_t raw = read_hx711();
        printf("%ld\n", raw);
        sleep_ms(10);
    }
}
