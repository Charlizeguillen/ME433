#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C 
#define I2C_PORT i2c0
#define SDA_PIN 8
#define SCL_PIN 9
#define MCP23008_ADDR 0x20

// MCP23008 addresses
#define IODIR 0x00   // 1 = input, 0 = output
#define GPIO  0x09   // read pins
#define OLAT  0x0A   // write outputs


int main()
{
    stdio_init_all(); 
    sleep_ms(2000);

    // LED Initialisation
    #ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    #endif

    //init I2CO at 100kHz
    i2c_init(I2C_PORT, 100*1000);
    
    //setting GP8 and GP9 to I2C mode
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    uint8_t buf[2];
    buf[0] = IODIR;
    buf[1] = 0x7F;
    i2c_write_blocking(I2C_PORT, MCP23008_ADDR, buf, 2, false);

    // Start with GP7 led off
    buf[0] = OLAT;
    buf[1] = 0x00;
    i2c_write_blocking(I2C_PORT, MCP23008_ADDR, buf, 2, false);

    
    while (true) {
        uint8_t reg = GPIO;
        uint8_t gpio_val = 0;
        uint8_t out_val = 0;

        //want to read GPIO register to get the state of the button
        i2c_write_blocking(I2C_PORT, MCP23008_ADDR, &reg, 1, true);

        //read the GPIO register 
        i2c_read_blocking(I2C_PORT, MCP23008_ADDR, &gpio_val, 1, false);

        if ((gpio_val & 0x01) == 0) {
            //turn GP7 led on (button pressed)
            out_val = 0x80;
        } else {
            // turn GP7 led off (button pressed)
            out_val = 0x00;
        }
        
        // Write the output value to the OLAT register
        buf[0] = OLAT;
        buf[1] = out_val;
        i2c_write_blocking(I2C_PORT, MCP23008_ADDR, buf, 2, false);

        // "Heartbeat" for led on pico
        #ifdef PICO_DEFAULT_LED_PIN
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(100);
        #else
        sleep_ms(200);  
        #endif
    }
    return 0;
}
