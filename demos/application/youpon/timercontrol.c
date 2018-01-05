#include "timercontrol.h"
#include "user_main.h"
#include "networkserver.h"
#include "user_config.h"
#include "timer_task.h"
#include "devicemanager.h"

#define Timer_log(M, ...) custom_log("Timer_log", M, ##__VA_ARGS__)

/*static*/ int timer_control_update_status_request(TIMER_CTRL_PARAM_INFO_T *pLinkageInfo);
static int timer_control_delete_request(uint8_t control_id);
static int timer_control_add_request(TIMER_CTRL_PARAM_INFO_T *pLinkageInfo);
static int timer_control_modify_request(TIMER_CTRL_PARAM_INFO_T *pLinkageInfo);
extern app_context_t*  get_app_context();
extern OSStatus pushSend2ServerMsgProcess(unsigned char *msg, unsigned int inBufLen, uint32_t device_id);
extern unsigned int charchangehex( unsigned char *data , unsigned char* value );

int clearInvalidTimerItem()
{
	int ret = -1;
	int i;
	uint8_t status;

	uint8_t dev_mac[32];
	app_context_t* app_context = get_app_context();
	uint8_t updateflag = 0;

	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < TIMER_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
		status = app_context->userDataStore->userData.timerinfo[i].status;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.timerinfo[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

		//Timer_log("ctrl_id=%d status=%d \n", ctrl_id, status);
			
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ))
		{
			if(!checkDeviceExist(dev_mac))
			{
				Timer_log("invalid timer item i:%d\n",i);
				mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
				memset(&(app_context->userDataStore->userData.timerinfo[i]), 0, sizeof(timer_ctrl_info_t));
				mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

			}
		}
	}
	
	if(updateflag)
		ret =  user_data_store_update();
	
	return ret;
}

int removeInvalidTimerItem(uint8_t mac[])
{
	int ret = -1;
	int i;
	uint8_t status;
	uint8_t ctrl_id;
	uint8_t dev_mac[32];
	
	app_context_t* app_context = get_app_context();
	

	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
		status = app_context->userDataStore->userData.timerinfo[i].status;
		ctrl_id = app_context->userDataStore->userData.timerinfo[i].ctrl_id;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.timerinfo[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

#ifdef USER_DEBUG_
		Timer_log("ctrl_id=%d status=%d \n", ctrl_id, status);
		//HexDump((uint8_t * )dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		//HexDump((uint8_t * )mac, DEVICE_MAC_ADDRESS_LENGHT);

#endif

		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && !memcmp(dev_mac, mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			Timer_log("delete  the  item .......\n");

			mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
			memset(&(app_context->userDataStore->userData.timerinfo[i]), 0, sizeof(timer_ctrl_info_t));
			app_context->userDataStore->userData.timerinfo[i].status = TIMER_LINKAGE_STATUS_DELETE;
			mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

			delTask(ctrl_id);
			//ret =  user_data_store_update(); //save in call
			//more than one
		}
	}
	
	return ret;
}


int timer_list_request(uint32_t device_id)
{
	int ret = -1;
	int i;
	uint8_t status;
	int32_t ctrl_id;
	cJSON *root;
	cJSON *cJsonValue;
	char mac_str[32];
	app_context_t* app_context = get_app_context();
	uint32_t tempRDM;
	char *out = NULL;

	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");
		return ret;
	}

	srand(mico_rtos_get_time());
	tempRDM = rand();

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
		status = app_context->userDataStore->userData.timerinfo[i].status;
		ctrl_id = app_context->userDataStore->userData.timerinfo[i].ctrl_id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

#ifdef USER_DEBUG_
		Timer_log("ctrl_id=%d status=%d\n", ctrl_id, status);
