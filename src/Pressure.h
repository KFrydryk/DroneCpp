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

typedef struct {
    float alt;
    float hPa;
    float t;
} Pressure_data;


class Pressure : I2C_comm
{
public:
    float alt;
    float hPa;
    float t;
    uint8_t addr;

    Pressure(uint8_t address);
    float ReadTemp();
    float ReadhPa();
    float ReadAltitude(float pressure_mbar);
    Pressure_data ReadData();
    void AssignValues(Pressure_data data);
};