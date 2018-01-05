
#include "networkserver.h"
#include "user_main.h"
#include "devicemanager.h"
#include "scenemanager.h"
#include "linkagemanager.h"
#include "timercontrol.h"
#include "user_config.h"
#include "rs485_uart.h"

#define Network_log(M, ...) custom_log("Network_log", M, ##__VA_ARGS__)

//CMD¶¨Òå
#define C2D_GET_TEMPLATE                            (100)
#define C2D_SEND_COMMANDS                           (101)
#define D2C_ACK_TEMPLATE                            (200)
#define D2C_SEND_PACKETS                            (201)
#define APP_REQUEST_GW_VERSION               (600)


extern app_context_t*  get_app_context();
extern OSStatus pushAppMainRecvMsgProcess(unsigned char *msg, unsigned int inBufLen, int riu);
extern OSStatus pushSend2ServerMsgProcess(unsigned char *msg, unsigned int inBufLen, uint32_t device_id);
extern unsigned short ChangeEndian16(unsigned short value);
extern  unsigned int charchangehex( unsigned char *data , unsigned char* value );
extern void broadcast_server_data_to_allapp(unsigned char * data, x_uint32 datalen,x_uint32 device_id);

int broadcast_rs485_cmdpayload(unsigned char * recvbuff,uint32_t device_id)
{
	cJSON *root;
	char *out = NULL;
	uint8_t  sendbuff[USER_NUMBER_VAL_256+USER_NUMBER_VAL_1]={0};
	

	if(NULL == recvbuff)
	{
		return -1;
	}

	root=cJSON_CreateObject();	
	 if(root != NULL)
	{
		memcpy(sendbuff, recvbuff, strlen(recvbuff));

		cJSON_AddItemToObject(root, "t", cJSON_CreateNumber(299));

		cJSON_AddItemToObject(root, "rs485",cJSON_CreateString((char const *)sendbuff));

		out = cJSON_PrintUnformatted(root);
		if(out)
		{
		//	Network_log("broadcast:%s ",out);

			broadcast_server_data_to_allapp((unsigned char *)out, strlen(out),0);//0 : broadcast
			free(out);
			out = NULL;
				
		}

		cJSON_Delete(root);
	 }

	return 0;
}

#if 0
//"cus scene recall 0x0001 0x00\r\n send 0xFFFF 1 1\r\n"
int parse_scene_racall_cmd( char *buf)
{
	int in=0;
	uint16_t groupid;
	uint8_t sceneid;
	char *p[5];

	if(buf){
	
		while((p[in]=strtok(buf," "))!=NULL) {
			if(in ==3){
			groupid = strtol((char const *)p[in],NULL, 16);	
			Network_log("groupid:%04x",groupid);
			}
			else if(in ==4){
				sceneid = strtol((char const *)p[in],NULL, 16);	
				Network_log("sceneid:%d",sceneid);
			//	devicechldata_syn_scenedata(groupid, sceneid);
				break;
			}
			in++;
			buf=NULL; 
			if(in>4)
				break;
		
		}
	}
	
	return 0;
}

#endif

int rs485data_send_to_queue_parse(unsigned char * recvbuff)
{
	cJSON *root;
	char *out = NULL;

	if(NULL == recvbuff)
	{
		return -1;
	}

	root=cJSON_CreateObject();	
	 if(root != NULL)
	{

		cJSON_AddItemToObject(root, "t", cJSON_CreateNumber(299));
		cJSON_AddItemToObject(root, "rs485",cJSON_CreateString((char const *)recvbuff));
		
		uart_zigbee_msg_insertinto_queue(root);//send to parse queue

		out = cJSON_PrintUnformatted(root);
		if(out)
		{
			//Network_log("rs485data broadcast:%s ",out);			
			free(out);
			out = NULL;
				
		}
	 }

	return 0;
}

