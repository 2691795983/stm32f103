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


void shell_pwm_setduty(const char * str_duty)
{	
	bool is_ok = false;
	long number_result = 0;
	number_result = safe_str_to_long(str_duty, &is_ok);
	if(number_result >= 0 && number_result <= 100)
	{
		pwm_setduty(2,(uint8_t)number_result);
		shellPrint(shellGetCurrent(), "pwm set duty %d%%.\n", number_result);
	}
	shellWriteString(shellGetCurrent(), "illegal duty value");
}


void shell_pwm_on(void)
{
	shellWriteString(shellGetCurrent(), "pwm on\n");
	pwm_start();

}


void shell_pwm_off(uint8_t id)
{
	
	pwm_stop(id);
	shellPrint(shellGetCurrent(), "id:%d already turn off\r\n", id);
}


void shell_pwm_list(void)
{
	channel_t c;
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
