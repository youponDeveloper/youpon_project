#include "mico.h"

#include "StringUtils.h"
#include "MICOAppDefine.h"
#include "user_main.h"
#include "wifi_led.h"
#include "../xlinkapp/xlink_demo.h"
#include "system_internal.h"
#include "user_data_store.h"

#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)
#define app_log_trace() custom_log_trace("APP")


static app_context_t* g_app_context = NULL;

extern void localTcpServer_thread( uint32_t inContext );
extern void remoteTcpClient_thread( uint32_t inContext );
extern void uartRecv_thread( uint32_t inContext );
extern OSStatus MICOStartBonjourService( WiFi_Interface interface, app_context_t * const inContext );
extern void ak_discovery_thread(void *arg);
extern OSStatus sppProtocolInit(app_context_t * const inContext);
extern int timer_control_init();
void rs485_request_subdevice_state();
extern void popAppMainSendMsgThread(mico_thread_arg_t arg);
extern void wifi_config_gpio_init();

void wifi_lighten_led_color(bool onoff, int color)
{
	WIFI_LED_GPIO_T wifigpio;
	app_log("onoff=%d color=%d \n", onoff, color);
	wifigpio.onoff = onoff;
	wifigpio.gpio  = wifigpio.color = color;
	wifi_led_onoff(&wifigpio);
}		
	
/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback( void * const user_config_data, uint32_t size )
{
    UNUSED_PARAMETER( size );
    application_config_t* appConfig = user_config_data;
    appConfig->configDataVer = CONFIGURATION_VERSION;
    appConfig->localServerPort = LOCAL_PORT;
    appConfig->localServerEnable = true;
   // appConfig->USART_BaudRate = 115200;
    appConfig->remoteServerEnable = true;
    sprintf( appConfig->remoteServerDomain, DEAFULT_REMOTE_SERVER );
    appConfig->remoteServerPort = DEFAULT_REMOTE_SERVER_PORT;
}

/* EasyLink callback: Notify wlan configuration type */
USED void mico_system_delegate_config_success( mico_config_source_t source )
{
    app_log( "Configured by %d", source );
}

/* Config server callback: Current Device configuration sent to config client */
USED void config_server_delegate_report( json_object *app_menu, mico_Context_t *in_context )
{
    OSStatus err = kNoErr;
    application_config_t *appConfig = mico_system_context_get_user_data( in_context );

    // SPP protocol remote server connection enable
    err = config_server_create_bool_cell( app_menu, "Connect SPP Server", appConfig->remoteServerEnable, "RW" );
    require_noerr( err, exit );

    //Server address cell
    err = config_server_create_string_cell( app_menu, "SPP Server", appConfig->remoteServerDomain, "RW", NULL );
    require_noerr( err, exit );

    //Server port cell
    err = config_server_create_number_cell( app_menu, "SPP Server Port", appConfig->remoteServerPort, "RW", NULL );
    require_noerr( err, exit );

    /*UART Baurdrate cell*/
    json_object *selectArray;
    selectArray = json_object_new_array( );
    require( selectArray, exit );
    json_object_array_add( selectArray, json_object_new_int( 9600 ) );
    json_object_array_add( selectArray, json_object_new_int( 19200 ) );
    json_object_array_add( selectArray, json_object_new_int( 38400 ) );
    json_object_array_add( selectArray, json_object_new_int( 57600 ) );
    json_object_array_add( selectArray, json_object_new_int( 115200 ) );
   // err = config_server_create_number_cell( app_menu, "Baurdrate", appConfig->USART_BaudRate, "RW", selectArray );
    require_noerr( err, exit );

    exit:
    return;
}

