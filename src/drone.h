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

#include "motor.h"
#include "Accelerometer.h"
#include "magnet.h"
#include "Pressure.h"


class drone{
motor FR;
motor FL;
motor RR;
motor RL;

drone();
};