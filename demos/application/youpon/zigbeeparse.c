/**
******************************************************************************
* @file    zigbeeparse.c
* @author  hehx_ybzn@163.com
* @version V1.0.0
* @date    12-oct-2016
* @brief   zigbee device function control api and define.
******************************************************************************
* @attention
*
******************************************************************************
*/

#include "zigbeeparse.h"
#include "StringUtils.h"
#include "appmainmsgqueue.h"
#include "user_config.h"
#include "user_main.h"
#include "devicemanager.h"
#include "rs485_uart.h"
#include "rs485_comm.h"

static mico_queue_t  	uart_zigbee_msg_recv_queue = NULL;
static mico_mutex_t  uart_zigbee_msg_recv_queue_mutex = NULL;
extern app_context_t* app_context_global ;

#define STACK_SIZE_USER_ZIGBEE_THREAD  0x800
#define ZIGBEE_MAX_RECV_QUEUE_LENGTH    6       // max num of msg in recv queue

#define user_zigbee(M, ...) custom_log("uart zigbee", M, ##__VA_ARGS__)


#define   ZB_MSG_INVALID_CMD						0
#define   ZB_MSG_ZB_COORDINATOR_POWERON		1
#define   ZB_MSG_ZCL_ZDO							2
#define   ZB_MSG_REQUEST_NET_ID   					3
#define   ZB_MSG_CREATE_NET_STATE   				4
#define   ZB_MSG_DEV_JOIN_ALLOW_STATE			5
#define   ZB_MSG_DEV_DELETE_JOINED_NET			6
#define   ZB_MSG_CHANGE_NET_CHANNEL				7
#define   ZB_MSG_CHANGE_COORDINATOR_CHANNEL	8
#define   ZB_MSG_SET_COORDINATOR_POWER			9
#define   ZB_MSG_ENDPOINT_STAE						10
#define   ZB_MSG_SENDERR							11
#define   ZB_MSG_STACK								12
#define   ZB_MSG_INTER_PAN_LOGOUT					100
#define   ZB_MSG_DEVICE_LOGOUT_MODE				13
#define	ZB_MSG_CLEAR_DEVICE_INFO					16

extern app_context_t* app_context_global;

extern void zigbee_join_work_callback(void);
extern void zigbee_update_device_property_state(int key_id, int state);
extern void device_joined_irq_cb();
extern OSStatus  sensor_attribute_report(cJSON *root,  ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo);
extern OSStatus  rs485_msg_zcl_zdo(cJSON *root);
extern int scenepanel_device_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
extern void request_DeviceState(mico_thread_arg_t arg);
extern void popRecvFromFlogCloudMsgThread(mico_thread_arg_t arg);

static OSStatus  zigbee_recv_device_logout(uint16_t nwkaddr);
//static void zigbee_request_subdevice_state();

/*
function:转化16进制的char 类型字符为ASCII码
input :	data 输入，char 类型字符串etc: "AA" or "0xAA"
output:	value，ASCII 码
*/
unsigned int charchangehex( unsigned char *data , unsigned char* value )
{
        unsigned char *p;
        unsigned char i, j;
        unsigned char num = 0;
        unsigned char date = 0;
        p = data;
        if(( p[USER_NUMBER_VAL_0] == '0' ) && (( p[USER_NUMBER_VAL_1] == 'x') || ( p[USER_NUMBER_VAL_1] == 'X' ) ))
                p = p+USER_NUMBER_VAL_2;
        i = USER_NUMBER_VAL_0;
        for( j = USER_NUMBER_VAL_0; j < USER_NUMBER_VAL_2; j++ )
        {
                if((( *p >= '0' ) && ( *p <= '9' )) || ((( *p >= 'a' ) && ( *p <= 'f' )) || (( *p >= 'A' ) && ( *p <= 'F' ))))
                {
                        if(( *p >= '0' ) && (*p <= '9' ))
                        {
                                num = *p - '0' ;
                        }
                        else
                        {
                                num =  toupper( *p ) - 'A'+USER_NUMBER_VAL_10 ;
                        }

                }
                else
                {
                        user_zigbee("Input error Hex character %s\n", data );
                        return USER_NUMBER_VAL_1;
                }
                if( i == USER_NUMBER_VAL_0 )
                {
                        date = num << USER_NUMBER_VAL_4;
                }
                else
                {
                        date = date + num;
                }
                i++;
                p++;

        }
		
        *value = date;
        return USER_NUMBER_VAL_0;

}

unsigned short ChangeEndian16(unsigned short value)
{
	unsigned short out_value = USER_NUMBER_VAL_0;
	out_value = (unsigned char)(value>>USER_NUMBER_VAL_8);
	out_value |= (value<<USER_NUMBER_VAL_8)&USER_NUMBER_VAL_0XFF00;
	return out_value;
}

