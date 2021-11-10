/*
 * controls.c
 *
 *  Created on: Jan 26, 2021
 *      Author: VHEMaster
 */


#include "controls.h"

#define BUT_CTRL(x) (BUT_##x##_GPIO_Port->IDR&BUT_##x##_Pin)
#define ISPRESS(x) ((x)>0)
#define BUT_PRESS_DELAY 20

#define TIM_STEP 5
static uint32_t TIM_CNT = 0;

volatile uint8_t BUT_LEFT_PRESS = 0;
volatile uint8_t BUT_RIGHT_PRESS = 0;
volatile uint8_t BUT_UP_PRESS = 0;
volatile uint8_t BUT_DOWN_PRESS = 0;
volatile uint8_t BUT_ENTER_PRESS = 0;

volatile uint32_t BUT_LEFT_TIME = 0;
volatile uint32_t BUT_RIGHT_TIME = 0;
volatile uint32_t BUT_UP_TIME = 0;
volatile uint32_t BUT_DOWN_TIME = 0;
volatile uint32_t BUT_ENTER_TIME = 0;

volatile uint8_t BUT_LEFT = 0;
volatile uint8_t BUT_RIGHT = 0;
volatile uint8_t BUT_UP = 0;
volatile uint8_t BUT_DOWN = 0;
volatile uint8_t BUT_ENTER = 0;

static uint8_t but_left_delay = 0;
static uint8_t but_right_delay = 0;
static uint8_t but_up_delay = 0;
static uint8_t but_down_delay = 0;

static uint8_t but_enter_delay = 0;

static uint32_t but_left_ftime = 0;
static uint32_t but_right_ftime = 0;
static uint32_t but_up_ftime = 0;
static uint32_t but_down_ftime = 0;

static uint32_t but_enter_ftime = 0;


void controls_irq(void)
{
  TIM_CNT += TIM_STEP;

  //UP
  if(ISPRESS(BUT_CTRL(UP)))
  {
    if(but_up_delay > BUT_PRESS_DELAY)
    {
      if(but_up_ftime == 0) but_up_ftime = TIM_CNT, BUT_UP_TIME = 0;
      BUT_UP_TIME += TIM_STEP;
      if(BUT_LEFT_PRESS == 0 && BUT_RIGHT_PRESS == 0 && BUT_UP_PRESS == 0 && BUT_DOWN_PRESS == 0)
        BUT_UP = 1;
      BUT_UP_PRESS = 1;
    } else but_up_delay+=TIM_STEP;
  } else BUT_UP_PRESS = but_up_delay = but_up_ftime = 0;

  //DOWN
  if(ISPRESS(BUT_CTRL(DOWN)))
  {
    if(but_down_delay > BUT_PRESS_DELAY)
    {
      if(but_down_ftime == 0) but_down_ftime = TIM_CNT, BUT_DOWN_TIME = 0;
      BUT_DOWN_TIME += TIM_STEP;
      if(BUT_LEFT_PRESS == 0 && BUT_RIGHT_PRESS == 0 && BUT_UP_PRESS == 0 && BUT_DOWN_PRESS == 0)
        BUT_DOWN = 1;
      BUT_DOWN_PRESS = 1;
    } else but_down_delay+=TIM_STEP;
  } else BUT_DOWN_PRESS = but_down_delay = but_down_ftime = 0;

  //LEFT
  if(ISPRESS(BUT_CTRL(LEFT)))
  {
    if(but_left_delay > BUT_PRESS_DELAY)
    {
      if(but_left_ftime == 0) but_left_ftime = TIM_CNT, BUT_LEFT_TIME = 0;
      BUT_LEFT_TIME += TIM_STEP;
      if(BUT_LEFT_PRESS == 0 && BUT_RIGHT_PRESS == 0 && BUT_UP_PRESS == 0 && BUT_DOWN_PRESS == 0)
        BUT_LEFT = 1;
      BUT_LEFT_PRESS = 1;
    } else but_left_delay+=TIM_STEP;
  } else BUT_LEFT_PRESS = but_left_delay = but_left_ftime = 0;

  //RIGHT
  if(ISPRESS(BUT_CTRL(RIGHT)))
  {
    if(but_right_delay > BUT_PRESS_DELAY)
    {
      if(but_right_ftime == 0) but_right_ftime = TIM_CNT, BUT_RIGHT_TIME = 0;
      BUT_RIGHT_TIME += TIM_STEP;
      if(BUT_LEFT_PRESS == 0 && BUT_RIGHT_PRESS == 0 && BUT_UP_PRESS == 0 && BUT_DOWN_PRESS == 0)
        BUT_RIGHT = 1;
      BUT_RIGHT_PRESS = 1;
    } else but_right_delay+=TIM_STEP;
  } else BUT_RIGHT_PRESS = but_right_delay = but_right_ftime = 0;

  //ENTER
  if(ISPRESS(BUT_CTRL(ENTER)))
  {
    if(but_enter_delay > BUT_PRESS_DELAY)
    {
      if(but_enter_ftime == 0) but_enter_ftime = TIM_CNT, BUT_ENTER_TIME = 0,BUT_ENTER = 1;
      BUT_ENTER_TIME += TIM_STEP;
      BUT_ENTER_PRESS = 1;
    } else but_enter_delay+=TIM_STEP;
  } else BUT_ENTER_PRESS = but_enter_delay = but_enter_ftime = 0;

}

