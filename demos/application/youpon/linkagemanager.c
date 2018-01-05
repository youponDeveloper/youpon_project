#include "linkagemanager.h"
#include "user_main.h"
#include "networkserver.h"
#include "zigbeeparse.h"
#include "user_config.h"
#include "devicemanager.h"

#define Linkage_log(M, ...) custom_log("Linkage_log", M, ##__VA_ARGS__)

static int linkage_control_delete_request(LINKAGE_CTRL_INFO_T *pLinkageInfo);
static int linkage_control_add_request(LINKAGE_CTRL_INFO_T *pLinkageInfo);
static int linkage_control_modify_request(LINKAGE_CTRL_INFO_T *pLinkageInfo);
extern app_context_t*  get_app_context();
extern OSStatus pushSend2ServerMsgProcess(unsigned char *msg, unsigned int inBufLen,uint32_t device_id);
extern unsigned int charchangehex( unsigned char *data , unsigned char* value );
extern void sensor_ir_timerStop();
extern int sensor_linkage_turnon_light_device(int flag, uint8_t dev_mac[],  uint8_t endpoint_id);
extern int sensor_linkage_control_device(linkage_dev_param_t *pLinkageParam);
extern void HexDump(uint8_t * pdata, uint32_t data_len);
extern int isKitchenSensorDevice(uint8_t mac[]);


extern SENSOR_LINKAGE_PARAM_T glinkage_param;
extern ZIGBEE_SENSOR_LINKAGE_PARAM_T gzigbee_sensor_linkage_param;

int linkage_cliear_invalid_items()
{
	int ret = -1;
	int i;
	uint8_t status;
	uint8_t ctrl_id;
	//char sensor_dev_mac[32];
	//char mac[32]={0};
	
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		status = app_context->userDataStore->userData.linkagectrl[i].status;
		memset(&app_context->userDataStore->userData.linkagectrl[i], 0, sizeof(linkage_dev_param_t));
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

		Linkage_log("ctrl_id=%d status=%d \n", ctrl_id, status);

#if 0		
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ))
		{
			Linkage_log("delete  the  item .......\n");
			int loop;
			for(loop=0; loop < 2; loop++)
			{
				memset(sensor_dev_mac, 0, sizeof(sensor_dev_mac));
				mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				memcpy(sensor_dev_mac, app_context->userDataStore->userData.linkagectrl[i].condition[loop].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
				if(app_context->userDataStore->userData.linkagectrl[i].condition[loop].valid == 1 && (!memcmp((char const *)sensor_dev_mac, mac, DEVICE_MAC_ADDRESS_LENGHT) ))
				{
					ret = 1;
					app_context->userDataStore->userData.linkagectrl[i].status = 0;
					memset(&app_context->userDataStore->userData.linkagectrl[i], 0, sizeof(linkage_dev_param_t));
					mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				}
				else
				{
					mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				}
			}			
			
			return ret;
		}
#endif
		
	}
	ret =  user_data_store_update();
	return ret;
}

int zigbee_check_linakge_airchange_ctrl_condistions(linkage_param_t  *plinkage_param)
{
	int ret = -1;
	app_context_t* app_context = get_app_context();
	uint8_t linkage_flag;
	uint8_t cond_flag;
	
	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	//Linkage_log("plinkage_param->sensor_type=%d  value=%d \n", plinkage_param->sensor_type, plinkage_param->value);
	if( (plinkage_param->sensor_type == SENSOR_TYPE_CO2 && plinkage_param->value >= CO2_MID ) ||
	    (plinkage_param->sensor_type == SENSOR_TYPE_TVOC && plinkage_param->value >= TVOC_MID ) ||
	    (plinkage_param->sensor_type == SENSOR_TYPE_COMBUSTIBLE && plinkage_param->value >= COMBUSTIBLE_NORMAL ) ||
	    (plinkage_param->sensor_type == SENSOR_TYPE_SMOKE && plinkage_param->value >= SMOKE_MID )||
	    (plinkage_param->sensor_type == SENSOR_TYPE_HUMIDITY && plinkage_param->value >= HUMI_MOIST)
	)
	{
		cond_flag = 1;
	}
	else
	{
		cond_flag = 2;
	}
	
	mico_rtos_lock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
	linkage_flag = gzigbee_sensor_linkage_param.airchange_linkage_flag;
	mico_rtos_unlock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);

	//Linkage_log("cond_flag=%d  linkage_flag=%d \n", cond_flag, linkage_flag);

	if(cond_flag == 1)
	{
		if(linkage_flag == USER_NUMBER_VAL_0)
		{
			mico_rtos_lock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
			gzigbee_sensor_linkage_param.airchange_linkage_flag = 1;
			mico_rtos_unlock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);

			zigbee_airchange_timerStart();

			plinkage_param->ctrl_type = 1;
			plinkage_param->obj_id = 6;
			plinkage_param->param = 1;
			zigbee_sensor_linkage_control_airchange(plinkage_param);
		}
	}
	else if(cond_flag == 2)
	{							
		if(linkage_flag == USER_NUMBER_VAL_2)
		{
			zigbee_airchange_timerStop();

			mico_rtos_lock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
			gzigbee_sensor_linkage_param.airchange_linkage_flag = 0;
			mico_rtos_unlock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);

			plinkage_param->ctrl_type = 1;
			plinkage_param->obj_id = 6;
			plinkage_param->param = 0;
			zigbee_sensor_linkage_control_airchange(plinkage_param);
		}
	}
	
	return ret;
}

