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


//function to draw letter at (x,y) using the ASCII lookup table (pt. 2)
void drawChar(unsigned char x, unsigned char y, char c) {
    int letter = c - 0x20;   // convert ASCII character to font index

    if (letter < 0 || letter > 95) {
        return;   // ignore unsupported characters
    }

    for (int col = 0; col < 5; col++) { // each character is 5 columns wide
        char column_data = ASCII[letter][col];  // get the column data for character

        for (int row = 0; row < 8; row++) { // each column is 8 bits tall
            int bit = (column_data >> row) & 0x01; //for pixel on/off
            ssd1306_drawPixel(x + col, y + row, bit);
        }
    }
}


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
        //blink pico led 
        state = !state;
        gpio_put(LED_PIN, state);

        //-----blink oled pixel (pt. 1)-----
        //ssd1306_clear();
        //ssd1306_drawPixel(10, 10, state);
        //ssd1306_update();

        //-----draw character (pt. 2)-----
        ssd1306_clear();
        drawChar(10, 10, 'A');
        ssd1306_update();

        sleep_ms(500);

        gpio_put(25, 0);

        ssd1306_clear();
        ssd1306_update();

        sleep_ms(500);
    }
}