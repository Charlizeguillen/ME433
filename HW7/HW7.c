#include <stdio.h>
#include "pico/stdlib.h"
#include <math.h>
#include "hardware/spi.h"

//add harware spi h file 
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);
void writeDAC(int channel, float voltage);

int main()
{
    stdio_init_all();

    //init the SPI pins and peripherals 
    spi_init(spi_default, 1000 * 1000); // the baud, or bits per second
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    //init the cs pin 
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);   // set CS high

    float t=0;

    //set cs pin high 
    gpio_put(PIN_CS, 1);

    while (true) {
        //for channel A (2hz sine)
        //float voltageA = (sine(2*M_PI*2*t)+1) /2*3.3; // calculate the voltage for channel A (sine wave between 0 and 3.3V)
        float voltageA = 1.65 * (sin(4 * M_PI * t) + 1);

        //for channel B (1hz triangle)
        float period = 1.0;   // 1 second for 1 Hz
        float phase = fmod(t, period) / period; // calculating phase of the triangle wave (0 to 1)

        float voltageB;
        if (phase < 0.5) {
            voltageB = phase * 2.0 * 3.3;
        } else {
            voltageB = 3.3 - 6.6 * (phase - 0.5);
        }

        //calling setDac function to set the voltage for both channels
        writeDac(0, voltageA); // set channel A 
        writeDac(1, voltageB); // set channel B 

        //sleep for 1/100th of a second
        sleep_ms(10);
        t += 0.01; // increment time by 1/100th of a second

    }
}


static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop");
}


void writeDac(int channel, float voltage) {
    if (voltage < 0) voltage = 0;
    if (voltage > 3.3) voltage = 3.3;
    
    uint16_t data[2];

    //converting the voltage to a 10-bit value
    uint16_t theV= (uint16_t)((voltage / 3.3f) * 1023); // 0b11111111111 is 1023 in decimal, which is the maximum value for a 10-bit DAC
    

    data[0]= 0; // control byte for channel A
    data[0]|= (channel <<7 ); // pu the channel in the 
    data[0] |= (0b111 << 4); // set the gain to 1 and shutdown bit to 0
    data[0] |= (theV >> 6);  // the upper 4 bits of the voltage value 

    data[1]= (theV <<2) & 0xFC; //0b11111100 is 0xFC in hexadecimal, which is used to mask the lower 6 bits of the voltage value

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}


///osciloscope 
//pointy end to board and alligator clip (or back probe) to gnd 
//voltage and time sensitvity 
//turn on takes ~2min 