static OSStatus  zigbee_msg_request_net_id(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	unsigned short nodeId;
	unsigned short pid;
	unsigned char tmp[32];
	unsigned char tmp2[4];
	int  str_len;
	int i;
	int bytesLen;
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("param error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "nId");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);

		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
		
		memcpy(&nodeId, tmp2, 2);
		nodeId = ChangeEndian16(nodeId);
	}
	
	objectValue = cJSON_GetObjectItem(root, "pId");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp2, 0, sizeof(tmp2));
		memset(tmp, 0, sizeof(tmp));
		
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
		
		memcpy(&pid, tmp2, 2);
	}


	if(nodeId != 0 /*&& app_context_global->appConfig->zigbeeConfig.panid > 0xfffc*/)
	{
		user_zigbee("uncreate network id.\n");
		zigbee_create_net();
	}
	else
	{
		//zigbee_request_subdevice_state(); // not use this cmd
		//print_device_infomation();
	}

	return err;
}

/*
static void zigbee_device_joined_ack(uint16_t nwid)
{
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "cus device discovery stop\r\n send 0x%04X 1 1 \r\n", nwid);
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}
*/
//{"t":2,"ids":"5377010400000101","pay":"182F0A05004201310500420132"}
static OSStatus  zigbee_msg_attribute_state_report(cJSON *root,  ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo)
{
	OSStatus err = kNoErr, ret =kGeneralErr ;
	cJSON *objectValue = NULL;
	int  str_len;
	int i;
	int bytesLen;
	uint32_t device_type;
	unsigned char payload[USER_NUMBER_VAL_64]={USER_NUMBER_VAL_0};
	ZB_DEV_JOINED_REQUEST_T  msgInfo;
	DEV_ENDPOINT_JOIN_REPORT_T *pDepStateInfo;
		
	if(NULL == root || NULL == pZclZdoMsgInfo)
	{
		err = kParamErr;
		user_zigbee("param error.");
		return err;
	}

	memset(&msgInfo, USER_NUMBER_VAL_0, sizeof(ZB_DEV_JOINED_REQUEST_T));
	objectValue = cJSON_GetObjectItem(root, "pay");
	if(objectValue && objectValue->valuestring != NULL)
	{
		str_len = strlen(objectValue->valuestring);
		if(str_len > USER_NUMBER_VAL_64)str_len=USER_NUMBER_VAL_64;
		memcpy(payload, objectValue->valuestring, str_len);

		bytesLen = str_len/USER_NUMBER_VAL_2;
		//user_zigbee("data: %s \n", payload);
		for( i = USER_NUMBER_VAL_0; i < bytesLen ; i++ )
		{
			charchangehex( &payload[i*USER_NUMBER_VAL_2] , &msgInfo.payload[i] );
		}
	}

	msgInfo.src = ChangeEndian16(pZclZdoMsgInfo->src);
	msgInfo.pro = ChangeEndian16(pZclZdoMsgInfo->pro);
	msgInfo.clu = ChangeEndian16(pZclZdoMsgInfo->clu);
	msgInfo.dep = pZclZdoMsgInfo->dep;
	msgInfo.sep = pZclZdoMsgInfo->sep;

	pDepStateInfo = (DEV_ENDPOINT_JOIN_REPORT_T *)(msgInfo.payload);
	device_type = strtol((char const *)pDepStateInfo->attri_value,NULL, 16);//atoi((char const *)pDepStateInfo->attri_value);

#ifdef USER_DEBUG_
	//user_zigbee("zigbee_msg_attribute_state_report device_type=%d \n", device_type);
#endif

	//user_zigbee("######### msgInfo.sep:%d pDepStateInfo->attri_value=%s device_type=%d msgInfo.len=%d msgInfo.src=%04x msgInfo.pro=%04x msgInfo.clu=%04x ", msgInfo.sep, pDepStateInfo->attri_value, device_type, msgInfo.len, msgInfo.src, msgInfo.pro, msgInfo.clu);

	switch(device_type)
	{
		case ZB_TYPE_WARM_WIND:
		case ZB_TYPE_LIGHT_WARM:
		case ZB_TYPE_LIGHT:
		case ZB_TYPE_CHANGEAIR:
		//case ZB_TYPE_INTEGRATE:
		//	device_report_attribute_update(&msgInfo);	
		break;
		case ZB_TYPE_KITCHEN_PANEL:
		{
			ret = kitchen_panel_device_report_attribute_update(&msgInfo);		
		}
		break;
		case ZB_TYPE_SINGLE_LIGHT:
			ret = device_light_one_report_attribute_update(&msgInfo);
		break;
		case ZB_TYPE_DOUBLE_LIGHT:
			ret = device_light_two_report_attribute_update(&msgInfo);
		break;
		case ZB_TYPE_THREE_LIGHT:
			ret = device_light_three_report_attribute_update(&msgInfo);
		break;
		case ZB_TYPE_SCENEPANEL:
			ret = scenepanel_device_report_attribute_update(&msgInfo);
		break;

	}
	if(ret == kNoErr)
	{
		device_delay_save_timerStart();		
	}

	return err;
}

static OSStatus  zigbee_msg_invalid(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	unsigned char  tmp[32];
	int 	str_len = 0;
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "invalidCmd");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);

		user_zigbee("invalidCmd=%s ", tmp);
	}
	
	return err;
}


