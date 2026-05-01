#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// IMU pins
#define I2C_PORT i2c0
#define SDA_PIN 8
#define SCL_PIN 9
#define MPU_ADDR 0x68

// Registers
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
#define WHO_AM_I 0x75

void init_imu();
void read_imu(float *accel, float *gyro, float *temp);

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    uint8_t reg = WHO_AM_I;
    uint8_t who_am_i = 0;

    i2c_write_blocking(I2C_PORT, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU_ADDR, &who_am_i, 1, false);

    if (who_am_i != 0x68 && who_am_i != 0x98) {
        while (1) {
            printf("MPU not found\n");
            sleep_ms(1000);
        }
    }

    init_imu();

    float accel[3];
    float gyro[3];
    float temp;

    while (1) {
        read_imu(accel, gyro, &temp);

        // Send only X and Y acceleration to the computer
        printf("%.3f,%.3f\n", accel[0], accel[1]);

        sleep_ms(20); // about 50 Hz
    }
}

void init_imu() {
    uint8_t buf[2];

    buf[0] = PWR_MGMT_1;
    buf[1] = 0x00;
    i2c_write_blocking(I2C_PORT, MPU_ADDR, buf, 2, false);

    buf[0] = ACCEL_CONFIG;
    buf[1] = 0x00; // +/-2g
    i2c_write_blocking(I2C_PORT, MPU_ADDR, buf, 2, false);

    buf[0] = GYRO_CONFIG;
    buf[1] = 0x18; // +/-2000 dps
    i2c_write_blocking(I2C_PORT, MPU_ADDR, buf, 2, false);
}

void read_imu(float *accel, float *gyro, float *temp) {
    uint8_t reg = ACCEL_XOUT_H;
    uint8_t data[14];

    i2c_write_blocking(I2C_PORT, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU_ADDR, data, 14, false);

    int16_t ax = (data[0] << 8) | data[1];
    int16_t ay = (data[2] << 8) | data[3];
    int16_t az = (data[4] << 8) | data[5];

    int16_t temp_raw = (data[6] << 8) | data[7];

    int16_t gx = (data[8] << 8) | data[9];
    int16_t gy = (data[10] << 8) | data[11];
    int16_t gz = (data[12] << 8) | data[13];

    accel[0] = ax * 0.000061;
    accel[1] = ay * 0.000061;
    accel[2] = az * 0.000061;

    gyro[0] = gx * 0.00763;
    gyro[1] = gy * 0.00763;
    gyro[2] = gz * 0.00763;

    *temp = temp_raw / 340.0 + 36.53;
}