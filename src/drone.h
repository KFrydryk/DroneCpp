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
#include "Kalman.h"

typedef struct {
    float X;
    float Y;
    float Z;
} vec3_f;

class drone
{
    uint8_t accelerometer_addr = 0x6B;
    uint8_t magnet_addr = 0x1E;
    uint8_t pressure_addr = 0x5D;

    motor *FR;
    motor *FL;
    motor *RR;
    motor *RL;

    Kalman Kalman_Roll;
    Kalman Kalman_Pitch;
    Kalman Kalman_Yaw;
    Accelerometer* Acc;
    magnet* Mag;
    Pressure* Press;

    double CurrentTime = 0;
    double LastTime = 0;

    bool gyroSet = false;
    
    float Rollintegral = 0;
    double RollRegCurrTime = 0;
    double RollRegLastTime = 0;
    
    float Pitchintegral = 0;
    double PitchRegCurrTime = 0;
    double PitchRegLastTime = 0;

    int PressureReadFrequency = 0;

    double ExecCurrTime = 0;
    double ExecLastTime = 0;
    float LastRollError = 0;
    float RollError = 0;
    float PitchError = 0;
    float LastPitchError = 0;
public:
    vec3_f position = {0, 0, 0};
    vec3_f velocities = {0, 0, 0};
    // float position[3] = {0, 0, 0};
    // float velocities[3] = {0, 0, 0};

    float Roll = 0;
    float Pitch = 0;
    float Yaw = 0;

    float RollRate = 0;
    float PitchRate = 0;
    float YawRate = 0;
    
    float lastRoll = 0;
    float lastPitch = 0;
    float lastYaw = 0;

    float RollP = 0;
    float RollI = 0;
    float RollD = 0;


    drone();

    void CalcState();

    void SetSpeed(int vel1, int vel2, int vel3, int vel4);

    float RollPID(float roll);
    float PitchPID(float pitch);

    vec3_f integrate3_f(vec3_f val, float deltaTime);


};