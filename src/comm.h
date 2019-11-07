#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "sys/time.h"
#include "driver/i2c.h"
#include "driver/mcpwm.h"
#include "esp_log.h"
#include <stdbool.h>
#include "math.h"

#define I2C_SCL_IO 19          //19               /*!< gpio number for I2C master clock */
#define I2C_SDA_IO 18          //18               /*!< gpio number for I2C master data  */
#define I2C_FREQ_HZ 100000     /*!< I2C master clock frequency */
#define I2C_PORT_NUM I2C_NUM_1 /*!< I2C port number for master dev */
#define I2C_TX_BUF_DISABLE 0   /*!< I2C master do not need buffer */
#define I2C_RX_BUF_DISABLE 0   /*!< I2C master do not need buffer */

class I2C_comm
{

protected:
    // virtual void init(uint8_t addr) = 0;

    I2C_comm();
    void WriteByte(uint8_t addr, uint8_t sub, uint8_t data);
    uint8_t ReadByte(uint8_t addr, uint8_t sub);

    int64_t GetCurrentTime();
};
// namespace comm

class PWM_comm
{
protected:
    mcpwm_config_t pwm_config;

public:
    PWM_comm();
};