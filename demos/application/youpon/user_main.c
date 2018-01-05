/**
******************************************************************************
* @file    user_main.c
* @author  hehx_ybzn@163.com
* @version V1.0.0
* @date    12-oct-2016
* @brief   Define user add all function interface in here.
******************************************************************************
* @attention
*
******************************************************************************
*/ 

#include "mico.h"

#include "user_uart.h"
#include "zigbeeparse.h"
#include "user_config.h"
#include "appmainmsgqueue.h"
#include "user_config.h"
#include "MICOAppDefine.h"
#include "push_button.h"
#include "timerctrl.h"
#include "wifi_led.h"
#include "rs485_comm.h"
#include "system_internal.h"
#include "timercontrol.h"
#include "devicemanager.h"

#define user_log(M, ...) custom_log("USER", M, ##__VA_ARGS__)
#define user_log_trace() custom_log_trace("USER")

mico_semaphore_t wifi_config_net_sem;
static TIMER_CREATE_PARAM_T  timerarray[TIMER_MAX];
static uint8_t mac_addr[32];

extern app_context_t* app_context_global ;
extern SENSOR_LINKAGE_PARAM_T glinkage_param;
extern ZIGBEE_SENSOR_LINKAGE_PARAM_T gzigbee_sensor_linkage_param;

extern void zigbee_init();
extern int parseServerMessage(int8_t *recvbuff, int32_t BufferLen, uint32_t device_id);
extern void loadingTimerTsk();
extern OSStatus sensor_linkage_init();
//extern int sensor_linkage_turnon_light_device(int flag);
extern system_context_t* get_sys_context();
extern int linkage_cliear_invalid_items();
extern int sensor_linkage_turnon_light_device(int flag, uint8_t dev_mac[],  uint8_t endpoint_id);
extern int zigbee_check_ir_lightray_linakge_ctrl_condistions(linkage_param_t  *plinkage_param);
extern app_context_t*  get_app_context();
//---------------------------- User work function ------------------------------

uint8_t* getMacAddress()
{
	return mac_addr;
}

void setMacAddress()
{
	getGatewayRs485Device(mac_addr);
}

/*
	Description: When receive allow join zigbee endpoint join net start timer.
*/
void zigbee_join_work_callback(void)
{
	user_log( "user_zigbee_search_clicked_callback" );
	zigbee_send_pjoin();
}

void zigbee_reset(void)
{
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	user_log( "zigbee_reset" );

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "reset\r\nreset\r\nreset\r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);

}

void wifi_keybutton_press()
{
	user_log( "wifi_keybutton_press" );

//	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
//	sprintf(ctrlparam.cmd, "cus buzzer tick 150 0 1\r\n");
//	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
//	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);

	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "reset\r\nreset\r\nreset\r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);

	
}



void zigbee_send_pjoin()
{
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "network pjoin 30\r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);

}

static void wifi_key_button_press_callback(mico_thread_arg_t arg)
{
	UNUSED_PARAMETER( arg );
	
	while(1)
	{
		/* wait for button pressed */
		mico_rtos_get_semaphore( &wifi_config_net_sem, MICO_WAIT_FOREVER );
		wifi_keybutton_press();
	}
}


void wifi_config_press_irq_cb()
{	
	mico_rtos_set_semaphore( &wifi_config_net_sem );
}

void wifi_config_gpio_init()
{
	mico_rtos_init_semaphore( &wifi_config_net_sem, 1 );
	mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "button_press_cb", wifi_key_button_press_callback, 0x250, 0 );
}

static void zigbee_airchange_delayTurnOffCb( void* arg)
{
	UNUSED_PARAMETER( arg );	

	dev_linkage_airchange_timerStop();
		
	mico_rtos_lock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
	gzigbee_sensor_linkage_param.airchange_linkage_flag = 2;
	mico_rtos_unlock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
}

void zigbee_airchange_timerStop()
{
	if(mico_is_timer_running(&timerarray[ZIGBEE_AIRCHANGE_LINKAGE].timer))
		mico_stop_timer(&timerarray[ZIGBEE_AIRCHANGE_LINKAGE].timer);

}

