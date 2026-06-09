#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// AS5600
#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5
#define AS5600_ADDR 0x36
#define AS5600_ANGLE_H 0x0E

// HX711
#define HX711_DT 16
#define HX711_SCK 17

uint16_t read_as5600_angle() {
    uint8_t reg = AS5600_ANGLE_H;
    uint8_t data[2];

    i2c_write_blocking(I2C_PORT, AS5600_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, AS5600_ADDR, data, 2, false);

    uint16_t angle = ((data[0] & 0x0F) << 8) | data[1];
    return angle; // 0 to 4095
}

int32_t read_hx711() {
    int32_t raw = 0;

    while (gpio_get(HX711_DT) == 1) {
        tight_loop_contents();
    }

    for (int i = 0; i < 24; i++) {
        gpio_put(HX711_SCK, 1);
        sleep_us(1);

        raw = raw << 1;

        gpio_put(HX711_SCK, 0);
        sleep_us(1);

        if (gpio_get(HX711_DT)) {
            raw++;
        }
    }

    // 25th pulse for gain = 128
    gpio_put(HX711_SCK, 1);
    sleep_us(1);
    gpio_put(HX711_SCK, 0);
    sleep_us(1);

    // sign extend 24-bit value
    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }

    return raw;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // I2C setup
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // HX711 setup
    gpio_init(HX711_DT);
    gpio_set_dir(HX711_DT, GPIO_IN);

    gpio_init(HX711_SCK);
    gpio_set_dir(HX711_SCK, GPIO_OUT);
    gpio_put(HX711_SCK, 0);

    printf("time_ms,angle_raw,angle_deg,load_raw\n");

    while (true) {
        uint32_t t = to_ms_since_boot(get_absolute_time());

        uint16_t angle_raw = read_as5600_angle();
        float angle_deg = angle_raw * 360.0f / 4096.0f;

        int32_t load_raw = read_hx711();

        //printf("%lu,%u,%.2f,%ld\n", t, angle_raw, angle_deg, load_raw);
        //printf("%u\n", angle_raw);
        //printf("%u,%ld\n", angle_raw, load_raw);
        printf("%.2f,%ld\n", angle_deg, load_raw);

        sleep_ms(20); // 50 Hz
    }
}