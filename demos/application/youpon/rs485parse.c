
#include "rs485parse.h"
#include "user_config.h"
#include "rs485sensorparse.h"
#include "devicemanager.h"

#define rs485Parse(M, ...) custom_log("uart_485", M, ##__VA_ARGS__)

extern app_context_t* app_context_global;
extern void zigbee_update_device_property_state(int key_id, int state);
extern unsigned short ChangeEndian16(unsigned short value);
extern int get_device_mac(uint16_t nwk_addr, int8_t mac[]);
extern int rs485_SensorAllIntegrateStatus(uint16_t dev_addr, uint8_t dev_mac[], uint16_t len, RS485_DEVICE_REPORT_STATE2_T *sensorStatus);
extern int rs485_DeviceAllIntegrateStatus(uint16_t dev_addr, uint32_t data_len, RS485_DEVICE_REPORT_STATE2_T *sensorStatus);
extern int rs485_DeviceAllIntegrateStatusEx(uint16_t dev_addr, uint32_t data_len, RS485_DEVICE_REPORT_STATE3_T *sensorStatus);
extern int rs485_update_device_state(uint16_t clu_id, uint16_t dev_addr, uint8_t endpoint, int32_t value);
extern int device_onoffline_update_flag(uint16_t nwk_addr, uint8_t flag);
extern int rs485_new_device_add_report(RS485_DATA_FORMAT_T *pRs485Data);
extern unsigned int charchangehex( unsigned char *data , unsigned char* value );
extern int rs485_sensor_device_onoffline_update_flag(uint16_t nwk_addr, uint8_t flag);
extern void sensor_device_reset_init();
extern int rs485_sensor_device_register(uint32_t devicetype, uint16_t dev_addr);
extern int rs485_scene_handle(uint8_t cmd, uint16_t groupid, uint8_t sceneid);

//12 11 00 00 00 01  01 01  FC00   01  00    01 0800 0000 20 00 0206000000100003080000002032040600000010000506000000100006080000002000
OSStatus  rs485_device_report_state(uint32_t  len, RS485_DATA_FORMAT_T *pRs485Data)
{
	OSStatus err = kNoErr;
	uint16_t dev_addr;
	uint8_t dev_mac[32];
	RS485_DEVICE_REPORT_STATE2_T *pRs485StateResp = NULL;

	if(NULL == pRs485Data)
	{
		err = kParamErr;
		rs485Parse("state response pkg error.");
		return err;
	}

	pRs485StateResp = (RS485_DEVICE_REPORT_STATE2_T *)pRs485Data->dataheader.mac_addr;//is is report status , no mac address
	dev_addr = ChangeEndian16(pRs485Data->dataheader.srcAddr);
	pRs485Data->dataheader.cluID = ChangeEndian16(pRs485Data->dataheader.cluID);

#ifdef USER_DEBUG_
	//rs485Parse("len:%d endpoint:%d dev_addr:%04x pRs485Data->cmdID: %d pRs485StateResp->attrState:%d pRs485Data->cluID=%04x pileId=%04x\n", len, pRs485StateResp->endport[USER_NUMBER_VAL_0].endpoint, dev_addr, pRs485Data->dataheader.cmdID, pRs485StateResp->attrState, pRs485Data->dataheader.cluID, pRs485StateResp->endport[0].pileId);
#endif

	if( pRs485Data->dataheader.cmdID == USER_NUMBER_VAL_1)//Obtain device state cmd response
	{
		if(pRs485StateResp->attrState == USER_NUMBER_VAL_0)
		{
			memset(dev_mac, 0, sizeof(dev_mac));
			err = rs485_SensorAllIntegrateStatus(dev_addr, dev_mac, len, pRs485StateResp);
			if(err == -3)//current device is not sensor device
			{
				err = rs485_DeviceAllIntegrateStatus(dev_addr, len, pRs485StateResp);
			}
		}
	}
	else if(pRs485Data->dataheader.cmdID == USER_NUMBER_VAL_2)//report device state cmd
	{		
		RS485_DEVICE_REPORT_STATE3_T *pRs485StateResp2 = (RS485_DEVICE_REPORT_STATE3_T *)pRs485Data->payload;
		err = rs485_DeviceAllIntegrateStatusEx(dev_addr, len, pRs485StateResp2);
	}
	else if(pRs485Data->dataheader.cmdID == USER_NUMBER_VAL_3)//all device close cmd
	{
		close_all_rs485_device();
	}
	
	return err;
}
OSStatus  rs485_msg_state_report(RS485_DATA_FORMAT_T *pRs485Data)
{
	OSStatus err = kNoErr;
	RS485_CTRL_STATE_RESP_T *pRs485StateResp = NULL;
	RS485_SET_STATE * pRs485SetState= NULL;
	uint16_t dev_addr,clu_id;
	uint8_t setPiont;
	uint8_t setValue;
	pRs485Data->dataheader.cluID = ChangeEndian16(pRs485Data->dataheader.cluID);

	if(pRs485Data->dataheader.cmdID == 2)
	{
		pRs485SetState = (RS485_SET_STATE *)pRs485Data->dataheader.mac_addr;
		dev_addr = ChangeEndian16(pRs485Data->dataheader.destAddr);
		clu_id = pRs485Data->dataheader.cluID;
		setPiont = pRs485Data->dataheader.destEendport;
		setValue = pRs485SetState->value;
	}
	else if(pRs485Data->dataheader.cmdID == 4||pRs485Data->dataheader.cmdID == 0x0A||pRs485Data->dataheader.cmdID == 0x01)
	{		
		pRs485StateResp = (RS485_CTRL_STATE_RESP_T *)pRs485Data->dataheader.mac_addr;
		dev_addr = ChangeEndian16(pRs485Data->dataheader.srcAddr);
		clu_id = pRs485Data->dataheader.cluID;
		setPiont = pRs485Data->dataheader.srcEendport;
		setValue = pRs485StateResp->value;		


		if(pRs485StateResp->attrId == 0x0016)
		{
			return -1;
		}
		
	}
	else
		return err;
	
#ifdef USER_DEBUG_
	//rs485Parse("pRs485Data->cmdID: %d setPiont:%d pRs485StateResp->attrState:%d pRs485Data->cluID=%04x attrId=%d attrType=%d value=%d \n", pRs485Data->dataheader.cmdID, setPiont, pRs485StateResp->attrState, pRs485Data->dataheader.cluID, pRs485StateResp->attrId, pRs485StateResp->attrType,setValue);
#endif

	rs485_update_device_state(clu_id, dev_addr,  setPiont, setValue);
	
	return err;
}

