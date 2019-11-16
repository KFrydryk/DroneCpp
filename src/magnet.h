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

typedef struct
{
    float val_x;
    float val_y;
    float val_z;
    short val_t;
} Magnet_data;

class magnet : I2C_comm
{
public:
    uint8_t addr;
    float x;
    float y;
    float z;
    short t;
    float Zero = 0;

private:
    enum
    {
        //MAGNET_ADDR = 0x1E, - not used
        MAGNET_X_AXIS = 0x28,
        MAGNET_Y_AXIS = 0x2A,
        MAGNET_Z_AXIS = 0x2C,
    };
    float MAGNET_SCALE = 3421.0f,
          M_X_MAX = 0.456592f,
          M_Y_MAX = 0.073078f,
          M_Z_MAX = 0.091494f,
          M_X_MIN = -0.152002f,
          M_Y_MIN = -0.399298f,
          M_Z_MIN = -0.068109f;

public:
    magnet(uint8_t addr);
    void TurnTempSensor(bool sensorOn);
    short ReadTemp();
    short ReadAxis(uint8_t axis);
    Magnet_data ReadData();
    Magnet_data Calibrate();

private:
    void AssignValues(Magnet_data data);
};