void zigbee_airchange_timerStart()
{
	if(mico_is_timer_running(&timerarray[ZIGBEE_AIRCHANGE_LINKAGE].timer))
		return;

	mico_start_timer(&timerarray[ZIGBEE_AIRCHANGE_LINKAGE].timer);
}

static void zigbee_ir_lightray_delayTurnOffCb( void* arg)
{
	UNUSED_PARAMETER( arg );	
	linkage_param_t  linkageparam;
	int i,j;
    	uint8_t valid;
    	uint8_t status;
	 uint8_t sensor_type=0;
	user_log( "zigbee_ir_lightray_delayTurnOffCb in" );
	zigbee_ir_lightray_timerStop();
		
    app_context_t* app_context = get_app_context();

	zigbee_ir_lightray_timerStop();
	
     if(NULL == app_context)
    {
       // Linkage_log("app_context is NULL.\n");
        return ;
    }
	
	memset(&linkageparam, 0, sizeof(linkage_param_t));
	linkageparam.value = gzigbee_sensor_linkage_param.ir_value;
	linkageparam.sensor_type = SENSOR_TYPE_IR;
	linkageparam.value2 = gzigbee_sensor_linkage_param.lightrayvalue;
	linkageparam.sensor_type2 = SENSOR_TYPE_BEAM;
	linkageparam.param = 0;

	memcpy(linkageparam.mac, timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].mac, DEVICE_MAC_ADDRESS_LENGHT);
      if(NULL == app_context)
     {
       // Linkage_log("app_context is NULL.\n");
        return ;
     }
     for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
     {
        mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
        status = app_context->userDataStore->userData.linkagectrl[i].status;
	 memset(linkageparam.mac, 0, DEVICE_MAC_ADDRESS_LENGHT);
        memcpy(linkageparam.mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
        mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
        if( status == 1&& !memcmp(linkageparam.mac, timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].mac, DEVICE_MAC_ADDRESS_LENGHT) )
        {
            for(j=0; j < 4; j++)
            {
                mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
                valid = app_context->userDataStore->userData.linkagectrl[i].condition[j].valid;
		sensor_type = app_context->userDataStore->userData.linkagectrl[i].condition[j].chnl;
                mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

                if(valid == 1)
                {
                    if(SENSOR_TYPE_IR == sensor_type)
                    {
			
					zigbee_sensor_linkage_turnon_light_device(&linkageparam);

                    }
                }
            }
        }
    }

	

}

void zigbee_ir_lightray_timerStop()
{	
	if(mico_is_timer_running(&timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].timer))
		mico_stop_timer(&timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].timer);

}

void zigbee_ir_lightray_timerStart(uint8_t mac[])
{
	if(mico_is_timer_running(&timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].timer))
		return;

	memcpy(timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].mac, mac, DEVICE_MAC_ADDRESS_LENGHT);
	mico_start_timer(&timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].timer);
}

//-----------------------------lightwarm begin----------------------------------
static void dev_airchange_autoTurnOffCb( void* arg)
{
	UNUSED_PARAMETER( arg );	

	dev_linkage_airchange_timerStop();
		
	mico_rtos_lock_mutex(&glinkage_param.sensorparam_mutex);
	glinkage_param.smoke_linkage_flag = 2;
	mico_rtos_unlock_mutex(&glinkage_param.sensorparam_mutex);
}


void dev_linkage_airchange_timerStop()
{
	if(mico_is_timer_running(&timerarray[AUTO_TURN_OFF].timer))
		mico_stop_timer(&timerarray[AUTO_TURN_OFF].timer);

}

void dev_linkage_airchange_timerStart(uint8_t color_status)
{
	if(mico_is_timer_running(&timerarray[AUTO_TURN_OFF].timer))
		return;

	mico_start_timer(&timerarray[AUTO_TURN_OFF].timer);
}
//-----------------------------lightwarm end----------------------------------


