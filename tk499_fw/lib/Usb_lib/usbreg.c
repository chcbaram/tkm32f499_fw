/**
******************************************************************************
* @file    usbreg.c
* @author  IC Applications Department
* @version  V0.8
* @date  2019_08_02
* @brief   This file provides all the usbreg firmware functions.
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
#include "usb_hal.h"

volatile uint8 USB_EP_STALL[5];
volatile uint8 USB_EP_IN_STALL[5];
volatile uint8 USB_EP_OUT_STALL[5];
// volatile uint8 usb_setup_flag;
// volatile uint8 usb_out_flag;
// volatile uint8 usb_in_flag;
volatile uint8 USB_FINISH_Flag[5];
// volatile uint8 usb_reset_flag;
// volatile uint8 usb_suspend_flag;
// volatile uint8 usb_resume_flag;
// volatile uint8 usb_state_suspend = 0;
// volatile uint8 usb_state_resume = 0;
volatile USB_STATE_t usb_state;
volatile uint8 usb_ep_flag;
//
volatile uint8 USB_SEND_OPEN_STALL[5];

/********************************************************************************************************
**������Ϣ ��USB_HP_CAN1_TX_IRQHandler(void)
**�������� ��USB�жϴ�������
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void USB_IRQHandler(void)
#ifdef USE_IAR
#else
#ifdef USB_INTR_MODE
    __irq
#endif
#endif
{
  uint32 temp;
  uint32 temp_ep;
  uint32 temp_epn;
  temp            = USB->rINT_STATE; //��ȡUSB�ж�״̬
  USB->rINT_STATE = temp;            //��USB�ж�״̬,д1����
  if (temp & USB_INT_STATE_EPINTF)   //�˵��ж�
  {
    temp_ep = USB->rEP_INT_STATE; //��ȡ�˵��жϺ� 0x1,0x2,0x4,0x8,0x10�ֱ��Ӧ0,1,2,3,4�˵��ж�

    USB->rEP_INT_STATE = temp_ep;    //��˵���ж�
    if (temp_ep & EP_INT_STATE_EP0F) //�˵�0�ж�
    {
      // GPIO_SetBits(GPIOA, GPIO_Pin_5);//���ڴ���
      usb_ep_flag = USB_EP0;             //��¼�˵��
      temp_epn    = USB->rEP0_INT_STATE; //��ȡ�˵�0�ж�״̬λ

      USB->rEP0_INT_STATE = temp_epn; //��˵�0�ж�

      if (temp_epn & EPn_INT_STATE_SETUP) // SETUP�ж�
      {
        //				if(USB_SEND_OPEN_STALL[0]==TRUE)//��ѡ
        //				{
        //				}
        usb_setup_handle(); // setup������,������ƴ���setup�׶�
      }
      if (temp_epn & EPn_INT_STATE_OUTACK) //�˵�0 OUT��Ӧ���жϣ��յ�����
      {
        usb_out_handle(); // OUT������
      }

      if (temp_epn & EPn_INT_STATE_INNACK) // IN����Ӧ���жϣ�׼��д������
      {
        usb_in_handle(); // IN������

        USB->rEP0_INT_STATE |= EPn_INT_STATE_INNACK; //��˵��ж�
      }

      if (temp_epn & (EPn_INT_EN_OUTSTALLIE | EPn_INT_EN_INSTALLIE)) //�˵�رգ����Լ����Զ��庯��
      {
        USB_EP_STALL[0] = TRUE;
      }
      if (temp_epn & EPn_INT_STATE_INSTALL)
      {
        USB_EP_IN_STALL[0] = TRUE;
      }
      if (temp_epn & EPn_INT_STATE_OUTSTALL)
      {
        USB_EP_OUT_STALL[0] = TRUE;
      }
    }
#if ((EPOUT != EPIN))             // if epin != epout,
    if (temp_ep & EPn_INT(EPOUT)) //����˵���ж�,ͬ����˵�0�ж�,�ж��Ƿ�Ϊ����˵�(�������ö˵�3Ϊ����˵�)�ж�
    {
      usb_ep_flag = USB_EPn(EPOUT);

      temp_epn            = USB->rEP3_INT_STATE;
      USB->rEP3_INT_STATE = temp_epn;
      if (temp_epn & EPn_INT_STATE_OUTACK) // OUT��Ӧ���ж�
      {
        usb_out_handle();
      }
      if (temp_epn & EPn_INT_STATE_END) //��������ж�
      {
        USB_FINISH_Flag[EPOUT] = TRUE;
      }
      //			if(temp_epn&EPn_INT_STATE_INNACK)	//����������˵�,�˴�������
      //			{
      //				usb_in_handle();
      //			}
      if (temp_epn & (EPn_INT_EN_OUTSTALLIE | EPn_INT_EN_INSTALLIE))
      {
        USB_EP_STALL[EPOUT] = TRUE;
      }
      if (temp_epn & EPn_INT_STATE_INSTALL)
      {
        USB_EP_IN_STALL[EPOUT] = TRUE;
      }
      if (temp_epn & EPn_INT_STATE_OUTSTALL)
      {
        USB_EP_OUT_STALL[EPOUT] = TRUE;
      }
    }
#endif
    if (temp_ep & EPn_INT(EPIN))
    {
      usb_ep_flag         = USB_EPn(EPIN);
      temp_epn            = USB->rEP1_INT_STATE;
      USB->rEP1_INT_STATE = temp_epn;
      if (temp_epn & EPn_INT_STATE_INNACK) // IN����Ӧ���ж�
      {
        usb_in_handle();
      }
      //			if(temp_epn&EPn_INT_STATE_OUTACK) 	//IN�˵�˴�������
      //			{
      //				usb_out_handle();
      //			}
      if (temp_epn & EPn_INT_STATE_END)
        USB_FINISH_Flag[EPIN] = TRUE;
      if (temp_epn & (EPn_INT_EN_OUTSTALLIE | EPn_INT_EN_INSTALLIE))
      {
        USB_EP_STALL[EPIN] = TRUE;
      }
      if (temp_epn & EPn_INT_STATE_INSTALL)
      {
        USB_EP_IN_STALL[EPIN] = TRUE;
      }
      if (temp_epn & EPn_INT_STATE_OUTSTALL)
      {
        USB_EP_OUT_STALL[EPIN] = TRUE;
      }
    }
  }
  else if (temp & USB_INT_STATE_RSTF) // USB��λ�ж�
  {
    usb_reset_handle();
  }
  else if (temp & USB_INT_STATE_SUSPENDF) // USB�����ж�,�Զ����д
  {
    usb_suspend_handle();
  }
  else if (temp & USB_INT_STATE_RESUMF) // USB�ָ��ж�,�Զ����д
  {
    usb_resume_handle();
  }
}
/********************************************************************************************************
**������Ϣ ��Read_Mreg32( uint32 mreg)
**�������� ��ָ����ַ��32bit����
**������� ��uint32 mreg(��ַ)
**������� ��return(*(volatile uint32 *)mreg);����
**��    ע ��
********************************************************************************************************/
uint32 Read_Mreg32(uint32 mreg)
{
  return (*(volatile uint32 *)mreg);
}
/********************************************************************************************************
**������Ϣ ��Read_Mreg32( uint32 mreg)
**�������� ��ָ����ַд32bit����
**������� ��uint32 mreg(��ַ), uint32 val(����)
**������� ��
**��    ע ��
********************************************************************************************************/
void Write_Mreg32(uint32 mreg, uint32 val)
{
  *(volatile uint32 *)mreg = (uint32)val;
}