static OSStatus  rs485_msg_delete_dev_joined(RS485_DATA_FORMAT_T *pRs485Data)
{
	OSStatus err = kNoErr;
	uint16_t dev_addr;

	if(NULL == pRs485Data)
	{
		err = kParamErr;
		rs485Parse("pRs485Data is NULL.");
		return err;
	}


	dev_addr = ChangeEndian16(pRs485Data->dataheader.srcAddr);
	
	if(pRs485Data->dataheader.cmdID == 0)
	{
		if(device_onoffline_update_flag(dev_addr, 2) < 0) 
			rs485_sensor_device_onoffline_update_flag(dev_addr, 2);
	}

	return err;
}


OSStatus  rs485_dev_joined_request(RS485_DATA_FORMAT_T *pRs485Data)
{
	OSStatus err = kNoErr;

	if(NULL == pRs485Data )
	{
		err = kParamErr;
		rs485Parse("auto_find_network error.");
		return err;
	}

	if(pRs485Data->dataheader.destEendport == USER_NUMBER_VAL_1 && pRs485Data->dataheader.srcEendport == USER_NUMBER_VAL_1)
	{
		return err;
	}

	rs485_new_device_add_report(pRs485Data);	


	return err;
}

//104E FFFF 0001 0000 0000 04 0F000000000000
int rs485_device_logout_report(RS485_DATA_FORMAT_T *pkgdata) 
{

	RS485_REQUEST_JOIN_T *pRs485Requst = NULL;
	uint32_t devicetype=0;
	uint16_t report_addr;
	uint8_t device_type_count;
	pRs485Requst = (RS485_REQUEST_JOIN_T *)pkgdata->payload;
	devicetype = pRs485Requst->devicetype[0];
	//memcpy(&devicetype, pRs485Requst->devicetype, 2);
	report_addr = ChangeEndian16(pkgdata->dataheader.srcAddr); 
	device_type_count = (pkgdata->data_len - sizeof(RS485_DATA_FORMAT_HEADER_T)-2); 
	rs485Parse("rs485_device_logout_report:device_type_count=%d,devicetype=%04x,pkgdata->srcAddr=%04x,pkgdata->destAddr=%04x\r\n", device_type_count,devicetype, pkgdata->dataheader.srcAddr, pkgdata->dataheader.destAddr);
	if(device_type_count==1)
	{ 
		switch(devicetype)
	    	{ 
	    		case RS485_TYPE_WARM_WIND: 
			case RS485_TYPE_LIGHT_WARM:
           		case RS485_TYPE_LIGHT:
            		case RS485_TYPE_CHANGEAIR: 
				device_onoffline_update_flag(report_addr, 2);
                	break; 
			case RS485_TYPE_SENSOR_LIGHTRAY:
           		case RS485_TYPE_SENSOR_GAS:
            		case RS485_TYPE_SENSOR_TEMPR_HUMITURE:
            		case RS485_TYPE_SENSOR_IR:
            		case RS485_TYPE_SENSOR_COMBUSTIBLE:
            		case RS485_TYPE_SENSOR_SMOKE:
				rs485_sensor_device_onoffline_update_flag(report_addr, 2);
                		break; 
			default:
            		{ 
				rs485Parse("unkown device type.\n"); 
			}
            			break;
	    	} }
	else if((device_type_count == CTRL_BOX_DEVICE_ENDPOINT_COUNT)||(device_type_count == 3))//rs485 Control box
	{ 
		device_onoffline_update_flag(report_addr, 2);
	}
	else if(device_type_count>0)
	{ 
		rs485_sensor_device_onoffline_update_flag(report_addr, 2); 

	}
	
    return 0; 
}

