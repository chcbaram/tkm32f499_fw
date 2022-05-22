
#include "usb.h"
#include "cli.h"
#include "usb_hal.h"
#include "cdc.h"


static bool is_init = false;
static UsbMode_t is_usb_mode = USB_NON_MODE;





bool usbInit(void)
{  


  return true;
}

void usbDeInit(void)
{
  if (is_init == true)
  {
    //USBD_DeInit(&hUsbDeviceFS);
  }
}

bool usbIsOpen(void)
{
  return cdcIsConnect();
}

bool usbIsConnect(void)
{
  if (usb_state != USB_STATE_CONFIGURED)
  {
    return false;
  }

  return true;
}

UsbMode_t usbGetMode(void)
{
  return is_usb_mode;
}

UsbType_t usbGetType(void)
{
  return (UsbType_t)cdcGetType();
}

bool usbBegin(UsbMode_t usb_mode)
{
  is_usb_mode = usb_mode;

  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE); //ʹ��USBʱ��

  USB_GPIO();
  delay(100);

  NVIC_InitStructure.NVIC_IRQChannel = USB_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  cdcInit();

  USB_Init();

  is_init = true;

  return true;
}

