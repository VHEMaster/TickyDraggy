#include "gps.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#define GPS_UART huart3
#define GPS_TIM htim6

volatile GPS_DataTypeDef GPS_Data;
extern UART_HandleTypeDef GPS_UART;
extern TIM_HandleTypeDef GPS_TIM;

static void GPS_Task(void);

#define GPS_CPLT_MSG (uint32_t)1
#define GPS_TX_MSG (uint32_t)2


volatile uint32_t GPS_Checksum_Errors = 0;
volatile uint32_t GPS_Interface_Errors = 0;
volatile uint32_t GPS_Timeouts = 0;


char GPS_DataStrings[64][128] __attribute__ ((aligned(32)));
volatile int8_t GPS_WritePos = 0;
volatile int8_t GPS_ReadPos = 0;
volatile uint8_t GPS_CharPos = 0;

volatile uint8_t GPS_TxCplt = 0;

int8_t GPS_WaitCplt()
{
  int8_t result = -1;
  if(GPS_ReadPos != GPS_WritePos)
  {
    result = GPS_ReadPos;
    if(++GPS_ReadPos >= 64)
      GPS_ReadPos = 0;
  }
  return result;
}

HAL_StatusTypeDef GPS_WaitTxCplt(void)
{
  while(!GPS_TxCplt) {};
  GPS_TxCplt = 0;
  return HAL_OK;
}


volatile uint8_t GPS_RX_DATA;


void GPS_UART_TxCallback(UART_HandleTypeDef * huart)
{
  if(huart == &GPS_UART)
  {
    GPS_TxCplt = 1;
  }
}


void GPS_UART_RxCallback(UART_HandleTypeDef * huart)
{
  if(huart == &GPS_UART)
  {
    if(GPS_RX_DATA == '\r')
    {
      GPS_DataStrings[GPS_WritePos][GPS_CharPos++] = 0;
    }
    else if(GPS_RX_DATA == '\n')
    {
      GPS_DataStrings[GPS_WritePos][GPS_CharPos] = 0;
      GPS_CharPos = 0;
      if(++GPS_WritePos >= 64) GPS_WritePos = 0;
    }
    else
    {
      GPS_DataStrings[GPS_WritePos][GPS_CharPos++] = GPS_RX_DATA;
    }
    HAL_UART_Receive_IT(huart, (uint8_t*)&GPS_RX_DATA, 1);
  }
}

void GPS_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
  if(huart == &GPS_UART)
  {
    HAL_UART_Receive_IT(&GPS_UART, (uint8_t*)&GPS_RX_DATA, 1);
    GPS_Interface_Errors++;
  }
}

void GPS_TIM_Callback(TIM_HandleTypeDef * htim)
{
  if(htim == &GPS_TIM)
  {
    GPS_Task();
  }
}

HAL_StatusTypeDef GPS_Send(const void * data, uint8_t length)
{
  if(GPS_TxCplt) return HAL_ERROR;
  GPS_TxCplt = 0;
  if(HAL_UART_Transmit_DMA(&GPS_UART, (uint8_t*)data, length) != HAL_OK) return HAL_ERROR;
  return GPS_WaitTxCplt();
}

