#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
//#include "hardware/adc.h"
#include "pico/time.h"
#include "ssd1306.h"
#include "font.h"


#define LED_PIN 25
#define SDA_PIN 4
#define SCL_PIN 5


int main() {
    stdio_init_all();

    // setting pico led
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // setting I2C for oled
    i2c_init(i2c_default, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // OLED init
    ssd1306_setup();

    int state = 0;

    while (1) {
        state = !state;

        //blink pico led
        gpio_put(LED_PIN, state);

        //blink oled pixel
        ssd1306_clear();
        ssd1306_drawPixel(10, 10, state);
        ssd1306_update();

        sleep_ms(500); // ~1 Hz
    }
}