int zigbee_check_ir_lightray_linakge_ctrl_condistions(linkage_param_t  *plinkage_param)
{
	int ret = -1;
	app_context_t* app_context = get_app_context();
	uint8_t linkage_flag;
	uint8_t cond1_flag = 0;
	uint8_t cond2_flag = 0;
	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}


	if(plinkage_param->sensor_type ==  SENSOR_TYPE_IR && plinkage_param->value >= 1)
	{
		cond1_flag = 1;
	}
	 if((plinkage_param->sensor_type2 == SENSOR_TYPE_BEAM) &&(plinkage_param->value2 >= BEAM_DARK)&& (plinkage_param->value2 <= BEAM_OFFICE))
	{
		cond2_flag = 1;
	}


	mico_rtos_lock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
	linkage_flag = gzigbee_sensor_linkage_param.linkage_flag;
	mico_rtos_unlock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
	//Linkage_log("cond2_flag=%d cond1_flag=%d  linkage_flag = %d  \n", cond2_flag, cond1_flag, linkage_flag);	

	if(cond1_flag == 1 && cond2_flag == 1)
	{
			zigbee_ir_lightray_timerStop();
			mico_rtos_lock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
			gzigbee_sensor_linkage_param.linkage_flag = 1;
			mico_rtos_unlock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
			plinkage_param->param = 1;

			zigbee_sensor_linkage_turnon_light_device(plinkage_param);
	}	
	#if 0
	else if(cond1_flag == 0)
	{	
		if(linkage_flag == USER_NUMBER_VAL_2)
		{
			zigbee_ir_lightray_timerStop();
			mico_rtos_lock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
			gzigbee_sensor_linkage_param.linkage_flag = 0;
			mico_rtos_unlock_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);
			turnon_flag =0;
			plinkage_param->param = 0;
			zigbee_sensor_linkage_turnon_light_device(plinkage_param);
		}
		
		if(turnon_flag ==1 )
			zigbee_ir_lightray_timerStart(plinkage_param->mac);
	}
	else if(cond1_flag == 1)
	{
		if(turnon_flag ==1 )
		zigbee_ir_lightray_timerStop();
	}
	#endif	
	
	return ret;
}

int check_ir_lightray_linakge_ctrl_condistions(linkage_param_t  *plinkage_param,uint8_t mac[])
{
	int ret = -1;
	int i,j;
	uint8_t status;
	app_context_t* app_context = get_app_context();
	uint8_t valid;
	int32_t param;
	uint8_t sensor_type,endpoint_id=0;
	uint8_t cond1_flag = 0;
	uint8_t cond2_flag = 0;
	int32_t action;
	char cond[16];
	char cond2[16];
	uint8_t dev_mac[DEVICE_MAC_ADDRESS_LENGHT];

	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		status = app_context->userDataStore->userData.linkagectrl[i].status;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, sizeof(app_context->userDataStore->userData.linkagectrl[i].dev_mac));
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);


	
		if( status == 1 && !memcmp(plinkage_param->mac, dev_mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			cond1_flag = 0;
			cond2_flag = 0;

			memset(cond, 0, sizeof(cond));
			mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			memcpy(cond, app_context->userDataStore->userData.linkagectrl[i].env_cond, 4);
			mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			
			for(j=0; j < 4; j++)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				valid = app_context->userDataStore->userData.linkagectrl[i].condition[j].valid;
				action = app_context->userDataStore->userData.linkagectrl[i].param;
				mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				
				if(valid == 1)
				{
					mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
					param = app_context->userDataStore->userData.linkagectrl[i].condition[j].param;
					sensor_type = app_context->userDataStore->userData.linkagectrl[i].condition[j].chnl;
					endpoint_id = app_context->userDataStore->userData.linkagectrl[i].obj_id;
					memset(cond2, 0, sizeof(cond2));
					memcpy(cond2, app_context->userDataStore->userData.linkagectrl[i].condition[j].cond, 4);
					mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);


					if(plinkage_param->sensor_type == sensor_type &&  SENSOR_TYPE_IR == sensor_type)
					{
						cond1_flag= 2;
						if(!strcmp(cond2, ">") || !strcmp(cond2, ">="))
						{
							uint16_t  min,max;
							max = param&0x0000FFFF;
							min = (param >> 16);

							#ifdef USER_DEBUG_
							Linkage_log("################# min:%d  max: %d \n", min, max);
							#endif
							
							if(plinkage_param->value >= min && plinkage_param->value <= max)
							{
								cond1_flag = 1;
							}
						}
						else if(!strcmp(cond2, "=="))
						{
							if(plinkage_param->value == param)
							{
								cond1_flag = 1;
							}
						}
					}
					else if(plinkage_param->sensor_type2 == sensor_type  &&  SENSOR_TYPE_BEAM  == sensor_type)
					{
						cond2_flag= 2;
						if(!strcmp(cond2, ">") || !strcmp(cond2, ">="))
						{
							uint16_t  min,max;
							max= param&0x0000FFFF;
							min = (param >> 16);

							#ifdef USER_DEBUG_
						//	Linkage_log("2################# min:%d  max: %d \n", min, max);
							#endif
							
							if(plinkage_param->value2 >= min && plinkage_param->value2 <= max)
							{
								cond2_flag = 1;
							}
						}
						else if(!strcmp(cond2, "=="))
						{
							if(plinkage_param->value2 == param)
							{
								cond2_flag = 1;
							}
						}
					}
				}
			}

			//Linkage_log("cond=%s cond2_flag=%d cond1_flag=%d action=%d endpoint_id=%d \n",cond ,cond2_flag, cond1_flag, action, endpoint_id);
			if(!strcmp(cond, "&&"))
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				memset(dev_mac, 0, sizeof(dev_mac));
				memcpy(dev_mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
				memset(mac, 0, DEVICE_MAC_ADDRESS_LENGHT);
				memcpy(mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
				mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

				if((cond1_flag !=0 && cond1_flag == 1) && (cond2_flag !=0 && cond2_flag == 1))
				{		
					sensor_linkage_turnon_light_device(action, dev_mac, endpoint_id);
				}
				else if(cond1_flag ==0  &&  cond2_flag == 1)
				{
					sensor_linkage_turnon_light_device(action, dev_mac, endpoint_id);
				}
				else if(cond2_flag ==0  && cond1_flag == 1)
				{
					sensor_linkage_turnon_light_device(action, dev_mac, endpoint_id);
				}
			}
			else if(!strcmp(cond, "||"))
			{
				if((cond1_flag !=0 && cond1_flag == 1) || (cond2_flag !=0 && cond2_flag == 1))
				{
					mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
					memset(dev_mac, 0, sizeof(dev_mac));
					memcpy(dev_mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
					memset(mac, 0, DEVICE_MAC_ADDRESS_LENGHT);
					memcpy(mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
					mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

					sensor_linkage_turnon_light_device(action, dev_mac, endpoint_id);
				}
			}
		}
	}
	
	return ret;
}

