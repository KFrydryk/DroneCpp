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

uint8_t I2C_comm::ReadMulti(uint8_t addr, uint8_t sub, uint8_t *value, uint8_t num)
{
    static uint8_t data[1];

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, sub, I2C_MASTER_ACK);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
    for (int i = 0; i<num-1; i++){
    i2c_master_read(cmd, &value[i], sizeof(uint8_t), I2C_MASTER_ACK);
    }
    i2c_master_read(cmd, &value[num-1], sizeof(uint8_t), I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 50);
    i2c_cmd_link_delete(cmd);
    return *value;
}

PWM_comm::PWM_comm()
{
   // static bool MotorPWM_initialised = false;

   // if (!MotorPWM_initialised)
   // {
   //     MotorPWM_initialised = true;

        uint32_t angle, count;
        pwm_config.frequency = 200; //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
        pwm_config.cmpr_a = 0;      //duty cycle of PWMxA = 0
        pwm_config.cmpr_b = 0;      //duty cycle of PWMxb = 0
        pwm_config.counter_mode = MCPWM_UP_COUNTER;
        pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  //  }
}