static OSStatus  zigbee_msg_config_network(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	unsigned char  tmp[32];
	unsigned char tmp2[32];
	int 	str_len = 0;
	int bytesLen, i;
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "nwkForm");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
	}

	return err;
}

static OSStatus  zigbee_msg_set_dev_join_allow(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	unsigned char  tmp[32];
	unsigned char  tmp2[32];
	int 	str_len = 0;
	int   bytesLen;
	int 	i;
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "pjnSta");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
	}
	return err;
}

//{"t":2,"ids":"D054000000130000","pay":"8154D0B2CB3F0D006F0D008E"}
static OSStatus  zigbee_msg_dev_joined_request(cJSON *root, ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	ZB_DEV_JOINED_REQUEST_T  msgInfo;
	unsigned char payload[128];
	int i;
	int bytesLen;
       int str_len;


	if(NULL == root || NULL == pZclZdoMsgInfo)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}
	
	memset(&msgInfo, USER_NUMBER_VAL_0, sizeof(ZB_DEV_JOINED_REQUEST_T));

	objectValue = cJSON_GetObjectItem(root, "pay");
	if(objectValue && objectValue->valuestring != NULL)
	{
		str_len = strlen(objectValue->valuestring);
		if(str_len > USER_NUMBER_VAL_128)str_len=USER_NUMBER_VAL_128;
		memcpy(payload, objectValue->valuestring, str_len);

		bytesLen = str_len/USER_NUMBER_VAL_2;
		//user_zigbee("data: %s \n", payload);
		for( i = USER_NUMBER_VAL_0; i < bytesLen ; i++ )
		{
			charchangehex( &payload[i*USER_NUMBER_VAL_2] , &msgInfo.payload[i] );
		}
	}

	msgInfo.src = ChangeEndian16(pZclZdoMsgInfo->src);
	msgInfo.pro = ChangeEndian16(pZclZdoMsgInfo->pro);
	msgInfo.clu = ChangeEndian16(pZclZdoMsgInfo->clu);
	msgInfo.dep = pZclZdoMsgInfo->dep;
	msgInfo.sep = pZclZdoMsgInfo->sep;
	msgInfo.len = bytesLen;


	if(msgInfo.clu==ZIGBEE_NCLU_JOIN_NWK)
		zigbee_new_device_add_report(&msgInfo);		
	else if(msgInfo.clu==ZIGBEE_NULU_DEVICE_REPORT_STATE)
		zigbee_new_device_add_report2(&msgInfo);


	return err;
}

static OSStatus  zigbee_msg_delete_dev_joined(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int 	str_len = 0;
	int   bytesLen;
	int 	i;
	unsigned char  tmp[32];
	unsigned char  tmp2[32];
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "lvReq");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
		//err =  user_data_store_update();
	}

	return err;
}

static OSStatus  zigbee_msg_change_dev_channel(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int 	str_len = 0;
	int   bytesLen;
	int 	i;
	unsigned char  tmp[32];
	unsigned char  tmp2[32];
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "chgCh");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
	}
	
	return err;
}

static OSStatus  zigbee_msg_change_coordinator_channel(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int 	str_len = 0;
	int   bytesLen;
	int 	i;
	unsigned char  tmp[32];
	unsigned char  tmp2[32];
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "setCh");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
	}
	

	return err;
}

static OSStatus  zigbee_msg_set_coordinator_channel(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int 	str_len = 0;
	int   bytesLen;
	int 	i;
	unsigned char  tmp[32];
	unsigned char  tmp2[32];
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "setPwr");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
		//memcpy(&app_context_global->appConfig->zigbeeConfig.zb_set_join_allow_st , tmp2, 1);
		//user_zigbee("zb_set_join_allow_st=%d ", app_context_global->appConfig->zigbeeConfig.zb_set_join_allow_st);
	}
	
	return err;
}

static OSStatus zigbee_msg_dev_logout_Inter_PAN(cJSON *root )
{
	OSStatus err = kNoErr;
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}
	
	return err;
}

static OSStatus  zigbee_msg_stack_info(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int 	str_len = 0;
	int   bytesLen;
	int 	i;
	unsigned char  tmp[32];
	unsigned char  tmp2[32];
	unsigned short stackSta;
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "stackSta");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
		memcpy(&stackSta , tmp2, 1);
		user_zigbee("stackSta=%02x ", stackSta);
	}

	switch(stackSta)
	{
		case 0x90:
			break;
		case 0x91:
			break;
		case 0x99:
			break;
		case 0x9a:
			break;
		case 0x9b:
			break;
	}

	return err;
}