int check_linakge_ctrl_condistions(linkage_param_t  *plinkage_param)
{
	int ret = -1;
	int i,j;
	uint8_t status;
	app_context_t* app_context = get_app_context();
	uint8_t valid;
	int32_t param;
	uint8_t sensor_type;
	//uint8_t linkage_flag;
	uint8_t cond_flag ;
	char cond2[16];
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	int happend = -1;
	
	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		status = app_context->userDataStore->userData.linkagectrl[i].status;
		
		memset(mac, 0, sizeof(mac));
		memcpy(mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		happend = -1;

		if( status == 1 && !memcmp(plinkage_param->mac, mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			cond_flag = 0;
			for(j=0; j < 4; j++)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				valid = app_context->userDataStore->userData.linkagectrl[i].condition[j].valid;
				mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				
				if(valid == 1)
				{
					mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
					param = app_context->userDataStore->userData.linkagectrl[i].condition[j].param;
					sensor_type = app_context->userDataStore->userData.linkagectrl[i].condition[j].chnl;
					memset(cond2, 0, sizeof(cond2));
					memcpy(cond2, app_context->userDataStore->userData.linkagectrl[i].condition[j].cond, 4);
					mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				//	Linkage_log("plinkage_param->value=%d param=%d plinkage_param->sensor_type=%d  sensor_type=%d \n", plinkage_param->value, param, plinkage_param->sensor_type, sensor_type);	

					if(plinkage_param->sensor_type == sensor_type)
					{
					      happend  = app_context->userDataStore->userData.linkagectrl[i].happend;
						if(!strcmp(cond2, ">") || !strcmp(cond2, ">="))
						{
							uint16_t  min,max;
							max= param&0x0000FFFF;
							min = (param >> 16);

							#ifdef USER_DEBUG_
							//Linkage_log("################# min:%d  max: %d  happend:%d \n", min, max,happend);
							#endif
							
							if(plinkage_param->value > min && plinkage_param->value <= max)
							{
								cond_flag = 1;
								break;
							}

						}
						else if(!strcmp(cond2, "=="))
						{
							if(plinkage_param->value == param )
							{
								cond_flag = 1;
							}
						}
					}
				}

			}


			if(cond_flag == 1 && happend ==0)
			{
				linkage_dev_param_t  linkageDevInfo;
				memset(&linkageDevInfo, 0, sizeof(linkage_dev_param_t));
				mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				app_context->userDataStore->userData.linkagectrl[i].happend = 1;
				linkageDevInfo.ctrl_type = app_context->userDataStore->userData.linkagectrl[i].ctrl_type;
				linkageDevInfo.obj_id = app_context->userDataStore->userData.linkagectrl[i].obj_id;
				linkageDevInfo.param = app_context->userDataStore->userData.linkagectrl[i].param;
				memset(linkageDevInfo.dev_mac, 0, sizeof(linkageDevInfo.dev_mac));
				memcpy(linkageDevInfo.dev_mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
				mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);				
				sensor_linkage_control_device(&linkageDevInfo);
			}
			else if(cond_flag == 0 && happend == 1)
			{	

				linkage_dev_param_t  linkageDevInfo;
				memset(&linkageDevInfo, 0, sizeof(linkage_dev_param_t));
				mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				app_context->userDataStore->userData.linkagectrl[i].happend = 0;
				linkageDevInfo.ctrl_type = app_context->userDataStore->userData.linkagectrl[i].ctrl_type;
				linkageDevInfo.obj_id = app_context->userDataStore->userData.linkagectrl[i].obj_id;
				linkageDevInfo.param = 0;
				memset(linkageDevInfo.dev_mac, 0, sizeof(linkageDevInfo.dev_mac));
				memcpy(linkageDevInfo.dev_mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
				mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);				
				sensor_linkage_control_device(&linkageDevInfo);
			}
		}
	}
	
	return ret;
}

int linkage_list_request(uint32_t device_id)
{
	int ret = -1;
	int i,j;
	uint8_t status;
	int32_t ctrl_id;
	cJSON *root;
	cJSON *cJsonValue;
	cJSON *condArray;
	cJSON *condArrayValue;
	app_context_t* app_context = get_app_context();
	uint32_t tempRDM;
	char mac_str[32];
	char *out = NULL;

	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	srand(mico_rtos_get_time());
	tempRDM = rand();
	
	Linkage_log("linkage list:\n");

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		status = app_context->userDataStore->userData.linkagectrl[i].status;
		ctrl_id = app_context->userDataStore->userData.linkagectrl[i].ctrl_id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

#ifdef USER_DEBUG_
		//Linkage_log("ctrl_id=%d status=%d\n", ctrl_id, status);
#endif
		if(( status == TIMER_LINKAGE_STATUS_START || status == TIMER_LINKAGE_STATUS_STOP ))
		{
			root=cJSON_CreateObject();
			cJSON_AddItemToObject(root, "cmd_id", cJSON_CreateNumber(tempRDM));
			cJSON_AddItemToObject(root, "CMD", cJSON_CreateNumber(LINKAGE_REQUEST));	
			cJSON_AddItemToObject(root, "gw_type", cJSON_CreateNumber(GATEWAY_TYPE));
			cJSON_AddItemToObject(root, "list", cJsonValue = cJSON_CreateObject());
		
			mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			cJSON_AddItemToObject(cJsonValue, "ctrl_id", cJSON_CreateNumber(ctrl_id));
			cJSON_AddItemToObject(cJsonValue, "ctrl_n", cJSON_CreateString(app_context->userDataStore->userData.linkagectrl[i].ctrl_name));
			cJSON_AddItemToObject(cJsonValue, "status", cJSON_CreateNumber(status));
			cJSON_AddItemToObject(cJsonValue, "env_cond", cJSON_CreateString(app_context->userDataStore->userData.linkagectrl[i].env_cond));
			cJSON_AddItemToObject(cJsonValue, "ctrl_type", cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].ctrl_type));
			cJSON_AddItemToObject(cJsonValue, "obj_id", cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].obj_id));
			cJSON_AddItemToObject(cJsonValue, "cmd", cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].cmd));
			cJSON_AddItemToObject(cJsonValue, "value", cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].param));

			memset(mac_str, 0, sizeof(mac_str));
			sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.linkagectrl[i].dev_mac[0],app_context->userDataStore->userData.linkagectrl[i].dev_mac[1],app_context->userDataStore->userData.linkagectrl[i].dev_mac[2],
				app_context->userDataStore->userData.linkagectrl[i].dev_mac[3],app_context->userDataStore->userData.linkagectrl[i].dev_mac[4],app_context->userDataStore->userData.linkagectrl[i].dev_mac[5],
				app_context->userDataStore->userData.linkagectrl[i].dev_mac[6],app_context->userDataStore->userData.linkagectrl[i].dev_mac[7],
				app_context->userDataStore->userData.linkagectrl[i].dev_mac[8],app_context->userDataStore->userData.linkagectrl[i].dev_mac[9],
				app_context->userDataStore->userData.linkagectrl[i].dev_mac[10],app_context->userDataStore->userData.linkagectrl[i].dev_mac[11]);
			cJSON_AddItemToObject(cJsonValue, "mac",cJSON_CreateString(mac_str));


			condArray = cJSON_CreateArray();
			if(condArray)
			{
				cJSON_AddItemToObject(cJsonValue, "env_paras", condArray);
				for(j=0; j < 4; j++)
				{
					if(1 == app_context->userDataStore->userData.linkagectrl[i].condition[j].valid)
					{
						cJSON_AddItemToObject(condArray, "", condArrayValue=cJSON_CreateObject());

						memset(mac_str, 0, sizeof(mac_str));
						sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[0],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[1],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[2],
							app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[3],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[4],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[5],
							app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[6],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[7],
							app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[8],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[9],
							app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[10],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[11]);
						cJSON_AddItemToObject(condArrayValue, "mac",cJSON_CreateString(mac_str));
						cJSON_AddItemToObject(condArrayValue, "sensor_type",cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].condition[j].chnl));
						cJSON_AddItemToObject(condArrayValue, "cond",cJSON_CreateString((char const *)app_context->userDataStore->userData.linkagectrl[i].condition[j].cond));
						cJSON_AddItemToObject(condArrayValue, "val",cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].condition[j].param));
					}
				}
			}
			mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

			out = cJSON_PrintUnformatted(root);
			if(out)
			{
#ifdef USER_DEBUG_
				Linkage_log("%d=%s \n", i, out);	
#endif
				pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
				free(out);
				out = NULL;
			}
			cJSON_Delete(root);
			
		}
	}
	
	return ret;
}

