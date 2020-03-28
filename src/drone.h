#pragma once

#include "math.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <stdbool.h>
#include "math.h"
#include "esp_timer.h"


#include "utils.h"
#include "motor.h"
#include "Accelerometer.h"
#include "magnet.h"
#include "Pressure.h"
#include "comm.h"



class drone
{
    uint8_t accelerometer_addr = 0x6B;
    uint8_t magnet_addr = 0x1E;
    uint8_t pressure_addr = 0x5D;

    motor *FR;
    motor *FL;
    motor *RR;
    motor *RL;

    Accelerometer* Acc;
    magnet* Mag;
    Pressure* Press;

    double CurrentTime = 0;
    double LastTime = 0;

    bool gyroSet = false;
    
    float integral = 0;
    double RegCurrTime = 0;
    double RegLastTime = 0;

    int PressureReadFrequency = 0;

    double ExecCurrTime = 0;
    double ExecLastTime = 0;
    float LastRollError = 0;
    float RollError = 0;
public:
    
    float Roll = 0;
    float Pitch = 0;
    float Yaw = 0;

    float lastRoll = 0;
    float lastPitch = 0;
    float lastYaw = 0;

    float RollP = 0;
    float RollI = 0;
    float RollD = 0;

    drone();

    void CalcPosition();

    void SetSpeed(int vel1, int vel2, int vel3, int vel4);

    float P_Roll(float roll);
};