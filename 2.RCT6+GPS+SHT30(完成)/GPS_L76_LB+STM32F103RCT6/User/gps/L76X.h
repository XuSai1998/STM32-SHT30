#ifndef _L76X_H_
#define _L76X_H_

#include <math.h>
#include <stdlib.h>
#include "stm32f10x.h"

#define BUFFSIZE 100

//Startup mode
#define HOT_START       "$PMTK101"
#define WARM_START      "$PMTK102"
#define COLD_START      "$PMTK103"
#define FULL_COLD_START "$PMTK104"

//Standby mode -- Exit requires high level trigger
#define SET_PERPETUAL_STANDBY_MODE      "$PMTK161"

#define SET_PERIODIC_MODE               "$PMTK225"
#define SET_NORMAL_MODE                 "$PMTK225,0"
#define SET_PERIODIC_BACKUP_MODE        "$PMTK225,1,1000,2000"
#define SET_PERIODIC_STANDBY_MODE       "$PMTK225,2,1000,2000"
#define SET_PERPETUAL_BACKUP_MODE       "$PMTK225,4"
#define SET_ALWAYSLOCATE_STANDBY_MODE   "$PMTK225,8"
#define SET_ALWAYSLOCATE_BACKUP_MODE    "$PMTK225,9"

//Set the message interval,100ms~10000ms
#define SET_POS_FIX         "$PMTK220"
#define SET_POS_FIX_100MS   "$PMTK220,100"
#define SET_POS_FIX_200MS   "$PMTK220,200"
#define SET_POS_FIX_400MS   "$PMTK220,400"
#define SET_POS_FIX_800MS   "$PMTK220,800"
#define SET_POS_FIX_1S      "$PMTK220,1000"
#define SET_POS_FIX_2S      "$PMTK220,2000"
#define SET_POS_FIX_4S      "$PMTK220,4000"
#define SET_POS_FIX_8S      "$PMTK220,8000"
#define SET_POS_FIX_10S     "$PMTK220,10000"

//Switching time output
#define SET_SYNC_PPS_NMEA_OFF   "$PMTK255,0"
#define SET_SYNC_PPS_NMEA_ON    "$PMTK255,1"

//Baud rate
#define SET_NMEA_BAUDRATE           "$PMTK251"
#define SET_NMEA_BAUDRATE_115200    "$PMTK251,115200"
#define SET_NMEA_BAUDRATE_57600     "$PMTK251,57600"
#define SET_NMEA_BAUDRATE_38400     "$PMTK251,38400"
#define SET_NMEA_BAUDRATE_19200     "$PMTK251,19200"
#define SET_NMEA_BAUDRATE_14400     "$PMTK251,14400"
#define SET_NMEA_BAUDRATE_9600      "$PMTK251,9600"
#define SET_NMEA_BAUDRATE_4800      "$PMTK251,4800"

//To restore the system default setting
#define SET_REDUCTION               "$PMTK314,-1"

//Set NMEA sentence output frequencies 
#define SET_NMEA_OUTPUT "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"


//extern char buff_t[BUFFSIZE];

//转换前的参数(数字形式)
typedef struct {
		double Lon;     //GPS Latitude and longitude
		double Lat;
	
    uint8_t Time_H;   //Time
    uint8_t Time_M;
    uint8_t Time_S;
}GNRMC_number;


extern GNRMC_number GPS_number;

//用来存储转换后的
typedef struct {
    double Lon;
    double Lat;
}Coordinates;



void L76X_Send_Command(char *data);
GNRMC_number L76X_Gat_GNRMC(void);
Coordinates L76X_Baidu_Coordinates();

#endif
