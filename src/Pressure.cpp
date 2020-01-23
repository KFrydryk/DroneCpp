#include "Pressure.h"

Pressure::Pressure(uint8_t addr) :
addr(addr)
{
    WriteByte(addr, 0x10, 0x00); //09 0f - 512 internal averages, 00 - 8
    WriteByte(addr, 0x20, 0xC2); //42
    WriteByte(addr, 0x21, 0x10); //00
    WriteByte(addr, 0x2E, 0xC7); //C7-8avgmean
                                 //WriteByte(addr, 0x22, 0x00); //default 0x00
                                 //Magnet_i2c_write_byte(addr, 0x23, 0x08); //0c
}

float Pressure::ReadTemp()
{
    uint8_t regs[2];
    regs[0] = ReadByte(addr, 0x2b);
    regs[1] = ReadByte(addr, 0x2c);
    short raw_temp = (short)((regs[1] << 8) | regs[0]);
    return (float)raw_temp / 480 + 42.5;
}

float Pressure::ReadhPa()
{
    uint8_t regs[3];
    while ((ReadByte(addr, 0x27) & 0x03) == 0)
    {
        vTaskDelay(portTICK_PERIOD_MS);
        //printf("waiting for pressure \n");
    }
    ReadMulti(addr, 0x28|0x80, regs, 3);
    // printf("%x\t%x\t%x\n", regs[0],regs[1],regs[2]);
    //  regs[0] = ReadByte(addr, 0x28);
    //  regs[1] = ReadByte(addr, 0x29);
    //  regs[2] = ReadByte(addr, 0x2A);
    // printf("%x\t%x\t%x\n", regs[0],regs[1],regs[2]);
    int32_t raw_val = (int32_t)(int8_t)regs[2] << 16 | (uint16_t)regs[1] << 8 | regs[0];
    // printf("%f \n", (float)raw_val/4096);
    return (float)raw_val / 4096;
}

float Pressure::ReadAltitude(float pressure_mbar)
{
    //float altimeter_setting_mbar = 1013.25;
    float altimeter_setting_mbar = 1014.706055;
    return (1 - pow(pressure_mbar / altimeter_setting_mbar, 0.190263)) * 44330.8;
}

Pressure_data Pressure::ReadData()
{
    float press = ReadhPa();
    float altitude = ReadAltitude(press);
    float temperature = 0; //ReadTemp();
    Pressure_data data = {altitude, press, temperature};
    AssignValues(data);
    return data;
}

void Pressure::AssignValues(Pressure_data data)
{
    alt = data.alt;
    hPa = data.hPa;
    t = data.t;
}