int linkage_ctrl_stop_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	
	LINKAGE_CTRL_INFO_T linkageInfo;

	if(NULL == root)
	{
		Linkage_log("NULL == root");
		return ret;
	}

	memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Linkage_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
		
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{

			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
			if(objectValue)
			{
				linkageInfo.ctrl_id = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.ctrl_id: %d -----\n", linkageInfo.ctrl_id);	
			}
		

			objectValue = cJSON_GetObjectItem(pArrayJson, "status");
			if(objectValue)
			{
				linkageInfo.status = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.status: %d -----\n", linkageInfo.status);	
			}	

			ret = linkage_control_update_status_request(&linkageInfo);
		}
	}
        
        return ret;
}

int linkage_ctrl_start_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	
	LINKAGE_CTRL_INFO_T linkageInfo;

	if(NULL == root)
	{
		Linkage_log("NULL == root");
		return ret;
	}

	memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Linkage_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));

		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{

			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
			if(objectValue)
			{
				linkageInfo.ctrl_id = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.ctrl_id: %d -----\n", linkageInfo.ctrl_id);	
			}
		

			objectValue = cJSON_GetObjectItem(pArrayJson, "status");
			if(objectValue)
			{
				linkageInfo.status = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.ctrlname: %d -----\n", linkageInfo.status);	
			}	
		}

		ret = linkage_control_update_status_request(&linkageInfo);
	}
        
        return ret;
}


int linkage_ctrl_delete_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	
	LINKAGE_CTRL_INFO_T linkageInfo;

	if(NULL == root)
	{
		Linkage_log("NULL == root");
		return ret;
	}

	memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Linkage_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{

			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
			if(objectValue)
			{
				linkageInfo.ctrl_id = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.ctrl_id: %d -----\n", linkageInfo.ctrl_id);	
			}
		}

		ret = linkage_control_delete_request(&linkageInfo);
	}
        
        return ret;
}


int linkage_control_update_status_request(LINKAGE_CTRL_INFO_T *pLinkageInfo)
{
	int ret = -1;
	int i;
	uint8_t status;
	int32_t ctrl_id;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pLinkageInfo)
	{
		Linkage_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);

		status = app_context->userDataStore->userData.linkagectrl[i].status;
		ctrl_id = app_context->userDataStore->userData.linkagectrl[i].ctrl_id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

		//Linkage_log("ctrl_id=%d status=%d\n", ctrl_id, status);
			
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && ctrl_id == pLinkageInfo->ctrl_id)
		{
			Linkage_log("update the  item status: %d .......\n", pLinkageInfo->status);

			mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			app_context->userDataStore->userData.linkagectrl[i].status = pLinkageInfo->status;
			mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			
			user_data_store_update();
			ret =  0;
			return ret;
		}
	}
	
	return ret;
}

int linkage_deleteFixedMacItem(char *pMac)
{
	int ret = -1;
	int i,j;
	uint8_t status;
	//uint8_t ctrl_id;
	uint8_t dev_mac[32];
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pMac)
	{
		Linkage_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		status = app_context->userDataStore->userData.linkagectrl[i].status;
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

#ifdef USER_DEBUG_
		Linkage_log("status=%d \n", status);
#endif
			
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) )
		{
			for(j=0; j < 4; j++)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
				memset(dev_mac, 0, sizeof(dev_mac));
				memcpy(dev_mac, app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
				mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

				//HexDump(dev_mac,DEVICE_MAC_ADDRESS_LENGHT);
				//HexDump(pMac,DEVICE_MAC_ADDRESS_LENGHT);
				if(!memcmp(dev_mac, pMac, DEVICE_MAC_ADDRESS_LENGHT))
				{
					Linkage_log("delete  the  item .......\n");

					mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
					memset(&app_context->userDataStore->userData.linkagectrl[i], 0, sizeof(linkage_dev_param_t));
					mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

					break;
					//ret =  user_data_store_update();// save in call
					//return 1;
				}
			}
		}
	}
	
	return 1;
}

