#include "motor.h"

motor::motor(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, mcpwm_timer_t timer_num, int gpio_num) :
 mcpwm_num(mcpwm_num), io_signal(io_signal), timer_num(timer_num),gpio_num(gpio_num)
{

    mcpwm_gpio_init(mcpwm_num, io_signal, gpio_num); //Set GPIO 18 as PWM0A, to which servo is connected
    mcpwm_init(mcpwm_num, timer_num, &pwm_config);   //Configure PWM0A & PWM0B with above settings
}
void motor::setSpeed(int speed)
{
        mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, speed);

}//aa