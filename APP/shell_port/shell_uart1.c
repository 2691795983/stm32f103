#include "shell_uart1.h"
#include "shell.h"
#include "uart1.h"




// 最多读取 len 个字节，填入 data，返回实际读取数量。
static signed short shell_uart_read(char * data, unsigned short len)               /**< shell读函数 */
{
	signed short length = 0;
	unsigned char result = 0;

	while(len--)
	{
		result = UART1_Read(data++);
		
		if(result == 0)
		{
				length++;
		}
	}
	
	return length;
		
}


static signed short shell_uart_write(char * data, unsigned short len)              /**< shell写函数 */
{
	signed short length = 0;
	unsigned char result = 0;
	
	while(len--)
	{
		result = UART1_Write((const unsigned char*)data++);
		
		if(result == 0)
		{
				length++;
		}
	}
	
	return length;
}


#define NULLPTR 0
void shell_uart_config(struct shell_def* s)
{
	if( s != NULLPTR)
	{
		s->read = shell_uart_read;
		s->write = shell_uart_write;
	}

}