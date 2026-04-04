#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"


#define SERVO_PIN 16

//function to set angle
void set_servo_angle(uint slice_num, uint channel, int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    //converting angle to pulse width 
    uint16_t pulse_width = (angle * 1000 / 180) + 1000; 
    pwm_set_chan_level(slice_num, channel, pulse_width);
}


int main()
{
    stdio_init_all();

    // Setting the servo to PWM mode
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

    // init PWM for the servo pin
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    uint channel = pwm_gpio_to_channel(SERVO_PIN);

    pwm_set_clkdiv(slice_num, 125.0); 
    pwm_set_wrap(slice_num, 19999); // 20ms period = 20000us so wrap at 19999 (0 to 19999 counts)
    pwm_set_enabled(slice_num, true);

    while (1) {
        for (int angle = 0; angle <= 180; angle += 10) {
            // Map angle to pulse width (1ms to 2ms)
            //so 0 deg -> 1000 us and 180 deg -> 2000 us
            set_servo_angle(slice_num, channel, angle);
            sleep_ms(50); // Wait for the servo to move
        }


        // Move back from 180 to 0 degrees
        for (int angle = 180; angle >= 0; angle -= 10) {
            set_servo_angle(slice_num, channel, angle);
            sleep_ms(50); 
        }
    }
}
