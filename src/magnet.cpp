#include "magnet.h"

magnet::magnet(uint8_t addr) :
addr(addr)
{
    WriteByte(addr, 0x20, 0x02); //F0 DC - temp sensor on, hih performance mode, 80HZ, fast mode off, no self test. 02- 1000hz
    WriteByte(addr, 0x21, 0x20); //00
    WriteByte(addr, 0x22, 0x00); //default 0x00
    WriteByte(addr, 0x23, 0x0c); //0c
    WriteByte(addr, 0x24, 0x80); //0x00  
}

void magnet::TurnTempSensor(bool sensorOn)
{
    uint8_t reg = ReadByte(addr, 0x20);
    uint8_t newreg;
    if (sensorOn == true)
    {
        newreg = reg | 0x80;
    }
    else
    {
        newreg = reg & 0x7F;
    }
    WriteByte(addr, 0x20, newreg);
}
short magnet::ReadTemp()
{
    uint8_t regs[2];
    ReadMulti(addr, 0x2e, regs, 2);
    return (((short)((regs[1] << 8) | regs[0])) >> 3) + 25;
}

Magnet_data magnet::ReadData()
{
    //int64_t ExecLastTime = esp_timer_get_time();
    //int64_t ExecCurrTime = 0;

    uint8_t regs[6];
    ReadMulti(addr, MAGNET_X_AXIS, regs, 3);
    short xReading = (((short)((regs[1] << 8) | 0x0))); // regs[0])));
    short yReading = (((short)((regs[2] << 8) | 0x0))); //regs[2])));
    short zReading = (((short)((regs[3] << 8) | 0x0))); //regs[4])));

    float x_axis = (float)xReading / MAGNET_SCALE - (M_X_MAX + M_X_MIN) / 2; // - X_BIAS;
    float y_axis = (float)yReading / MAGNET_SCALE - (M_Y_MAX + M_Y_MIN) / 2; // - Y_BIAS;
    float z_axis = (float)zReading / MAGNET_SCALE - (M_Z_MAX + M_Z_MIN) / 2; // - Z_BIAS;

    short temp = 0;
    Magnet_data values = {x_axis, y_axis, z_axis, temp};

    //ExecCurrTime = esp_timer_get_time();
    //printf("Exec time: %f \n", (float)(ExecCurrTime-ExecLastTime));

    AssignValues(values);
    return values;
}

void magnet::AssignValues(Magnet_data data)
{
    x = data.val_x;
    y = data.val_y;
    z = data.val_z;
    t = data.val_t;
}

Magnet_data magnet::Calibrate()
{
    Magnet_data calib_data = {0, 0, 0, 0};
    calib_data = ReadData();
    return calib_data;
}
