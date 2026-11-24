#include "shell_port.h"
#include "shell.h"
#include "shell_uart1.h"
#include <stdint.h>

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