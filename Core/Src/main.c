#include "main.h"
#include "lcd.h"
#include "graph.h"
#include "RREFont.h"
#include "gps.h"
#include "controls.h"
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;

DMA_HandleTypeDef hdma_memtomem_dma1_channel1;
SRAM_HandleTypeDef hsram1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_FSMC_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  GPS_UART_TxCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  GPS_UART_RxCallback(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  GPS_UART_ErrorCallback(huart);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  static uint8_t gps_cnt = 0;
  if(++gps_cnt >= 10) {
    GPS_TIM_Callback(htim);
    gps_cnt = 0;
  }
  controls_irq();
}

#define LIST_SIZE 5

uint16_t list_speeds[LIST_SIZE] = {20,50,80,100,120};
uint16_t list_distances[LIST_SIZE] = {50,101,201,402,805};

float list_best_speeds[LIST_SIZE] = {0};
float list_best_distances[LIST_SIZE] = {0};
float list_last_speeds[LIST_SIZE] = {0};
float list_last_distances[LIST_SIZE] = {0};

float list_best_speeds_relative[LIST_SIZE] = {0};
float list_best_distances_relative[LIST_SIZE] = {0};
float list_last_speeds_relative[LIST_SIZE] = {0};
float list_last_distances_relative[LIST_SIZE] = {0};
float speed_rel_last = 0.0f;
float dist_rel_last = 0.0f;

#define GRAPH_SIZE 1200

float graph_time[GRAPH_SIZE];
float graph_speed[GRAPH_SIZE];
uint32_t graph_count = 0;




