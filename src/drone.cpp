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
}

void drone::SetSpeed(int velFR, int velFL, int velRR, int velRL)
{
    FR->setSpeed(velFR);
    FL->setSpeed(velFL);
    RR->setSpeed(velRR);
    RL->setSpeed(velRL);
}

void drone::CalcPosition()
{

    float Acc_total_vec = 0;
    float Acc_roll;
    float Acc_pitch;
    float Acc_yaw;

    float Magnet_total_vec = 0;
    float Magnet_yaw;

    Acc->ReadData();
    //ExecLastTime = esp_timer_get_time();
    Mag->ReadData();
    //ExecCurrTime = esp_timer_get_time();
    if (PressureReadFrequency++ > 10){ 
    Press->ReadData();
    PressureReadFrequency = 0;
    }
  
    CurrentTime = esp_timer_get_time()/1000;

    Roll += (Acc->gX * (CurrentTime - LastTime) / 1000);

    //calculate gyro readings
    Pitch += (Acc->gY * (CurrentTime - LastTime) / 1000);
    Yaw += (Acc->gZ * (CurrentTime - LastTime) / 1000);
    Roll -= Pitch * sin(Acc->gZ * (CurrentTime - LastTime) / 1000 * M_PI / 180.0);
    Pitch += Roll * sin(Acc->gZ * (CurrentTime - LastTime) / 1000 * M_PI / 180.0);

    //calculate acc readings
    Acc_total_vec = sqrt(pow(Acc->aX, 2) + pow(Acc->aY, 2) + pow(Acc->aZ, 2));
    Acc_roll = asin(Acc->aY / Acc_total_vec) * (180.0 / M_PI);
    Acc_pitch = asin(Acc->aX / Acc_total_vec) * (180.0 / M_PI);
    //Acc_yaw = asin(Acc-aZ/Acc_total_vec)*(180.0/M_PI);

    //calculate magnet readings
    Magnet_total_vec = sqrt(pow(Mag->x, 2) + pow(Mag->y, 2));
    Magnet_yaw = atan2(Mag->y, Mag->x) * (180 / M_PI) - Mag->Zero;

    if (gyroSet)
    {
        Roll = 0.95 * Roll + 0.05 * Acc_roll;
        Pitch = 0.95 * Pitch - 0.05 * Acc_pitch;
        Yaw = 0.995 * Yaw - 0.005 * Magnet_yaw;
    }
    else
    {
        Roll = Acc_roll;
        Pitch = Acc_pitch;
        gyroSet = true;
        Mag->Zero = Magnet_yaw;
    }

    LastTime = CurrentTime;
 
   //printf("Exec time: %f \n", (float)(ExecCurrTime-ExecLastTime)/1000);
 
}

void drone::P_Roll(float roll)
{
    RegCurrTime = esp_timer_get_time()/1000;
    integral += roll * (float)(RegCurrTime - RegLastTime)/1000;

    if (integral > 20)
    {
        integral = 20;
    }
    else if (integral < -20)
    {
        integral = -20;
    }

    float u = RollP * roll + RollI*integral;
    if (u > 30)
    {
        u = 30;
    }
    else if (u < -30)
    {
        u = -30;
    }

    float MIDSPD = 70;
    float Rspd = MIDSPD + u;
    float Lspd = MIDSPD - u;
    RegLastTime = RegCurrTime;
    SetSpeed(Lspd, Rspd, Rspd, Lspd);
    //bl br fr fl
}