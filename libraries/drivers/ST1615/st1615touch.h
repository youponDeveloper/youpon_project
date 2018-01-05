/**
******************************************************************************
* @file    st1615touch.h
* @author  hehx
* @version V1.0.0
* @date    20-Sep-2016
* @brief   apds9930 controller operation
******************************************************************************
* @attention
*
******************************************************************************
*/ 
#ifndef __ST1615TOUCH_H_
#define __ST1615TOUCH_H_

#include "mico.h"
#include "platform.h"

//#ifndef APDS9930_I2C_DEVICE
  //#define APDS9930_I2C_DEVICE      MICO_I2C_NONE
//#endif

 #define FT6X06_I2C_DEVICE      1

#define ST1615_ID   		(0x55) 
#define GA  0.49              //Glass (or Lens) Attenuation Factor
#define DF  52                //Device Factor, DF = 52 for APDS-9930

//reg map
#define DEVICE_MODE             (0x00)                  //This register is the device mode register, configure it to determine the current mode of the chip
#define GEST_ID                 (0x01)                  //This register describes the gesture of a valid touch
#define TD_STATUS               (0x02)                  //How many points detected
#define P1_XH                   (0x03)                  //This register describes MSB of the X coordinate of the nth touch point and the corresponding event flag
#define P1_XL                   (0x04)                  //This register describes LSB of the X coordinate of the nth touch point
#define P1_YH                   (0x05)                  //This register describes MSB of the Y coordinate of the nth touch point and corresponding touch ID
#define P1_YL                   (0x06)                  //This register describes LSB of the Y coordinate of the nth touch point
#define P2_XH                   (0x09)                  
#define P2_XL                   (0x0A)                  
#define P2_YH                   (0x0B)                  
#define P2_YL                   (0x0C) 
#define ID_G_THGROUP            (0x80)                  //灵敏度
#define ID_G_PERIODACTIVE       (0x88)                  //扫描频率，This register describes the period of active status, it should not less than 12，最大14      
#define ID_G_AUTO_CLB_MODE      (0xA0)                  //This register describes auto calibration mode
#define G_MODE                  (0xA4)
#define ID_G_STATE              (0xA7)                  //This register is used to configure the run mode of TPM.
#define ID_G_FTID               (0xA8)                  //芯片ID
#define ID_G_ERR                (0xA9)                  //This register describes the error code when the TPM is running  


//触摸点事件
#define EVENT_PRESS_DOWN        (0) 
#define EVENT_LIFT_UP           (1)
#define EVENT_CONTACT           (2)
#define EVENT_NONE              (3)
//手势
#define GUSTURE_MOVE_UP         (0x10)
#define GUSTURE_MOVE_RIGHT      (0x14)
#define GUSTURE_MOVE_DOWN       (0x18)
#define GUSTURE_MOVE_LEFT       (0x1C)
#define GUSTURE_MOVE_ZOOM_IN    (0x48)
#define GUSTURE_MOVE_ZOOM_OUT   (0x49)
#define GUSTURE_NONE            (0x00)
 
#define CT_READ_START           (GEST_ID)               //每次检测到触摸屏中断信号时从哪个寄存器开始读取数据
#define CT_READ_LENGTH          (12)                    //每次检测到触摸屏中断信号时读取多少个数据
#define CT_READ_NO_GEST         (0)                     //手势在读出数据的位置
#define CT_READ_NO_TD_STATUS    (CT_READ_NO_GEST+1)     //有效点数
#define CT_READ_NO_P1           (CT_READ_NO_TD_STATUS+1)
#define CT_READ_NO_P2           (CT_READ_NO_P1+6)
 
#define MAX_TOUCH_POINT     (2)


typedef enum{
	XY_COORD_H,
	X_COORD_L,
	Y_COORD_L,
	PIXEL_DATA_LENGTH_B,
	PIXEL_DATA_LENGTH_A,
}PIXEL_DATA_FORMAT;

typedef enum{
	FIRMWARE_VERSION,
	STATUS_REG,
	DEVICE_CONTROL_REG,
	TIMEOUT_TO_IDLE_REG,
	XY_RESOLUTION_HIGH,
	X_RESOLUTION_LOW,
	Y_RESOLUTION_LOW,
	DEVICE_CONTROL_REG2 = 0x09,
	FIRMWARE_REVISION_3 = 0x0C,
	FIRMWARE_REVISION_2,
	FIRMWARE_REVISION_1,
	FIRMWARE_REVISION_0,
	FINGERS,
	KEYS_REG,
	XY0_COORD_H,
	X0_COORD_L,
	Y0_COORD_L,
	I2C_PROTOCOL = 0x3E,
	MAX_NUM_TOUCHES,
	DATA_0_HIGH,
	DATA_0_LOW,
	MISC_CONTROL = 0xF1,
	SMART_WAKE_UP_REG = 0xF2,
	CHIP_ID = 0xF4,
	PAGE_REG = 0xff,
}RegisterOffset;

#define ST_MAX_TOUCH_POINTS		3


#if 0
typedef struct
{   
    uint8_t M_EventFlag;        //触摸点事件
    uint8_t M_TouId; 
    unsigned short M_Xpos;
    unsigned short M_Ypos;
    unsigned short M_PreXpos;         //上一次的点位置
    unsigned short M_PreYpos;
    unsigned short M_Tmp;
}ST_CTPoint;
 
typedef struct
{
    uint8_t M_nPoint;           //当前有效，触摸点个数,对于FT6X06最多两个
    uint8_t M_Gesture;          //手势             
    ST_CTPoint M_pStCTPoint[MAX_TOUCH_POINT]; 
}ST_CTTouchStatus;
#endif

typedef struct {
	unsigned char y_h: 3,
	reserved: 1,
	x_h: 3,
	valid: 1;
	unsigned char x_l;
	unsigned char y_l;
	unsigned char z;
} xyz_data_t;

typedef struct {
	unsigned char fingers: 4,
	reserved: 4;
	unsigned char keys;
	xyz_data_t xyz_data[2];
} stx_report_data_t;



typedef struct
{   
    //uint8_t M_EventFlag;        //触摸点事件
    //uint8_t M_TouId; 
    unsigned short M_Xpos;
    unsigned short M_Ypos;
    unsigned short M_PreXpos;         //上一次的点位置
    unsigned short M_PreYpos;
   // unsigned short M_Tmp;
}ST1615_CTPoint;
 
typedef struct
{
    uint8_t M_nPoint;           //当前有效，触摸点个数,对于FT6X06最多两个
    uint8_t M_Gesture;          //手势   
    uint8_t M_key;
    uint8_t M_updown;
    ST1615_CTPoint M_pStCTPoint[MAX_TOUCH_POINT]; 
    uint32_t time_stamp;
}ST_CTTouchStatus;

OSStatus st1615_touchpanel_sensor_init(void);
OSStatus st1615_touchpanel_sensor_deinit(void);
OSStatus st1615_touchpanel_data_readout(ST_CTTouchStatus *pTouchStatus);
void st1615_touchpanel_enable(void);
int st1615_setSmartWakeupSta(void);
void st1615_swk_set_swk_enable();
int st1615_ts_set_powerdown_bit(int value);
int st1615_ts_resume();
int st1615_ts_suspend();
 void st1615_tp_data_readout(unsigned char *valid, unsigned char *Xpos, unsigned char *Ypos);


#endif  // __FT6X06_TOUCHPANEL_H_