static OSStatus  zigbee_msg_endpoint_state(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int 	str_len = 0;
	int   bytesLen;
	int 	i;
	unsigned char  tmp[32];
	unsigned char  tmp2[32];
	unsigned short nodeId;
	uint16_t  nwadr;
	
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "nId");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 32)str_len=32;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		user_zigbee("data: %s \n", tmp);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &tmp2[i] );
		}
		memcpy(&nodeId , tmp2, 2);
		nwadr = ChangeEndian16(nodeId);
		user_zigbee("nodeId=%04x ", nwadr);
	}

	objectValue = cJSON_GetObjectItem(root, "nwkSta");
	if(objectValue )
	{
		switch(objectValue->valueint)
		{
			case 0:
			{
				zigbee_recv_device_logout(nwadr);
			}
			break;
			case 1:
			case 3:
			{
				if(device_onoffline_update_flag(nwadr, 1)<0) 
					rs485_sensor_device_onoffline_update_flag(nwadr, 1);
			}
			break;
			case 2:
			case 4:
			{
				//device_onoffline_update_flag(nwadr, 0);
			}
			break;
		}

		user_zigbee("netSta=%02x ", objectValue->valueint);
	}
	
	return err;
}


static OSStatus  zigbee_recv_device_logout(uint16_t nwk_addr)
{
	OSStatus err = kNoErr;
	err = device_logout_update_flag(nwk_addr);
	if(err == kGeneralErr) 
		delet_dev_from_scenelist(nwk_addr);
	
	return err;
}

static OSStatus  zigbee_msg_dev_quit_joined(cJSON *root, ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	ZB_DEV_JOINED_REQUEST_T  msgInfo;
	unsigned char payload[64];
	int  str_len;
	int i;
	int bytesLen;
	
	if(NULL == root || NULL == pZclZdoMsgInfo)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	memset(&msgInfo, 0, sizeof(ZB_DEV_JOINED_REQUEST_T));
	
	objectValue = cJSON_GetObjectItem(root, "pay");
	if(objectValue && objectValue->valuestring != NULL)
	{
		str_len = strlen(objectValue->valuestring);
		if(str_len > 64)str_len=64;
		memcpy(payload, objectValue->valuestring, str_len);

		bytesLen = str_len/2;
		user_zigbee("data: %s \n", payload);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &payload[i*2] , &msgInfo.payload[i] );
		}
	}

	msgInfo.src = ChangeEndian16(pZclZdoMsgInfo->src);
	msgInfo.pro = ChangeEndian16(pZclZdoMsgInfo->pro);
	msgInfo.clu = ChangeEndian16(pZclZdoMsgInfo->clu);
	msgInfo.dep = pZclZdoMsgInfo->dep;
	msgInfo.sep = pZclZdoMsgInfo->sep;

	user_zigbee("msgInfo.src=%04x msgInfo.pro=%04x msgInfo.clu=%04x", msgInfo.src, msgInfo.pro, msgInfo.clu);
	zigbee_recv_device_logout(msgInfo.src);
		
	return err;
		
	}

static OSStatus  zigbee_msg_send_error_code(cJSON *root)
	{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	unsigned short  nclu;
	unsigned char tmp[64];
	unsigned char ids_buff[64];
	int i;
	int bytesLen;
       int str_len;

	if(NULL == root)
		{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "ids");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, 0, sizeof(tmp));
		memset(ids_buff, 0, sizeof(ids_buff));
		str_len = strlen(objectValue->valuestring);
		if(str_len > 64)str_len=64;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/2;
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*2] , &ids_buff[i] );
}

		memcpy(&nclu, ids_buff, 2);
	}

	return err;
}

static OSStatus  zigbee_msg_dev_request_net_addr(cJSON *root, ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	ZB_DEV_JOINED_REQUEST_T  msgInfo;
	unsigned char  payload[64];
	int  str_len;
	int i;
	int bytesLen;
	
	if(NULL == root || NULL == pZclZdoMsgInfo)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	memset(&msgInfo, 0, sizeof(ZB_DEV_JOINED_REQUEST_T));
	
	objectValue = cJSON_GetObjectItem(root, "pay");
	if(objectValue && objectValue->valuestring != NULL)
	{
		str_len = strlen(objectValue->valuestring);
		if(str_len > 64)str_len=64;
		memcpy(payload, objectValue->valuestring, str_len);

		bytesLen = str_len/2;
		user_zigbee("data: %s \n", payload);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &payload[i*2] , &msgInfo.payload[i] );
		}
	}

	msgInfo.src = ChangeEndian16(pZclZdoMsgInfo->src);
	msgInfo.pro = ChangeEndian16(pZclZdoMsgInfo->pro);
	msgInfo.clu = ChangeEndian16(pZclZdoMsgInfo->clu);
	msgInfo.dep = pZclZdoMsgInfo->dep;
	msgInfo.sep = pZclZdoMsgInfo->sep;

	user_zigbee("msgInfo.len=%d msgInfo.src=%04x msgInfo.pro=%04x msgInfo.clu=%04x", msgInfo.len, msgInfo.src, msgInfo.pro, msgInfo.clu);

	return err;

}