int broadcast_zigbee_cmdpayload(unsigned char * recvbuff,uint32_t device_id)
{
	cJSON *root;
	char *out = NULL;
	char idsbuf[16+1] = {0};
	char paybuf[12+1] = {0};
	char groupid[4]={0};
	char seneid[2]={0};
	char id[4] =  {0};
	char  sendpoint[1]={0};  
	char  dendpoint[1]={0};


	if(NULL == recvbuff)
	{
		return -1;
	}

	root=cJSON_CreateObject();

	 if(root != NULL)
	{

		strncpy(groupid, recvbuff+19, 4);
		strncpy(seneid, recvbuff+26, 2);
		strncpy(id, recvbuff+38, 4);	
		strncpy(sendpoint, recvbuff+43, 1);
		strncpy(dendpoint, recvbuff+45, 1);
		
		sprintf(idsbuf, "%.4s01040005%02d%02d",id, dendpoint[0]-'0', sendpoint[0]-'0'); 
     		sprintf(paybuf, "190005%.4s%.2s", groupid, seneid);	


		cJSON_AddItemToObject(root, "t", cJSON_CreateNumber(17));
		cJSON_AddItemToObject(root, "ids",cJSON_CreateString((char const *)idsbuf));
		cJSON_AddItemToObject(root, "pay",cJSON_CreateString((char const *)paybuf));

		out = cJSON_PrintUnformatted(root);
		if(out)
		{
			Network_log("broadcast:%s ",out);

			broadcast_server_data_to_allapp((unsigned char *)out, strlen(out),0);//0 : broadcast to all app
			free(out);
			out = NULL;
				
		}

		cJSON_Delete(root);
	 }

	return 0;
}



int send_device_connected_info2panel(DEVICE_INFO_T *deviceInfo)
{
	int ret = -1;
	if(NULL == deviceInfo)
	{
		Network_log("NULL == deviceInfo");
		return ret;
	}
	
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));
	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "%04X %04X %d %d \r\n", deviceInfo->dev_nwkid, deviceInfo->dev_nclu, deviceInfo->endpoit_id, deviceInfo->dev_val);
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
	

	return ret;
}

