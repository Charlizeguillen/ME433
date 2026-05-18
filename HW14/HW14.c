#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#define HX711_DT 16
#define HX711_SCK 17

#define MAX_SAMPLES 2000

int32_t raw_data[MAX_SAMPLES];
float filtered_data[MAX_SAMPLES];
uint32_t time_data[MAX_SAMPLES];

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

    // sign extend 24-bit two's complement
    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }

    return (int32_t)raw;
}

int main() {
    stdio_init_all();

    sleep_ms(3000);
    printf("Program started\n");

    gpio_init(HX711_DT);
    gpio_set_dir(HX711_DT, GPIO_IN);

    gpio_init(HX711_SCK);
    gpio_set_dir(HX711_SCK, GPIO_OUT);
    gpio_put(HX711_SCK, 0);

    printf("GPIO initialized\n");

    int samples = 500;
    printf("Collecting 500 samples...\n");

    float alpha = 0.05;
    float filtered = read_hx711();

    absolute_time_t start = get_absolute_time();

    for (int i = 0; i < samples; i++) {
        printf("Reading samples %d\n", i);
        
        int32_t raw = read_hx711();
        filtered = filtered + alpha * (raw - filtered);
        raw_data[i] = raw;
        filtered_data[i] = filtered;
        time_data[i] = absolute_time_diff_us(start, get_absolute_time()) / 1000;
    }

    printf("time_ms,raw,filtered\n");

    for (int i = 0; i < samples; i++) {
        printf("%lu,%ld,%f\n",
               time_data[i],
               raw_data[i],
               filtered_data[i]);
    }

    while (true) {
        sleep_ms(1000);
    }
}