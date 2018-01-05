#include "xlink_config.h"
#include "../xlinksrc/xlink_data.h"
#include "user_data_store.h"

#include "mico.h"
#include "platform.h"
#include "platform_config.h"

#define xlink_config(M, ...) custom_log("config", M, ##__VA_ARGS__)

unsigned char pidkey_buf[300];
static mico_mutex_t  xlink_config_mutex;
static int XLINK_FUNC writeconfig(char *data, unsigned int datalen);
static int XLINK_FUNC readconfig(char *Buffer, unsigned int bufferlen);
extern app_context_t*  get_app_context();

void XLINK_FUNC xlinkConfigInit(XLINK_USER_CONFIG *config) 
{
	mico_rtos_init_mutex( &xlink_config_mutex);
	config->readConfig = readconfig;
	config->writeConfig = writeconfig;
}

static int XLINK_FUNC writeconfig(char *data, unsigned int datalen) {
       int ret=0;
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		return ret;
	}

	mico_rtos_lock_mutex(&xlink_config_mutex);
	xlink_memcpy(app_context->appConfig->xlinkconfig.auth_pwd_name_buf, data, datalen);
	mico_rtos_unlock_mutex(&xlink_config_mutex);
	ret = mico_system_context_update( mico_system_context_get());

       return ret;
}

static int XLINK_FUNC readconfig(char *Buffer, unsigned int bufferlen) {

 	OSStatus err = kNoErr;
 
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		return err;
	}
	
	mico_rtos_lock_mutex(&xlink_config_mutex);
	xlink_memcpy(Buffer, app_context->appConfig->xlinkconfig.auth_pwd_name_buf, bufferlen);
	mico_rtos_unlock_mutex(&xlink_config_mutex);

	return err;
}

int XlinkWriteProductIDKEY(unsigned char *flag,char *proid, char *proKey) {

	int ret=0;
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		return ret;
	}

	mico_rtos_lock_mutex(&xlink_config_mutex);
	app_context->appConfig->xlinkconfig.product_key_id[0] = *flag;
	memcpy(app_context->appConfig->xlinkconfig.product_key_id + 1, proid, 32);
    	memcpy(app_context->appConfig->xlinkconfig.product_key_id + 33, proKey, 32);
	mico_rtos_unlock_mutex(&xlink_config_mutex);
	ret = mico_system_context_update( mico_system_context_get());

    	return ret;
}


int XlinkReadProductIDKEY(unsigned char *flag,char *proid, char *proKey) 
{
	OSStatus err = kNoErr;
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		return err;
	}

	mico_rtos_lock_mutex(&xlink_config_mutex);
	*flag = app_context->appConfig->xlinkconfig.product_key_id[0];
	memcpy(proid,app_context->appConfig->xlinkconfig.product_key_id + 1, 32);
	memcpy(proKey,app_context->appConfig->xlinkconfig.product_key_id + 33, 32);
	mico_rtos_unlock_mutex(&xlink_config_mutex);
	
  	return err;
}

