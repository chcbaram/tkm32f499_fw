/**
******************************************************************************
* @file    ctrlusb.c
* @author  IC Applications Department
* @version  V0.8
* @date  2019_08_02
* @brief   This file provides all the ctrlusb firmware functions.
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
#include "usb_hal.h"

volatile uint8 usb_running_state;      // running stage
volatile uint8 usb_running_ctrl_state; // running stage
volatile uint8 bmRequestType;
volatile uint8 bRequest;
volatile uint16 wValue;
volatile uint16 wIndex;
volatile uint16 wLength;
volatile uint8 req_dir;   // bmRequestType[7];
volatile uint8 req_type;  // bmRequestType[6:5];
volatile uint8 req_recip; // bmRequestType[4:0];
//=========================== Each Request =============================

//=================================== Handler================================
#if 1

void (*StandardDeviceRequest[MAX_STD_REQUEST])(void) =
    {
        Chap9_GetStatus,        // 0x00
        Chap9_ClearFeature,     // 0x01
        MLsup_StallEP0,         // reserve
        Chap9_SetFeature,       // 0x03
        MLsup_StallEP1,         // reserve
        Chap9_SetAddress,       // 0x05
        Chap9_GetDescriptor,    // 0x06
        Chap9_SetDescriptor,    // 0x07
        Chap9_GetConfiguration, // 0x08
        Chap9_SetConfiguration, // 0x09
        Chap9_GetInterface,     // 0x0a
        Chap9_SetInterface,     // 0x0b
};
#endif
/********************************************************************************************************
**������Ϣ ��usb_setup_handle( void)
**�������� �����ƴ���setup�׶����ݶ�ȡ,��Э����ص�ֵ������ȫ�ֱ���
**������� ��
**������� ��
**��    ע ��USB����ѯ���Ƶ����ߣ����еĲ���������host��ʼ
********************************************************************************************************/
void usb_setup_handle(void)
{
  uint8 temp;
  if (usb_running_ctrl_state == USB_IDLE)
  {
    // bmrequest type one byte
    bmRequestType = USB->rSETUP[0]; //��ȡsetup�����ֽ�,��¼host��������
    // brequest one byte

    bRequest = USB->rSETUP[1];
    // wvalue two bytes

    temp = USB->rSETUP[2];

    wValue = USB->rSETUP[3];
    wValue = (wValue << 8) + temp;
    // windex two bytes

    temp = USB->rSETUP[4];

    wIndex = USB->rSETUP[5];

    wIndex = (wIndex << 8) + temp;
    // wlength two bytes

    temp = USB->rSETUP[6];

    wLength = USB->rSETUP[7];
    wLength = (wLength << 8) + temp;
    // direction
    req_dir = bmRequestType & USB_REQ_DIR_MASK;
    // recipient
    req_recip = bmRequestType & USB_REQ_RECIP_MASK;
    // type
    req_type = bmRequestType & USB_REQ_TYPE_MASK;
    //		for(i = 0;i < 8 ;i ++)
    //		{
    //			//printf("%x ",USB->rSETUP[i]);//��ӡsetup����
    //		}

#if 0
        printf("bmRequestType = 0x%x\r\n", bmRequestType);
        printf("bRequest = 0x%x\r\n", bRequest);
        printf("wValue = 0x%x\r\n", wValue);
        printf("wIndex = 0x%x\r\n", wIndex);
        printf("wLength = 0x%x\r\n", wLength);
#endif

    usb_running_ctrl_state = USB_CTRL_SETUP;
  }
  ctrl_deal_handle();
}

/********************************************************************************************************
**������Ϣ ��ctrl_deal_handle(void)
**�������� �����ƴ������ݼ�USB״̬����
**������� ��
**������� ��
**��    ע ����¼USB״̬����USB��ȷ��Ӧ
********************************************************************************************************/

