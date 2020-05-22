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
#include "esp_task_wdt.h"
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
    vTaskDelay(1000/portTICK_PERIOD_MS);
 
    printf("aa");
    recSockStruct gotData;
    sendSockStruct sendData;
    float RollcontrolSignal = 0;
    float PitchcontrolSignal = 0;
    printf("aa");
    while (true)
    {

        Curr_Time = esp_timer_get_time()/1000;
        dron.CalcState();

        gotData = sockSendReceive(sendData, gotData);
        dron.RollP = gotData.P;
        dron.RollI = gotData.I;
        dron.RollD = gotData.D;

        RollcontrolSignal = 0;
        RollcontrolSignal = dron.RollPID(dron.Roll);
        PitchcontrolSignal = dron.RollPID(dron.Pitch);
        float Fz = gotData.Fz;
        
        //dron.SetSpeed(Fz + RollcontrolSignal, Fz - RollcontrolSignal, Fz - RollcontrolSignal, Fz + RollcontrolSignal);
        dron.SetSpeed(Fz + PitchcontrolSignal+ RollcontrolSignal, Fz + PitchcontrolSignal- RollcontrolSignal, Fz - PitchcontrolSignal - RollcontrolSignal, Fz - PitchcontrolSignal + RollcontrolSignal);
        if (Fz <= 0.0005)
        {
            dron.SetSpeed(0, 0, 0, 0);
            RollcontrolSignal = 0;
        }
        sendData = {dron.Roll, dron.Pitch, dron.Yaw, RollcontrolSignal};
        Last_Delay_Time = Curr_Delay_Time;
        Curr_Delay_Time = esp_timer_get_time();
        //printf("time delay = %ld \n", (long)(Curr_Delay_Time-Last_Delay_Time));
        if (Curr_Time - Last_Time > 200)
        {
            vTaskDelay(1);
            //printf("time delay = %f \n", (float)(Curr_Delay_Time-Last_Delay_Time)/1000);
            //printf("cs = %f, P = %f, I = %f, D = %f \n", controlSignal, dron.RollP, dron.RollI, dron.RollD);
                //int64_t time_since_boot = esp_timer_get_time();
                //printf("time: %f \n", (float)(time_since_boot/1000));
            //printf("P: %f, I: %f, D: %f \n", gotData.P, dron.RollI, dron.RollD);

            Last_Time = Curr_Time;

        }

        //bl br fr fl
    }
}