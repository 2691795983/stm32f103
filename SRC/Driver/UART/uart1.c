#include "uart1.h"
#include "stm32f1xx_hal.h"
#include "lwrb/lwrb.h"
#include <stdint.h>
static uint8_t Rxbyte = 0;
lwrb_t rx_ring;
static uint8_t rx_buffer_data[256];


UART_HandleTypeDef huart1;

void UART1_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
		while(1){}
  }
	
	lwrb_init(&rx_ring, rx_buffer_data, sizeof(rx_buffer_data));
	HAL_UART_Receive_IT(&huart1,&Rxbyte,1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		lwrb_write(&rx_ring, &Rxbyte, 1);
		HAL_UART_Receive_IT(&huart1, &Rxbyte,1); 
	}
}

unsigned char UART1_Write(const unsigned char* c)
{
	if(c != NULL)
	{
			return HAL_UART_Transmit(&huart1, c, 1, 10);
	}
	return 1; // HAL_ERROR
}

unsigned char UART1_Read(void* c)
{
	if(c != NULL)
	{
			if(lwrb_read(&rx_ring, c, 1) == 1)
			{
				return 0;
			}
	}
	return 1; // HAL_ERROR
}


