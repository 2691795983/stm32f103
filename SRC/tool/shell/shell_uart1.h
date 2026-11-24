#ifndef SHELL_UART1_H
#define SHELL_UART1_H

//#define SHELL_GET_TICK() HAL_GetTick()
struct shell_def;

void shell_uart_config(struct shell_def* s);


#endif