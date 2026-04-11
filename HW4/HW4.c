#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define I2C_PORT i2c0
#define SDA_PIN 8
#define SCL_PIN 9

#define LED_PIN 25

int main() {
    stdio_init_all();

    // LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // I2C init
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // OLED init
    ssd1306_setup();

    int pixel_state = 0;

    while (1) {
        gpio_put(LED_PIN, pixel_state);

        ssd1306_clear();
        ssd1306_drawPixel(10, 10, pixel_state);
        ssd1306_update();

        pixel_state = !pixel_state;

        sleep_ms(500); // ~1 Hz
    }
}