//-----------------------------IR DELAY TURN OFF LIGHT begin----------------------------------
static void sensor_ir_delayTurnOffCb( void* arg)
{
	UNUSED_PARAMETER( arg );
	
	sensor_ir_timerStop();
	//sensor_linkage_turnon_light_device(0);
	int i,j;
    uint8_t valid;
    uint8_t status;
    uint8_t sensor_type,endpoint_id=0;
    uint8_t dev_mac[32];

    app_context_t* app_context = get_app_context();

	sensor_ir_timerStop();
	
    if(NULL == app_context)
    {
       // Linkage_log("app_context is NULL.\n");
        return ;
    }

    for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
    {
        mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
        status = app_context->userDataStore->userData.linkagectrl[i].status;
	 memset(dev_mac, 0, sizeof(dev_mac));
        memcpy(dev_mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
        mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
        if( status == 1&& !memcmp(dev_mac, timerarray[IR_DELAY_TURN_OFF_LIGHT].mac, DEVICE_MAC_ADDRESS_LENGHT) )
        {
            for(j=0; j < 4; j++)
            {
                mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
                valid = app_context->userDataStore->userData.linkagectrl[i].condition[j].valid;
                mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

                if(valid == 1)
                {
                    mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
                    sensor_type = app_context->userDataStore->userData.linkagectrl[i].condition[j].chnl;
                    endpoint_id = app_context->userDataStore->userData.linkagectrl[i].obj_id;
                    mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

                    if(SENSOR_TYPE_IR == sensor_type)
                    {
                        sensor_linkage_turnon_light_device(0, dev_mac, endpoint_id);
                        return ;
                    }
                }
            }
        }
    }
}


void sensor_ir_timerStop()
{	
	if(mico_is_timer_running(&timerarray[IR_DELAY_TURN_OFF_LIGHT].timer))
		mico_stop_timer(&timerarray[IR_DELAY_TURN_OFF_LIGHT].timer);

}

void sensor_ir_timerStart( uint8_t mac[])
{	
	if(mico_is_timer_running(&timerarray[IR_DELAY_TURN_OFF_LIGHT].timer))
		return;
	memcpy(timerarray[IR_DELAY_TURN_OFF_LIGHT].mac, mac, DEVICE_MAC_ADDRESS_LENGHT);
	mico_start_timer(&timerarray[IR_DELAY_TURN_OFF_LIGHT].timer);
}
//-----------------------------IR DELAY TURN OFF LIGHT end----------------------------------


void device_delay_save_timerStop()
{	
	if(mico_is_timer_running(&timerarray[DEVICE_DELAY_SAVE].timer))
		mico_stop_timer(&timerarray[DEVICE_DELAY_SAVE].timer);

}

void device_delay_save_timerStart()
{	
	if(mico_is_timer_running(&timerarray[DEVICE_DELAY_SAVE].timer))
	{	
		user_log("reload save timer.");
		mico_reload_timer(&timerarray[DEVICE_DELAY_SAVE].timer);
		return;	
	}
	user_log("start save timer.");
	mico_start_timer(&timerarray[DEVICE_DELAY_SAVE].timer);
}

static void device_delay_saveCb( void* arg)
{
	user_log("stop save timer.");
	UNUSED_PARAMETER( arg );	
	device_delay_save_timerStop();
	user_data_store_update();
}




static OSStatus timerArrayInit()
{
	OSStatus err = kNoErr;

	memset((void *)&timerarray[0], 0, sizeof(TIMER_CREATE_PARAM_T)*4);

	mico_rtos_init_mutex( &timerarray[AUTO_TURN_OFF].source_mutex);
	timerarray[AUTO_TURN_OFF].timer_interval = 30000;
	timerarray[AUTO_TURN_OFF].timer_times = 1;
	mico_init_timer( &timerarray[AUTO_TURN_OFF].timer, timerarray[AUTO_TURN_OFF].timer_interval, dev_airchange_autoTurnOffCb, NULL );

	mico_rtos_init_mutex( &timerarray[IR_DELAY_TURN_OFF_LIGHT].source_mutex);
	timerarray[IR_DELAY_TURN_OFF_LIGHT].timer_interval = 300000;//5min, unit:ms
	timerarray[IR_DELAY_TURN_OFF_LIGHT].timer_times = 0;
	mico_init_timer( &timerarray[IR_DELAY_TURN_OFF_LIGHT].timer, timerarray[IR_DELAY_TURN_OFF_LIGHT].timer_interval, sensor_ir_delayTurnOffCb, NULL );

	mico_rtos_init_mutex( &timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].source_mutex);
	timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].timer_interval = 300000;//5min, unit:ms
	timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].timer_times = 1;
	mico_init_timer( &timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].timer, timerarray[ZIGBEE_IR_LIGHTRAY_LINKAGE].timer_interval, zigbee_ir_lightray_delayTurnOffCb, NULL );

	mico_rtos_init_mutex( &timerarray[ZIGBEE_AIRCHANGE_LINKAGE].source_mutex);
	timerarray[ZIGBEE_AIRCHANGE_LINKAGE].timer_interval = 30000;
	timerarray[ZIGBEE_AIRCHANGE_LINKAGE].timer_times = 1;
	mico_init_timer( &timerarray[ZIGBEE_AIRCHANGE_LINKAGE].timer, timerarray[ZIGBEE_AIRCHANGE_LINKAGE].timer_interval, zigbee_airchange_delayTurnOffCb, NULL );

	mico_rtos_init_mutex( &timerarray[DEVICE_DELAY_SAVE].source_mutex);
	timerarray[DEVICE_DELAY_SAVE].timer_interval = 5000;// 3S
	timerarray[DEVICE_DELAY_SAVE].timer_times = 1;
	mico_init_timer( &timerarray[DEVICE_DELAY_SAVE].timer, timerarray[DEVICE_DELAY_SAVE].timer_interval, device_delay_saveCb, NULL );


	return err;
}