OSStatus getSensorListInfo(uint32_t device_id)
{
	OSStatus err = kNoErr;
	cJSON *root = NULL;
	cJSON  * cJsonArray = NULL;
	cJSON  * cJsonValue = NULL;	
	
	cJSON *sensorCntRoot = NULL;
	cJSON *sensorMacArray= NULL;
	
	cJSON  * endpointArray = NULL;
	cJSON  * endPointValue = NULL;	
	uint8_t   alloc_flag;
	int32_t   value;
	int i = 0;
	uint32_t tempRDM;
	uint32_t dev_class;
	char *out = NULL;
	uint8_t is_can_add;
	int j;
	char mac_str[32];
	char nwk_str[16];
	app_context_t* app_context = (app_context_t* )get_app_context();

	#ifdef USER_DEBUG_
	Network_log("sensorList : ");	
	#endif
	
	srand(mico_rtos_get_time());
	tempRDM = rand();

	sensorCntRoot = cJSON_CreateObject();
	cJSON_AddItemToObject(sensorCntRoot, "cmd_id", cJSON_CreateNumber(tempRDM));
	cJSON_AddItemToObject(sensorCntRoot, "CMD", cJSON_CreateNumber(98));
	cJSON_AddItemToObject(sensorCntRoot, "sensorMacArray", sensorMacArray = cJSON_CreateArray());

	
	for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		dev_class = app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

		if(is_can_add == 1 )
		{
			root=cJSON_CreateObject();
			cJSON_AddItemToObject(root, "cmd_id", cJSON_CreateNumber(tempRDM));
			cJSON_AddItemToObject(root, "CMD", cJSON_CreateNumber(GET_ALL_SENSOR_LIST));	
			cJSON_AddItemToObject(root, "gw_type", cJSON_CreateNumber(GATEWAY_TYPE));
			cJSON_AddItemToObject(root, "devices", cJsonArray = cJSON_CreateArray());

			cJSON_AddItemToObject(cJsonArray, "", cJsonValue=cJSON_CreateObject());

			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			memset(mac_str, 0, sizeof(mac_str));
			sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[0],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[1],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[2],
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[3],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[4],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[5],
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[6],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[7],
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[8],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[9],
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[10],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[11]);
			cJSON_AddItemToObject(cJsonValue, "mac",cJSON_CreateString(mac_str));
			cJSON_AddItemToObject(cJsonValue, "class",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_class));
			cJSON_AddItemToObject(cJsonValue, "online",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_onoffline));
			cJSON_AddItemToObject(cJsonValue, "riu",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.sensor_info[i].riu));

			cJSON_AddItemToArray(sensorMacArray, cJSON_CreateString(mac_str));

			memset(nwk_str, 0, sizeof(nwk_str));
			sprintf(nwk_str, "%04X", app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr);
			cJSON_AddItemToObject(cJsonValue, "nwkid",cJSON_CreateString(nwk_str));
			
			endpointArray = cJSON_CreateArray();
			if(endpointArray)
			{
				cJSON_AddItemToObject(cJsonValue, "chnl", endpointArray);

				for(j=0; j < SENSOR_CHANL_MAX; j++)
				{
					alloc_flag = app_context->userDataStore->userData.deviceinfo.sensor_info[i].chnl[j].alloc_flag;
					value = app_context->userDataStore->userData.deviceinfo.sensor_info[i].chnl[j].value;
					if((dev_class != 300  && 1 == alloc_flag) || (dev_class == 300 &&  1 == alloc_flag))
					//if( 1 == alloc_flag)
					{
						cJSON_AddItemToObject(endpointArray, "", endPointValue=cJSON_CreateObject());
						cJSON_AddItemToObject(endPointValue, "type",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.sensor_info[i].chnl[j].dev_type));
						cJSON_AddItemToObject(endPointValue, "val",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.sensor_info[i].chnl[j].value));
						cJSON_AddItemToObject(endPointValue, "alloc",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.sensor_info[i].chnl[j].alloc_flag));
					}
				}
			}
			
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
			
			out = cJSON_PrintUnformatted(root);
			if(out)
			{
			#ifdef USER_DEBUG_
				Network_log("%d:%s", i, out);	
			#endif
				pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
				//mico_rtos_thread_msleep(500);
				free(out);
				out = NULL;
			}

			cJSON_Delete(root);
		}
	}

	out = cJSON_PrintUnformatted(sensorCntRoot);
	if(out)
	{
		Network_log("sensormacjson:%s", out);
		pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
		free(out);
		out = NULL;
	}
	cJSON_Delete(sensorCntRoot);



	return err;	
}


/*
	Description: remove device(light\warmwind\airchange\lightwarm) success return 1 else return -1.
*/
static int removeDevice( char mac_str2[], char nwk_str2[] )
{
	int i;
	uint8_t is_can_add,riu;
	uint16_t nw_addr;
	int ret = -1;
	app_context_t* app_context = (app_context_t* )get_app_context();
	char mac_str[32];
	char nwk_str[16];
	Network_log("removeDevice in\r\nmac_str2:%s", mac_str2);
	
	
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1 )
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			memset(mac_str, 0, sizeof(mac_str));
			sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.deviceinfo.device_info[i].mac[0],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[1],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[2],
				app_context->userDataStore->userData.deviceinfo.device_info[i].mac[3],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[4],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[5],
				app_context->userDataStore->userData.deviceinfo.device_info[i].mac[6],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[7],
				app_context->userDataStore->userData.deviceinfo.device_info[i].mac[8],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[9],
				app_context->userDataStore->userData.deviceinfo.device_info[i].mac[10],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[11] );
			memset(nwk_str, 0, sizeof(nwk_str));
			nw_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
			riu = app_context->userDataStore->userData.deviceinfo.device_info[i].riu;
			sprintf(nwk_str, "%04X", nw_addr);
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			Network_log("mac_str mac:%s \r\n", mac_str);
			Network_log("nwk addr:%s", nwk_str);
			Network_log("mac_str len:%d",sizeof(mac_str));

		
			if(!memcmp(mac_str, mac_str2, sizeof(mac_str)))// || !memcmp(nwk_str2, nwk_str, sizeof(nwk_str))
			{
				//memset(&app_context->userDataStore->userData.deviceinfo.device_info[i], 0, sizeof(device_info_list_t));
				//app_context->userDataStore->userData.deviceinfo.device_info[i].is_used = 0;
				if(1 == riu)
				{
					zigbee_device_delete_from_net(nw_addr);
				}
				else
				{
					rs485_device_delete_from_net(nw_addr);
				}
				
				removeInvalidTimerItem((uint8_t *)mac_str);
				user_data_store_update();
				ret = 0;
				break;
			}
		}
	}

	//can't find out device in the device list. find sensor list.
	if(ret == -1)
	{
		ret = removeSensorDevice(mac_str2, nwk_str2);
	}
	
	return ret;
}