/********************************************************************************************************
**������Ϣ ��Read_Mreg32( uint32 mreg)
**�������� ��ָ����ַ��8bit����
**������� ��uint32 mreg(��ַ)
**������� ��
**��    ע ��
********************************************************************************************************/
uint8 read_mreg8(uint32 mreg)
{
  return (*(volatile uint8 *)mreg);
}
/********************************************************************************************************
**������Ϣ ��write_mreg8( uint32 mreg, uint8 val)
**�������� ��ָ����ַд8bit����
**������� ��uint32 mreg(��ַ), uint8 val(����)
**������� ��
**��    ע ��
********************************************************************************************************/
void write_mreg8(uint32 mreg, uint8 val)
{
  *(volatile uint8 *)mreg = (uint8)val;
}
/********************************************************************************************************
**������Ϣ ��usb_delay1ms(uint32 dly)
**�������� ��usb�ڲ���ʱ����ʼ�������Ѳ���
**������� ��uint32 dly(��ʱ����)
**������� ��
**��    ע ��
********************************************************************************************************/
void usb_delay1ms(uint32 dly)
{
  uint32 cnt, i, j, k;

  for (cnt = 0; cnt < dly; cnt++)
  {
    for (i = 0; i < 24; i++)
    {
      for (j = 0; j < 2; j++)
      {
        for (k = 0; k < 100; k++)
          ;
      }
    }
  }
}

/*-------------------------(C) COPYRIGHT 2016 HOLOCENE ----------------------*/
