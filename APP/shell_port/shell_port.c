#include "shell_port.h"
#include "shell.h"
#include "shell_uart1.h"
#include "shell_adc1.h"
#include "shell_pwm.h"
#include <stdint.h>
#include <string.h>


#define SHELL_BUFFER_SIZE 256
static Shell shell;
static char shell_buffer[SHELL_BUFFER_SIZE];


void shell_init(void)
{
		shell_uart_config(&shell);
		shellInit(&shell, shell_buffer, SHELL_BUFFER_SIZE);
}

void shell_dotask(void)
{
	shellTask(&shell);
}

void cmdfunction_echo(int argc, char* argv[])
{

    for( int i = 1; i < argc; i++)
    {
        shellPrint(shellGetCurrent(),"%s ",argv[i]);
    }
    shellPrint(shellGetCurrent(), "\r\n");

}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) |
    SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    echo,
    cmdfunction_echo,
    echo string
);


void cmdfunction_adc(int argc, char* argv[])
{

	for (int i = 1; i < argc; i++) 
	{
		if (strcmp(argv[i], "get") == 0) 
		{ 
			shell_adc1_get();
		}else
		{
			shellPrint(shellGetCurrent(),"not support");
			break;
		}

}
    shellPrint(shellGetCurrent(), "\r\n");

}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) |
    SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    adc,
    cmdfunction_adc,
    get adc value
);

/*#include "stm32f1xx_hal.h"
void cmdfunction_led(int argc, char* argv[])
{
	if(argc == 2)
	{
		if(strcmp(argv[1],"on") == 0)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		}
		else if(strcmp(argv[1],"off") == 0)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		}
	}
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) |
    SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    led,
    cmdfunction_led,
    led
);
*/
void cmdfunction_pwm(int argc, char* argv[])
{	
	uint8_t i = 1;
	if(argc > 1)
	{
		if (strcmp(argv[i], "on") == 0) 
		{ 
			shell_pwm_on();
		}else if(strcmp(argv[i], "list") == 0)
		{
			shell_pwm_list();
		}
		else if(strcmp(argv[i], "off") == 0)
		{
			if( argc == 2)
			{
				shell_pwm_off(1);
				shell_pwm_off(2);
			}else if(strcmp(argv[i+1], "1") == 0)
			{
				shell_pwm_off(1);
			}else if(strcmp(argv[i+1], "2") == 0)
			{
				shell_pwm_off(2);
			}
		}
		else if (argc >= 2 && strcmp(argv[1], "set") == 0)
		{
			shellPrint(shellGetCurrent(), "argc = %d\r\n. argv[2]:%s, argv[3]:%s", argc, argv[2], argv[3]);
			if (argc == 4)
			{
					shell_pwm_setduty(argv[2],argv[3]);
			}
			else
			{
					shellWriteString(shellGetCurrent(), "Usage: pwm set <1-MAX_CHANNEL_ID> <0-100>\r\n");
			}
		}
		else
		{
			shellPrint(shellGetCurrent(),"not support");
		}

}
    shellPrint(shellGetCurrent(), "\r\n");

}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) |
    SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    pwm,
    cmdfunction_pwm,
    pwm led
);