#endif
		if(( status == TIMER_LINKAGE_STATUS_START || status == TIMER_LINKAGE_STATUS_STOP ))
		{
			root=cJSON_CreateObject();
			cJSON_AddItemToObject(root, "cmd_id", cJSON_CreateNumber(tempRDM));
			cJSON_AddItemToObject(root, "CMD", cJSON_CreateNumber(TIMER_CTRL_REQUEST));	
			cJSON_AddItemToObject(root, "gw_type", cJSON_CreateNumber(GATEWAY_TYPE));
			cJSON_AddItemToObject(root, "list", cJsonValue = cJSON_CreateObject());
		
			mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
			cJSON_AddItemToObject(cJsonValue, "ctrl_id", cJSON_CreateNumber(ctrl_id));
			cJSON_AddItemToObject(cJsonValue, "ctrl_n", cJSON_CreateString(app_context->userDataStore->userData.timerinfo[i].ctrl_name));
			cJSON_AddItemToObject(cJsonValue, "status", cJSON_CreateNumber(status));
			cJSON_AddItemToObject(cJsonValue, "ctrl_type", cJSON_CreateNumber(app_context->userDataStore->userData.timerinfo[i].ctrl_type));
			cJSON_AddItemToObject(cJsonValue, "obj_id", cJSON_CreateNumber(app_context->userDataStore->userData.timerinfo[i].obj_id));
			cJSON_AddItemToObject(cJsonValue, "cmd", cJSON_CreateNumber(app_context->userDataStore->userData.timerinfo[i].cmd));
			cJSON_AddItemToObject(cJsonValue, "val", cJSON_CreateNumber(app_context->userDataStore->userData.timerinfo[i].value));

			cJSON_AddItemToObject(cJsonValue, "act_times", cJSON_CreateNumber(app_context->userDataStore->userData.timerinfo[i].action_times));
			cJSON_AddItemToObject(cJsonValue, "timer_inter", cJSON_CreateNumber(app_context->userDataStore->userData.timerinfo[i].timer_inter));
			cJSON_AddItemToObject(cJsonValue, "timer_week", cJSON_CreateNumber(app_context->userDataStore->userData.timerinfo[i].timer_week));
			cJSON_AddItemToObject(cJsonValue, "timer_time_exe", cJSON_CreateNumber(app_context->userDataStore->userData.timerinfo[i].timer_time_exe));

			memset(mac_str, 0, sizeof(mac_str));
			sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.timerinfo[i].mac[0],app_context->userDataStore->userData.timerinfo[i].mac[1],app_context->userDataStore->userData.timerinfo[i].mac[2],
				app_context->userDataStore->userData.timerinfo[i].mac[3],app_context->userDataStore->userData.timerinfo[i].mac[4],app_context->userDataStore->userData.timerinfo[i].mac[5],
				app_context->userDataStore->userData.timerinfo[i].mac[6],app_context->userDataStore->userData.timerinfo[i].mac[7],
				app_context->userDataStore->userData.timerinfo[i].mac[8],app_context->userDataStore->userData.timerinfo[i].mac[9],
				app_context->userDataStore->userData.timerinfo[i].mac[10],app_context->userDataStore->userData.timerinfo[i].mac[11]);
			
			cJSON_AddItemToObject(cJsonValue, "mac",cJSON_CreateString((char const *)mac_str));

			mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

			out = cJSON_PrintUnformatted(root);
			if(out)
			{
				Timer_log("out=%s \n", out);	
				pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
				free(out);
				out = NULL;
			}
			cJSON_Delete(root);
			
		}
	}
	
	return ret;
}

int timer_ctrl_stop_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	
	TIMER_CTRL_PARAM_INFO_T timerctrlInfo;

	if(NULL == root)
	{
		Timer_log("NULL == root");
		return ret;
	}

	memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Timer_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
			if(objectValue)
			{
				timerctrlInfo.ctrl_id = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.ctrl_id: %d -----\n", timerctrlInfo.ctrl_id);	
			}
		
			objectValue = cJSON_GetObjectItem(pArrayJson, "status");
			if(objectValue)
			{
				timerctrlInfo.status = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.status: %d -----\n", timerctrlInfo.status);	
			}	

			ret = timer_control_update_status_request(&timerctrlInfo);
		}
	}
        
        return ret;
}

int timer_ctrl_start_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	
	TIMER_CTRL_PARAM_INFO_T timerctrlInfo;

	if(NULL == root)
	{
		Timer_log("NULL == root");
		return ret;
	}

	memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Timer_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));

		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{

			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
			if(objectValue)
			{
				timerctrlInfo.ctrl_id = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.ctrl_id: %d -----\n", timerctrlInfo.ctrl_id);	
			}
		

			objectValue = cJSON_GetObjectItem(pArrayJson, "status");
			if(objectValue)
			{
				timerctrlInfo.status = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.status: %d -----\n", timerctrlInfo.status);	
			}	
		}

		ret = timer_control_update_status_request(&timerctrlInfo);
	}
        
        return ret;
}


