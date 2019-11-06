#include "comm.h"

// using namespace comm;
// using namespace std;

I2C_comm::I2C_comm()
{
    static bool I2C_initialised = false;
    if (!I2C_initialised)
    {
        I2C_initialised = true;
        i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = (gpio_num_t)I2C_SDA_IO;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = (gpio_num_t)I2C_SCL_IO;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = I2C_FREQ_HZ;
        i2c_param_config(I2C_PORT_NUM, &conf);

        i2c_driver_install(I2C_PORT_NUM, conf.mode, I2C_RX_BUF_DISABLE,
                           I2C_TX_BUF_DISABLE, 0);
    }
}

void I2C_comm::WriteByte(uint8_t addr, uint8_t sub, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, sub, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, data, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 50);
    i2c_cmd_link_delete(cmd);
}

uint8_t I2C_comm::ReadByte(uint8_t addr, uint8_t sub)
{
    uint8_t data = 0;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, sub, I2C_MASTER_ACK);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_read(cmd, &data, sizeof(data), I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 50);
    i2c_cmd_link_delete(cmd);
    return data;
}

int64_t I2C_comm::GetCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}
