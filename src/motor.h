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

class motor : PWM_comm
{
private:
    mcpwm_unit_t mcpwm_num;
    mcpwm_io_signals_t io_signal;
    mcpwm_timer_t timer_num;

public:
    int gpio_num;
    motor(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, mcpwm_timer_t timer_num, int gpio_num);
    void setSpeed(int speed);
};