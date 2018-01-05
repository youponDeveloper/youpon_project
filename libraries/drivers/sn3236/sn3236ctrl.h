/**
******************************************************************************
* @file    sn3236ctrl.h
* @author  hehongxing
* @version V1.0.0
* @date    20-Sep-2016
* @brief   sn3236 controller operation
******************************************************************************
* @attention
******************************************************************************
*/ 
#ifndef __LEDCONTROL_H_
#define __LEDCONTROL_H_

#include "platform.h"

#define SN3236_I2C_DEVICE      0x00
#define SN3236_ID   0x3f//0x78

OSStatus sn3236_sensor_init(void);
OSStatus sn3236_sensor_deinit(void);
OSStatus SN3236_Write_RegData(uint8_t reg_addr, uint8_t reg_data);

#endif  // __SN3236_H_

