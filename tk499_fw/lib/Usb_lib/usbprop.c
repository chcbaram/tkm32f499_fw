/**
******************************************************************************
* @file    usbprop.c
* @author  IC Applications Department
* @version  V0.8
* @date  2019_08_02
* @brief   This file provides all the usbprop firmware functions.
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

#include "usbprop.h"
#include "HAL_device.h"
#include "usb_hal.h"
#include "qbuffer.h"



unsigned char *pucClassDrcData = NULL;

//���⴮�ڲ���
LINE_CODING linecoding =
{
    460800, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
};

static qbuffer_t q_rx;
static qbuffer_t q_tx;

static uint8_t q_rx_buf[2048];
static uint8_t q_tx_buf[2048];

static bool is_opened = false;
static bool is_rx_full = false;
static uint8_t cdc_type = 0;

static uint8_t rx_buf[128];


bool cdcIfInit(void)
{
  is_opened = false;
  qbufferCreate(&q_rx, q_rx_buf, 2048);
  qbufferCreate(&q_tx, q_tx_buf, 2048);

  return true;
}

uint32_t cdcIfAvailable(void)
{
  return qbufferAvailable(&q_rx);
}

uint8_t cdcIfRead(void)
{
  uint8_t ret = 0;

  qbufferRead(&q_rx, &ret, 1);

  return ret;
}

uint32_t cdcIfWrite(uint8_t *p_data, uint32_t length)
{
  uint32_t pre_time;
  uint32_t tx_len;
  uint32_t buf_len;
  uint32_t sent_len;


  if (cdcIfIsConnected() != true) return 0;


  sent_len = 0;

  pre_time = millis();
  while(sent_len < length)
  {
    buf_len = (q_tx.len - qbufferAvailable(&q_tx)) - 1;
    tx_len = length - sent_len;

    if (tx_len > buf_len)
    {
      tx_len = buf_len;
    }

    if (tx_len > 0)
    {
      qbufferWrite(&q_tx, p_data, tx_len);
      p_data += tx_len;
      sent_len += tx_len;
    }
    else
    {
      delay(1);
    }
    
    if (cdcIfIsConnected() != true)
    {
      break;
    }

    if (millis()-pre_time >= 100)
    {
      break;
    }
  }

  return sent_len;
}

uint32_t cdcIfGetBaud(void)
{
  return linecoding.bitrate;
}

bool cdcIfIsConnected(void)
{
  if (pucClassDrcData == NULL)
  {
    return false;
  }
  if (is_opened == false)
  {
    return false;
  }
  if (usb_state != USB_STATE_CONFIGURED)
  {
    return false;
  }

  return true;
}

uint8_t cdcIfGetType(void)
{
  return cdc_type;
}

/********************************************************************************************************
**������Ϣ ��USBCDC_senddata(unsigned char *data,unsigned short length)
**�������� ������ת��
**������� ��unsigned char *data(ת��������ָ��),unsigned short length(ת�����ݳ���)
**������� ��
**��    ע ��
********************************************************************************************************/
void USBCDC_senddata(unsigned char *data, unsigned short length)
{
  if (linecoding.datatype == 7)
  {
    while (length)
    {
      *data &= 0x7f;
      length--;
      data++;
    }
  }
  else if (linecoding.datatype == 8) //������
  {
    while (length)
    {
    }
  }
}

