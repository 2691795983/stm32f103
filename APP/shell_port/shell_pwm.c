#include "shell_pwm.h"
#include "pwm.h"
#include "shell.h"
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

long safe_str_to_long(const char *str, bool *ok) {
    if (!str || *str == '\0') { *ok = false; return 0; }

    char *endptr = NULL;
    errno = 0;
    long val = strtol(str, &endptr, 10);

    // 检查三种失败情况
    if (errno == ERANGE)          { *ok = false; return 0; } // 溢出
    if (endptr == str)            { *ok = false; return 0; } // 无有效数字
    if (*endptr != '\0')          { *ok = false; return 0; } // 尾部有非法字符

    *ok = true;
    return val;
}


void shell_pwm_setduty(const char * str_channel_id,const char * str_duty)
{	
	bool is_ok = false;
	long number_duty = 0;
	long number_id = 0;
	number_duty = safe_str_to_long(str_duty, &is_ok);
	if (!is_ok || number_duty < 0 || number_duty > 100)
	{
			shellWriteString(shellGetCurrent(), "illegal duty value\r\n");
			return;
	}
	
	number_id = safe_str_to_long(str_channel_id, &is_ok);
	if (!is_ok || number_id < 1 || number_id > PWM1_CHANNEL_NUMBER)
	{
			shellWriteString(shellGetCurrent(), "illegal channel id\r\n");
			return;
	}
	pwm_setduty((uint8_t)number_id,(uint8_t)number_duty);
	shellPrint(shellGetCurrent(), "pwm set channel:%ld  duty:%ld%%.\n", number_id,number_duty);
}


void shell_pwm_on(void)
{
	shellWriteString(shellGetCurrent(), "pwm on\r\n");
	pwm_on();
}


void shell_pwm_off(void)
{
	pwm_off();
	shellPrint(shellGetCurrent(), "pwm on\r\n");
}


void shell_pwm_list(void)
{
	channel_t c;
	shellPrint(shellGetCurrent(),"%s...\r\n",pwm_isrunning() ? "pwm is running" : "pwm is not running");
	for(uint8_t i = 1; i <= PWM1_CHANNEL_NUMBER; i++)
	{
		pwm_get_state(i,&c);

		shellPrint(shellGetCurrent(),"id:%d, name:%s, duty:%d%%, frequency:%d, status:",c.channel_id, c.name, c.duty, c.frequency);
		switch(c.state)
		{
			case 0:
				shellWriteString(shellGetCurrent(), "OFF");
				break;
			case 1:
				shellWriteString(shellGetCurrent(), "ON");
				break;
		}
		shellWriteString(shellGetCurrent(), ".\r\n");
	}

}

void shell_pwm_start(const char* str_channel_id)
{
	bool is_ok = false;
	long number_id = 0;
	
	number_id = safe_str_to_long(str_channel_id, &is_ok);
	if (!is_ok || number_id < 0 || number_id > 100)
	{
			shellWriteString(shellGetCurrent(), "illegal channel id value\r\n");
			return;
	}
	
	pwm_start((uint8_t)number_id);
	
	shell_pwm_list();
}
void shell_pwm_stop(const char* str_channel_id)
{
	bool is_ok = false;
	long number_id = 0;
	
	number_id = safe_str_to_long(str_channel_id, &is_ok);
	if (!is_ok || number_id < 0 || number_id > 100)
	{
			shellWriteString(shellGetCurrent(), "illegal channel id value\r\n");
			return;
	}
	
	pwm_stop((uint8_t)number_id);
	
	shell_pwm_list();
}