void ctrl_deal_handle(void)
{
  if (req_type == USB_REQ_TYPE_STANDARD) // standard request type
  {
    if (bRequest < MAX_STD_REQUEST) // handle 0--12
    {
      (*StandardDeviceRequest[bRequest])(); //���ñ�׼������
    }
  }
  else if (req_type == USB_REQ_TYPE_CLASS) // class request type
  {
    switch (bRequest) //������ɸ��ݾ����豸���ͱ�д,��μ�������Э��
    {
      //�˴��ο�USB_CDCЭ��Requests  Abstract Control Model
      case 0x20: // set_line_coding 20h�����ò����ʣ�ֹͣλ��У��λ������λ
      {
        Class_Set_Line_Coding();
        break;
      }
      case 0x21: // get_line_coding 21h��get_line_coding����ʵʱ������...
      {
        Class_Get_Line_Coding();
        break;
      }
      case 0x22:
      {
        Class_Set_Control_Line_State(); //����״̬,����������
        break;
      }
      default:
      {
        usb_running_ctrl_state = USB_IDLE;
        break;
      }
    }
  }
}
/********************************************************************************************************
**������Ϣ ��ctrl_in_token_handle( void)
**�������� �����ƴ���IN������
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void ctrl_in_token_handle(void)
{
  if (usb_running_ctrl_state == USB_CTRL_STATUS)
  {
    EP0_Send_Empty_Packet();
    usb_running_ctrl_state = USB_IDLE;
  }
  else
  {
    ctrl_deal_handle();
  }
}
/********************************************************************************************************
**������Ϣ ��ctrl_out_handle( void)
**�������� �����ƴ���OUT������
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void ctrl_out_handle(void)
{

  if (req_dir == USB_REQ_DIR_OUT) // data stage when control write
  {
    ctrl_deal_handle();
  }
  else // status stage when control read, IP will send ack auto
  {
    // printf("dir_nout\r\n");
  }
}
/********************************************************************************************************
**������Ϣ ��USB_Init( void)
**�������� ��USB��ʼ��
**������� ��
**������� ��
**��    ע ������ʹ�ö˵�0(Ĭ�϶˵�),1(��ΪIN�˵�),3(��ΪOUT�˵�)
********************************************************************************************************/
void USB_Init(void)
{
  uint8 i;
  //	uint32 tmpdata = 0x0;
  //	tmpdata = tmpdata;

  //	usb_setup_flag = 0;
  //	usb_out_flag = 0;
  //	usb_in_flag = 0;
  //	usb_reset_flag = 0;
  //	usb_suspend_flag = 0;
  //	usb_resume_flag = 0;

  usb_ep_flag = USB_EP0;
  //	write_finish = 1;
  //	dma_finished_flag[0] = 0;
  //	dma_finished_flag[1] = 0;
  usb_state = USB_STATE_POWERED;
  usb_running_state = USB_IDLE;
  usb_running_ctrl_state = USB_IDLE;
  for (i = 0; i < 5; i++)
  {
    USB_EP_STALL[i] = FALSE;
    USB_EP_IN_STALL[i] = FALSE;
    USB_EP_OUT_STALL[i] = FALSE;
    USB_FINISH_Flag[i] = FALSE;
    USB_SEND_OPEN_STALL[i] = FALSE;
  }

  USB->rTOP = USB_TOP_RESET; // reset usb
  USB->rTOP &= ~USB_TOP_RESET;
  USB->rTOP &= ~USB_TOP_CONNECT; // usb disconnect

  USB->rINT_STATE |= 0; //��USB״̬
  USB->rEP_INT_STATE |= 0;
  USB->rEP0_INT_STATE |= 0;
  USB->rEP1_INT_STATE |= 0;
  USB->rEP2_INT_STATE |= 0;
  USB->rEP3_INT_STATE |= 0;
  USB->rEP4_INT_STATE |= 0;

  USB->rEP0_CTRL = 0; //������
  USB->rEP1_CTRL = 0;
  USB->rEP2_CTRL = 0;
  USB->rEP3_CTRL = 0;
  USB->rEP4_CTRL = 0;

  USB->rINT_EN = USB_INT_EN_RSTIE | USB_INT_EN_SUSPENDIE | USB_INT_EN_RESUMIE | USB_INT_EN_EPINTIE; //ʹ��USB����жϣ���λ�����𣬻ָ����˵��ж�

  USB->rEP_INT_EN = EP_INT_EN_EP0IE | EP_INT_EN_EP1IE | EP_INT_EN_EP3IE;                                                                               //ʹ��USB�˵��жϣ��˵�0��1��3
  USB->rEP0_INT_EN = EPn_INT_EN_SETUPIE | EPn_INT_EN_ENDIE | EPn_INT_EN_INNACKIE | EPn_INT_EN_OUTACKIE | EPn_INT_EN_OUTSTALLIE | EPn_INT_EN_INSTALLIE; //ʹ��USB�˵�0�жϣ�setup��������ɣ�INNACK,OUTACK,INSTALL,OUTSTALL�ж�

  USB->rEP1_INT_EN = EPn_INT_EN_ENDIE | EPn_INT_EN_INNACKIE | EPn_INT_EN_INSTALLIE | EPn_INT_EN_OUTSTALLIE; //���ö˵�1Ϊ����˵�(��ö�ٽ׶��ڶ˵���������Ӧ);
  USB->rEP3_INT_EN = EPn_INT_EN_ENDIE | EPn_INT_EN_OUTACKIE | EPn_INT_EN_INSTALLIE | EPn_INT_EN_OUTSTALLIE; //���ö˵�3Ϊ����˵㣬����Ӧ��ʽ�����˵㷽��;

  USB->rEP_EN = EP_EN_EP0EN | EP_EN_EP2EN | EP_EN_EP1EN | EP_EN_EP3EN | EP_EN_EP4EN; //���˵�0&1&3�ж�

  /////���ø���˵�ɰ������淽ʽ����

  USB->rADDR = 0; // USB�豸��ַĬ��Ϊ0

  USB->rTOP = USB_TOP_CONNECT | ((~USB_TOP_SPEED) & 0x01); //����USB���빤��ģʽ
  USB->rPOWER = USB_POWER_SUSPEN | USB_POWER_SUSP;         //�ɿ�������������״̬
}

