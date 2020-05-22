#include "drone.h"

drone::drone()
{

    FR = new motor(MCPWM_UNIT_0, MCPWM0A, MCPWM_TIMER_0, 32);
    FL = new motor(MCPWM_UNIT_0, MCPWM1A, MCPWM_TIMER_1, 33);
    RR = new motor(MCPWM_UNIT_0, MCPWM2A, MCPWM_TIMER_2, 25);
    RL = new motor(MCPWM_UNIT_1, MCPWM0A, MCPWM_TIMER_0, 26);
    Acc = new Accelerometer(accelerometer_addr);
    Mag = new magnet(magnet_addr);
    Press = new Pressure(pressure_addr);

    Mag->TurnTempSensor(true);

    Kalman_Roll.setAngle(0);
    Kalman_Pitch.setAngle(0);
    Kalman_Yaw.setAngle(0);
}

void drone::SetSpeed(int velFR, int velFL, int velRR, int velRL)
{
    FR->setSpeed(velFR);
    FL->setSpeed(velFL);
    RR->setSpeed(velRR);
    RL->setSpeed(velRL);
}

void drone::CalcState()
{
    float Acc_total_vec = 0;
    float Acc_roll;
    float Acc_pitch;
    float Acc_yaw;
    float Magnet_yaw;

    CurrentTime = esp_timer_get_time();
    Acc->ReadData();
    Mag->ReadData();

    Acc_roll = atan2f(Acc->aY, Acc->aZ) * (180.0F / M_PI);
    Acc_pitch = atanf(-Acc->aX / sqrt(Acc->aY * Acc->aY + Acc->aZ * Acc->aZ)) * (180.0F / M_PI);

    Magnet_yaw = atan2(Mag->y, Mag->x) * (180 / M_PI) - Mag->Zero;

    float timeDiff = (float)(CurrentTime - LastTime) / 1000000.0F;

    Roll = Kalman_Roll.getAngle(Acc_roll, Acc->gX, timeDiff);
    Pitch = Kalman_Pitch.getAngle(Acc_pitch, Acc->gY, timeDiff);
    Yaw = Kalman_Yaw.getAngle(Magnet_yaw, Acc->gZ, timeDiff);

    RollRate = (Roll - lastRoll) / timeDiff;
    PitchRate = (Pitch - lastPitch) / timeDiff;
    YawRate = (Yaw - lastYaw) / timeDiff;

    velocities = integrate3_f((vec3_f){Acc->aX, Acc->aY, Acc->aZ}, timeDiff);
    position = integrate3_f(velocities, timeDiff);

    lastRoll = Roll;
    lastYaw = Yaw;
    lastPitch = Pitch;
    LastTime = CurrentTime;
}

float drone::RollPID(float roll)
{
    RollRegCurrTime = esp_timer_get_time() / 1000;
    LastRollError = RollError;
    RollError = (0 - roll);
    Rollintegral += RollI * RollError * (float)(RollRegCurrTime - RollRegLastTime) / 1000;
    RollRegLastTime = RollRegCurrTime;

    if (Rollintegral > 700)
    {
        Rollintegral = 700;
    }
    else if (Rollintegral < -700)
    {
        Rollintegral = -700;
    }
    //float u = sqrt(RollP * RollError +integral+RollD*(RollError-LastRollError)*(RegCurrTime-RegLastTime)/1000);
    float u = RollP * RollError + Rollintegral; //+RollD*(RollError-LastRollError)*(RegCurrTime-RegLastTime)/1000;
    //printf("%f \n", u);
    // if (u<1)
    // {u = 1;}
    // u=sqrt(u);
    // if (u>100)
    // {u=100;}
    //SetSpeed(u, 0, 0, u);
    return u;
    // if (u > 30)
    // {
    //     u = 30;
    // }
    // else if (u < -30)
    // {
    //     u = -30;
    // }

    // float MIDSPD = 70;
    // float Rspd = MIDSPD + u;
    // float Lspd = MIDSPD - u;

    // SetSpeed(Lspd, Rspd, Rspd, Lspd);
    //bl br fr fl
}

float drone::PitchPID(float pitch)
{
    PitchRegCurrTime = esp_timer_get_time() / 1000;
    LastPitchError = PitchError;
    PitchError = (0 - pitch);
    Pitchintegral += RollI * PitchError * (float)(PitchRegCurrTime - PitchRegLastTime) / 1000;
    PitchRegLastTime = PitchRegCurrTime;

    if (Pitchintegral > 700)
    {
        Pitchintegral = 700;
    }
    else if (Pitchintegral < -700)
    {
        Pitchintegral = -700;
    }
    float u = RollP * PitchError + Pitchintegral;

    return u;
}

vec3_f integrate3_f(vec3_f val, float deltaTime)
{
    vec3_f iVals;
    iVals.X = val.X * deltaTime;
    iVals.Y = val.Y * deltaTime;
    iVals.Z = val.Z * deltaTime;
    return iVals;
}