int rs485_msg_parse_scene_cluster(RS485_DATA_FORMAT_T *pRs485Data)
{
	
	RS485_SCENE_GROUP *pSceneGroup;
	uint16_t groupid;
	uint8_t  sceneid;


	pSceneGroup= (RS485_SCENE_GROUP *)pRs485Data->dataheader.mac_addr;

	rs485Parse("frametype:0x%02x,0x%02x",pRs485Data->dataheader.frametype,(pRs485Data->dataheader.frametype&0x08) );
	groupid = ChangeEndian16(pSceneGroup->group_id );
	sceneid = pSceneGroup->scene_id;
	//if((pRs485Data->dataheader.frametype&0x08)==0)
	{
		if(pRs485Data->dataheader.cmdID == USER_NUMBER_VAL_5||pRs485Data->dataheader.cmdID == USER_NUMBER_VAL_7)
		{	
			rs485_scene_handle(pRs485Data->dataheader.cmdID, groupid, sceneid);		
		}
		
	}
	//scenepanel_device_attribute_update(pRs485Data->dataheader.cmdID ,groupid, sceneid);
	return 0;
}

//"55AA55001F3200488B00000101000509000101020406000000100105060000001001FF";
int rs485_parse_write_scene_info(uint8_t *str)
{
	int i, bytesLen, str_len;
	short int net_addr;
	
	uint8_t  buff[5]={0};   
	unsigned char tmp[USER_NUMBER_VAL_128+11]={0};
	unsigned char ids_buff[USER_NUMBER_VAL_64+5]={0};

	if(!str)
		return -1;

	for(i = 0; i<4; i++)
            buff[i] =*(str+14+i); //net_id string
		
	str_len = strlen(str) - 32 -2; //str_len is groupid,sceneid, cnt, payload.
	if(str_len >= (USER_NUMBER_VAL_128+10))
		str_len=USER_NUMBER_VAL_128+10;
	
	memcpy(tmp, str + 32 , str_len);
		
	bytesLen = str_len/USER_NUMBER_VAL_2;
	if(bytesLen>(USER_NUMBER_VAL_64+4))
		bytesLen = USER_NUMBER_VAL_64+4;
		
	for( i = USER_NUMBER_VAL_0; i < bytesLen ; i++ )
	{
		charchangehex( &tmp[i*USER_NUMBER_VAL_2] , &ids_buff[i] );
	}

 	net_addr = strtol((char const *)buff,NULL, 16);	

	rs485Parse("tmp:[%d][%s]",str_len, tmp);
	rs485Parse("net_addr:0x%x",net_addr);
	rs485Parse("ids:buff:");
	//HexDump(ids_buff,bytesLen);	

	write_scene_info(net_addr, ids_buff, bytesLen);

	return 0;
}


OSStatus  rs485_msg_zcl_zdo(cJSON *root)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	cJSON *objectValue1 = NULL;
	unsigned short  nclu;
	uint32_t  data_len=0;
	unsigned char tmp[USER_NUMBER_VAL_0X8C]={0};
	unsigned char ids_buff[USER_NUMBER_VAL_128]={0};
	unsigned char recvbuff[USER_NUMBER_VAL_256] ={0};
	unsigned short pileId;

	int i;
	int bytesLen;
       int str_len;
	RS485_DATA_FORMAT_T *pRs485Data = NULL;
	RS485_DEVICE_REPORT_STATE2_T *pRs485StateResp = NULL;

	
	if(NULL == root)
	{
		err = kParamErr;
		rs485Parse("auto_find_network error.");
		return err;
	}