int timer_ctrl_delete_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	
	TIMER_CTRL_PARAM_INFO_T timerctrlInfo;

	if(NULL == root)
	{
		Timer_log("NULL == root");
		return ret;
	}

	memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Timer_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{

			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
			if(objectValue)
			{
				timerctrlInfo.ctrl_id = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.ctrl_id: %d -----\n", timerctrlInfo.ctrl_id);	
			}
		}

		ret = timer_control_delete_request(timerctrlInfo.ctrl_id);
	}
        
        return ret;
}


int timer_control_update_status_request(TIMER_CTRL_PARAM_INFO_T *pLinkageInfo)
{
	int ret = -1;
	int i;
	uint8_t status;
	int32_t ctrl_id;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pLinkageInfo)
	{
		Timer_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);

		status = app_context->userDataStore->userData.timerinfo[i].status;
		ctrl_id = app_context->userDataStore->userData.timerinfo[i].ctrl_id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

		Timer_log("ctrl_id=%d status=%d\n", ctrl_id, status);
			
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && ctrl_id == pLinkageInfo->ctrl_id)
		{
			Timer_log("update the  item status: %d .......\n", pLinkageInfo->status);

			mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
			app_context->userDataStore->userData.timerinfo[i].status = pLinkageInfo->status;
			status = pLinkageInfo->status;
			mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);
			user_data_store_update();

			setTaskFlag(ctrl_id, status);
			ret =  0;
			return ret;
		}
	}
	
	return ret;
}

int timer_control_delete_request(uint8_t control_id)
{
	int ret = -1;
	int i;
	uint8_t status;
	uint8_t ctrl_id;
	app_context_t* app_context = get_app_context();
	

	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
		status = app_context->userDataStore->userData.timerinfo[i].status;
		ctrl_id = app_context->userDataStore->userData.timerinfo[i].ctrl_id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

		Timer_log("ctrl_id=%d status=%d \n", ctrl_id, status);
			
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && ctrl_id == control_id)
		{
			Timer_log("delete  the  item .......\n");

			mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
			app_context->userDataStore->userData.timerinfo[i].status = TIMER_LINKAGE_STATUS_DELETE;
			mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

			delTask(ctrl_id);
			
			user_data_store_update();
			ret =  0;
			return ret;
		}
	}
	
	return ret;
}

