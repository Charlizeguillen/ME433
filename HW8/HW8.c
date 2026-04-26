#include <stdio.h>
#include "pico/stdlib.h"
#include <math.h>
#include "hardware/spi.h"

//add harware spi h file 
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS_DAC   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_CS_RAM 5

//control byte for RAM 
#define RAM_WRITE 0b00000010
#define RAM_READ  0b00000011
#define RAM_WRSR  0b00000001

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);

void spi_ramInit();
void spi_writeRam(uint16_t address, uint8_t *data, size_t len);
void spi_readRam(uint16_t address, uint8_t *data, size_t len);


int main()
{
    stdio_init_all();

    //init the SPI pins and peripherals 
    spi_init(SPI_PORT, 1000 * 1000); // the baud, or bits per second
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    //init the cs pin 
    gpio_init(PIN_CS_DAC);
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1);   // set CS high

    //init the cs pin for RAM
    gpio_init(PIN_CS_RAM);
    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    gpio_put(PIN_CS_RAM, 1);   // set CS high   

    spi_ramInit();

    //set cs pin high 
    gpio_put(PIN_CS_DAC, 1);

    for (int i = 0; i < 1000; i++) {
        float voltage = 1.65f * (sin(2.0f * M_PI * i / 1000.0f) + 1.0f);

        uint8_t data[2];

        uint16_t theV = (uint16_t)((voltage / 3.3f) * 1023);

        data[0] = 0;
        data[0] |= (0b111 << 4);
        data[0] |= (theV >> 6);

        data[1] = (theV << 2) & 0xFC;

        spi_writeRam(2 * i, data, 2);
    }

    while (true) {
        for (int i = 0; i < 1000; i++) {
        uint8_t data[2];

        spi_readRam(2 * i, data, 2);

        cs_select(PIN_CS_DAC);
        spi_write_blocking(SPI_PORT, data, 2);
        cs_deselect(PIN_CS_DAC);

        sleep_ms(1);

        }
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

void spi_ramInit() {
    uint8_t data[2];

    data[0] = RAM_WRSR;
    data[1] = 0b01000000;   // sequential mode

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, data, 2);
    cs_deselect(PIN_CS_RAM);
}

void spi_writeRam(uint16_t address, uint8_t *data, size_t len) {
    uint8_t cmd[3];

    cmd[0] = RAM_WRITE;
    cmd[1] = address >> 8;
    cmd[2] = address & 0xFF;

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, cmd, 3);
    spi_write_blocking(SPI_PORT, data, len);
    cs_deselect(PIN_CS_RAM);
}

void spi_readRam(uint16_t address, uint8_t *data, size_t len) {
    uint8_t cmd[3];

    cmd[0] = RAM_READ;
    cmd[1] = address >> 8;
    cmd[2] = address & 0xFF;

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, cmd, 3);
    spi_read_blocking(SPI_PORT, 0, data, len);
    cs_deselect(PIN_CS_RAM);
}