/* Config server callback: New Device configuration received from config client */
USED void config_server_delegate_recv( const char *key, json_object *value, bool *need_reboot,
                                       mico_Context_t *in_context )
{
    application_config_t *appConfig = mico_system_context_get_user_data( in_context );

    if ( !strcmp( key, "Connect SPP Server" ) )
    {
        appConfig->remoteServerEnable = json_object_get_boolean( value );
        *need_reboot = true;
    } else if ( !strcmp( key, "SPP Server" ) )
    {
        strncpy( appConfig->remoteServerDomain, json_object_get_string( value ), 64 );
        *need_reboot = true;
    } else if ( !strcmp( key, "SPP Server Port" ) )
    {
        appConfig->remoteServerPort = json_object_get_int( value );
        *need_reboot = true;
    } else if ( !strcmp( key, "Baurdrate" ) )
    {
       // appConfig->USART_BaudRate = json_object_get_int( value );
        *need_reboot = true;
    }
}

static int data_resource_mutex_init(app_context_t* app_context)
{
	int ret = -1;

	mico_rtos_init_mutex( &(app_context->userDataStore->scene_info_mutex));
	mico_rtos_init_mutex( &(app_context->userDataStore->linkagectrl_mutex));
	mico_rtos_init_mutex( &(app_context->userDataStore->timerinfo_mutex));
	
	mico_rtos_init_mutex( &(app_context->userDataStore->device_info_mutex));
	mico_rtos_init_mutex( &(app_context->userDataStore->panel_info_mutex));
	mico_rtos_init_mutex( &(app_context->userDataStore->sensor_info_mutex));

	return ret;
}

app_context_t*  get_app_context()
{
	return g_app_context;
}

extern void mico_app_info(char *str, int len);

int application_start( void )
{
	char appversion[128];
	app_log_trace();
	OSStatus err = kNoErr;
	app_context_t* app_context;
	mico_Context_t* mico_context;                    

	init_wifi_connect_status();
	
	/* Create application context */
	g_app_context = app_context = (app_context_t *) calloc( 1, sizeof(app_context_t) );
	require_action( app_context, exit, err = kNoMemoryErr );

	/* Create mico system context and read application's config data from flash */
	app_log("################## application size:%d ", sizeof(application_config_t));
	mico_context = mico_system_context_init(sizeof(application_config_t));
	app_context->appConfig = mico_system_context_get_user_data( mico_context );

	app_context->userDataStore = (void *)userDataStoreInit();

#if 0
	memset(app_context->appConfig, 0, sizeof(application_config_t));
	mico_system_context_update( mico_system_context_get());
#endif

	data_resource_mutex_init(app_context);

	/* mico system initialize */
	err = mico_system_init( mico_context );
	require_noerr( err, exit );

	user_application(app_context);

	/* Initialize service discovery */
	err = MICOStartBonjourService( Station, app_context );
	require_noerr( err, exit );

	xlink_demo_init();  //add xlink init
	/* Create a new thread */
//	err = mico_rtos_create_thread(NULL, MICO_NETWORK_WORKER_PRIORITY, "ping thread", test_pingcmd_thread, 0x900,  (mico_thread_arg_t)app_context );
//	require_noerr_string( err, exit, "ERROR: Unable to start theping_thread" );

	err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "xlink domain_loop", xlink_domain_loop, 0x2000,  (mico_thread_arg_t)app_context);
	require_noerr_string( err, exit, "ERROR: Unable to start the xlink_domain_loop" );

	err = mico_rtos_create_thread(NULL, MICO_DEFAULT_WORKER_PRIORITY, "xlink thread", xlink_function_thread, 0x1200,  (mico_thread_arg_t)app_context );
	require_noerr_string( err, exit, "ERROR: Unable to start the xlink_function_thread" );

	timer_control_init();

	mico_app_info(appversion, 128);
    	app_log("OTA TEST #####user_main#####: appversion:%s  num_of_chunks:%d, free:%d", appversion, MicoGetMemoryInfo()->num_of_chunks, MicoGetMemoryInfo()->free_memory);

exit:
	mico_rtos_delete_thread( NULL );
	return err;
}