OSStatus deleteDeviceFromList(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON  * pArrayJson = NULL;
	cJSON  * objectValue = NULL;	
	cJSON  * stateArrayJson = NULL;	
	int i = 0,nstrlen, nArrayNum;
	char mac_str2[32];
	char nwk_str2[16];
	
	app_context_t* app_context = (app_context_t* )get_app_context();

	if(NULL == root)
	{
		Network_log("NULL == root\n");
		return -1; 
	}

	stateArrayJson = cJSON_GetObjectItem(root,"devices");
	nArrayNum = cJSON_GetArraySize(stateArrayJson);
	Network_log("-------array size:%d --------------\n", nArrayNum);
	if(nArrayNum <= 0)
	{
		return -1;
	}

	for(i = 0; i < nArrayNum; i++)
	{
		memset(mac_str2, 0, sizeof(mac_str2));
		memset(nwk_str2, 0, sizeof(nwk_str2));
		pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
		if(pArrayJson)
		{
			objectValue = cJSON_GetObjectItem(pArrayJson, "mac");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 32)nstrlen=32;
					memcpy(mac_str2, objectValue->valuestring, nstrlen);

				Network_log("-----------mac_str: %s -----\n", mac_str2);	
			}

			objectValue = cJSON_GetObjectItem(pArrayJson, "nwkid");
			if(objectValue && objectValue->valuestring != NULL)
			{
				nstrlen = strlen(objectValue->valuestring);
				if(nstrlen > 16)nstrlen=16;
					memcpy(nwk_str2, objectValue->valuestring, nstrlen);

				Network_log("-----------nwk_str: %s -----\n", nwk_str2);	
			}

			if(strlen(mac_str2) > 0 || strlen(nwk_str2) > 0)
			{
				err = removeDevice(mac_str2, nwk_str2);
			}
		}
	}
	
	return err;	
}


