/**
******************************************************************************
* @file    usbapp.c
* @author  IC Applications Department
* @version  V0.8
* @date  2019_08_02
* @brief   This file provides all the usbapp firmware functions.
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, HOLOCENE SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2016 HOLOCENE</center></h2>
*/
#include "HAL_device.h"
#include "bsp.h"
#include "stdio.h"
#include "usb_hal.h"
/********************************************************************************************************
**������Ϣ �� USB_GPIO
**�������� �� USB IO��ʼ��
**������� ��
**������� ��
********************************************************************************************************/
void USB_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIOA->AFRH &= 0xFFF00FFF;
  GPIOA->AFRH |= 0x000AA000;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; // PA11&PA12
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC->APB1ENR |= 1 << 28; // USB CLOCK ENABLE;
  RCC->CFGR |= 4 << 17;    // USB ʱ�ӣ���Ƶ��4+1�η�Ƶ
}
/********************************************************************************************************
**������Ϣ �� usb_test
**�������� �� USB�����ʼ��
**������� ��
**������� ��
********************************************************************************************************/
void usb_test(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE); //ʹ��USBʱ��
  printf("This is a USB Demo \r\n");
  USB_GPIO();

  //����USB�ж�
  NVIC_InitStructure.NVIC_IRQChannel = USB_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USB_Init();
  printf("This is %x \r\n", (int)*(u32 *)0x40010804); //���ڲ���
}

/*-------------------------(C) COPYRIGHT 2016 HOLOCENE ----------------------*/
