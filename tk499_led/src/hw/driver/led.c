/*
 * led.c
 *
 *  Created on: Dec 7, 2020
 *      Author: baram
 */


#include "led.h"
#include "cli.h"


#ifdef _USE_HW_LED

typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin;
  BitAction     on_state;
  BitAction     off_state;
} led_tbl_t;


led_tbl_t led_tbl[LED_MAX_CH] =
    {
        {GPIOA, GPIO_Pin_8, Bit_RESET, Bit_SET},
    };


#ifdef _USE_HW_CLI
static void cliLed(cli_args_t *args);
#endif


bool ledInit(void)
{
  bool ret = true;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);


	GPIO_InitStructure.GPIO_Pin    =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;

  for (int i=0; i<LED_MAX_CH; i++)
  {
    GPIO_InitStructure.GPIO_Pin = led_tbl[i].pin;
    GPIO_Init(led_tbl[i].port, &GPIO_InitStructure);

    ledOff(i);
  }

#ifdef _USE_HW_CLI
  cliAdd("led", cliLed);
#endif

  return ret;
}

void ledOn(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  GPIO_WriteBit(led_tbl[ch].port, led_tbl[ch].pin, led_tbl[ch].on_state);
}

void ledOff(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  GPIO_WriteBit(led_tbl[ch].port, led_tbl[ch].pin, led_tbl[ch].off_state);
}

void ledToggle(uint8_t ch)
{
  uint8_t out_bit;

  if (ch >= LED_MAX_CH) return;

  
  out_bit = !GPIO_ReadInputDataBit(led_tbl[ch].port, led_tbl[ch].pin);
  GPIO_WriteBit(led_tbl[ch].port, led_tbl[ch].pin, out_bit);
}





#ifdef _USE_HW_CLI

void cliLed(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 3 && args->isStr(0, "toggle") == true)
  {
    uint8_t  led_ch;
    uint32_t toggle_time;
    uint32_t pre_time;

    led_ch      = (uint8_t)args->getData(1);
    toggle_time = (uint32_t)args->getData(2);

    if (led_ch > 0)
    {
      led_ch--;
    }

    pre_time = millis();
    while(cliKeepLoop())
    {
      if (millis()-pre_time >= toggle_time)
      {
        pre_time = millis();
        ledToggle(led_ch);
      }
    }

    ret = true;
  }


  if (ret != true)
  {
    cliPrintf("led toggle ch[1~%d] time_ms\n", LED_MAX_CH);
  }
}


#endif


#endif
