#include "Accelerometer.h"

// using namespace comm;
// using namespace std;

uint8_t Accelerometer::WhoAmI()
{
    return ReadByte(addr, 0x0F); //0x0f
}

Accelerometer::Accelerometer(uint8_t addres)
{
    addr = addres;
    WriteByte(addr, 0x10, 0x68); //0x80 88
    //	uint8_t data = ReadByte(ACCELEROMETER_ADDR, 0x10);
    //	printf("data: %02x", data);
    WriteByte(addr, 0x11, 0x68); //0x80 88
    WriteByte(addr, 0x12, 0x04); //0x04
    WriteByte(addr, 0x13, 0x00); //0x00 80
    WriteByte(addr, 0x14, 0x00); //0x00
    WriteByte(addr, 0x15, 0x00); //0x00
    WriteByte(addr, 0x16, 0x00); //0x00 70
    WriteByte(addr, 0x17, 0x00); //0x00
    WriteByte(addr, 0x18, 0x38); //0x38
    WriteByte(addr, 0x19, 0x38); //0x38

    Calib();
}

void Accelerometer::Calib()
{
    Accelerometer_data read_data = ReadData();
    int tests = 2000;
    for (int i = 0; i < tests; i++)
    {
        accelerometer_calib_data.acc_x += read_data.acc_x;
        accelerometer_calib_data.acc_y += read_data.acc_y;
        accelerometer_calib_data.acc_z += read_data.acc_z;
        accelerometer_calib_data.gyro_x += read_data.gyro_x;
        accelerometer_calib_data.gyro_y += read_data.gyro_y;
        accelerometer_calib_data.gyro_z += read_data.gyro_z;
        vTaskDelay(3 / portTICK_PERIOD_MS);
    }
    accelerometer_calib_data.acc_x /= tests;
    accelerometer_calib_data.acc_y /= tests;
    accelerometer_calib_data.acc_z /= tests;
    accelerometer_calib_data.gyro_x /= tests;
    accelerometer_calib_data.gyro_y /= tests;
    accelerometer_calib_data.gyro_z /= tests;
    accelerometer_calib_data.val_t = 0;
    //	return accelerometer_calib_data;
}

short Accelerometer::ReadAxis(uint8_t axis)
{
    uint8_t regs[2];
    regs[0] = ReadByte(addr, axis);
    regs[1] = ReadByte(addr, axis + 1); //&0x80
    return (((short)((regs[1] << 8) | regs[0])));
}

short Accelerometer::ReadTemp()
{
    uint8_t regs[2];
    regs[0] = ReadByte(addr, 0x20);
    regs[1] = ReadByte(addr, 0x21);
    return (((short)((regs[1] << 8) | regs[0])) >> 4) + 0;
}

Accelerometer_data Accelerometer::ReadData()
{
    acc_lastTick = acc_currentTick;
    acc_currentTick = GetCurrentTime();
    while ((ReadByte(addr, 0x1E) & 0x07) == 0)
    {
        //uint8_t acc_name = Accelerometer_who_am_i(ACCELEROMETER_ADDR);
        //printf("%08x", acc_name);
        vTaskDelay(portTICK_PERIOD_MS);
    }
    float accel_x_axis = (float)ReadAxis(ACCELEROMETER_X_AXIS) * ACCELEROMETER_SCALE / UNIT_SCALE;                          // - X_BIAS;
    float accel_y_axis = (float)ReadAxis(ACCELEROMETER_Y_AXIS) * ACCELEROMETER_SCALE / UNIT_SCALE;                          // - Y_BIAS;
    float accel_z_axis = (float)ReadAxis(ACCELEROMETER_Z_AXIS) * ACCELEROMETER_SCALE / UNIT_SCALE;                          // - Z_BIAS;
    float gyro_x_axis = (float)ReadAxis(GYROSCOPE_X_AXIS) * GYROSCOPE_SCALE / UNIT_SCALE - accelerometer_calib_data.gyro_x; // - Z_BIAS;
    float gyro_y_axis = (float)ReadAxis(GYROSCOPE_Y_AXIS) * GYROSCOPE_SCALE / UNIT_SCALE - accelerometer_calib_data.gyro_y; // - Z_BIAS;
    float gyro_z_axis = (float)ReadAxis(GYROSCOPE_Z_AXIS) * GYROSCOPE_SCALE / UNIT_SCALE - accelerometer_calib_data.gyro_z; // - Z_BIAS;
    short temp = ReadTemp();
    Accelerometer_data values = {accel_x_axis, accel_y_axis, accel_z_axis, gyro_x_axis, gyro_y_axis, gyro_z_axis, temp};
    AssignValues(values);
    return values;
}

void Accelerometer::AssignValues(Accelerometer_data data)
{
    aX = data.acc_x;
    aY = data.acc_y;
    aZ = data.acc_z;
    gX = data.gyro_x;
    gY = data.gyro_y;
    gZ = data.gyro_z;
    t = data.val_t;
}