OSStatus getSceneListInfo(uint32_t device_id)
{
	OSStatus err = kNoErr;

	cJSON *root = NULL;

	int i , j, k;
	char *out = NULL;
	uint16_t bytesLen;
	uint16_t net_addr;

	uint16_t groupId,classId;
	uint8_t sceneId ,cnt;
	uint8_t tmp[USER_NUMBER_VAL_128+1]={0};
	uint8_t  paybuf[USER_NUMBER_VAL_256+USER_NUMBER_VAL_1]={0};
	char idsbuf[16+1] = {0};
	
	app_context_t* app_context = (app_context_t* )get_app_context();

	#ifdef USER_DEBUG_
		//Network_log("sceneList: ");	
	#endif
	
	for(i = 0; i < SCENE_SCENEID_MAX; i++)
	{
		for(j=0; j < DEVICE_INFO_LIST_MAX; j++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
			net_addr  = app_context->userDataStore->userData.scenectrl[i].devlist[j].net_addr;
			classId     = app_context->userDataStore->userData.scenectrl[i].devlist[j].classid;
			sceneId   = app_context->userDataStore->userData.scenectrl[i].sceneId;
			groupId   = app_context->userDataStore->userData.scenectrl[i].groupId;
			cnt         = app_context->userDataStore->userData.scenectrl[i].devlist[j].cnt;
			mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);
					
			if(net_addr != 0 && cnt != 0)
			{
		
				root=cJSON_CreateObject();
				
				bytesLen =  cnt *7;
				if(bytesLen > USER_NUMBER_VAL_64)bytesLen=USER_NUMBER_VAL_64;
				memset(tmp, 0, USER_NUMBER_VAL_128);
				memset(paybuf, 0, USER_NUMBER_VAL_256);

				for( k = USER_NUMBER_VAL_0; k < bytesLen ; k++ )
				{
					sprintf(&tmp[k*2], "%02X", app_context->userDataStore->userData.scenectrl[i].devlist[j].payload[k]);
				}
				//Network_log("tmp:%s", tmp);
				
				if(classId == 299)
				{				
					sprintf(paybuf, "1A000000%04X010100050800%04X%02X%02X%s%02X",net_addr ,groupId,sceneId,cnt,tmp,0x12);//0x12 is crc
				
					cJSON_AddItemToObject(root, "t", cJSON_CreateNumber(299));
					cJSON_AddItemToObject(root, "rs485",cJSON_CreateString((char const *)paybuf));
				}
				else
				{				
					sprintf(idsbuf, "%04X010400050101",net_addr);       
					sprintf(paybuf, "1D0210040800%04X%02X%02X%s", groupId, sceneId, cnt, tmp);
					
					cJSON_AddItemToObject(root, "t", cJSON_CreateNumber(2));
					cJSON_AddItemToObject(root, "ids",cJSON_CreateString((char const *)idsbuf));
					cJSON_AddItemToObject(root, "pay",cJSON_CreateString((char const *)paybuf));
				}
				
				out = cJSON_PrintUnformatted(root);
				if(out)
				{
					//Network_log("%d:%s", i, out);
					pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
					mico_rtos_thread_msleep(50);
					free(out);
					out = NULL;
				}

				cJSON_Delete(root);
			}
			
		}
	}

	return err;	
}



