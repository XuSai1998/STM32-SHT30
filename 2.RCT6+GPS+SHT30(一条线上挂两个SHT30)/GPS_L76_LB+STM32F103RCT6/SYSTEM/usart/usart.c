#include "sys.h"
#include "usart.h"
#include "string.h"

char DebugBufferCounter, 	DebugBuffer[100]; 
char USART3_RXBUF[100];
uint16_t RxCounter3;
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
		/* 发送一个字节数据到串口 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}
#endif 

void DebugUsart_init(u32 bound)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
		//USART1_TX   GPIOA.9
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
		 
		//USART1_RX	  GPIOA.10初始化
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

		//Usart1 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
		 //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//串口波特率
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx ;	//接收模式

		USART_Init(USART1, &USART_InitStructure); //初始化串口1
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
		USART_Cmd(USART1, ENABLE);                    //使能串口1 
}
void DebugUsart_SendData(u8 Data)
{
		 USART_SendData(USART1,Data);	  //发送一个字节数据
		 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=1);  //等待发送数据寄存器为空
}
void DebugUsart_SendStr(char*SendBuf)//串口1打印数据
{
	while(*SendBuf)
	{
        while((USART1->SR&0X40)==0);//等待发送完成 
        USART1->DR = (u8) *SendBuf; 
        SendBuf++;
	}
}

void USART1_IRQHandler(void)                	//串口1（Debug）中断服务程序
{
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			DebugBuffer[DebugBufferCounter++]=USART_ReceiveData(USART1);		
    } 
} 

void Usart3_init(u32 bound)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART1，GPIOA时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
		
		//USART3_TX   GPIOB.10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.10
		 
		//USART3_RX	  GPIOB.11初始化
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.11

		//Usart3 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
		//USART 初始化设置
		USART_InitStructure.USART_BaudRate = bound;//串口波特率
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

		USART_Init(USART3, &USART_InitStructure); //初始化串口3
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
		USART_Cmd(USART3, ENABLE);
}


void Usart3_SendData(u8 Data)
{
		 USART_SendData(USART3,Data);	  //发送一个字节数据
		 while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=1);  //等待发送数据寄存器为空
}
void Usart3_SendNByte(u8 *Data,u8 length)
{
		u8 i;
    for(i=0;i<length;i++)
    {
        USART_SendData(USART3, Data[i]);
			  //Usart3_SendData(Data[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//?????? 
    }
}
void CleanRxBuf3(void)
{
		memset(USART3_RXBUF,0,100);
		RxCounter3 = 0;
		
}
void CleanDebugBuffer(void)
{
		memset(DebugBuffer,0,100);
		DebugBufferCounter = 0;
		
}




//保证数据是以‘$’开头
void USART3_IRQHandler(void)                	//串口3中断服务程序
{
		uint16_t res;
		
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{	
			/*
			通过处理使每次数组存的都是一组有效，而且每次存的不一样的数据
			如果使用像USART1那样的中断来接数据可能会导致，清除的频率和处理的频率不一致而导致丢数据
			这样子可以不丢数据，而且每次接受的数据都都是新数据      
			*/
				res=USART_ReceiveData(USART3);
				if(res=='$')
				{
					CleanRxBuf3();
				}
				
				USART3_RXBUF[RxCounter3++] = res;
			
		} 
}


