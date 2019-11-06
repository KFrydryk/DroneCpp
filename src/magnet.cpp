#include "magnet.h"

magnet::magnet(uint8_t address)
{
    addr = address;
    WriteByte(addr, 0x20, 0xDC); //F0
    WriteByte(addr, 0x21, 0x20); //00
    WriteByte(addr, 0x22, 0x00); //default 0x00
    WriteByte(addr, 0x23, 0x08); //0c
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
    regs[0] = ReadByte(addr, 0x2e);
    regs[1] = ReadByte(addr, 0x2f);
    return (((short)((regs[1] << 8) | regs[0])) >> 3) + 25;
}

short magnet::ReadAxis(uint8_t axis)
{
    uint8_t regs[2];
    regs[0] = ReadByte(addr, axis);
    regs[1] = ReadByte(addr, axis + 1); //&0x80
    return (((short)((regs[1] << 8) | regs[0])));
}

Magnet_data magnet::ReadData()
{
    while ((ReadByte(addr, 0x27) & 0x08) == 0)
    {
        vTaskDelay(portTICK_PERIOD_MS);
    }
    float x_axis = (float)ReadAxis(MAGNET_X_AXIS) / MAGNET_SCALE - (M_X_MAX + M_X_MIN) / 2; // - X_BIAS;
    float y_axis = (float)ReadAxis(MAGNET_Y_AXIS) / MAGNET_SCALE - (M_Y_MAX + M_Y_MIN) / 2; // - Y_BIAS;
    float z_axis = (float)ReadAxis(MAGNET_Z_AXIS) / MAGNET_SCALE - (M_Z_MAX + M_Z_MIN) / 2; // - Z_BIAS;
    short temp = ReadTemp();
    Magnet_data values = {x_axis, y_axis, z_axis, temp};
    AssignValues(values);
    return values;
}

void magnet::AssignValues(Magnet_data data){
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