#if 0	
	char	*out = cJSON_PrintUnformatted(root);
	if(out)
	{
		rs485Parse("rs485_msg_zcl_zdo:%s", out);
		free(out);
		out = NULL;
	}
#endif
	objectValue1 = cJSON_GetObjectItem(root, "t");
	objectValue = cJSON_GetObjectItem(root, "rs485");
	if(objectValue && objectValue->valuestring != NULL)
	{
		memset(tmp, USER_NUMBER_VAL_0, sizeof(tmp));
		memset(ids_buff, USER_NUMBER_VAL_0, sizeof(ids_buff));
		str_len = strlen(objectValue->valuestring);
		if(str_len > USER_NUMBER_VAL_0X8C)str_len=USER_NUMBER_VAL_0X8C;
		memcpy(tmp, objectValue->valuestring, str_len);

		data_len = bytesLen = str_len/USER_NUMBER_VAL_2;//not include frameheader and length field
		for( i = USER_NUMBER_VAL_0; i < bytesLen ; i++ )
		{
			charchangehex( &tmp[i*USER_NUMBER_VAL_2] , &ids_buff[i] );
		}
	}

	pRs485Data = (RS485_DATA_FORMAT_T*)ids_buff;
	pRs485StateResp = (RS485_DEVICE_REPORT_STATE2_T *)pRs485Data->dataheader.mac_addr;

	nclu = ChangeEndian16(pRs485Data->dataheader.cluID);
	pileId = ChangeEndian16(pRs485StateResp->endport[0].pileId);
	
	pRs485Data->data_len = data_len;
	if(nclu != ZIGBEE_NCLU_SCENE_CLUSTER)
	{
	//	if(pRs485Data->dataheader.destAddr != RS485_CTRL_ATTR_ID 
	//		&& pRs485Data->dataheader.destAddr != USER_NUMBER_VAL_0XFFFF)
	//	{
	//		rs485Parse("package is myself.\n");
	//		return err;
	//	}
	}




#ifdef USER_DEBUG_
	rs485Parse("**********************rs485 nclu: %04X cmdID:%d \n", nclu, pRs485Data->dataheader.cmdID);
#endif
	
	switch(nclu)
	{
		case ZIGBEE_NCLU_STATE_REPORT://request joined net
		{
			if(pRs485Data->dataheader.cmdID == USER_NUMBER_VAL_4)
			{
				rs485_device_logout_report(pRs485Data);
			}
			else if(pRs485Data->dataheader.cmdID == ZIGBEE_RESP_EVENT_FLAG_A)
			{
				rs485_msg_state_report(pRs485Data);
			}
			else if(pRs485Data->dataheader.cmdID == USER_NUMBER_VAL_2)
			{
				rs485_dev_joined_request(pRs485Data);
			}
		}
		break;
		case ZIGBEE_NCLU_JOIN_NWK:
		{
			rs485_new_device_joined_success_ack(pRs485Data);
		}
		break;
		case ZIGBEE_NCLU_REQUEST_STATE://joined net broadcast 
		case ZIGBEE_NCLU_SWITCH_STATE_REPORT:
			rs485_msg_state_report(pRs485Data);
		break;
		case ZIGBEE_NCLU_QUIT_JOIN_NWK://logout response
			rs485_msg_delete_dev_joined(pRs485Data);
			break;
		case RS485_DEVICE_REPORT_STATE:
			{
				err = rs485_device_report_state(data_len, pRs485Data);
			}
			break;
		case ZIGBEE_NCLU_SCENE_CLUSTER:
			rs485_msg_parse_scene_cluster(pRs485Data);

			break;
	}


	if((objectValue1->valueint == 15)||(nclu== ZIGBEE_NCLU_SCENE_CLUSTER&&pRs485Data->dataheader.cmdID==0x08))
		return err;

	if((nclu!= RS485_DEVICE_REPORT_STATE)|| \
	   (nclu== RS485_DEVICE_REPORT_STATE && pileId == 0x0008)|| \
	   (nclu== RS485_DEVICE_REPORT_STATE && pileId == 0x0402)|| \
	   (nclu== RS485_DEVICE_REPORT_STATE && pileId == 0x0405))
	{
		
		sprintf(recvbuff, "cus rs485 rx {%s}\r\n", tmp);
		#ifdef USER_DEBUG_
		rs485Parse("rs485 data sent to uart2=%s\n", recvbuff);
		#endif
		pushAppMainRecvMsgProcess( (unsigned char *)recvbuff, strlen(recvbuff), 1);//send to zigbee queue
	}

	return err;
}

