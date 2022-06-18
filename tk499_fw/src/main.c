/*
 * main.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "main.h"





static void mainThread(void const *argument);


int main(void)
{
  bspInit();


  osThreadDef(mainThread, mainThread, _HW_DEF_RTOS_THREAD_PRI_MAIN, 0, _HW_DEF_RTOS_THREAD_MEM_MAIN);
  if (osThreadCreate(osThread(mainThread), NULL) == NULL)
  {
    ledInit();

    while(1)
    {
      ledOn(_DEF_LED1);
      delay(100);
      ledOff(_DEF_LED1);
      delay(100);
    }
  }

  osKernelStart();

  return 0;
}

void mainThread(void const *argument)
{
  (void)(argument);

  hwInit();
  apInit();
  apMain();
}

