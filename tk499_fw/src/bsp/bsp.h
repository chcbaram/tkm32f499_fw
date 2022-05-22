/*
 * bsp.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "def.h"


#include "HAL_conf.h"


void logPrintf(const char *fmt, ...);

#if 1
#define LOG_ERROR_(fmt, ...) \
    logPrintf("%s:%d:error: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) LOG_ERROR_(__VA_ARGS__, "")
#else
#define LOG_ERROR(...)
#endif


void bspInit(void);

void delay(uint32_t ms);
uint32_t millis(void);


#ifdef __cplusplus
 }
#endif

#endif /* SRC_BSP_BSP_H_ */