static void GPS_Task(void)
{
  static char * strptr;
  static char string[256];
  static char chkarr[3] = {0,0,0};
  static char chkarrr[3] = {0,0,0};
  static uint16_t strindex;
  static uint16_t strl;
  static uint16_t i;
  static char checksum;
  int8_t readpos;
  float dummy;
  char cdummy;

  while((readpos = GPS_WaitCplt()) != -1)
  {

    strptr = GPS_DataStrings[readpos];
    strl = strlen(strptr);

    if(strl > 10 &&
        (
          (strncmp(strptr, "$GPGGA", 6) == 0 || strncmp(strptr, "$GPRMC", 6) == 0) ||
          (strncmp(strptr, "$GNGGA", 6) == 0 || strncmp(strptr, "$GNRMC", 6) == 0)
        )
      )
    {

      checksum = 0;
      strindex = 0;
      chkarr[0] = 0;
      chkarr[1] = 0;
      for(i=1;i<strl;i++)
      {

        if(strptr[i] == '*')
        {
          chkarr[0] = strptr[i+1];
          chkarr[1] = strptr[i+2];
          chkarr[2] = 0;
          break;
        }
        else
        {
          checksum ^= strptr[i];
          if(strptr[i] == ',' && strptr[i-1] == ',')
          {
            string[strindex++] = '0';
            string[strindex++] = ',';
          }
          else string[strindex++] = strptr[i];
        }
      }

      sprintf(chkarrr, "%02X", checksum);

      if(chkarr[0] == chkarrr[0] && chkarr[1] == chkarrr[1])
      {
        if(strncmp(string,"GPGGA",5) == 0 || strncmp(string,"GNGGA",5) == 0)
        {
          static char lat_ch,lon_ch;
          static uint32_t sat_used;
          static uint32_t fix;
          static float lat,lon,acc;

          //GPGGA
          sscanf(&string[6],"%f,%f,%c,%f,%c,%lu,%lu,%f,%f",&dummy,&lat,&lat_ch,&lon,&lon_ch,&fix,&sat_used,&acc,&dummy);

          lat = floorf(lat*0.01f)+(fmodf(lat, 100.0f)*0.0166667f);
          lon = floorf(lon*0.01f)+(fmodf(lon, 100.0f)*0.0166667f);


          GPS_Data.FixType = (GPS_FixTypeTypeDef)fix;
          lat_ch == 'S' ? (GPS_Data.Latitude = 0.0f-lat) : (GPS_Data.Latitude = lat);
          lon_ch == 'W' ? (GPS_Data.Longtitude = 0.0f-lon) : (GPS_Data.Longtitude = lon);
          GPS_Data.SatelitesUsed = sat_used;
          GPS_Data.Accuracy = acc;

          GPS_Data.LastUpdated = HAL_GetTick();

          GPS_Data.Status = HAL_OK;
        }

        else if(strncmp(string,"GPRMC",5) == 0 || strncmp(string,"GNRMC",5) == 0)
        {
          uint32_t date;
          char stat;
          float time;
          float speed;
          //GPRMC
          sscanf(&string[6],"%f,%c,%f,%c,%f,%c,%f,%f,%lu,",&time,&stat,&dummy,&cdummy,&dummy,&cdummy,&speed,&dummy,&date);

          GPS_Data.Speed = speed < 200 ? speed * 1.852f : GPS_Data.Speed;
          GPS_Data.DateYear = date%100;
          GPS_Data.DateMonth = date/100%100;
          GPS_Data.DateDay = date/10000%100;

          GPS_Data.TimeHour = (uint32_t)(time*0.0001f);
          GPS_Data.TimeMinute = (uint32_t)(time*0.01f)%100;
          GPS_Data.TimeSecond = (uint32_t)(time)%100;
          GPS_Data.TimeSubSecond = (uint32_t)(time*100.0f)%100;

          GPS_Data.LastUpdated = HAL_GetTick();
        }
      }
      else
      {
        GPS_Checksum_Errors++;
      }
    }
  }
  if(HAL_GetTick() - GPS_Data.LastUpdated > 1000)
    GPS_Data.Status = HAL_ERROR;

}

const uint8_t GPS_Baud_115200[] = { 0xB5 ,0x62 ,0x06 ,0x00 ,0x14 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0xD0 ,0x08 ,0x00 ,0x00 ,0x00 ,0xC2 ,0x01 ,0x00 ,0x07 ,0x00 ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xC0 ,0x7E ,0xB5 ,0x62 ,0x06 ,0x00 ,0x01 ,0x00 ,0x01 ,0x08 ,0x22 }; //115200
const uint8_t GPS_Rate_150[] =  { 0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0x96 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0xAC ,0x3E ,0xB5 ,0x62 ,0x06 ,0x08 ,0x00 ,0x00 ,0x0E ,0x30 }; //150
const uint8_t GPS_Rate_100[] =  { 0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0x64 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x7A ,0x12 ,0xB5 ,0x62 ,0x06 ,0x08 ,0x00 ,0x00 ,0x0E ,0x30 }; //100


HAL_StatusTypeDef GPS_Init(void)
{
  GPS_UART.Init.BaudRate = 9600;
  HAL_UART_Init(&GPS_UART);

  HAL_Delay(500);
  GPS_Send(GPS_Baud_115200,sizeof(GPS_Baud_115200));
  HAL_Delay(100);
  GPS_UART.Init.BaudRate = 115200;
  HAL_UART_Init(&GPS_UART);
  HAL_Delay(100);
  GPS_Send(GPS_Rate_100,sizeof(GPS_Rate_100));
  HAL_Delay(100);

  HAL_UART_Receive_IT(&GPS_UART, (uint8_t*)&GPS_RX_DATA, 1);

  return HAL_OK;

}

float GPS_Distance(float lng1, float lat1, float lng2, float lat2)
{
  float sd = sinf((lat2-lat1)*0.00872664626f);
  float sf = sinf((lng2-lng1)*0.00872664626f);

  float dist = asinf(sqrtf(sd*sd+(cosf(lat2*0.01745329252f)*cosf(lat1*0.01745329252f)*(sf*sf))))*12734889.314245f;

  return dist;
}


