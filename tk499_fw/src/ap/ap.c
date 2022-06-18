/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"




extern uint32_t test_cnt;
static void ledThread(void const *argument);;


void apInit(void)
{
  cliOpen(HW_UART_CH_CLI, 115200);

  osThreadDef(ledThread, ledThread, _HW_DEF_RTOS_THREAD_PRI_LED, 0, _HW_DEF_RTOS_THREAD_MEM_LED);
  if (osThreadCreate(osThread(ledThread), NULL) != NULL)
  {
    logPrintf("ledThread  \t\t: OK\r\n");
  }
  else
  {
    logPrintf("ledThread  \t\t: Fail\r\n");
  }  
}

void apMain(void)
{
  uint32_t pre_time;


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      //ledToggle(_DEF_LED1);
    }

    cliMain();
  }
}

void ledThread(void const *argument)
{
  (void)argument;



  while(1)
  {
    logPrintf("test\n");
    ledToggle(0);
    delay(1000);
  }
}