int linkage_control_delete_request(LINKAGE_CTRL_INFO_T *pLinkageInfo)
{
	int ret = -1;
	int i;
	uint8_t status;
	uint8_t ctrl_id;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pLinkageInfo)
	{
		Linkage_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		status = app_context->userDataStore->userData.linkagectrl[i].status;
		ctrl_id = app_context->userDataStore->userData.linkagectrl[i].ctrl_id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

		Linkage_log("ctrl_id=%d status=%d \n", ctrl_id, status);
			
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && ctrl_id == pLinkageInfo->ctrl_id)
		{
			Linkage_log("delete  the  item .......\n");

			mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			app_context->userDataStore->userData.linkagectrl[i].status = TIMER_LINKAGE_STATUS_DELETE;
			mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			
			ret =  0;
			user_data_store_update();
			return ret;
		}
	}
	
	return ret;
}

int linkage_ctrl_modify_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i, j, nstrlen;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	cJSON *condArrayJson = NULL;
	cJSON *condItemObj = NULL;
	cJSON *condItemValue = NULL;
	int  condArraySize;
	uint8_t mac_str[32];
	LINKAGE_CTRL_INFO_T linkageInfo;

	if(NULL == root)
	{
		Linkage_log("NULL == root");
		return ret;
	}

	memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
		
	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Linkage_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
			if(objectValue)
			{
				linkageInfo.ctrl_id = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.ctrl_id: %d -----\n", linkageInfo.ctrl_id);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "mac");
			if(objectValue && objectValue->valuestring != NULL)
			{
				memset(mac_str, 0, sizeof(mac_str));
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(mac_str, objectValue->valuestring, nstrlen);

				Linkage_log("-----------mac_str: %s ------\n", mac_str);	
				nstrlen /= 2;
				for( j = 0; j < nstrlen ; j++ )
				{
					charchangehex( &mac_str[j*2] , &linkageInfo.dev_mac[j] );
				}		
			}
		
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_n");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(linkageInfo.ctrlname, objectValue->valuestring, nstrlen);

				Linkage_log("-----------linkageInfo.ctrlname: %s -----\n", linkageInfo.ctrlname);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "env_cond");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 8)nstrlen=8;
					memcpy(linkageInfo.env_cond, objectValue->valuestring, nstrlen);

				Linkage_log("-----------linkageInfo.env_cond: %s -----\n", linkageInfo.env_cond);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "cmd");
			if( objectValue )
			{
				linkageInfo.cmd = objectValue->valueint;
				Linkage_log("-----------linkageInfo.cmd: %d -----\n", linkageInfo.cmd);	
			}
			

			objectValue = cJSON_GetObjectItem(pArrayJson, "status");
			if(objectValue)
			{
				linkageInfo.status = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.status: %d -----\n", linkageInfo.status);	
			}
			
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_type");
			if(objectValue)
			{
				linkageInfo.ctrl_type = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.ctrl_type: %d -----\n", linkageInfo.ctrl_type);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "obj_id");
			if(objectValue)
			{
				linkageInfo.obj_id = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.obj_id: %d -----\n", linkageInfo.obj_id);	
			}
			objectValue = cJSON_GetObjectItem(pArrayJson, "value");
			if(objectValue)
			{
				linkageInfo.value = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.value: %d -----\n", linkageInfo.value);	
			}

			condArrayJson = cJSON_GetObjectItem(pArrayJson,"env_paras");
			if(NULL == condArrayJson)
			{
				Linkage_log("========env_paras a===========\n");
				continue;
			}
			
			condArraySize = cJSON_GetArraySize(condArrayJson);
			Linkage_log("-------condArraySize:%d --------------\n", condArraySize);
			if(condArraySize <= 0){
				continue;
			}

			if(condArraySize > 2)condArraySize=2;
			
			for(j=0; j < condArraySize; j++)
			{
				condItemObj = cJSON_GetArrayItem(condArrayJson, i);
				if(condItemObj)
				{

					condItemValue = cJSON_GetObjectItem(condItemObj, "mac");
					if(condItemValue && condItemValue->valuestring != NULL)
					{
						int index;
						memset(mac_str, 0, sizeof(mac_str));
						nstrlen = strlen(condItemValue->valuestring);
						if(nstrlen > 32)nstrlen=32;
							memcpy(mac_str, condItemValue->valuestring, nstrlen);

						Linkage_log("-----------mac_str: %s ------\n", mac_str);	
						nstrlen /= 2;
						for( index = 0; index < nstrlen ; index++ )
						{
							charchangehex( &mac_str[index*2] , &linkageInfo.linkageCond[j].mac[index] );
						}
						
						//if(isKitchenSensorDevice(linkageInfo.linkageCond[j].mac)==1)
						//	return -1;
					}

					condItemValue = cJSON_GetObjectItem(condItemObj, "sensor_type");
					if(condItemValue)
					{
						linkageInfo.linkageCond[j].sensor_type = condItemValue->valueint;
						Linkage_log("-----------linkageInfo.sensor_type: %d -----\n", linkageInfo.linkageCond[j].sensor_type );	
					}
				
					condItemValue = cJSON_GetObjectItem(condItemObj, "cond");
					if(condItemValue && condItemValue->valuestring != NULL)
					{
						nstrlen = strlen(condItemValue->valuestring);
						if(nstrlen > 8)nstrlen=8;
							memcpy(linkageInfo.linkageCond[j].env_cond, condItemValue->valuestring, nstrlen);

						Linkage_log("-----------linkageInfo.linkageCond[j].env_cond: %s -----\n", linkageInfo.linkageCond[j].env_cond);	
					}

					condItemValue = cJSON_GetObjectItem(condItemObj, "val");
					if(condItemValue)
					{
						linkageInfo.linkageCond[j].value = condItemValue->valueint;
						Linkage_log("-----------linkageInfo.value: %d -----\n", linkageInfo.linkageCond[j].value );	
					}	
				}
			}	
		}

		ret = linkage_control_modify_request(&linkageInfo);
	}
        
        return ret;
}