int timer_ctrl_modify_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i,j,nstrlen;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	uint8_t mac_str[32]={0};	
	TIMER_CTRL_PARAM_INFO_T timerctrlInfo;

	if(NULL == root)
	{
		Timer_log("NULL == root");
		return ret;
	}

	memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Timer_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
			if(objectValue)
			{
				timerctrlInfo.ctrl_id = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.ctrl_id: %d -----\n", timerctrlInfo.ctrl_id);	
			}
		
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_n");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(timerctrlInfo.ctrlname, objectValue->valuestring, nstrlen);

				Timer_log("-----------timerctrlInfo.ctrlname: %s -----\n", timerctrlInfo.ctrlname);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "cmd");
			if(objectValue )
			{
				timerctrlInfo.cmd = objectValue->valueint;
				Timer_log("-----------timerctrlInfo.cmd: %d -----\n", timerctrlInfo.cmd);	
			}
			

			objectValue = cJSON_GetObjectItem(pArrayJson, "status");
			if(objectValue)
			{
				timerctrlInfo.status = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.status: %d -----\n", timerctrlInfo.status);	
			}
			
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_type");
			if(objectValue)
			{
				timerctrlInfo.ctrl_type = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.ctrl_type: %d -----\n", timerctrlInfo.ctrl_type);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "obj_id");
			if(objectValue)
			{
				timerctrlInfo.obj_id = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.obj_id: %d -----\n", timerctrlInfo.obj_id);	
			}
			
			objectValue = cJSON_GetObjectItem(pArrayJson, "val");
			if(objectValue)
			{
				timerctrlInfo.value = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.value: %d -----\n", timerctrlInfo.value);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "act_times");
			if(objectValue)
			{
				timerctrlInfo.act_times = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.act_times: %d -----\n", timerctrlInfo.act_times);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "timer_inter");
			if(objectValue)
			{
				timerctrlInfo.timer_inter = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.timer_inter: %d -----\n", timerctrlInfo.timer_inter);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "timer_week");
			if(objectValue)
			{
				timerctrlInfo.timer_week = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.timer_week: %d -----\n", timerctrlInfo.timer_week);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "timer_time_exe");
			if(objectValue)
			{
				timerctrlInfo.timer_time_exe = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.timer_time_exe: %d -----\n", timerctrlInfo.timer_time_exe);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "mac");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(mac_str, objectValue->valuestring, nstrlen);

				Timer_log("-----------mac_str: %s ------\n", mac_str);	
				nstrlen /= 2;
				for( j = 0; j < nstrlen ; j++ )
				{
					charchangehex( &mac_str[j*2] , &timerctrlInfo.mac[j] );
				}
			}
			
		}

		ret = timer_control_modify_request(&timerctrlInfo);
	}
        
        return ret;
}

int timer_ctrl_new_add_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i,j,nstrlen;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	uint8_t mac_str[32]={0};
	
	TIMER_CTRL_PARAM_INFO_T timerctrlInfo;

	if(NULL == root)
	{
		Timer_log("NULL == root");
		return ret;
	}

	memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Timer_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_n");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(timerctrlInfo.ctrlname, objectValue->valuestring, nstrlen);

				Timer_log("-----------timerctrlInfo.ctrlname: %s -----\n", timerctrlInfo.ctrlname);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "cmd");
			if(objectValue )
			{
				timerctrlInfo.cmd = objectValue->valueint;
				Timer_log("-----------timerctrlInfo.cmd: %d -----\n", timerctrlInfo.cmd);	
			}
			

			objectValue = cJSON_GetObjectItem(pArrayJson, "status");
			if(objectValue)
			{
				timerctrlInfo.status = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.status: %d -----\n", timerctrlInfo.status);	
			}
			
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_type");
			if(objectValue)
			{
				timerctrlInfo.ctrl_type = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.ctrl_type: %d -----\n", timerctrlInfo.ctrl_type);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "obj_id");
			if(objectValue)
			{
				timerctrlInfo.obj_id = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.obj_id: %d -----\n", timerctrlInfo.obj_id);	
			}
			
			objectValue = cJSON_GetObjectItem(pArrayJson, "val");
			if(objectValue)
			{
				timerctrlInfo.value = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.value: %d -----\n", timerctrlInfo.value);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "act_times");
			if(objectValue)
			{
				timerctrlInfo.act_times = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.act_times: %d -----\n", timerctrlInfo.act_times);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "timer_inter");
			if(objectValue)
			{
				timerctrlInfo.timer_inter = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.timer_inter: %d -----\n", timerctrlInfo.timer_inter);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "timer_week");
			if(objectValue)
			{
				timerctrlInfo.timer_week = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.timer_week: %d -----\n", timerctrlInfo.timer_week);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "timer_time_exe");
			if(objectValue)
			{
				timerctrlInfo.timer_time_exe = objectValue->valueint;
				
				Timer_log("-----------timerctrlInfo.timer_time_exe: %d -----\n", timerctrlInfo.timer_time_exe);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "mac");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(mac_str, objectValue->valuestring, nstrlen);

				Timer_log("-----------mac_str: %s ------\n", mac_str);	
				nstrlen /= 2;
				for( j = 0; j < nstrlen ; j++ )
				{
					charchangehex( &mac_str[j*2] , &timerctrlInfo.mac[j] );
				}
			}

		}

		ret = timer_control_add_request(&timerctrlInfo);
		
	}
        
        return ret;
}

int timer_control_modify_request(TIMER_CTRL_PARAM_INFO_T *pLinkageInfo)
{
	int ret = -1;
	int i;
	uint8_t status;
	uint8_t ctrl_id;
	struct timer_task_struct  tsk;
	app_context_t* app_context = get_app_context();
	
	if(NULL == pLinkageInfo)
	{
		Timer_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
		ctrl_id = app_context->userDataStore->userData.timerinfo[i].ctrl_id;
		status = app_context->userDataStore->userData.timerinfo[i].status;
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

		Timer_log("ctrl_id=%d \n", ctrl_id);
			
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && ctrl_id == pLinkageInfo->ctrl_id)
		{
			Timer_log("Modify the same item .......\n");

			mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);

			app_context->userDataStore->userData.timerinfo[i].status = pLinkageInfo->status;
			app_context->userDataStore->userData.timerinfo[i].ctrl_type = pLinkageInfo->ctrl_type;
			app_context->userDataStore->userData.timerinfo[i].obj_id = pLinkageInfo->obj_id;
			app_context->userDataStore->userData.timerinfo[i].cmd = pLinkageInfo->cmd;
			app_context->userDataStore->userData.timerinfo[i].value = pLinkageInfo->value;

			app_context->userDataStore->userData.timerinfo[i].action_times = pLinkageInfo->act_times;
			app_context->userDataStore->userData.timerinfo[i].timer_inter = pLinkageInfo->timer_inter;
			app_context->userDataStore->userData.timerinfo[i].timer_week = pLinkageInfo->timer_week;
			app_context->userDataStore->userData.timerinfo[i].timer_time_exe = pLinkageInfo->timer_time_exe;
			memcpy(app_context->userDataStore->userData.timerinfo[i].ctrl_name, pLinkageInfo->ctrlname, sizeof(app_context->userDataStore->userData.timerinfo[i].ctrl_name));
			memcpy(app_context->userDataStore->userData.timerinfo[i].mac, pLinkageInfo->mac, sizeof(app_context->userDataStore->userData.timerinfo[i].mac));
			mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);
			ret =  user_data_store_update();

			memset(&tsk, 0, sizeof(struct timer_task_struct));
			tsk.control_id = pLinkageInfo->ctrl_id;
			tsk.status = pLinkageInfo->status;
			tsk.control_type = pLinkageInfo->ctrl_type;
			tsk.obj_ctrl_id = pLinkageInfo->obj_id;
			tsk.func_command = pLinkageInfo->cmd;
			tsk.func_value = pLinkageInfo->value;
			tsk.action_times = pLinkageInfo->act_times;

			tsk.timer_inter = pLinkageInfo->timer_inter;
			tsk.timer_week = pLinkageInfo->timer_week;
			tsk.timer_time_exe = pLinkageInfo->timer_time_exe;
			memcpy(tsk.ctrl_name, pLinkageInfo->ctrlname, sizeof(app_context->userDataStore->userData.timerinfo[i].ctrl_name));
			memcpy(tsk.mac, pLinkageInfo->mac, sizeof(app_context->userDataStore->userData.timerinfo[i].mac));

			alterTask(&tsk);
			ret=0;
			return ret;
		}

	
	}
	
	
	return ret;
}

