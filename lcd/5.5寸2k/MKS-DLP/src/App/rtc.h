/**
  ******************************************************************************
  * File Name          : RTC.h
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __rtc_H
#define __rtc_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


#define RTC_REG_CLOCKVALID		0x00
#define RTC_REG_FWREADY			0x01



extern RTC_HandleTypeDef hRTC;



void		RTC_Init(void);
void		RTC_Enable(RTC_HandleTypeDef* rtcHandle);
void		RTC_Disable(RTC_HandleTypeDef* rtcHandle);
void		RTC_BKUPWrite(uint32_t BackupRegister, uint32_t Data);
uint32_t	RTC_BKUPRead(uint32_t BackupRegister);

#ifdef __cplusplus
}
#endif
#endif /*__ rtc_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
