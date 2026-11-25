#ifndef UART1_H
#define UART1_H


void UART1_Init(void);
unsigned char UART1_Write(const unsigned char* c);
unsigned char UART1_Read(void* c);
#endif