/********************************************************************************************************
**������Ϣ ��Class_Get_Line_Coding(void)
**�������� ��CDC Abstract Control Model(���⴮��) ������GET_LINE_CODING
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void Class_Get_Line_Coding(void) // bRequest = 0x21
{
  int count;
  switch (usb_running_ctrl_state)
  {
  case USB_CTRL_SETUP:
    if (req_dir == USB_REQ_DIR_IN)
    {
      pucClassDrcData = (u8 *)&linecoding;
      usb_running_ctrl_state = USB_CTRL_IN;
    }
    else
    {
      usb_running_ctrl_state = USB_IDLE;
    }
    break;

  case USB_CTRL_IN:

    while (USB->rEP0_CTRL & 0x80)
      ;
    for (count = 0; count < 7; count++)
    {
      USB->rEP0_FIFO = *pucClassDrcData;
      pucClassDrcData++;
    }
    USB->rEP0_CTRL = 0x87;

    usb_running_ctrl_state = USB_IDLE;
    break;

  default:
    //		usb_running_ctrl_state = USB_IDLE ;
    break;
  }
}
unsigned char baud_read[64];
/********************************************************************************************************
**������Ϣ ��Class_Get_Line_Coding(void)
**�������� ��CDC Abstract Control Model(���⴮��) ������SET_LINE_CODING
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void Class_Set_Line_Coding(void) // bRequest = 0x20
{
  int count, i;
  uint32_t bitrate;

  switch (usb_running_ctrl_state)
  {
  case USB_CTRL_SETUP:
    if (req_dir == USB_REQ_DIR_OUT)
    {
      usb_running_ctrl_state = USB_CTRL_OUT;
    }
    else
    {
      usb_running_ctrl_state = USB_IDLE;
    }
    break;

  case USB_CTRL_OUT:
    count = USB->rEP0_AVIL;
    for (i = 0; i < count; i++)
    {
      baud_read[i] = USB->rEP0_FIFO;
    }
    bitrate = ((u32)baud_read[0] << 0) | ((u32)baud_read[1] << 8) | ((u32)baud_read[2] << 16) | ((u32)baud_read[3] << 24);
    linecoding.format  = baud_read[4];
    linecoding.paritytype = baud_read[5];
    linecoding.datatype = baud_read[6];
    linecoding.bitrate = bitrate = (bitrate%10);
    
    cdc_type = bitrate %10;
    usb_running_ctrl_state = USB_CTRL_STATUS;
    break;

  default:
    //			usb_running_ctrl_state = USB_IDLE ;
    break;
  }
}
/********************************************************************************************************
**������Ϣ ��Class_Get_Line_Coding(void)
**�������� ��CDC Abstract Control Model(���⴮��) ������SET_LINE_CODING
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
typedef  struct  usb_setup_req
{
  uint8_t   bmRequest;
  uint8_t   bRequest;
  uint16_t  wValue;
  uint16_t  wIndex;
  uint16_t  wLength;
} USBD_SetupReqTypedef;

void Class_Set_Control_Line_State(void) // bRequest = 0x22
{
  int count, i;
  USBD_SetupReqTypedef *req;

  switch (usb_running_ctrl_state)
  {
  case USB_CTRL_SETUP: //���ƴ���״̬�׶�
    if (req_dir == USB_REQ_DIR_OUT)
    {
      usb_running_ctrl_state = USB_CTRL_OUT;
    }
    else
    {
      usb_running_ctrl_state = USB_IDLE;
    }
    break;
  case USB_CTRL_OUT:
    count = USB->rEP0_AVIL;
    for (i = 0; i < count; i++)
    {
      baud_read[i] = USB->rEP0_FIFO; 
    }
    
    req = (USBD_SetupReqTypedef *)baud_read;
    // TODO : 나중에 다른 터미널에서 문제 없는지 확인 필요
    is_opened = req->wValue&0x01;  // 0 bit:DTR, 1 bit:RTS
    /*
    if (req->wValue == 0x03)
      is_opened = true;
    if (req->wValue == 0x02)
      is_opened = false;
    */
    usb_running_ctrl_state = USB_CTRL_STATUS;
    break;

  default:
  {
    //	        usb_running_ctrl_state = USB_IDLE ;
    break;
  }
  }
}

unsigned char ep1_Tx_dataBuf[2048]; // USB ���ͻ�����,���Զ����С
unsigned int ui_curTxLen = 0;       //ʵʱ���ͳ���
unsigned int ep1_Tx_dataLen = 0;    //���������ܳ���(С���Զ��建������С)
unsigned char ep1_tx_flag = 0;      //���ͱ�־