OSStatus getDeviceListInfo(uint32_t device_id)
{
	OSStatus err = kNoErr;
	cJSON *root = NULL;
	cJSON  * cJsonArray = NULL;
	cJSON  * cJsonValue = NULL;	

	cJSON  * endpointArray = NULL;
	cJSON  * endPointValue = NULL;	

	cJSON *devCntRoot = NULL;
	cJSON *devMacArray= NULL;
	
	uint32_t  dev_class;
	int i = 0;
	uint32_t tempRDM;
	char *out = NULL;
	uint8_t is_can_add;
	uint8_t endpoint_id;
	int j;
	char mac_str[32];
	char nwk_str[16];
	app_context_t* app_context = (app_context_t* )get_app_context();

	#ifdef USER_DEBUG_
		Network_log("DeviceList : ");	
	#endif
	
	srand(mico_rtos_get_time());
	tempRDM = rand();

	devCntRoot = cJSON_CreateObject();
	cJSON_AddItemToObject(devCntRoot, "cmd_id", cJSON_CreateNumber(tempRDM));
	cJSON_AddItemToObject(devCntRoot, "CMD", cJSON_CreateNumber(99));
	cJSON_AddItemToObject(devCntRoot, "devMacArray", devMacArray = cJSON_CreateArray());

	
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_class = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1 )
		{
			root=cJSON_CreateObject();
			cJSON_AddItemToObject(root, "cmd_id", cJSON_CreateNumber(tempRDM));
			cJSON_AddItemToObject(root, "CMD", cJSON_CreateNumber(GET_ALL_SUBDEVICE_LIST));	
			cJSON_AddItemToObject(root, "gw_type", cJSON_CreateNumber(GATEWAY_TYPE));	
			cJSON_AddItemToObject(root, "devices", cJsonArray = cJSON_CreateArray());

			cJSON_AddItemToObject(cJsonArray, "", cJsonValue=cJSON_CreateObject());

			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			memset(mac_str, 0, sizeof(mac_str));
			sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.deviceinfo.device_info[i].mac[0],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[1],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[2],
				app_context->userDataStore->userData.deviceinfo.device_info[i].mac[3],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[4],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[5],
				app_context->userDataStore->userData.deviceinfo.device_info[i].mac[6],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[7],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[8],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[9],
				app_context->userDataStore->userData.deviceinfo.device_info[i].mac[10],app_context->userDataStore->userData.deviceinfo.device_info[i].mac[11]);
			cJSON_AddItemToObject(cJsonValue, "mac",cJSON_CreateString(mac_str));
			cJSON_AddItemToObject(cJsonValue, "class",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class));
			cJSON_AddItemToObject(cJsonValue, "online",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline));
			cJSON_AddItemToObject(cJsonValue, "riu",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.device_info[i].riu));

			cJSON_AddItemToArray(devMacArray, cJSON_CreateString(mac_str));

			memset(nwk_str, 0, sizeof(nwk_str));
			sprintf(nwk_str, "%04X", app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr);
			cJSON_AddItemToObject(cJsonValue, "nwkid",cJSON_CreateString(nwk_str));
			
			endpointArray = cJSON_CreateArray();
			if(endpointArray)
			{
				cJSON_AddItemToObject(cJsonValue, "endp", endpointArray);
				for(j=0; j < DEVICE_MAC_ENDPOINT; j++)
				{
					endpoint_id = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id;
					
					if(endpoint_id > 0 && endpoint_id < DEVICE_MAC_ENDPOINT)
					{
						cJSON_AddItemToObject(endpointArray, "", endPointValue=cJSON_CreateObject());
						cJSON_AddItemToObject(endPointValue, "dstid",cJSON_CreateNumber(endpoint_id));
						cJSON_AddItemToObject(endPointValue, "t",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].dev_type));
						cJSON_AddItemToObject(endPointValue, "val",cJSON_CreateNumber(app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].cur_value));						
					}
				}
			}
			
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
			out = cJSON_PrintUnformatted(root);
			if(out)
			{
				Network_log("%d:%s", i, out);
				pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
				//sendMessageToServer(out, strlen(out), device_id);//device may > queue Size
				mico_rtos_thread_msleep(50);

				free(out);
				out = NULL;
			}

			cJSON_Delete(root);

		}
		
	}

	out = cJSON_PrintUnformatted(devCntRoot);
	if(out)
	{
		Network_log("devmacjson:%s", out);
		pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
		free(out);
		out = NULL;
	}
	cJSON_Delete(devCntRoot);


	return err;	
}

OSStatus send_user_system_version( uint32_t device_id)
{
	cJSON *root = NULL;
	OSStatus err = kGeneralErr;
	char *out = NULL;
	uint32_t tempRDM;
	
	root=cJSON_CreateObject();	

	srand(mico_rtos_get_time());
	tempRDM = rand();
	
	cJSON_AddItemToObject(root, "command_id", cJSON_CreateNumber(tempRDM));
	cJSON_AddItemToObject(root, "CMD", cJSON_CreateNumber(GET_GATEWAY_INFO));
	cJSON_AddItemToObject(root, "gateway_type", cJSON_CreateNumber(GATEWAY_TYPE));
	cJSON_AddItemToObject(root, "app_version", cJSON_CreateString(SYSTEM_APP_FIRMWAR_VERSION));
	cJSON_AddItemToObject(root, "sys_version", cJSON_CreateString("v3.2.3"));
	cJSON_AddItemToObject(root, "module_type", cJSON_CreateString("1"));

	out = cJSON_PrintUnformatted(root);
	if(out)
	{
		pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
		free(out);
		out = NULL;
	}

	cJSON_Delete(root);
	return err;
}


