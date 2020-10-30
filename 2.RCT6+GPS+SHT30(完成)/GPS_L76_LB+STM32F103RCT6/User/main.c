#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "L76X.h"
#include "SHT30.h"

int main(void)
{

	int t[20];
	u16 h[20];
//	NVIC_Configuration();
	delay_init();
//	Usart3_init(115200);
	DebugUsart_init(115200);
	SHT30_Init();
	
	printf("开始\n");
	
	while(1)
	{
		SHT30_Read_Humiture(t,h);
		printf("温湿度为%f,%f\n",t[0]/10.0,h[0]/10.0);
		delay_ms(1000);
		delay_ms(1000);
		
	}
	
	
#if 0
	GNRMC_number GPS;
	Coordinates GPS_Baidu;
	
	printf("开始\n");
	delay_ms(100);
	
	//设置从微笑采集信息间隔为4s
	L76X_Send_Command(SET_POS_FIX_4S);
	delay_ms(1000);


	//多次发送，防止没收到
	L76X_Send_Command(SET_NMEA_OUTPUT);
	delay_ms(1000);
	L76X_Send_Command(SET_NMEA_OUTPUT);
	delay_ms(1000);

	L76X_Send_Command(SET_NMEA_OUTPUT);
	delay_ms(1000);
	L76X_Send_Command(SET_NMEA_OUTPUT);
	delay_ms(1000);


	
	while(1)
	{
		GPS=L76X_Gat_GNRMC();
		printf("原始经纬度为%lf,%lf\n",GPS.Lon,GPS.Lat);
		delay_ms(1000);	
		
		GPS_Baidu = L76X_Baidu_Coordinates();
		printf("转换为百度经纬度\n%.10lf,%.10lf\n",GPS_Baidu.Lon,GPS_Baidu.Lat);
		
		delay_ms(1000);	

	}
#endif	
	
}
