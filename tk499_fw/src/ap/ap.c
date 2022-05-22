/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"




extern uint32_t test_cnt;



void apInit(void)
{
  cliOpen(HW_UART_CH_CLI, 115200);
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
      ledToggle(_DEF_LED1);
    }

    if (usbIsOpen() && usbGetType() == USB_CON_CLI)
    {
      cliMain();
    }
  }
}

