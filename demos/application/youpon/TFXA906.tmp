/**
  ******************************************************************************
  * @file    user_config.h 
  * @author  Eshen Wang
  * @version V1.0.0
  * @date    17-Mar-2015
  * @brief   This file contains user configuration.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
  ******************************************************************************
  */ 

#ifndef __USER_CONFIG_H_
#define __USER_CONFIG_H_

#include "mico_config.h"
#include "user_data_store.h"

#define PROTOCOL_COMMAND_ID                         ("command_id")
#define PROTOCOL_CMD                                ("CMD")
#define PROTOCOL_G                                  ("G")
#define PROTOCOL_G_PID                              ("PID")
#define PROTOCOL_G_D                                ("D")
#define PROTOCOL_G_T                                ("T")
#define PROTOCOL_G_N                                ("N")
#define PROTOCOL_G_E                                ("E")
#define PROTOCOL_G_U                                ("U")
#define PROTOCOL_G_DMIN                             ("D_MIN")
#define PROTOCOL_G_DMAX                             ("D_MAX")

#define FOG_V2_PAYLOAD_LEN_MAX              (2048)

#define 	USER_JSON_BEGIN_FLAG		 	'{'
#define 	USER_JSON_END_FLAG		 	'}'
#define 	USER_JSON_PROTOCON_TAB	 	'\r'
#define 	USER_JSON_PROTOCON_TAB2	 	'\n'
#define 	USER_DATA_INIT_DEFAULT_ZERO	 '\0'
#define 	USER_DATA_DOUBLE_QUOTE	 	'\"'

#define 	USER_NUMBER_VAL_FF		 (0xFF)
#define 	USER_NUMBER_VAL_INIT 	 	(-1)
#define 	USER_NUMBER_VAL_0		 (0)
#define 	USER_NUMBER_VAL_1	   	 (1)
#define 	USER_NUMBER_VAL_2		 (2)
#define 	USER_NUMBER_VAL_3		 (3)
#define 	USER_NUMBER_VAL_4		 (4)
#define 	USER_NUMBER_VAL_5		 (5)
#define 	USER_NUMBER_VAL_6		 (6)
#define 	USER_NUMBER_VAL_7		 (7)
#define 	USER_NUMBER_VAL_8		 (8)
#define 	USER_NUMBER_VAL_10		 (10)
#define 	USER_NUMBER_VAL_12		 (12)
#define 	USER_NUMBER_VAL_15		 (15)
#define 	USER_NUMBER_VAL_30		 (25)
#define 	USER_NUMBER_VAL_1_5		 (1.5)
#define 	USER_NUMBER_VAL_96		 (96)
#define 	USER_NUMBER_VAL_32		 (32)
#define 	USER_NUMBER_VAL_64		 (64)
#define 	USER_NUMBER_VAL_128		 (128)
#define 	USER_NUMBER_VAL_0X8C		 (0x8c)
#define 	USER_NUMBER_VAL_255		 (255)
#define	USER_NUMBER_VAL_256		 (256)
#define 	USER_NUMBER_VAL_1023		 (1023)
#define 	USER_NUMBER_VAL_0XFFFF	 (0xFFFF)
#define 	USER_NUMBER_VAL_0XFF00	 (0xFF00)


#define  TIMER_LINKAGE_STATUS_START	1
#define  TIMER_LINKAGE_STATUS_STOP	2
#define  TIMER_LINKAGE_STATUS_DELETE	0

#define 	ZIGBEE_ATTRI_ID_0X0000	 (0x0000)
#define   RS485_DATA_PARAM_TYPE		(0x10)
#define   RS485_DATA_PARAM_TYPE2		(0x20)

#define 	LED_BRIGHTNESS_VALUE		 (0x66)
#define 	SCENE_ACTION_INTERVAL_MS_TIME	(50)
#define 	SCENE_CONTROL_INTERVAL_MS_TIME	(200)


#define 	ZIGBEE_RESP_EVENT_FLAG_01	(1)
#define 	ZIGBEE_RESP_EVENT_FLAG_A	 	(0X0A)
#define 	ZIGBEE_RESP_EVENT_FLAG_B	 	(0X0B)


#define RS485_RXTX_ENABLE 		MICO_GPIO_12

// disalbe FogCloud OTA check when system start
#define DISABLE_FOGCLOUD_OTA_CHECK


/*******************************************************************************
 *                             RESOURCES
 ******************************************************************************/
#define STACK_SIZE_USER_MAIN_THREAD         0x800
#define STACK_SIZE_USER_MSG_HANDLER_THREAD  0x1300
#define STACK_SIZE_NOTIFY_THREAD            0x3200
#define MICO_PROPERTIES_NOTIFY_INTERVAL_MS  1400
#define RS485_UART_RECV_STACK_SIZE_THREAD         0x1600

typedef enum
{
    SCENE_ID_1=1,
    SCENE_ID_2,
    SCENE_ID_3,
    SCENE_ID_4,
    SCENE_ID_MAX
} SCENE_ID_DEFINE_e;



/*User provided configurations*/
//#define CONFIGURATION_VERSION               0x00000008 // if your configuration is changed, update this number

#define DEFAULT_ROM_VERSION                FIRMWARE_REVISION
#define DEFAULT_DEVICE_NAME                MODEL   // device name upload to cloud defined in platform_config.h

#define  USER_DEBUG_	0

#endif  // __USER_CONFIG_H_
