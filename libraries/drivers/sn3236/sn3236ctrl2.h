/**
******************************************************************************
* @file    sn3236ctrl2.h
* @author  hehongxing
* @version V1.0.0
* @date    20-Sep-2016
* @brief   sn3236 controller operation
******************************************************************************
* @attention
******************************************************************************
*/ 
#ifndef __LEDCONTROL2_H_
#define __LEDCONTROL2_H_

#include "platform.h"

//#define SN3236_I2C_DEVICE      1
#define SN3236_2_ID   0x3C//0x7E

OSStatus sn3236_2_sensor_init(void);
OSStatus sn3236_2_sensor_deinit(void);
OSStatus SN3236_2_Write_RegData(uint8_t reg_addr, uint8_t reg_data);

#endif  // __SN3236_H_