static OSStatus  zigbee_msg_dev_request_mac_addr(cJSON *root, ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	ZB_DEV_JOINED_REQUEST_T  msgInfo;
	unsigned char  payload[64];
	int  str_len;
	int i;
	int bytesLen;
	
	if(NULL == root || NULL == pZclZdoMsgInfo)
	{
		err = kParamErr;
		user_zigbee("auto_find_network error.");
		return err;
	}

	memset(&msgInfo, 0, sizeof(ZB_DEV_JOINED_REQUEST_T));
	
	objectValue = cJSON_GetObjectItem(root, "pay");
	if(objectValue && objectValue->valuestring != NULL)
	{
		str_len = strlen(objectValue->valuestring);
		if(str_len > 64)str_len=64;
		memcpy(payload, objectValue->valuestring, str_len);

		bytesLen = str_len/2;
		user_zigbee("data: %s \n", payload);
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &payload[i*2] , &msgInfo.payload[i] );
		}
	}

	msgInfo.src = ChangeEndian16(pZclZdoMsgInfo->src);
	msgInfo.pro = ChangeEndian16(pZclZdoMsgInfo->pro);
	msgInfo.clu = ChangeEndian16(pZclZdoMsgInfo->clu);
	msgInfo.dep = pZclZdoMsgInfo->dep;
	msgInfo.sep = pZclZdoMsgInfo->sep;

	user_zigbee("msgInfo.len=%d msgInfo.src=%04x msgInfo.pro=%04x msgInfo.clu=%04x", msgInfo.len, msgInfo.src, msgInfo.pro, msgInfo.clu);

	return err;

}

static OSStatus zigbee_parser_scene_cluster_msg(cJSON *root, ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;

	int  str_len;
	int i;
	int bytesLen;
	uint16_t groupid, netaddr;
	uint8_t sceneid;
	
	unsigned char payload[USER_NUMBER_VAL_128]={USER_NUMBER_VAL_0};
	ZB_DEV_JOINED_REQUEST_T  msgInfo;
	DEV_SCENE_INFO_PARAM_T *pSceneInfo;


		
	if(NULL == root || NULL == pZclZdoMsgInfo)
	{
		err = kParamErr;
		user_zigbee("param error.");
		return err;
	}

	memset(&msgInfo, USER_NUMBER_VAL_0, sizeof(ZB_DEV_JOINED_REQUEST_T));
	objectValue = cJSON_GetObjectItem(root, "pay");
	if(objectValue && objectValue->valuestring != NULL)
	{
		str_len = strlen(objectValue->valuestring);
		if(str_len > USER_NUMBER_VAL_128)str_len=USER_NUMBER_VAL_128;
		memcpy(payload, objectValue->valuestring, str_len);

		bytesLen = str_len/USER_NUMBER_VAL_2;
		//user_zigbee("data: %s \n", payload);
		for( i = USER_NUMBER_VAL_0; i < bytesLen ; i++ )
		{
			charchangehex( &payload[i*USER_NUMBER_VAL_2] , &msgInfo.payload[i] );
		}
	}
	
	pSceneInfo = (DEV_SCENE_INFO_PARAM_T *)(msgInfo.payload);
	netaddr = ChangeEndian16(pZclZdoMsgInfo->src);
	groupid = pSceneInfo->groupID;
	sceneid = pSceneInfo->sceneID;

	if(pSceneInfo->cmdID== USER_NUMBER_VAL_0)
	{
		save_scenelist_parame(netaddr, groupid, sceneid);
	}
	else if(pSceneInfo->cmdID== USER_NUMBER_VAL_5)
	{
	//	devicechldata_syn_scenedata(groupid, sceneid);	
	}
	else if(pSceneInfo->cmdID== USER_NUMBER_VAL_7)
	{
	
	}


	return err;
}

static OSStatus  zigbee_msg_device_state_resp(cJSON *root, ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	unsigned char  payload[64];
	ZB_DEV_JOINED_REQUEST_T  msgInfo;
	int  str_len;
	int i;
	int bytesLen;
	//uint16_t net_addr;
	
	//DEV_CTRL_ENDPOINT_STATE_T  *pDepStateInfo = NULL;
	
	if(NULL == root || NULL == pZclZdoMsgInfo)
	{
		err = kParamErr;
		user_zigbee("param error.");
		return err;
	}

	memset(&msgInfo, 0, sizeof(ZB_DEV_JOINED_REQUEST_T));
	
	objectValue = cJSON_GetObjectItem(root, "sentErr");
	if(objectValue && objectValue->valuestring != NULL)
	{
		if(!strcmp(objectValue->valuestring, "66"))
		{
			user_zigbee("sentErr: %s \n", objectValue->valuestring);
			return err;
		}
	}
	
	objectValue = cJSON_GetObjectItem(root, "pay");
	if(objectValue && objectValue->valuestring != NULL)
	{
		str_len = strlen(objectValue->valuestring);
		if(str_len > USER_NUMBER_VAL_64)str_len=USER_NUMBER_VAL_64;
			memcpy(payload, objectValue->valuestring, str_len);

		bytesLen = str_len/USER_NUMBER_VAL_2;
		for( i = USER_NUMBER_VAL_0; i < bytesLen ; i++ )
		{
			charchangehex( &payload[i*USER_NUMBER_VAL_2] , &msgInfo.payload[i] );
		}
	}

	msgInfo.src = ChangeEndian16(pZclZdoMsgInfo->src);
	msgInfo.pro = ChangeEndian16(pZclZdoMsgInfo->pro);
	msgInfo.clu = ChangeEndian16(pZclZdoMsgInfo->clu);
	msgInfo.dep = pZclZdoMsgInfo->dep;
	msgInfo.sep = pZclZdoMsgInfo->sep;

	update_device_state(&msgInfo);
	
	return err;
}