uint32_t gps_last = 0;
uint32_t temp = 0;
int32_t speed = INT_MAX;
float speedf;
int32_t oldspeed = INT_MAX;
int32_t speed_max;
uint32_t speed_nzero_t = 0;
uint8_t status = 0; //GPS = 0, Idle = 1, ready = 2, ongoing = 3
uint8_t oldstatus = UCHAR_MAX;
uint16_t speed_color = COLOR_LGRAY;
uint16_t speed_color_old = COLOR_LGRAY;
uint32_t tick;
uint8_t stats_changed = 1;
const char *status_str = "N/A";
char string[256];
uint32_t strw;
uint32_t start_time;
float start_lat;
float start_lng;
float distance;
uint8_t index_speed = 0;
uint8_t index_distance = 0;
uint16_t graph_index = 0;
float time;
uint8_t graph_relative = 0;

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FSMC_Init();
  MX_SPI2_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();

  graph_init();
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  /*
   * rre_ArialDig150b speed
   * rre_arial_62 big Text
   * rre_ubuntu_32 Regular text
   * rre_vga_8x16 Small text
   */


  GPS_Init();
  HAL_TIM_Base_Start_IT(&htim6);

  speed_nzero_t = HAL_GetTick();

  while (1)
  {
    tick = HAL_GetTick();
    temp = GPS_Data.LastUpdated;
    if(gps_last != temp)
    {
      gps_last = temp;

      if(GPS_Data.Status != HAL_OK || GPS_Data.FixType == GPS_FIX_NONE) {
        status = 0;
        speed = INT_MIN;
        speed_nzero_t = tick;
      }
      else {
        speedf = GPS_Data.Speed;
        speed = (int32_t)speedf;
        if(speed > 0)
          speed_nzero_t = tick;

        if(status == 0)
          status = 1;

        if(status == 1 && tick - speed_nzero_t > 1000)
        {
          status = 2;
          start_lat = GPS_Data.Latitude;
          start_lng = GPS_Data.Longtitude;
        }

        if(status == 2)
        {
          if(speed > 3) {
            status = 3;
            start_time = tick;
            speed_max = speed;
            index_speed = 0;
            index_distance = 0;
            graph_index = 0;
            graph_count = 0;
            for(int i = 0; i < LIST_SIZE; i++) {
              list_last_speeds[i] = 0;
              list_last_distances[i] = 0;
              list_last_speeds_relative[i] = 0;
              list_last_distances_relative[i] = 0;
            }
            for(int i = 0; i < GRAPH_SIZE; i++) {
              graph_time[i] = 0;
              graph_speed[i] = 0;
            }
            speed_rel_last = 0.0f;
            dist_rel_last = 0.0f;
            stats_changed = 1;
          }
        }

        if(status == 3)
        {
          if(speed > speed_max)
            speed_max = speed;

          distance = GPS_Distance(start_lng, start_lat, GPS_Data.Longtitude, GPS_Data.Latitude);
          time = ((float)(tick-start_time)) * 0.001f;
          while(index_speed < LIST_SIZE && speedf >= list_speeds[index_speed]) {
            list_last_speeds[index_speed] = time;
            list_last_speeds_relative[index_distance] = time - speed_rel_last;
            speed_rel_last = time;
            index_speed++;
            stats_changed = 1;
          }

          while(index_distance < LIST_SIZE && distance >= list_distances[index_distance]) {
            list_last_distances[index_distance] = time;
            list_last_distances_relative[index_distance] = time - dist_rel_last;
            dist_rel_last = time;
            index_distance++;
            stats_changed = 1;
          }

          if(graph_index < GRAPH_SIZE) {
            graph_speed[graph_index] = speedf;
            graph_time[graph_index] = time;
            graph_index++;
          }


          if(speed_max - speed > 3 || (index_speed >= LIST_SIZE && index_distance >= LIST_SIZE) || graph_index >= GRAPH_SIZE) {
            status = 1;

            for(int i = 0; i < index_speed; i++) {
              if(list_best_speeds[i] == 0 || list_last_speeds[i] < list_best_speeds[i])
                list_best_speeds[i] = list_last_speeds[i];
              if(list_best_speeds_relative[i] == 0 || list_last_speeds_relative[i] < list_best_speeds_relative[i])
                list_best_speeds_relative[i] = list_last_speeds_relative[i];
            }

            for(int i = 0; i < index_distance; i++) {
              if(list_best_distances[i] == 0 || list_last_distances[i] < list_best_distances[i])
                list_best_distances[i] = list_last_distances[i];
              if(list_best_distances_relative[i] == 0 || list_last_distances_relative[i] < list_best_distances_relative[i])
                list_best_distances_relative[i] = list_last_distances_relative[i];
            }

            stats_changed = 1;
            graph_count = graph_index;

          }
        }
      }

      if(BUT_ENTER_PRESS && BUT_ENTER_TIME > 500) {
        for(int i = 0; i < LIST_SIZE; i++) {
          list_last_speeds[i] = 0;
          list_last_distances[i] = 0;
          list_best_speeds[i] = 0;
          list_best_distances[i] = 0;
          list_last_speeds_relative[i] = 0;
          list_last_distances_relative[i] = 0;
          list_best_speeds_relative[i] = 0;
          list_best_distances_relative[i] = 0;
        }
        for(int i = 0; i < GRAPH_SIZE; i++) {
          graph_time[i] = 0;
          graph_speed[i] = 0;
        }
        speed_nzero_t = tick;
        BUT_ENTER_TIME = 0;
        status = 1;
        stats_changed = 1;
      }

      if(BUT_LEFT || BUT_RIGHT) {
        graph_relative ^= 1;
        BUT_LEFT = 0;
        BUT_RIGHT = 0;
        stats_changed = 1;
      }

      if((BUT_UP_PRESS && BUT_UP_TIME > 200) || (BUT_DOWN_PRESS && BUT_DOWN_TIME > 200)) {
        if(status == 1) {
          status = 2;
          start_lat = GPS_Data.Latitude;
          start_lng = GPS_Data.Longtitude;
        }
      }

      font_setColor2(COLOR_WHITE,COLOR_BLACK);
      font_setFont(&rre_ubuntu_32);
      font_setSpacing(2);
      if(oldstatus != status) {
        oldstatus = status;
        if(status == 0) {
          status_str = "Waiting for GPS...";
          speed_color = COLOR_RED;
        }
        else if(status == 1) {
          speed_color = COLOR_RED;
          status_str = "Stop the vehicle to run";
        }
        else if(status == 2) {
          speed_color = COLOR_GREEN;
          status_str = "Ready to run";
        }
        else if(status == 3) {
          speed_color = COLOR_YELLOW;
          status_str = "Running";
        }
        else {
          speed_color = COLOR_LGRAY;
          status_str = "Undefined";
        }
        graph_square_bg(COLOR_BLACK, COLOR_BLACK, 0, 36 ,853 , 36 + font_getHeight());
        font_printf(20,36,"Status: %s", status_str);
      }

      font_printStr(600,36, graph_relative ? "Relative " : "Absolute ");

      if(oldspeed != speed || speed_color != speed_color_old) {
        oldspeed = speed;
        speed_color_old = speed_color;
        font_setColor2(speed_color ,COLOR_BLACK);
        font_setFont(&rre_ArialDig150b);
        if(speed > INT_MIN) {
          font_setSpacing(12);
          sprintf(string, "%ld", speed);
        }
        else strcpy(string, "- - -");

        strw = font_strWidth(string);
        graph_square_bg(COLOR_BLACK, COLOR_BLACK, 0, 100, 250-strw/2, 100+font_getHeight());
        graph_square_bg(COLOR_BLACK, COLOR_BLACK, 250+strw/2, 100, 500, 100+font_getHeight());
        font_printStr(250-strw/2, 100, string);
      }

      font_setColor2(COLOR_WHITE,COLOR_BLACK);
      font_setFont(&rre_vga_8x16);
      font_setSpacing(2);

      font_printf(10, 10, "GPS: %s   Used satelites: %d   Lat: %9.6f Lng: %9.6f   Accuracy: %4.1fm     ", GPS_Data.FixType ? "OK" : "NG", GPS_Data.SatelitesUsed, GPS_Data.Latitude, GPS_Data.Longtitude, GPS_Data.Accuracy);
      font_printf(680, 10, "%d.%02d.%04d %02d:%02d:%02d.%02d     ", GPS_Data.DateDay, GPS_Data.DateMonth, GPS_Data.DateYear+2000, GPS_Data.TimeHour, GPS_Data.TimeMinute, GPS_Data.TimeSecond, GPS_Data.TimeSubSecond);

      if(stats_changed) {
        stats_changed = 0;
        font_setColor2(COLOR_WHITE,COLOR_BLACK);
        font_setFont(&rre_vga_8x16);
        font_setSpacing(2);

        graph_square_bg(COLOR_BLACK, COLOR_BLACK, 600-90, 120-52, 853, 479);

        strcpy(string, "Speed");
        font_printStr(600 - font_strWidth(string), 120-36, string);
        strcpy(string, "Current");
        font_printStr(735 - font_strWidth(string), 120-36, string);
        strcpy(string, "Best");
        font_printStr(830 - font_strWidth(string), 120-36, string);
        graph_line(600-90, 120-8, 840, 120-8, COLOR_DGRAY);
        for(int i = 0; i < LIST_SIZE; i++) {
          font_setColor2(COLOR_WHITE,COLOR_BLACK);
          sprintf(string, "%d km/h", list_speeds[i]);
          font_printStr(600 - font_strWidth(string), 120+i*30, string);

          if(graph_relative && list_last_speeds_relative[i] > 0.0f)
            sprintf(string, "%.1f s", list_last_speeds_relative[i]);
          else if(!graph_relative && list_last_speeds[i] > 0.0f)
            sprintf(string, "%.1f s", list_last_speeds[i]);
          else {
            font_setColor2(COLOR_DGRAY,COLOR_BLACK);
            strcpy(string, "---");
          }
          font_printStr(735 - font_strWidth(string), 120+i*30, string);

          font_setColor2(COLOR_WHITE,COLOR_BLACK);
          if(graph_relative && list_best_speeds_relative[i] > 0.0f)
            sprintf(string, "%.1f s", list_best_speeds_relative[i]);
          else if(!graph_relative && list_best_speeds[i] > 0.0f)
            sprintf(string, "%.1f s", list_best_speeds[i]);
          else {
            font_setColor2(COLOR_DGRAY,COLOR_BLACK);
            strcpy(string, "---");
          }
          font_printStr(830 - font_strWidth(string), 120+i*30, string);
        }

        font_setColor2(COLOR_WHITE,COLOR_BLACK);
        strcpy(string, "Distance");
        font_printStr(600 - font_strWidth(string), 310-36, string);
        strcpy(string, "Current");
        font_printStr(735 - font_strWidth(string), 310-36, string);
        strcpy(string, "Best");
        font_printStr(830 - font_strWidth(string), 310-36, string);
        graph_line(600-90, 310-8, 840, 310-8, COLOR_DGRAY);
        for(int i = 0; i < LIST_SIZE; i++) {
          font_setColor2(COLOR_WHITE,COLOR_BLACK);
          sprintf(string, "%d m", list_distances[i]);
          font_printStr(600 - font_strWidth(string), 310+i*30, string);

          if(graph_relative && list_last_distances_relative[i] > 0.0f)
            sprintf(string, "%.1f s", list_last_distances_relative[i]);
          else if(!graph_relative && list_last_distances[i] > 0.0f)
            sprintf(string, "%.1f s", list_last_distances[i]);
          else {
            font_setColor2(COLOR_DGRAY,COLOR_BLACK);
            strcpy(string, "---");
          }
          font_printStr(735 - font_strWidth(string), 310+i*30, string);

          font_setColor2(COLOR_WHITE,COLOR_BLACK);
          if(graph_relative && list_best_distances_relative[i] > 0.0f)
            sprintf(string, "%.1f s", list_best_distances_relative[i]);
          else if(!graph_relative && list_best_distances[i] > 0.0f)
            sprintf(string, "%.1f s", list_best_distances[i]);
          else {
            font_setColor2(COLOR_DGRAY,COLOR_BLACK);
            strcpy(string, "---");
          }
          font_printStr(830 - font_strWidth(string), 310+i*30, string);
        }

        int graph_x1 = 40;
        int graph_x2 = 500;
        int graph_y1 = 290;
        int graph_y2 = 460;

        graph_square_bg(COLOR_BLACK, COLOR_BLACK, 0, graph_y1, graph_x2, 479);

        float time_max = 0.0f;
        float speed_max = 0.0f;

        font_setColor2(COLOR_WHITE,COLOR_BLACK);
        font_setFont(&rre_5x8);
        font_setSpacing(1);

        graph_line(10, graph_y2, graph_x2, graph_y2, COLOR_LGRAY);
        graph_line(graph_x1, graph_y1, graph_x1, graph_y2+10, COLOR_LGRAY);

        if(graph_count > 0) {
          time_max = graph_time[graph_count-1];
          for(int i = 0; i < graph_count; i++) {
            if(graph_speed[i] > speed_max)
              speed_max = graph_speed[i];
          }

          float pp10kmh = (graph_y2-graph_y1) / speed_max * 10.0f;
          int y_steps = (uint32_t)roundf(speed_max) / 10;
          for(int i = 0; i < y_steps || i == 0; i++) {
            float y = graph_y2 - (uint32_t)(pp10kmh * (float)i);
            graph_line(graph_x1 - 10, y, graph_x1 + 10, y, COLOR_LGRAY);
            sprintf(string, "%d", i*10);
            strw = font_strWidth(string);
            font_printStr(graph_x1-strw-2, y-font_getHeight(), string);

            if(i>0)
              graph_line(graph_x1+1, y, graph_x2, y, COLOR_DGRAY);
          }

          float pp1s = (graph_x2-graph_x1) / time_max;
          int x_steps = (uint32_t)roundf(time_max);
          for(int i = 0; i < x_steps || i == 0; i++) {
            float x = graph_x1 + (uint32_t)(pp1s * (float)i);
            graph_line(x, graph_y2 - 10, x, graph_y2 + 10, COLOR_LGRAY);

            font_printf(x+2, graph_y2+3, "%d", i);

            if(i>0)
              graph_line(x, graph_y1, x, graph_y2-1, COLOR_DGRAY);
          }

          float x = -1.0f ,y = -1.0f , prevx = -1.0f , prevy = -1.0f;

          for(int i = 0; i < graph_count; i++)
          {
            x = (graph_x2-graph_x1) * graph_time[i] / time_max + graph_x1;
            y = graph_y2 - graph_speed[i] / speed_max * (graph_y2-graph_y1);

            if(prevx == -1.0f && prevy == -1.0f)
              prevx = x, prevy = y;

            graph_line(prevx, prevy, x, y, COLOR_YELLOW);

            prevx = x;
            prevy = y;
          }
        }
      }

    }

  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 800;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 7199;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 49;
  htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim6, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim6);

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_memtomem_dma1_channel1
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* Configure DMA request hdma_memtomem_dma1_channel1 on DMA1_Channel1 */
  hdma_memtomem_dma1_channel1.Instance = DMA1_Channel1;
  hdma_memtomem_dma1_channel1.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma1_channel1.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma1_channel1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma1_channel1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_memtomem_dma1_channel1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_memtomem_dma1_channel1.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma1_channel1.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_memtomem_dma1_channel1) != HAL_OK)
  {
    Error_Handler( );
  }

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TP_INT_Pin TP_BUSY_Pin */
  GPIO_InitStruct.Pin = TP_INT_Pin|TP_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI2_NSS_Pin */
  GPIO_InitStruct.Pin = SPI2_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI2_NSS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FLASH_CS_Pin LCD_RST_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin|LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BUT_LEFT_Pin|BUT_RIGHT_Pin|BUT_UP_Pin|BUT_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BUT_ENTER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};

  /* USER CODE BEGIN FSMC_Init 1 */

  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  /* Timing */
  Timing.AddressSetupTime = 1;
  Timing.AddressHoldTime = 0;
  Timing.DataSetupTime = 2;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 0;
  Timing.DataLatency = 0;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /** Disconnect NADV
  */

  __HAL_AFIO_FSMCNADV_DISCONNECTED();

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
