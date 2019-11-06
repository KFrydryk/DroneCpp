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

#include "Accelerometer.h"
#include "magnet.h"
#include "Pressure.h"

#define ACCELEROMETER_ADDR  0x6B
#define MAGNET_ADDR         0x1E
#define PRESSURE_ADDR		0x5D

using namespace std;

extern "C"
{
    void app_main(void);
}

void app_main(void)
{
    printf("zaczynam");

    Accelerometer Acc(ACCELEROMETER_ADDR);
      printf("zaczynam2 \n");
    magnet Mag(MAGNET_ADDR);
      printf("zaczynam3 \n");
    Pressure Press(PRESSURE_ADDR);
    printf("zaczynam4 \n");

    Mag.TurnTempSensor(true);
    printf("wchodze do while");
    while (true)
    {
        Acc.ReadData();
        Mag.ReadData();
        Press.ReadData();
        printf("accX: %f magX: %f, pres: %f \n", Acc.aX, Mag.x, Press.alt);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}