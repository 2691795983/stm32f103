#ifndef SHELL_PWM_H
#define SHELL_PWM_H
#include <stdint.h>

void shell_pwm_setduty(const char* str_channel_id, const char* str_duty);
void shell_pwm_on(void);
void shell_pwm_off(uint8_t id);
void shell_pwm_list(void);


#endif