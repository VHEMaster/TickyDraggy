/*
 * gps.h
 *
 *  Created on: Oct 27, 2021
 *      Author: VHEMaster
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_
#include "main.h"

typedef enum
{
  GPS_FIX_NONE = 0,
  GPS_FIX_GPS = 1,
  GPS_FIX_DGPS = 2
} GPS_FixTypeTypeDef;

typedef struct
{
  float Latitude;
  float Longtitude;
  float Speed;
  float Accuracy;
  int DateDay;
  int DateMonth;
  int DateYear;
  int TimeHour;
  int TimeMinute;
  int TimeSecond;
  int TimeSubSecond;
  GPS_FixTypeTypeDef FixType;
  HAL_StatusTypeDef Status;
  uint32_t LastUpdated;
  uint8_t SatelitesUsed;
} GPS_DataTypeDef;

extern volatile GPS_DataTypeDef GPS_Data;

extern HAL_StatusTypeDef GPS_Init(void);
extern void GPS_UART_TxCallback(UART_HandleTypeDef * huart);
extern void GPS_UART_RxCallback(UART_HandleTypeDef * huart);
extern void GPS_UART_ErrorCallback(UART_HandleTypeDef * huart);
extern void GPS_TIM_Callback(TIM_HandleTypeDef * htim);

extern float GPS_Distance(float lng1, float lat1, float lng2, float lat2);


#endif /* INC_GPS_H_ */