/*
	Description: return 0 add success. return -1 add failed. return 1 this timer name had exist.
*/
int timer_control_add_request(TIMER_CTRL_PARAM_INFO_T *pLinkageInfo)
{
	int ret = -1;
	int i;
	int new_add_device_index = -1;
	uint8_t status;
	uint8_t linkage_name[32+1];
	struct timer_task_struct  tsk;
	app_context_t* app_context = get_app_context();
	
	if(NULL == pLinkageInfo)
	{
		Timer_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
		status = app_context->userDataStore->userData.timerinfo[i].status;
		memset(linkage_name, 0, sizeof(linkage_name));
		memcpy(linkage_name, app_context->userDataStore->userData.timerinfo[i].ctrl_name, sizeof(app_context->userDataStore->userData.timerinfo[i].ctrl_name));
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

		Timer_log("status=%d  timer_name=%s \n", status, linkage_name);
			
		if(strlen((char const *)linkage_name) > 0 && (status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && !strcmp((char const *)linkage_name, pLinkageInfo->ctrlname))
		{
			Timer_log("had same timer name item.\n");
			ret = 1;
			return ret;
		}


		if(new_add_device_index == -1 && (status != TIMER_LINKAGE_STATUS_START && status != TIMER_LINKAGE_STATUS_STOP ) )
		{	
			new_add_device_index = i;
		}	
	}

	Timer_log("************************** new_add_device_index: %d \n", new_add_device_index);
	if(new_add_device_index >= 0)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);

		app_context->userDataStore->userData.timerinfo[new_add_device_index].ctrl_id = new_add_device_index;
		app_context->userDataStore->userData.timerinfo[new_add_device_index].status = pLinkageInfo->status;
		app_context->userDataStore->userData.timerinfo[new_add_device_index].ctrl_type = pLinkageInfo->ctrl_type;
		app_context->userDataStore->userData.timerinfo[new_add_device_index].obj_id = pLinkageInfo->obj_id;
		app_context->userDataStore->userData.timerinfo[new_add_device_index].cmd = pLinkageInfo->cmd;
		app_context->userDataStore->userData.timerinfo[new_add_device_index].value = pLinkageInfo->value;

		app_context->userDataStore->userData.timerinfo[new_add_device_index].action_times = pLinkageInfo->act_times;
		app_context->userDataStore->userData.timerinfo[new_add_device_index].timer_inter = pLinkageInfo->timer_inter;
		app_context->userDataStore->userData.timerinfo[new_add_device_index].timer_week = pLinkageInfo->timer_week;
		app_context->userDataStore->userData.timerinfo[new_add_device_index].timer_time_exe = pLinkageInfo->timer_time_exe;
		
		memcpy(app_context->userDataStore->userData.timerinfo[new_add_device_index].ctrl_name, pLinkageInfo->ctrlname, sizeof(app_context->userDataStore->userData.timerinfo[new_add_device_index].ctrl_name));
		memcpy(app_context->userDataStore->userData.timerinfo[new_add_device_index].mac, pLinkageInfo->mac, sizeof(app_context->userDataStore->userData.timerinfo[new_add_device_index].mac));

		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);
		user_data_store_update();

		memset(&tsk, 0, sizeof(struct timer_task_struct));
		tsk.control_id = new_add_device_index;//shibingmo add
		tsk.status = pLinkageInfo->status;
		tsk.control_type = pLinkageInfo->ctrl_type;
		tsk.obj_ctrl_id = pLinkageInfo->obj_id;
		tsk.func_command = pLinkageInfo->cmd;
		tsk.func_value = pLinkageInfo->value;
		tsk.action_times = pLinkageInfo->act_times;

		tsk.timer_inter = pLinkageInfo->timer_inter;
		tsk.timer_week = pLinkageInfo->timer_week;
		tsk.timer_time_exe = pLinkageInfo->timer_time_exe;
		memcpy(tsk.ctrl_name, pLinkageInfo->ctrlname, sizeof(app_context->userDataStore->userData.timerinfo[new_add_device_index].ctrl_name));
		memcpy(tsk.mac, pLinkageInfo->mac, sizeof(app_context->userDataStore->userData.timerinfo[new_add_device_index].mac));
		ret =  0;
		addTask(&tsk);
	}
	else
	{
		Timer_log("No space to add new panel device..............!\n");
	}
	
	
	return ret;
}