static OSStatus  zigbee_msg_zcl_zdo(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	unsigned short  nclu;
	unsigned char tmp[USER_NUMBER_VAL_64]={USER_NUMBER_VAL_0};
	unsigned char ids_buff[USER_NUMBER_VAL_64]={USER_NUMBER_VAL_0};
	int i;
	int bytesLen;
        int str_len;
	ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo = NULL;
		
	if(NULL == root)
	{
		err = kParamErr;
		user_zigbee("param error.");
		return err;
	}
	
#if 0	
	char	*out = cJSON_PrintUnformatted(root);
	if(out)
	{
		user_zigbee("zigbee_msg_zcl_zdo:%s", out);
		free(out);
		out = NULL;
	}
#endif
	objectValue = cJSON_GetObjectItem(root, "ids");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, USER_NUMBER_VAL_0, sizeof(tmp));
		memset(ids_buff, USER_NUMBER_VAL_0, sizeof(ids_buff));
		str_len = strlen(objectValue->valuestring);
		if(str_len > USER_NUMBER_VAL_64)str_len=USER_NUMBER_VAL_64;
		memcpy(tmp, objectValue->valuestring, str_len);


		bytesLen = str_len/USER_NUMBER_VAL_2;
		for( i = USER_NUMBER_VAL_0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*USER_NUMBER_VAL_2] , &ids_buff[i] );
		}
		
		memcpy(&nclu, ids_buff, USER_NUMBER_VAL_2);
	}

	pZclZdoMsgInfo = (ZB_ZCL_ZDO_MSG_T *)ids_buff;
	nclu = ChangeEndian16(pZclZdoMsgInfo->clu);

#ifdef USER_DEBUG_
//	user_zigbee("##### nclu:%04X", nclu);
#endif

	switch(nclu)
	{
		case ZIGBEE_NCLU_JOIN_NWK://device joined network	
		case ZIGBEE_NULU_DEVICE_REPORT_STATE:
			zigbee_msg_dev_joined_request(root, pZclZdoMsgInfo);
			break;
		case ZIGBEE_NCLU_STATE_REPORT:
			zigbee_msg_attribute_state_report(root, pZclZdoMsgInfo);
			break;
		case ZIGBEE_NCLU_QUIT_JOIN_NWK://device quit joined network
			zigbee_msg_dev_quit_joined(root, pZclZdoMsgInfo);
			break;
		case ZIGBEE_NCLU_REQUEST_NET_ADDR:
			zigbee_msg_dev_request_net_addr(root, pZclZdoMsgInfo);
			break;
		case ZIGBEE_NCLU_REQUEST_MAC_ADDR:
			zigbee_msg_dev_request_mac_addr(root, pZclZdoMsgInfo);
			break;
		case ZIGBEE_NCLU_REQUEST_STATE:// wind warm state response
		case ZIGBEE_NCLU_SWITCH_STATE_REPORT:
			zigbee_msg_device_state_resp(root, pZclZdoMsgInfo);
			break;
			
		case ZIGBEE_NCLU_SENSOR_ATTR_REPORT://红外感应
		case ZIGBEE_NCLU_SENSOR_BEAM_ATTR://光线感应
		case ZIGBEE_NCLU_SENSOR_TEMP_ATTR://温度
		case ZIGBEE_NCLU_SENSOR_HUMIDITY_ATTR://湿度
		case SENSOR_GAS_PILE_ID:
			sensor_attribute_report(root, pZclZdoMsgInfo);
			break;

		case ZIGBEE_NCLU_SCENE_CLUSTER:
			zigbee_parser_scene_cluster_msg(root, pZclZdoMsgInfo);
				
			break;
	}


	return err;
}

static OSStatus zigbee_msg_clear_device_info(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int clear_type =  -1;
	if(NULL == root)
	{
		user_zigbee("root is NULL.");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "clr");
	if(objectValue)
	{
		clear_type = objectValue->valueint;
	}

	user_zigbee("clear_type = %d", clear_type);
	if(1==clear_type)
	{
		err = clear_all_rs485_device();
	}
	else if(0==clear_type)
	{
		err = clear_all_device_restore_factory_set();
	}

	return err;

}

OSStatus uart_zigbee_msg_dispatch(cJSON *root, int *code)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int nmsg_type = 0;
	char *out;
	int needsend =0;
		
	if(NULL == root)
	{
		user_zigbee("root is NULL.\n");
		return err;
	}

	objectValue = cJSON_GetObjectItem(root, "t");
	if(objectValue)
	{
		nmsg_type = objectValue->valueint;
	}
#if 1	
	out = cJSON_PrintUnformatted(root);
	if(out)
	{
		user_zigbee("uart_zigbee_msg_dispatch:%s", out);
		free(out);
		out = NULL;
	}
