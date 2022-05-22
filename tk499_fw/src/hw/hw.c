/*
 * hw.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "hw.h"



void hwInit(void)
{
  bspInit();

  cliInit();
  logInit();
  ledInit();
  usbInit();

  usbBegin(USB_CDC_MODE);

  uartInit();
  uartOpen(_DEF_UART1, 115200);  

  logOpen(_DEF_UART1, 115200);
  logPrintf("\r\n[ Firmware Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", _DEF_BOARD_NAME);
  logPrintf("Booting..Ver  \t\t: %s\r\n", _DEF_FIRMWATRE_VERSION);
}