//{"command_id":3840230,"CMD":101,"G":[{"PID":"2010","T":"90","D":"cus device discovery\r\n"}]}
int parseServerMessage(int8_t *recvbuff, int32_t BufferLen, uint32_t device_id)
{
	cJSON *root;
	cJSON *stateArrayJson;
	cJSON *pArrayJson;
	cJSON *pCJSONFunCode;
	int nArrayNum;
	int ret = 0;
	int nstrlen;
	int i,riu;
	int n_cmd_id;
	cJSON *objectValue = NULL;
 	
	Network_log("recvbuff=%s ", recvbuff);
	root = cJSON_Parse((char *)recvbuff);
	 if(root != NULL)
	 {
		objectValue = cJSON_GetObjectItem(root, "CMD");
		if(objectValue)
		{
			n_cmd_id = objectValue->valueint;
		}
			

		switch(n_cmd_id)
		{
			case DELETE_DEVICE:
			{
				ret = deleteDeviceFromList(root);
				if(ret == 0)
				{
					device_id = 0;
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;

			case GET_SCENE_LISTS:
				getSceneListInfo(device_id);
				break;
			
			case GET_ALL_SUBDEVICE_LIST:
				getDeviceListInfo(device_id);				
				break;
			case GET_ALL_PANEL_LIST:
				break;
			case GET_ALL_SENSOR_LIST:
			{
				getSensorListInfo(device_id);
			}
			break;
			case GET_SCENE_ACTIONS_LIST:
			break;
			case GET_GATEWAY_INFO:
				send_user_system_version(device_id);	
			break;
			case SET_PANEL_CONNECT_WITH_DEVICE:
			{
				Network_log("========122==========\n");
			}
			break;
			case SET_SCENE_ACTIONS:
			{
				Network_log("========123==========\n");
			}
			break;

			case TIMER_CTRL_REQUEST:
			{
				timer_list_request(device_id);
			}
			break;
			case TIMER_CTRL_NEW_ADD:
			{
				ret = timer_ctrl_new_add_request(root);
				if(ret == 0)
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else if(ret == 1)
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("exist"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case TIMER_CTRL_MODIFY:
			{
				ret = timer_ctrl_modify_request(root);
				if(ret >= 0)
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case TIMER_CTRL_DELETE:
			{
				ret = timer_ctrl_delete_request(root);
				if(ret >= 0)
				{
					device_id = 0;
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case TIMER_CTRL_START:
			{
				ret = timer_ctrl_start_request(root);
				if(ret >= 0)
				{
					device_id = 0;
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case TIMER_CTRL_STOP:
			{
				ret = timer_ctrl_stop_request(root);
				if(ret >= 0)
				{
					device_id = 0;
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;

			case LINKAGE_NEW_ADD:
			{
				ret = linkage_ctrl_new_add_request(root);
				if(ret == 0)
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else if(ret == 1)
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("exist"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case LINKAGE_MODIFY:
			{
				ret = linkage_ctrl_modify_request(root);
				if(ret >= 0)
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case LINKAGE_DELETE:
			{
				ret = linkage_ctrl_delete_request(root);
				if(ret >= 0)
				{
					device_id = 0;
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case LINKAGE_START:
			{
				ret = linkage_ctrl_start_request(root);
				if(ret >= 0)
				{
					device_id = 0;
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case LINKAGE_STOP:
			{
				ret = linkage_ctrl_stop_request(root);
				if(ret >= 0)
				{
					device_id = 0;
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("success"));	
				}
				else
				{
					cJSON_AddItemToObject(root, "result", cJSON_CreateString("failture"));	
				}
			}
			break;
			case LINKAGE_REQUEST:
			{
				linkage_list_request(device_id);
			}
			break;
			case 101:
			{
				stateArrayJson = cJSON_GetObjectItem(root,"G");
				nArrayNum = cJSON_GetArraySize(stateArrayJson);
				//Network_log("-------array size:%d --------------\n", nArrayNum);
				if(nArrayNum <= 0)
				{
					return ret;
				}
				
				for(i = 0; i < nArrayNum; i++)
				{
					pArrayJson = cJSON_GetArrayItem(stateArrayJson, i);
					if(pArrayJson)
					{
						riu = 0;
						pCJSONFunCode = cJSON_GetObjectItem(pArrayJson,"RIU");	 
						if(pCJSONFunCode != NULL)
						{
							riu = pCJSONFunCode->valueint;
						}
						
						pCJSONFunCode = cJSON_GetObjectItem(pArrayJson,"D");	 
						if(pCJSONFunCode != NULL && pCJSONFunCode->valuestring)
						{
							nstrlen = strlen(pCJSONFunCode->valuestring);
							//Network_log("-------pCJSONFunCode->valuestring[%d]: %s  --------------\n", nstrlen, pCJSONFunCode->valuestring);

							if(0 == riu)//rs485
							{
								int j;
								uint16_t bytesLen;
								uint8_t tmp[USER_NUMBER_VAL_256]={0};
								uint8_t ids_buff[USER_NUMBER_VAL_256]={0};
								
								if(nstrlen > USER_NUMBER_VAL_256)nstrlen=USER_NUMBER_VAL_256;
								
								memcpy(tmp, pCJSONFunCode->valuestring, nstrlen);
								bytesLen = nstrlen/USER_NUMBER_VAL_2;


								for( j = USER_NUMBER_VAL_0; j < bytesLen ; j++ )
								{
									charchangehex( &tmp[j*USER_NUMBER_VAL_2] , &ids_buff[j] );
								}

								 if(strncmp(pCJSONFunCode->valuestring+26, "000509", 6)==0)
								{
									rs485_parse_write_scene_info(pCJSONFunCode->valuestring);
								}
								 if(strncmp(pCJSONFunCode->valuestring+26, "FC0003", 6)==0)
								 {
								 	rs485_date_sendto_parse(pCJSONFunCode->valuestring);
								 }
							
								pushAppMainRecvMsgProcess((uint8_t*)ids_buff, bytesLen, 0);//sent to rs485

								memset(ids_buff, USER_NUMBER_VAL_0, USER_NUMBER_VAL_256);
								sprintf(ids_buff, "cus rs485 tx {%s}\r\n", tmp+10);//10 is frameheader and length
								
								pushAppMainRecvMsgProcess((uint8_t*)ids_buff,strlen(ids_buff), 1);//sent to zigbee
								mico_rtos_thread_msleep(100);
								broadcast_rs485_cmdpayload( tmp+10,device_id);
								rs485data_send_to_queue_parse(tmp+10);

							}
							else if(1 == riu)//zigbee
							{				
								pushAppMainRecvMsgProcess((uint8_t*)pCJSONFunCode->valuestring, nstrlen, riu);

								if(strncmp(pCJSONFunCode->valuestring, "cus scene recall", 16)==0)
								{		
									//parse_scene_racall_cmd(pCJSONFunCode->valuestring);
									//mico_rtos_thread_msleep(100);
									broadcast_zigbee_cmdpayload(pCJSONFunCode->valuestring ,device_id);	
								}
								else if(strncmp(pCJSONFunCode->valuestring, "cus scene write", 15)==0)
								{
									parse_zigbee_write_scene_info(pCJSONFunCode->valuestring+16);
								}
								
							}
						}
					}

					mico_rtos_thread_msleep(100);
				}
			}
			break;
			default:
			{
				Network_log("-------invalid json cmd  --------------\n");
			}
			break;
		}

		objectValue = cJSON_GetObjectItem(root, "result");
		if(objectValue && objectValue->valuestring != NULL && strlen(objectValue->valuestring) > 0)
		{
			char	*out = cJSON_PrintUnformatted(root);
			if(out)
			{
			//	Network_log("OUT:%s", out);
				pushSend2ServerMsgProcess((uint8_t *)out, strlen(out), device_id);
				free(out);
				out = NULL;
			}
		}


		cJSON_Delete(root);
	 }
	 else
	 {
		Network_log("val is not json format......\n");
		return ret;
	 }

	 return ret;
}