#endif
	switch(nmsg_type)
	{
		case ZB_MSG_INVALID_CMD://						0
			zigbee_msg_invalid(root);
		break;
		case ZB_MSG_ZB_COORDINATOR_POWERON://		1
			
		break;
		case 15://zigbee->485 msg										2
		case 299:
		{		
			objectValue = cJSON_GetObjectItem(root, "rs485");
			if(objectValue && objectValue->valuestring != NULL)
			{
				err = rs485_msg_zcl_zdo(root);
				if(err < 0)//if protocol data is invalid. not send to server.
				{
					return err;
				}
				if(nmsg_type==15)
				cJSON_ReplaceItemInObject(root, "t", cJSON_CreateNumber(299));
			}
		}
		case ZB_MSG_ZCL_ZDO://							2
		{
			zigbee_msg_zcl_zdo(root);
		}
		needsend = 1;
		
		break;
		case ZB_MSG_REQUEST_NET_ID://   					3
			zigbee_msg_request_net_id(root);
		break;
		case ZB_MSG_CREATE_NET_STATE://  				4
			zigbee_msg_config_network(root);
			break;
		case ZB_MSG_DEV_JOIN_ALLOW_STATE://			5
			zigbee_msg_set_dev_join_allow(root);
			break;
		case ZB_MSG_DEV_DELETE_JOINED_NET://			6
			zigbee_msg_delete_dev_joined(root);
			break;
		case ZB_MSG_CHANGE_NET_CHANNEL://				7
			zigbee_msg_change_dev_channel(root);
			break;
		case ZB_MSG_CHANGE_COORDINATOR_CHANNEL:	//	8
			zigbee_msg_change_coordinator_channel(root);
		break;
		case ZB_MSG_SET_COORDINATOR_POWER://			9
			zigbee_msg_set_coordinator_channel(root);
			break;
		case ZB_MSG_ENDPOINT_STAE://					10
			zigbee_msg_endpoint_state(root);
			needsend = 1;
		break;
		case ZB_MSG_SENDERR://							11
			zigbee_msg_send_error_code(root);
			break;
		case ZB_MSG_STACK://							12
			zigbee_msg_stack_info(root);
		break;
		case ZB_MSG_INTER_PAN_LOGOUT://				100
			zigbee_msg_dev_logout_Inter_PAN(root);
		break;
		case ZB_MSG_DEVICE_LOGOUT_MODE:                           //13
			//zigbee_msg_set_dev_logout_mode(root);
		break;

		case ZB_MSG_CLEAR_DEVICE_INFO:					//16
			zigbee_msg_clear_device_info(root);
			needsend = 1;
			break; 

		default:
			user_zigbee("unkown msg type!");
			break;
		
	}

	out = cJSON_PrintUnformatted(root);
	if(out)
	{	
	#ifdef USER_DEBUG_
			//	user_zigbee("out=%s \n", out);	
	#endif
		if(needsend)
			pushSend2ServerMsgProcess((uint8_t*)out, strlen(out), 0);
		free(out);
		out = NULL;
	}
	
	return err;
}

OSStatus uart_zigbee_msg_insertinto_queue( cJSON *pRoot )
{
	OSStatus err = kNoErr;
  	cJSON *p_tem_msg = NULL; 

	if(NULL == pRoot)
	{
		err = kGeneralErr;
		return err;
	}
	
	if(NULL != uart_zigbee_msg_recv_queue)
	{
		mico_rtos_lock_mutex(&uart_zigbee_msg_recv_queue_mutex);
		if(mico_rtos_is_queue_full(&uart_zigbee_msg_recv_queue))
		{
			user_zigbee("WARNGING: zigbee msg overrun, abandon old messages!");
			err = mico_rtos_pop_from_queue(&uart_zigbee_msg_recv_queue, &p_tem_msg, USER_NUMBER_VAL_0);  // just pop old msg pointer from queue 
			if(kNoErr == err)
			{
				if(NULL != p_tem_msg)
				{  // delete msg, free msg memory
					//free(p_tem_msg);
					cJSON_Delete(p_tem_msg);
					p_tem_msg = NULL;
				}
			}
			else
			{
				user_zigbee("WARNGING: uart zigbee msg overrun, abandon current message!");
				if(NULL != pRoot){  // queue full, new msg abandoned
					//free(pRoot);
					cJSON_Delete(p_tem_msg);
					pRoot = NULL;
				}
				mico_rtos_unlock_mutex(&uart_zigbee_msg_recv_queue_mutex);
				return kOverrunErr;
			}
		}
		
		// insert a msg into recv queue
		if (kNoErr != mico_rtos_push_to_queue(&uart_zigbee_msg_recv_queue, &pRoot, USER_NUMBER_VAL_5)) {  // just push msg pointer in queue
			 // free(pRoot);
			 cJSON_Delete(p_tem_msg);
			  pRoot = NULL;
			  err = kWriteErr;
		}
		else{
		  	err = kNoErr;
		}
		
		mico_rtos_unlock_mutex(&uart_zigbee_msg_recv_queue_mutex);
	}
	else
	{
		if(NULL != pRoot){
		  //free(pRoot);
		  cJSON_Delete(p_tem_msg);
		  pRoot = NULL;
		}
		return kNotInitializedErr;
	}

	return err;
}

