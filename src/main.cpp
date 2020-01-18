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
#include "esp_timer.h"

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
    int64_t Curr_Delay_Time = 0;
    int64_t Last_Delay_Time = 0;

    drone dron;

    WifiStart();
    printf("costamblabla \n");
    startSocket();
    recSockStruct gotData;
    sendSockStruct sendData;
    while (true)
    {
        //printf("%f, %f, %f \n", gotData.P, gotData.I, gotData.D);
        Curr_Time = esp_timer_get_time()/1000;
        dron.CalcPosition();

        //vTaskDelay(1000 / portTICK_PERIOD_MS);
        // aaa = mcpwm_get_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A);
        // printf("duty %f \n", aaa);
        sendData = {dron.Roll, dron.Pitch, dron.Yaw};
        gotData = sockSendReceive(sendData);
        dron.RollP = gotData.P;
        dron.RollI = gotData.I;
        dron.RollD = gotData.D;

        dron.P_Roll(dron.Roll);
        if (dron.RollP == 0)
        {
            dron.SetSpeed(0, 0, 0, 0);
        }
        Last_Delay_Time = Curr_Time;
        Curr_Delay_Time = esp_timer_get_time()/1000;
        //printf("time delay = %ld \n", (long)(Curr_Delay_Time-Last_Delay_Time));
        if (Curr_Time - Last_Time > 500)
        {
                //int64_t time_since_boot = esp_timer_get_time();
                //printf("time: %f \n", (float)(time_since_boot/1000));
            //printf("P: %f, I: %f, D: %f \n", gotData.P, dron.RollI, dron.RollD);

            Last_Time = Curr_Time;
        }

        //bl br fr fl
    }
}