static void touchpanel_clicked_test()
{

}

void test_panel(mico_thread_arg_t arg)
{
	UNUSED_PARAMETER( arg );
	while(1)
	{
		touchpanel_clicked_test();
		mico_thread_sleep(1);
	}
}


int timer_control_init()
{
	OSStatus err = kNoErr;
	//linkage_cliear_invalid_items();
	clearInvalidTimerItem();
	initTimer();
	loadingTimerTsk();
	
	err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "runTask", 
	                            runTask, STACK_SIZE_USER_MAIN_THREAD, 0 );
	require_noerr_string( err, exit, "ERROR: Unable to start the runTask" );

	return err;
	
 exit:
	mico_rtos_delete_thread( NULL );    
	return err;
	
}

int scene_devlist_init(void)
{
	int i, j, ret=0;

	uint16_t netaddr = 0;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		user_log("app_context is NULL.\n");
		ret=-1;
	}
	

	for(i  = 0 ; i < SCENE_SCENEID_MAX; i++)
	{		
		for(j = 0; j < DEVICE_INFO_LIST_MAX; j++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
			netaddr = app_context->userDataStore->userData.scenectrl[i].devlist[j].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);			
			if(netaddr == 0xFFFF)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
				memset(&(app_context->userDataStore->userData.scenectrl[i]), 0 , sizeof(scene_ctrl_config_t));
				mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);	
				break;
			}
		}
	}

	return ret;
}


OSStatus user_application( app_context_t * const app_context )
{
	user_log_trace();
	OSStatus err = kUnknownErr;

	setMacAddress();
	wifi_config_gpio_init();
	sensor_linkage_init();
	uart_zigbee_queue_init();
	scene_devlist_init();
	timerArrayInit();
		
	//Server module init
	app_recv_send_msg_queue_init(app_context);
		
	//Uart init
	err = user_uartInit();

	wifi_led_slow_glitter_start((int)WIFI_LED_GPIO_G);
	
	Rs485_Init();
	require_noerr_action( err, exit, user_log("ERROR: user_uartInit err = %d.", err) );

	//Zigbee module init interface
	uart_zigbee_recv_start(app_context);

	zigbee_init();
	
	return err;

exit:
	err = mico_system_power_perform(mico_system_context_get(), eState_Software_Reset);
	mico_rtos_delete_thread(NULL);
	return err;
}

