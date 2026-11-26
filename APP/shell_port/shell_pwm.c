#include "shell_pwm.h"
#include "pwm.h"
#include "shell.h"


void shell_pwm_setduty(uint8_t duty)
{
	pwm_setduty(2,duty);
	shellPrint(shellGetCurrent(), "pwm set duty %d%%.\n", duty);
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
		shellWriteString(shellGetCurrent(), "\r\n");
	}

}