/********************************************************************************************************
**������Ϣ ��EP1_USB_IN_Data(void)
**�������� ��USB�ӷ��ͻ�����������ͨ��IN�˵㷢��
**������� ��
**������� ��
**��    ע ��USB���ݷ���ʱ�����ݳ������˵�sizeʱ����Ҫ�������
********************************************************************************************************/
void EP1_USB_IN_Data(void) // bRequest = 0x22
{
  #if 1
  //-- TX
  //
  uint32_t tx_len;
  tx_len = qbufferAvailable(&q_tx);
  if (tx_len == 0)
  {
    return;
  }
  if (tx_len > 64)
  {
    tx_len = 64;
  }
  while (USB->rEP1_CTRL & 0x80)
    ;

  qbufferRead(&q_tx, ep1_Tx_dataBuf, tx_len);

  for (int i = 0; i<tx_len; i++)
  {
    USB->rEP1_FIFO = ep1_Tx_dataBuf[i];
  }
  USB->rEP1_CTRL = 0x80 | tx_len;

  tx_len = qbufferAvailable(&q_tx);
  if (tx_len == 0)
  {
    usb_running_ctrl_state = USB_IDLE;
  }
  #else
  int count, i;

  if (ep1_tx_flag != 0) // tx_buf�ǿ�
  {
    while (USB->rEP1_CTRL & 0x80)
      ;
    if (ep1_Tx_dataLen > 64) // USB���ݲ�ַ���
    {
      count = 64;
      ep1_Tx_dataLen -= 64;
    }
    else
    {
      count = ep1_Tx_dataLen;
      ep1_Tx_dataLen = 0;
    }
    for (i = 0; i < count; i++)
    {
      USB->rEP1_FIFO = *(ep1_Tx_dataBuf + ui_curTxLen + i);
    }
    USB->rEP1_CTRL = 0x80 | count;
    if (ep1_Tx_dataLen == 0)
    {
      ep1_tx_flag = 0;
      ui_curTxLen = 0;
      usb_running_ctrl_state = USB_IDLE;
    }
    else
    {
      ui_curTxLen += count;
    }
  }
  #endif
}
/********************************************************************************************************
**������Ϣ ��UsbVcomSend(unsigned char*Info,unsigned int *infoLenth)
**�������� ����Ҫ���͵�����д��USB���ͻ�����
**������� ��unsigned char*Info(��������ָ��),unsigned int *infoLenth(�������ݳ���ָ��)
**������� ��return status(0��ʾ�������,1��ʾ���ڷ���);
**��    ע ��
********************************************************************************************************/
char UsbVcomSend(unsigned char *Info, unsigned int *infoLenth)
{
  unsigned int i, status = 0;
  if ((ep1_tx_flag == 0) && (*infoLenth != 0))
  {
    ep1_Tx_dataLen = *infoLenth;
    *infoLenth = 0;
    for (i = 0; i < ep1_Tx_dataLen; i++)
    {
      ep1_Tx_dataBuf[i] = *(Info + i);
    }
    ep1_tx_flag = 1;
  }
  else
  {
    status = 1;
  }
  return status;
}
unsigned char ep3_rx_flag = 0; //�˵�3���ձ�־
/********************************************************************************************************
**������Ϣ ��EP3_USB_OUT_Data(void)
**�������� ��USB_OUT�˵���յ���Ч����,���ձ�־λ��1
**������� ��
**������� ��
**��    ע ��
********************************************************************************************************/
void EP3_USB_OUT_Data(void) // bRequest = 0x22
{
  uint8_t len;
  
  len = USB->rEP3_AVIL & 0x7f;
  for (int i = 0; i < len; i++)
  {
    rx_buf[i] = USB->rEP3_FIFO;
  }

  qbufferWrite(&q_rx, rx_buf, len);


  uint32_t buf_len;

  buf_len = (q_rx.len - qbufferAvailable(&q_rx)) - 1;
  if (buf_len < 64)
  {
    is_rx_full = true;
  }

  usb_running_ctrl_state = USB_IDLE;
}
/********************************************************************************************************
**������Ϣ ��UsbVcomRec(unsigned char*rxInfo)
**�������� ����USB device�� OUT�˵��������
**������� ��unsigned char*rxInfo(������Ϣbuf)
**������� ��
**��    ע ��
********************************************************************************************************/
unsigned int UsbVcomRec(unsigned char *rxInfo)
{
  unsigned int i, temp_len;
  if (ep3_rx_flag == 1) //����buf�ǿ�
  {
    temp_len = USB->rEP3_AVIL & 0x7f;
    for (i = 0; i < temp_len; i++)
    {
      *(rxInfo + i) = USB->rEP3_FIFO;
    }
    ep3_rx_flag = 0;
  }
  else
  {
    return 0;
  }
  return temp_len;
}

/*-------------------------(C) COPYRIGHT 2016 HOLOCENE ----------------------*/