int timer_control_delete_by_devmac(char *pMac)
{
	int ret = -1;
	int i;
	uint8_t dev_mac[12];
	uint8_t ctrl_id;

	app_context_t* app_context = get_app_context();
	
	if(NULL == pMac)
	{
		Timer_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < TIMER_CTRL_LIST_MAX; i++)
	{

		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
		memset(dev_mac,  0,  sizeof(dev_mac));
		memcpy(dev_mac,  app_context->userDataStore->userData.timerinfo[i].mac,  DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

		if(!memcmp(dev_mac, pMac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			Timer_log("delete  the  timerctrl:%d .......\n",i );
			mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
			ctrl_id = app_context->userDataStore->userData.timerinfo[i].ctrl_id;
			memset(&app_context->userDataStore->userData.timerinfo[i], 0, sizeof(timer_ctrl_info_t));						
			mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);	
			delTask(ctrl_id);
		}
	}

	return ret;
}


void print_timer_ctrl_info()
{
	app_context_t* app_context = get_app_context();
	int i;
	timer_ctrl_info_t timer_ctrl;
	if(NULL == app_context)
	{
		Timer_log("app_context is NULL.\n");

	}
        else{
            Timer_log("******************* timer ctrl list **************************\n");
            for(i=0; i < TIMER_CTRL_LIST_MAX; i++)
            {	
            		memset(&timer_ctrl, 0, sizeof(timer_ctrl_info_t) );
                    mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
                    timer_ctrl = app_context->userDataStore->userData.timerinfo[i];
                    mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

                    Timer_log("i:%d  name:%s  id:%x  status:%x  start:%x  inter:%x  times:%x  week:%x  time_exe:%x  time_stop:%x  type:%x  obj_id:%x  cmd:%x  value:%x  mac:%02x%02x\
%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",i,timer_ctrl.ctrl_name,timer_ctrl.ctrl_id,timer_ctrl.status,timer_ctrl.timer_time_start,timer_ctrl.timer_inter,\
                            timer_ctrl.action_times,timer_ctrl.timer_week,timer_ctrl.timer_time_exe,timer_ctrl.timer_time_stop, timer_ctrl.ctrl_type,timer_ctrl.obj_id,timer_ctrl.cmd,timer_ctrl.value,\
                            timer_ctrl.mac[0],timer_ctrl.mac[1],timer_ctrl.mac[2],timer_ctrl.mac[3],timer_ctrl.mac[4],timer_ctrl.mac[5],timer_ctrl.mac[6],timer_ctrl.mac[7],timer_ctrl.mac[8],\
                            timer_ctrl.mac[9],timer_ctrl.mac[10],timer_ctrl.mac[11]);		
            }
        }
}



