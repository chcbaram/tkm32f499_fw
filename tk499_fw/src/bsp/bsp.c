/*
 * bsp.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "bsp.h"
#include "uart.h"


static volatile uint32_t systick_counter = 0;
static uint32_t SystemCoreClock = 240000000;


void SysTick_Handler(void)
{
  systick_counter++;
}



void bspInit(void)
{
  int i;


  RCC->AHB1ENR |= 1<<13;//bkp clk,enable sram

  for(i = 0;i<90;i++)
  { 
      NVIC_DisableIRQ((IRQn_Type)i); 
  }
	SCB->VTOR = 0;
	SCB->VTOR |= 0X20000000;
	for(i = 0;i < 512;i+=4)
	*(u32*)(T_SRAM_BASE + i) = *(u32*)(T_SDRAM_BASE+i);


  //-- PLL 12MHz*20 = 240MHz
  //
	SystemClk_HSEInit(RCC_PLLMul_20);     
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  //-- SysTick 1ms interrupt
  //
  getSystemClock(&SystemCoreClock);
  SysTick_Config(SystemCoreClock/1000);  

  __enable_irq();


  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
}

void delay(uint32_t ms)
{
#ifdef _USE_HW_RTOS
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    osDelay(ms);
  }
  else
  {
    HAL_Delay(ms);
  }
#else
  uint32_t pre_time = systick_counter;

  while(systick_counter-pre_time < ms);  
#endif
}

uint32_t millis(void)
{
  return systick_counter;
}

int __io_putchar(int ch)
{
  //uartWrite(_DEF_UART1, (uint8_t *)&ch, 1);
  return 1;
}