int linkage_ctrl_new_add_request(cJSON *root)
{
	int ret = -1;
	cJSON *stateArrayJson = NULL;
	int nArrayNum;
	int i, j, nstrlen;
	cJSON *pArrayJson = NULL;
	cJSON *objectValue;
	cJSON *condArrayJson = NULL;
	cJSON *condItemObj = NULL;
	cJSON *condItemValue = NULL;
	int  condArraySize;
	uint8_t mac_str[32];
	LINKAGE_CTRL_INFO_T linkageInfo;

	if(NULL == root)
	{
		Linkage_log("NULL == root");
		return ret;
	}

	memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));

	stateArrayJson = cJSON_GetObjectItem(root,"list");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Linkage_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return ret;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(&linkageInfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_n");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(linkageInfo.ctrlname, objectValue->valuestring, nstrlen);

				Linkage_log("-----------linkageInfo.ctrlname: %s -----\n", linkageInfo.ctrlname);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "env_cond");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 4)nstrlen=4;
					memcpy(linkageInfo.env_cond, objectValue->valuestring, nstrlen);

				Linkage_log("-----------linkageInfo.env_cond: %s -----\n", linkageInfo.env_cond);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "mac");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(mac_str, objectValue->valuestring, nstrlen);

				Linkage_log("-----------mac_str: %s ------\n", mac_str);	
				nstrlen /= 2;
				for( j = 0; j < nstrlen ; j++ )
				{
					charchangehex( &mac_str[j*2] , &linkageInfo.dev_mac[j] );
				}	
			}
			

			objectValue = cJSON_GetObjectItem(pArrayJson, "cmd");
			if(objectValue )
			{
				linkageInfo.cmd = objectValue->valueint;
				Linkage_log("-----------linkageInfo.cmd: %d -----\n", linkageInfo.cmd);	
			}
			

			objectValue = cJSON_GetObjectItem(pArrayJson, "status");
			if(objectValue)
			{
				linkageInfo.status = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.status: %d -----\n", linkageInfo.status);	
			}
			
			objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_type");
			if(objectValue)
			{
				linkageInfo.ctrl_type = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.ctrl_type: %d -----\n", linkageInfo.ctrl_type);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "obj_id");
			if(objectValue)
			{
				linkageInfo.obj_id = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.obj_id: %d -----\n", linkageInfo.obj_id);	
			}
			objectValue = cJSON_GetObjectItem(pArrayJson, "value");
			if(objectValue)
			{
				linkageInfo.value = objectValue->valueint;
				
				Linkage_log("-----------linkageInfo.value: %x -----\n", linkageInfo.value);	
			}

			condArrayJson = cJSON_GetObjectItem(pArrayJson,"env_paras");
			if(NULL == condArrayJson)
			{
				Linkage_log("------- env_paras is NULL ---- -----\n");	
				continue;
			}
			
			condArraySize = cJSON_GetArraySize(condArrayJson);
			Linkage_log("-------condArraySize:%d --------------\n", condArraySize);
			if(condArraySize <= 0){
				continue;
			}

			if(condArraySize > 2)condArraySize=2;

			linkageInfo.conds_size = condArraySize;
			
			for(j=0; j < condArraySize; j++)
			{
				condItemObj = cJSON_GetArrayItem(condArrayJson, j);
				if(condItemObj)
				{

					condItemValue = cJSON_GetObjectItem(condItemObj, "mac");
					if(condItemValue && condItemValue->valuestring != NULL)
					{
						int index;
						memset(mac_str, 0, sizeof(mac_str));
						nstrlen = strlen(condItemValue->valuestring);
						if(nstrlen > 32)nstrlen=32;
							memcpy(mac_str, condItemValue->valuestring, nstrlen);

						Linkage_log("-----------mac_str: %s ------\n", mac_str);	
						nstrlen /= 2;
						for( index = 0; index < nstrlen ; index++ )
						{
							charchangehex( &mac_str[index*2], &linkageInfo.linkageCond[j].mac[index] );
						}	

						//if(isKitchenSensorDevice(linkageInfo.linkageCond[j].mac)==1)
						//	return -1;
					}
				
					condItemValue = cJSON_GetObjectItem(condItemObj, "sensor_type");
					if(condItemValue)
					{
						linkageInfo.linkageCond[j].sensor_type = condItemValue->valueint;
						Linkage_log("-----------linkageInfo.sensor_type: %d -----\n", linkageInfo.linkageCond[j].sensor_type );	
					}	
				
					condItemValue = cJSON_GetObjectItem(condItemObj, "cond");
					if(condItemValue && condItemValue->valuestring != NULL)
					{
						nstrlen = strlen(condItemValue->valuestring);
						if(nstrlen > 4)nstrlen=4;
							memcpy(linkageInfo.linkageCond[j].env_cond, condItemValue->valuestring, nstrlen);

						Linkage_log("-----------linkageInfo.linkageCond[j].env_cond: %s -----\n", linkageInfo.linkageCond[j].env_cond);	
					}

					condItemValue = cJSON_GetObjectItem(condItemObj, "val");
					if(condItemValue)
					{
						linkageInfo.linkageCond[j].value = condItemValue->valueint;
						Linkage_log("-----------linkageInfo.value: %d -----\n", linkageInfo.linkageCond[j].value );	
					}	
				}
			}

			ret = linkage_control_add_request(&linkageInfo);
			if( ret== 0)
			{
				objectValue = cJSON_GetObjectItem(pArrayJson, "ctrl_id");
				if(objectValue)
				{
					cJSON_ReplaceItemInObject(pArrayJson, "ctrl_id", cJSON_CreateNumber(linkageInfo.ctrl_id));
				}	
			}
			
		}
		
	}
        
        return ret;
}

