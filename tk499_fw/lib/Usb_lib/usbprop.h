/**
******************************************************************************
* @file    usbprop.h
* @author  IC Applications Department
* @version  V0.8
* @date  2019_08_02
* @brief   This file contains all the usbprop inc file for the library.
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
#ifndef __usb_prop_H
#define __usb_prop_H

#include "hw_def.h"

typedef struct
{
  unsigned int bitrate;
  unsigned char format;
  unsigned char paritytype;
  unsigned char datatype;
} LINE_CODING;

unsigned int UsbVcomRec(unsigned char *rxInfo);

bool     cdcIfInit(void);
uint32_t cdcIfAvailable(void);
uint8_t  cdcIfRead(void);
uint32_t cdcIfGetBaud(void);
uint32_t cdcIfWrite(uint8_t *p_data, uint32_t length);
bool     cdcIfIsConnected(void);
uint8_t  cdcIfGetType(void);

#endif

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/*-------------------------(C) COPYRIGHT 2016 HOLOCENE ----------------------*/
