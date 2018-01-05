#ifndef __WIFI_LED_H_
#define __WIFI_LED_H_

/**
******************************************************************************
* @file    wifi_led.h
* @author  hehx_ybzn@163.com
* @version V1.0.0
* @date    12-oct-2016
* @brief   wifi led control api.
******************************************************************************
* @attention
*
******************************************************************************
*/

#include "mico.h"

#define  HEARD_WARE_OLD_VERSION

#define  WIFI_LED_GPIO_R	(MICO_GPIO_7)
#define  WIFI_LED_GPIO_G	(MICO_GPIO_8)
#define  WIFI_LED_GPIO_B	(MICO_GPIO_28)
#define  PANEL_BEEPER_GPIO	(MICO_GPIO_30)

#define  TP_SAME_COMD_DELAY  (50)

typedef struct _WIFI_LED_GPIO_T
{
	bool  onoff;
	mico_gpio_t  gpio;
	int  color;//0:r,1:g,2:b
	bool reserved;
	bool  reserved2;
}WIFI_LED_GPIO_T;

void wifi_led_init();
void wifi_led_onoff(WIFI_LED_GPIO_T *pflag);
void wifi_led_quick_glitter_start(int gpio);
void wifi_led_slow_glitter_start();
void wifi_led_stop();
void panel_beeper_onoff(bool flag);

#endif

