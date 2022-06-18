/*
 * rtos.c
 *
 *  Created on: 2021. 11. 22.
 *      Author: baram
 */




#include "rtos.h"
#include "bsp.h"
#include "hw_def.h"



void rtosInit(void)
{

}

#ifndef _USE_HW_TOUCHGFX
void vApplicationStackOverflowHook(xTaskHandle xTask,
                                   signed portCHAR* pcTaskName)
{
  logPrintf("StackOverflow : %s\r\n", pcTaskName);
  while (1);
}

void vApplicationMallocFailedHook(xTaskHandle xTask,
                                  signed portCHAR* pcTaskName)
{
  logPrintf("MallocFailed : %s\r\n", pcTaskName);
  while (1);
}
#endif




