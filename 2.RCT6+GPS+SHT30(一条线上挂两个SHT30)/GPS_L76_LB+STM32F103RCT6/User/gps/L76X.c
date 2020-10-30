#include "L76X.h"
#include "usart.h"
#include "delay.h"
#include "math.h"

char const Temp[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static const double pi = 3.14159265358979324;
static const double a = 6378245.0;
static const double ee = 0.00669342162296594323;
static const double x_pi = 3.14159265358979324 * 3000.0 / 180.0;

//char buff_t[BUFFSIZE];


GNRMC_number GPS_number;


static double transformLat(double x,double y)
{
	double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 *sqrt(abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
    return ret;
}

/******************************************************************************
function:	
	Longitude conversion
******************************************************************************/
static double transformLon(double x,double y)
{
	double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs(x));
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
    return ret;
}

/******************************************************************************
function:	
	GCJ-02 international standard converted to Baidu map BD-09 standard
******************************************************************************/
static Coordinates bd_encrypt(Coordinates gg)
{
	Coordinates bd;
    double x = gg.Lon, y = gg.Lat;
	double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);
	double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);
	bd.Lon = z * cos(theta) + 0.0065;
	bd.Lat = z * sin(theta) + 0.006;
	return bd;
}

/******************************************************************************
function:	
	GPS's WGS-84 standard is converted into GCJ-02 international standard
******************************************************************************/
static Coordinates transform(Coordinates gps)
{
	Coordinates gg;
    double dLat = transformLat(gps.Lon - 105.0, gps.Lat - 35.0);
    double dLon = transformLon(gps.Lon - 105.0, gps.Lat - 35.0);
    double radLat = gps.Lat / 180.0 * pi;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
    gg.Lat = gps.Lat + dLat;
    gg.Lon = gps.Lon + dLon;
	return gg;
}


/******************************************************************************
function:	
	Convert GPS latitude and longitude into Baidu map coordinates
******************************************************************************/
Coordinates L76X_Baidu_Coordinates()
{
    Coordinates temp;
    temp.Lat =GPS_number.Lat;
		temp.Lon =GPS_number.Lon;
    temp = transform(temp);
    temp = bd_encrypt(temp);
    return temp;
}



/******************************************************************************
function:	
	向L76发送指令，自动转化成$ +..+检验位发送出去
parameter:
    data ：The end of the command ends with ‘\0’ or it will go wrong, 
           no need to increase the validation code.
******************************************************************************/
void L76X_Send_Command(char *data)
{
    char Check = data[1], Check_char[3]={0};
    uint8_t i = 0;
    //printf(" 1i = %d Check =%x \n", i, Check);
    for(i=2; data[i] != '\0'; i++){
        Check ^= data[i];       //Calculate the check value
    }
    //printf(" i = %d Check =%x \n", i, Check);
    Check_char[0] = Temp[Check/16%16];
    Check_char[1] = Temp[Check%16];
		Check_char[2] = '\0';
    
    Usart3_SendNByte(data,strlen(data));
    Usart3_SendData('*');
    Usart3_SendNByte(Check_char,strlen(Check_char));
    Usart3_SendData('\r');
    Usart3_SendData('\n');
    delay_ms(200);
}


/******************************************************************************
function:	
自动分析GNRMC数据，转换为经纬度和时间
******************************************************************************/
GNRMC_number L76X_Gat_GNRMC()
{
		char *strx,*buff_t;

    uint16_t add = 0, x = 0, z = 0, i = 0;
  


		GPS_number.Time_H = 0;
    GPS_number.Time_M = 0;
    GPS_number.Time_S = 0;
	
  
   for(i = 0; USART3_RXBUF[i]!=0; i++){
       buff_t[i] = USART3_RXBUF[i];
   }
   buff_t[i]='\0';
	
    printf("原始数据为%s\n",buff_t);
	
		strx=strstr((const char*)buff_t,(const char*)"A,");//返回A，表明经纬度数据被正确获取了
		if(strx)
		{
			
				//<1>UTC国际时间				
				GPS_number.Time_H=(buff_t[7]-'0')*10+(buff_t[8]-'0');
				GPS_number.Time_M=(buff_t[9]-'0')*10+(buff_t[10]-'0');
				GPS_number.Time_S=(buff_t[11]-'0')*10+(buff_t[12]-'0');
				
				printf("时间是 %d:%d:%d\n",GPS_number.Time_H,GPS_number.Time_M,GPS_number.Time_S);
				
				//<3>纬度buff_t的A后面地三个字符开始
				//6是‘.’
				GPS_number.Lat=(strx[2]-'0')*10+(strx[3]-'0')+\
				((strx[4]-'0')*10+(strx[5]-'0')+(strx[7]-'0')/10.0+(strx[8]-'0')/100.0+(strx[9]-'0')/1000.0\
				+(strx[10]-'0')/10000.0+(strx[11]-'0')/100000.0+(strx[12]-'0')/1000000.0)/60.0;
			
				strx=strstr((const char*)buff_t,(const char*)"N,");//获取经度
				GPS_number.Lon=(strx[2]-'0')*100+(strx[3]-'0')*10+(strx[4]-'0')+\
				((strx[5]-'0')*10+(strx[6]-'0')+(strx[8]-'0')/10.0+(strx[9]-'0')/100.0+(strx[10]-'0')/1000.0\
				+(strx[11]-'0')/10000.0+(strx[12]-'0')/100000.0+(strx[13]-'0')/1000000.0)/60.0;
			
			
			//	printf("经纬度为%lf,%lf\n",GPS_number.Lon,GPS_number.Lat);

		}else printf("\n无效数据，重新获取\n");
	
	

		return GPS_number;
		
}

//默认L76打印出来的只是某个标准，还需要转换百度标准