int linkage_control_modify_request(LINKAGE_CTRL_INFO_T *pLinkageInfo)
{
	int ret = -1;
	int i,j;
	uint8_t status;
	uint8_t ctrl_id;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pLinkageInfo)
	{
		Linkage_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		ctrl_id = app_context->userDataStore->userData.linkagectrl[i].ctrl_id;
		status = app_context->userDataStore->userData.linkagectrl[i].status;
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

		Linkage_log("ctrl_id=%d \n", ctrl_id);
			
		if((status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && ctrl_id == pLinkageInfo->ctrl_id)
		{
			Linkage_log("Modify the same item i=%d .......\n", i);

			mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);

			app_context->userDataStore->userData.linkagectrl[i].status = pLinkageInfo->status;
			app_context->userDataStore->userData.linkagectrl[i].ctrl_type = pLinkageInfo->ctrl_type;
			app_context->userDataStore->userData.linkagectrl[i].obj_id = pLinkageInfo->obj_id;
			app_context->userDataStore->userData.linkagectrl[i].cmd = pLinkageInfo->cmd;
			app_context->userDataStore->userData.linkagectrl[i].param = pLinkageInfo->value;
			memcpy(app_context->userDataStore->userData.linkagectrl[i].ctrl_name, pLinkageInfo->ctrlname, sizeof(app_context->userDataStore->userData.linkagectrl[i].ctrl_name));
			memcpy(app_context->userDataStore->userData.linkagectrl[i].dev_mac, pLinkageInfo->dev_mac, DEVICE_MAC_ADDRESS_LENGHT);

			for(j=0; j < 2; j++)
			{
				if(1 == app_context->userDataStore->userData.linkagectrl[i].condition[j].valid && app_context->userDataStore->userData.linkagectrl[i].condition[j].chnl == pLinkageInfo->linkageCond[j].sensor_type)
				{
					app_context->userDataStore->userData.linkagectrl[i].condition[j].chnl = pLinkageInfo->linkageCond[j].sensor_type;
					app_context->userDataStore->userData.linkagectrl[i].condition[j].param = pLinkageInfo->linkageCond[j].value;

					//HexDump(pLinkageInfo->linkageCond[j].mac, DEVICE_MAC_ADDRESS_LENGHT);
					memcpy(app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac, pLinkageInfo->linkageCond[j].mac, DEVICE_MAC_ADDRESS_LENGHT);
					memset(app_context->userDataStore->userData.linkagectrl[i].condition[j].cond, 0, sizeof(app_context->userDataStore->userData.linkagectrl[i].condition[j].cond));
					memcpy(app_context->userDataStore->userData.linkagectrl[i].condition[j].cond, pLinkageInfo->linkageCond[j].env_cond, sizeof(app_context->userDataStore->userData.linkagectrl[i].condition[j].cond));
				}
			}
			mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			
			user_data_store_update();
			ret =0;
			return ret;
		}

	
	}
	
	
	return ret;
}

int linkage_control_add_request(LINKAGE_CTRL_INFO_T *pLinkageInfo)
{
	int ret = -1;
	int i,j;
	int32_t act_param;
	int new_add_device_index = -1;
	uint8_t status, endpoint_id,sensor_type;
	uint8_t dev_mac[32+1];
	uint8_t sensor_dev_mac[32+1];
	app_context_t* app_context = get_app_context();
	
	if(NULL == pLinkageInfo)
	{
		Linkage_log("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);

		status = app_context->userDataStore->userData.linkagectrl[i].status;
		endpoint_id = app_context->userDataStore->userData.linkagectrl[i].obj_id;
		act_param = app_context->userDataStore->userData.linkagectrl[i].param;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.linkagectrl[i].dev_mac, sizeof(app_context->userDataStore->userData.linkagectrl[i].dev_mac));
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

		Linkage_log("status=%d  \n", status);
		HexDump(dev_mac, DEVICE_MAC_ADDRESS_LENGHT );
			
		if( (status == TIMER_LINKAGE_STATUS_START ||status == TIMER_LINKAGE_STATUS_STOP ) && \
			!memcmp((char const *)dev_mac, (char const *)pLinkageInfo->dev_mac, DEVICE_MAC_ADDRESS_LENGHT) && endpoint_id == pLinkageInfo->obj_id &&  act_param == pLinkageInfo->value)
		{
			int loop;
			for(loop=0; loop < 2; loop++)
			{
				memset(sensor_dev_mac, 0, sizeof(sensor_dev_mac));
				sensor_type = app_context->userDataStore->userData.linkagectrl[i].condition[loop].chnl;
				memcpy(sensor_dev_mac, app_context->userDataStore->userData.linkagectrl[i].condition[loop].dev_mac, DEVICE_MAC_ADDRESS_LENGHT);

				if(app_context->userDataStore->userData.linkagectrl[i].condition[loop].valid == 1)
				{
					if((!memcmp((char const *)sensor_dev_mac, (char const *)pLinkageInfo->linkageCond[0].mac, DEVICE_MAC_ADDRESS_LENGHT) && sensor_type == pLinkageInfo->linkageCond[0].sensor_type) || \
						(!memcmp((char const *)sensor_dev_mac, (char const *)pLinkageInfo->linkageCond[1].mac, DEVICE_MAC_ADDRESS_LENGHT) && sensor_type == pLinkageInfo->linkageCond[1].sensor_type))
					{
						Linkage_log("had same linkage name item.\n");
						ret = 1;
						return ret;
					}
				}
			}
		}

		if(new_add_device_index == -1 && (status != TIMER_LINKAGE_STATUS_START && status != TIMER_LINKAGE_STATUS_STOP ) )
		{	
			new_add_device_index = i;
			break;
		}	
	}

	Linkage_log("************************** new_add_device_index: %d \n", new_add_device_index);
	if(new_add_device_index >= 0)
	{
		pLinkageInfo->ctrl_id = new_add_device_index;
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		memset(&app_context->userDataStore->userData.linkagectrl[new_add_device_index], 0, sizeof(linkage_dev_param_t));
		app_context->userDataStore->userData.linkagectrl[new_add_device_index].ctrl_id = new_add_device_index;
		app_context->userDataStore->userData.linkagectrl[new_add_device_index].status = pLinkageInfo->status;
		app_context->userDataStore->userData.linkagectrl[new_add_device_index].ctrl_type = pLinkageInfo->ctrl_type;
		app_context->userDataStore->userData.linkagectrl[new_add_device_index].obj_id = pLinkageInfo->obj_id;
		app_context->userDataStore->userData.linkagectrl[new_add_device_index].cmd = pLinkageInfo->cmd;
		app_context->userDataStore->userData.linkagectrl[new_add_device_index].param = pLinkageInfo->value;
		app_context->userDataStore->userData.linkagectrl[new_add_device_index].happend = 0;
		//HexDump(pLinkageInfo->dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		memcpy(app_context->userDataStore->userData.linkagectrl[new_add_device_index].dev_mac, pLinkageInfo->dev_mac, DEVICE_MAC_ADDRESS_LENGHT);

		memcpy(app_context->userDataStore->userData.linkagectrl[new_add_device_index].ctrl_name, pLinkageInfo->ctrlname, sizeof(app_context->userDataStore->userData.linkagectrl[new_add_device_index].ctrl_name));
		memcpy(app_context->userDataStore->userData.linkagectrl[new_add_device_index].env_cond, pLinkageInfo->env_cond, sizeof(app_context->userDataStore->userData.linkagectrl[new_add_device_index].env_cond));

		if(pLinkageInfo->conds_size > 4)pLinkageInfo->conds_size=4;
		for(j=0; j < pLinkageInfo->conds_size; j++)
		{
			app_context->userDataStore->userData.linkagectrl[new_add_device_index].condition[j].chnl = pLinkageInfo->linkageCond[j].sensor_type;
			app_context->userDataStore->userData.linkagectrl[new_add_device_index].condition[j].param = pLinkageInfo->linkageCond[j].value;
			app_context->userDataStore->userData.linkagectrl[new_add_device_index].condition[j].valid = 1;
			//HexDump(pLinkageInfo->linkageCond[j].mac, DEVICE_MAC_ADDRESS_LENGHT);
			memcpy(app_context->userDataStore->userData.linkagectrl[new_add_device_index].condition[j].dev_mac, pLinkageInfo->linkageCond[j].mac, DEVICE_MAC_ADDRESS_LENGHT);
			memset(app_context->userDataStore->userData.linkagectrl[new_add_device_index].condition[j].cond, 0, sizeof(app_context->userDataStore->userData.linkagectrl[new_add_device_index].condition[j].cond));
			memcpy(app_context->userDataStore->userData.linkagectrl[new_add_device_index].condition[j].cond, pLinkageInfo->linkageCond[j].env_cond, sizeof(app_context->userDataStore->userData.linkagectrl[new_add_device_index].condition[j].cond));
		}

		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		//user_data_store_update();//use timer delay save
		device_delay_save_timerStart();
		return 0;
	}
	else
	{
		Linkage_log("No space to add new panel device..............!\n");
	}
	
	
	return ret;
}


