#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "sys/time.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <stdbool.h>
#include "math.h"

#include "esp_wifi.h"
#include "nvs_flash.h"

#define SSID "Jezus cie widzi 2.4"
#define PASS "lubieplacki"

void WifiStart();