static OSStatus uart_zigbee_MsgRecv(cJSON **root, uint32_t timeout_ms)
{
	OSStatus err = kUnknownErr;

	if(NULL == root){
		return kParamErr;
	}
	 
	if(NULL != uart_zigbee_msg_recv_queue)
	{
		//  mico_rtos_lock_mutex(&uart_zigbee_msg_recv_queue_mutex);
		//  if(mico_rtos_is_queue_empty(&uart_zigbee_msg_recv_queue)){
		// mico_rtos_unlock_mutex(&uart_zigbee_msg_recv_queue_mutex);
		//      return kUnderrunErr;
		// }
		err = mico_rtos_pop_from_queue(&uart_zigbee_msg_recv_queue, root, timeout_ms);  // just pop msg pointer from queue
		//  mico_rtos_unlock_mutex(&uart_zigbee_msg_recv_queue_mutex);
		//  if(kNoErr == err){
		// total_recv_buf_len -= (sizeof(ST_CTTouchStatus) - 1 + (*msg)->topic_len + (*msg)->data_len);
		// }
	}
	else
	{
		err = kNotInitializedErr;
	}

	return err;
}

void zigbee_create_net()
{
	user_zigbee("zigbee_create_net.\n");
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "network find unused\r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}
/*
static void zigbee_request_subdevice_state()
{
	user_zigbee("zigbee_request_subdevice_state.\n");

	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));
	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "cus device discovery\r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}
*/

static void zigbee_request_panid()
{
	user_zigbee("zigbee_request_panid.\n");
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "network id\r\n");
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}

void zigbee_device_delete_from_net(uint16_t dwNet)
{
	user_zigbee("zigbee_device_delete_from_net.\n");

	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, 0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "zdo leave  0x%04X 1 0\r\n", dwNet);
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}

void rs485_device_delete_from_net(uint16_t dwNet)
{
	user_zigbee("zigbee_device_delete_from_net dwNet=%04x .\n", dwNet);
	RS485_CTRL_COMMAND_PARAM_T  ctrlparam;	
	memset(&ctrlparam, USER_NUMBER_VAL_0, sizeof(RS485_CTRL_COMMAND_PARAM_T));
	memset(ctrlparam.cmd, USER_NUMBER_VAL_0, sizeof(ctrlparam.cmd));

	ctrlparam.frametype = USER_NUMBER_VAL_0;
	ctrlparam.destinationAddress = dwNet;
	ctrlparam.srcAddress = RS485_CTRL_ATTR_ID;
	ctrlparam.destEndpoit = 0;
	ctrlparam.srcEndpoit = 0;
	ctrlparam.pileId = 0x0034;
	ctrlparam.cmdId = USER_NUMBER_VAL_0;
	ctrlparam.cmd_length = USER_NUMBER_VAL_0;

	ctrlparam.cmd[ctrlparam.cmd_length++] = 0x01;

	Rs485_SendDataPackage(&ctrlparam);
		
}


int uart_zigbee_queue_init()
{
	OSStatus err = kNoErr;
	
	if(NULL == uart_zigbee_msg_recv_queue_mutex){
		err = mico_rtos_init_mutex(&uart_zigbee_msg_recv_queue_mutex);
	}
	err = mico_rtos_init_queue(&uart_zigbee_msg_recv_queue, "zb_recv_queue", sizeof(int), 20);

	return err;
}

void  uart_zigbee_msg_recv( mico_thread_arg_t arg )
{
	OSStatus err = kNoErr;
	cJSON *pJson = NULL;
	int retCode =0 ;
	UNUSED_PARAMETER( arg );

	while(USER_NUMBER_VAL_1)
	{
		err = uart_zigbee_MsgRecv(&pJson, MICO_WAIT_FOREVER);
		if(kNoErr == err)
		{
			uart_zigbee_msg_dispatch( pJson, &retCode);    
			// NOTE: msg memory must be free after been used.
			cJSON_Delete(pJson);
		}
	}
  
	mico_rtos_delete_thread(NULL);	
  	return;
}

void zigbee_init()
{
	zigbee_request_panid();
}	

int uart_zigbee_recv_start( app_context_t *app_context )
{
	OSStatus err = kNoErr;

	err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "user_zigbee_handler", uart_zigbee_msg_recv, 0x1000, 0 );
	require_noerr_string( err, exit, "ERROR: Unable to start the user_zigbee_handler" );
	
	err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "popRecvFromFlogCloudMsgThread", popRecvFromFlogCloudMsgThread, 0x2000, 0 );
	
	require_noerr_string( err, exit, "ERROR: Unable to start the popRecvFromFlogCloudMsgThread" );

	err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "rs485_uart_recv",rs485_uart_data_recv, 0x800, 0 );
	require_noerr_string( err, exit, "ERROR: Unable to start the rs485_uart_recv" );

	err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "request_DeviceState", request_DeviceState, 0x400, (uint32_t)app_context );
	require_noerr_string( err, exit, "ERROR: Unable to start the request_DeviceState" );

	return err;
 exit:
	mico_rtos_delete_thread( NULL );    
	return err;
}

