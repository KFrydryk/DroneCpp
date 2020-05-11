#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <stdbool.h>
#include "math.h"

#include "comm.h"

// #define ACCELEROMETER_ADDR 0X6B
// #define ACCELEROMETER_X_AXIS 0x28
// #define ACCELEROMETER_Y_AXIS 0x2A
// #define ACCELEROMETER_Z_AXIS 0x2C
// #define GYROSCOPE_X_AXIS 0x22
// #define GYROSCOPE_Y_AXIS 0x24
// #define GYROSCOPE_Z_AXIS 0x26
// #define ACCELEROMETER_SCALE 0.122f
// #define GYROSCOPE_SCALE 35.0f
// #define UNIT_SCALE 1000.0f

// namespace comm
// {

typedef struct
{
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    short val_t;
} Accelerometer_data;

class Accelerometer : I2C_comm
{
public:
    uint8_t addr;
    float aX;
    float aY;
    float aZ;
    float gX;
    float gY;
    float gZ;
    short t;

private:
    enum
    {
        ACCELEROMETER_X_AXIS = 0x28,
        ACCELEROMETER_Y_AXIS = 0x2A,
        ACCELEROMETER_Z_AXIS = 0x2C,
        GYROSCOPE_X_AXIS = 0x22,
        GYROSCOPE_Y_AXIS = 0x24,
        GYROSCOPE_Z_AXIS = 0x26
    };
    float ACCELEROMETER_SCALE = 0.122f,
          GYROSCOPE_SCALE = 35.0f,
          UNIT_SCALE = 1000.0f;

    int64_t acc_lastTick = 0;
    int64_t acc_currentTick = 0;
    Accelerometer_data accelerometer_calib_data = {0, 0, 0, 0, 0, 0, 0};

public:
    Accelerometer(uint8_t addr);
    void Calib();
    uint8_t WhoAmI();
    short ReadAxis(uint8_t axis);
    short ReadTemp();
    Accelerometer_data ReadData();

private:
    void AssignValues(Accelerometer_data data);
};

// } // namespace comm