void  print_linkage_list()
{
	int i,j;
	uint8_t status;
	int32_t ctrl_id;
	cJSON *root;
	cJSON *cJsonValue;
	cJSON *condArray;
	cJSON *condArrayValue;
	app_context_t* app_context = get_app_context();
	uint32_t tempRDM;
	char mac_str[32];
	char *out = NULL;

	if(NULL == app_context)
	{
		Linkage_log("app_context is NULL.\n");
		return;
	}

	srand(mico_rtos_get_time());
	tempRDM = rand();

	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
		status = app_context->userDataStore->userData.linkagectrl[i].status;
		ctrl_id = app_context->userDataStore->userData.linkagectrl[i].ctrl_id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

//		if(( status == TIMER_LINKAGE_STATUS_START || status == TIMER_LINKAGE_STATUS_STOP ))
		{
			root=cJSON_CreateObject();
			cJSON_AddItemToObject(root, "cmd_id", cJSON_CreateNumber(tempRDM));
			cJSON_AddItemToObject(root, "CMD", cJSON_CreateNumber(LINKAGE_REQUEST));	
			cJSON_AddItemToObject(root, "gw_type", cJSON_CreateNumber(GATEWAY_TYPE));
			cJSON_AddItemToObject(root, "list", cJsonValue = cJSON_CreateObject());
		
			mico_rtos_lock_mutex(&app_context->userDataStore->linkagectrl_mutex);
			cJSON_AddItemToObject(cJsonValue, "ctrl_id", cJSON_CreateNumber(ctrl_id));
			cJSON_AddItemToObject(cJsonValue, "ctrl_n", cJSON_CreateString(app_context->userDataStore->userData.linkagectrl[i].ctrl_name));
			cJSON_AddItemToObject(cJsonValue, "status", cJSON_CreateNumber(status));
			cJSON_AddItemToObject(cJsonValue, "env_cond", cJSON_CreateString(app_context->userDataStore->userData.linkagectrl[i].env_cond));
			cJSON_AddItemToObject(cJsonValue, "ctrl_type", cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].ctrl_type));
			cJSON_AddItemToObject(cJsonValue, "obj_id", cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].obj_id));
			cJSON_AddItemToObject(cJsonValue, "cmd", cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].cmd));
			cJSON_AddItemToObject(cJsonValue, "value", cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].param));

			memset(mac_str, 0, sizeof(mac_str));
			sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.linkagectrl[i].dev_mac[0],app_context->userDataStore->userData.linkagectrl[i].dev_mac[1],app_context->userDataStore->userData.linkagectrl[i].dev_mac[2],
				app_context->userDataStore->userData.linkagectrl[i].dev_mac[3],app_context->userDataStore->userData.linkagectrl[i].dev_mac[4],app_context->userDataStore->userData.linkagectrl[i].dev_mac[5],
				app_context->userDataStore->userData.linkagectrl[i].dev_mac[6],app_context->userDataStore->userData.linkagectrl[i].dev_mac[7],
				app_context->userDataStore->userData.linkagectrl[i].dev_mac[8],app_context->userDataStore->userData.linkagectrl[i].dev_mac[9],
				app_context->userDataStore->userData.linkagectrl[i].dev_mac[10],app_context->userDataStore->userData.linkagectrl[i].dev_mac[11]);
			cJSON_AddItemToObject(cJsonValue, "mac",cJSON_CreateString(mac_str));


			condArray = cJSON_CreateArray();
			if(condArray)
			{
				cJSON_AddItemToObject(cJsonValue, "env_paras", condArray);
				for(j=0; j < 4; j++)
				{
					if(1 == app_context->userDataStore->userData.linkagectrl[i].condition[j].valid)
					{
						cJSON_AddItemToObject(condArray, "", condArrayValue=cJSON_CreateObject());

						memset(mac_str, 0, sizeof(mac_str));
						sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[0],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[1],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[2],
							app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[3],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[4],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[5],
							app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[6],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[7],
							app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[8],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[9],
							app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[10],app_context->userDataStore->userData.linkagectrl[i].condition[j].dev_mac[11]);
						cJSON_AddItemToObject(condArrayValue, "mac",cJSON_CreateString(mac_str));
						cJSON_AddItemToObject(condArrayValue, "sensor_type",cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].condition[j].chnl));
						cJSON_AddItemToObject(condArrayValue, "cond",cJSON_CreateString((char const *)app_context->userDataStore->userData.linkagectrl[i].condition[j].cond));
						cJSON_AddItemToObject(condArrayValue, "val",cJSON_CreateNumber(app_context->userDataStore->userData.linkagectrl[i].condition[j].param));
					}
				}
			}
			mico_rtos_unlock_mutex(&app_context->userDataStore->linkagectrl_mutex);

			out = cJSON_PrintUnformatted(root);
			if(out)
			{
				Linkage_log("i:%d  linkageinfo=%s \n",i, out);	
				free(out);
				out = NULL;
			}
			cJSON_Delete(root);			
		}
	}
	

}