/********************************************************************************************************
**������Ϣ ��usb_reset_handle(void)
**�������� ��USB��λ
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/

void usb_reset_handle(void)
{
  uint32 val;
  (void)val;
  USB->rTOP |= USB_TOP_RESET; // reset usb
  USB->rTOP &= ~USB_TOP_RESET;
  val = USB->rTOP;
  usb_state = USB_STATE_DEFAULT;
}

/********************************************************************************************************
**������Ϣ ��usb_suspend_handle(void)
**�������� ��USB�������
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void usb_suspend_handle(void)
{
  uint32 val;
  (void)val;
  if (usb_state > USB_STATE_ADDRESS) // note it
  {
    USB->rPOWER = USB_POWER_SUSPEN; // enter suspend state, will shutdown phy clock
    val = USB->rTOP;

    /***************JUST FOR RESUME TEST START*******************/
    usb_delay1ms(10000); // 10s

    usb_wakeup_handle();

    /***************JUST FOR RESUME TEST END*********************/
    usb_state = USB_STATE_SUSPENDED;
  }
}
/********************************************************************************************************
**������Ϣ ��usb_resume_handle(void)
**�������� ��USB�ָ�����
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void usb_resume_handle(void)
{
  uint32 val;
  (void)val;

  //	val = USB->rTOP;
  //	#ifdef printf
  //	printf("USB got resume 0x%lx\r\n",val);
  //	#endif
}
/********************************************************************************************************
**������Ϣ ��usb_wakeup_handle(void)
**�������� ��USB����
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void usb_wakeup_handle(void)
{
  uint32 val;
  (void)val;

  val = USB->rTOP;

  USB->rPOWER = USB_POWER_SUSPEN | USB_POWER_SUSP;                  //���빤��ģʽ
  USB->rPOWER = USB_POWER_WKUP | USB_POWER_SUSPEN | USB_POWER_SUSP; //����
  usb_delay1ms(3);                                                  // 3ms wait bus wakeup

  USB->rPOWER = USB_POWER_SUSPEN | USB_POWER_SUSP; //���빤��ģʽ
  val = USB->rTOP;
  usb_state = USB_STATE_CONFIGURED;
}

/*-------------------------(C) COPYRIGHT 2016 HOLOCENE ----------------------*/
