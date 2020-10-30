#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "string.h"
#include "sys.h" 


extern	char DebugBufferCounter, 	DebugBuffer[100];
extern  char USART3_RXBUF[100]; 
extern  uint16_t RxCounter3;	

//DebugÓÃµÄ´®¿ÚUsart1
void DebugUsart_init(u32 bound);
void DebugUsart_SendData(u8 Data);
void DebugUsart_SendStr(char*SendBuf);

void Usart3_init(u32 bound);
void Usart3_SendData(u8 Data);
void Usart3_SendNByte(u8 *Data,u8 length);

void CleanRxBuf3(void);
void CleanDebugBuffer(void);
#endif


