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
#include "drone.h"
#include "utils.h"
#include "Wifi.h"

// #define ACCELEROMETER_ADDR  0x6B
// #define MAGNET_ADDR         0x1E
// #define PRESSURE_ADDR		0x5D

using namespace std;

extern "C"
{
    void app_main(void);
}

void printtask(void *parameters)
{
    printf("jestem w tasku");
    vTaskDelete(NULL);
}

void app_main(void)
{

    printf("zaczynam");

    //xTaskCreate(printtask, "printtask", 1000, NULL, 1, NULL);

    int64_t Curr_Time = 0;
    int64_t Last_Time = 0;

    drone dron;

    WifiStart();
    printf("costamblabla \n");
    startSocket();
    float aaa;
    while (true)
    {
        Curr_Time = GetCurrentTime();
        dron.CalcPosition();

        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        dron.SetSpeed(0, 0, 0, 0);
        // aaa = mcpwm_get_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A);
        // printf("duty %f \n", aaa);
        //dron.P_Roll(dron.Roll);

        // if (Curr_Time - Last_Time > 500)
        // {
        //   printf("R: %f, P: %f, Y: %f \n", dron.Roll, dron.Pitch, dron.Yaw);
        //   Last_Time = Curr_Time;
        // }

        //bl br fr fl
    }
}