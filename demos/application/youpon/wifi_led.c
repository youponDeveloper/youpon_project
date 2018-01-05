/**
******************************************************************************
* @file    wifi_led.c
* @author  hehx_ybzn@163.com
* @version V1.0.0
* @date    12-oct-2016
* @brief   wifi led control api.
******************************************************************************
* @attention
*
******************************************************************************
*/

#include "wifi_led.h"
#include "zigbeeparse.h"

#define SYS_LED_TRIGGER_WIFI_INTERVAL    1000
#define SYS_LED_TRIGGER_WIFI_QUICK_INTERVAL    100
#define SYS_LED_5_SECONDS    (60*1000)

#define user_wifiled(M, ...) custom_log("wifi_led", M, ##__VA_ARGS__)

extern mico_semaphore_t uart_idle_irq_sem2;

extern OSStatus pushAppMainRecvMsgProcess(unsigned char *msg, unsigned int inBufLen, int riu);

void wifi_led_init()
{ 
	mico_rtos_init_semaphore( &uart_idle_irq_sem2, 1 );
}

void wifi_led_slow_glitter_start(int gpio_index)
{
	user_wifiled("wifi_led_slow_glitter_start.\n");
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "cus led green-blink 1000 \r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
	
}

void wifi_led_quick_glitter_start(int  gpio)
{
	user_wifiled("wifi_led_slow_glitter_start.\n");
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "cus led green-blink 200 \r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}


void wifi_ota_glitter_start(mico_gpio_t gpio)
{
	user_wifiled("wifi_led_slow_glitter_start.\n");
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "cus led green-blink 100 \r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);		
}


void wifi_led_onoff(WIFI_LED_GPIO_T *pflag)
{
	if(NULL == pflag)
	{
		user_wifiled("pflag is NULL.\n");
		return;
	}
	mico_gpio_t gpio = pflag->gpio;


	user_wifiled("gpio=====pflag->onoff:%d ======%d \n", pflag->onoff, gpio);

	if(pflag->onoff)
	{
		user_wifiled("wifi_led_slow_glitter_start.\n");
		KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
		memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

		memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
		sprintf(ctrlparam.cmd, "cus led green-on \r\n");
		ctrlparam.inBufLen = strlen(ctrlparam.cmd);
		
		pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
	}
	else
	{
		wifi_led_slow_glitter_start((int)gpio);
	}  
}

void wifi_led_stop()
{
 
}

