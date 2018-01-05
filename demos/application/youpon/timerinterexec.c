#include "timerinterexec.h"
#include "timerctrl.h"
#include "zigbeeparse.h"
#include "rs485_comm.h"

#define TimerExe_mng(M, ...) custom_log("TimerExemng", M, ##__VA_ARGS__)

extern int get_device_param_info(struct device_param_info_struct *pDeviceInfo);
extern OSStatus pushAppMainRecvMsgProcess(unsigned char *msg, unsigned int inBufLen, int riu);
extern int timer_scene_control(struct timer_task_struct * pTsk);

void zigbee_send_scene_control(struct device_param_info_struct *pDevParam)
{
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, USER_NUMBER_VAL_0, sizeof(ctrlparam.cmd));
	if( pDevParam->devclass == 9)
		sprintf(ctrlparam.cmd, "cus scene recall 0 %d \r\n send 0x%04X 1 1 \r\n\n",  pDevParam->endpoint_id, pDevParam->dev_addr);
	else
		sprintf(ctrlparam.cmd, "cus scene recall 1 %d \r\n send 0xFFFF 1 1 \r\n\n",  pDevParam->endpoint_id);
	
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}


int rs485_device_scene_control(struct device_param_info_struct * deviceParamInfo)
{
	TimerExe_mng("rs485_device_scene_control............................");
	int ret = -1;
	SCENE_ATTRI_T mDeviceAttr;
	memset(&mDeviceAttr, 0, sizeof(SCENE_ATTRI_T));

	RS485_CTRL_COMMAND_PARAM_T  ctrlparam;	
	memset(&ctrlparam, USER_NUMBER_VAL_0, sizeof(RS485_CTRL_COMMAND_PARAM_T));
	memset(ctrlparam.cmd, USER_NUMBER_VAL_0, sizeof(ctrlparam.cmd));

	ctrlparam.frametype = 0x02;
	ctrlparam.destinationAddress = deviceParamInfo->dev_addr;
	ctrlparam.srcAddress = RS485_CTRL_ATTR_ID;
	ctrlparam.destEndpoit = USER_NUMBER_VAL_1;
	ctrlparam.srcEndpoit = USER_NUMBER_VAL_1;
	ctrlparam.pileId = 0x0005;
	ctrlparam.cmdId = USER_NUMBER_VAL_5;
	ctrlparam.cmd_length = USER_NUMBER_VAL_0;

	mDeviceAttr.attriId = 0x0000;
	mDeviceAttr.attriValue = deviceParamInfo->endpoint_id;
	ctrlparam.cmd_length = sizeof(mDeviceAttr);
	memcpy(ctrlparam.cmd, &mDeviceAttr, sizeof(mDeviceAttr));

	Rs485_SendDataPackage(&ctrlparam);
	
	return ret;
}


void zigbee_send_light_single_control(struct device_param_info_struct *pDevParam)
{
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, USER_NUMBER_VAL_0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "zcl on-off %s \r\n send 0x%04X 1 %d \r\n\n",  (pDevParam->value==1) ? DEVICE_ACTION_ON : DEVICE_ACTION_OFF, pDevParam->dev_addr, pDevParam->endpoint_id);
	TimerExe_mng("ctrlparam.cmd=%s \n", ctrlparam.cmd);
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}

void zigbee_send_airchange_control(struct device_param_info_struct *pDevParam)
{
	KEY_BUTTON_CTRL_PARAM_T  ctrlparam;
	memset(&ctrlparam, 0, sizeof(KEY_BUTTON_CTRL_PARAM_T));

	memset(ctrlparam.cmd, USER_NUMBER_VAL_0, sizeof(ctrlparam.cmd));
	sprintf(ctrlparam.cmd, "zcl level-control o-mv-to-level %d 0 \r\n send 0x%04X 1 %d \r\n", pDevParam->value, pDevParam->dev_addr,  pDevParam->endpoint_id);
	ctrlparam.inBufLen = strlen(ctrlparam.cmd);
	TimerExe_mng("ctrlparam.cmd=%s \n", ctrlparam.cmd);
	pushAppMainRecvMsgProcess( (unsigned char *)ctrlparam.cmd, ctrlparam.inBufLen, 1);
}


int rs485_device_single_control(struct device_param_info_struct * deviceParamInfo)
{
	TimerExe_mng("rs485_device_single_control....................: %d %04X   %04x   %d ........", deviceParamInfo->endpoint_id, deviceParamInfo->nclu, deviceParamInfo->attrcmd, deviceParamInfo->value);
	int ret = -1;
	DEVICE_ATTRI_T mDeviceAttr;
	memset(&mDeviceAttr, 0, sizeof(DEVICE_ATTRI_T));

	RS485_CTRL_COMMAND_PARAM_T  ctrlparam;	
	memset(&ctrlparam, USER_NUMBER_VAL_0, sizeof(RS485_CTRL_COMMAND_PARAM_T));
	memset(ctrlparam.cmd, USER_NUMBER_VAL_0, sizeof(ctrlparam.cmd));

	ctrlparam.frametype = USER_NUMBER_VAL_1;
	ctrlparam.destinationAddress = deviceParamInfo->dev_addr;
	ctrlparam.srcAddress = RS485_CTRL_ATTR_ID;
	ctrlparam.destEndpoit = deviceParamInfo->endpoint_id;
	ctrlparam.srcEndpoit = USER_NUMBER_VAL_1;
	ctrlparam.pileId = deviceParamInfo->nclu;;
	ctrlparam.cmdId = USER_NUMBER_VAL_2;
	ctrlparam.cmd_length = USER_NUMBER_VAL_0;

	mDeviceAttr.attriId = RS485_CTRL_ATTR_ID;
	mDeviceAttr.attriCmd = deviceParamInfo->attrcmd;//RS485_DATA_PARAM_TYPE;
	mDeviceAttr.attriValue = deviceParamInfo->value;
	ctrlparam.cmd_length = sizeof(mDeviceAttr);
	memcpy(ctrlparam.cmd, &mDeviceAttr, sizeof(mDeviceAttr));
	Rs485_SendDataPackage(&ctrlparam);
	
	return ret;
}

int timer_single_control(struct timer_task_struct * pTsk)
{
	int ret = -1;
	uint8_t end_point;
	uint8_t value;
	uint8_t attr_type=0x20;
	struct device_param_info_struct  deviceParamInfo;
	DEVICE_ATTRI_T mDeviceAttr;


	if(NULL == pTsk)
	{
		return ret;
	}

	TimerExe_mng("timer_single_control mac: %s .......deviceParamInfo.dev_type:%d .....................", pTsk->mac, deviceParamInfo.dev_type);

	memset(&mDeviceAttr, 0, sizeof(DEVICE_ATTRI_T));

	RS485_CTRL_COMMAND_PARAM_T  ctrlparam;	
	memset(&ctrlparam, USER_NUMBER_VAL_0, sizeof(RS485_CTRL_COMMAND_PARAM_T));
	memset(ctrlparam.cmd, USER_NUMBER_VAL_0, sizeof(ctrlparam.cmd));

	memset(&deviceParamInfo, 0, sizeof(struct device_param_info_struct));
	memcpy(deviceParamInfo.mac, pTsk->mac, DEVICE_MAC_ADDRESS_LENGHT);
	
	end_point = pTsk->obj_ctrl_id;
	value = pTsk->func_value;

	deviceParamInfo.endpoint_id = end_point;
	get_device_param_info(&deviceParamInfo);

	if(deviceParamInfo.riu == 0)
	{
		if((deviceParamInfo.dev_type == 2 && deviceParamInfo.nclu== 0x0006) || deviceParamInfo.dev_type == 3 || deviceParamInfo.dev_type == 5 || deviceParamInfo.dev_type == 6 || deviceParamInfo.dev_type == 7)
			attr_type=0x10;
			
		deviceParamInfo.value = value;
		deviceParamInfo.attrcmd = attr_type;
		rs485_device_single_control(&deviceParamInfo);	
	}
	else if(deviceParamInfo.riu == 1)
	{
		deviceParamInfo.value = value;
		if((deviceParamInfo.dev_type == 1 && end_point == 2) || deviceParamInfo.dev_type == 3 || deviceParamInfo.dev_type == 5 || deviceParamInfo.dev_type == 6 || deviceParamInfo.dev_type == 7)
		{
			zigbee_send_light_single_control(&deviceParamInfo);
		}
		else
		{
			zigbee_send_airchange_control(&deviceParamInfo);
		}
	}			



	return ret;
}


int timer_scene_control(struct timer_task_struct * pTsk)
{
	
	int ret = -1;
	uint8_t end_point;
	//uint8_t value;
	//uint8_t attr_type=0x20;
	struct device_param_info_struct  deviceParamInfo;
	DEVICE_ATTRI_T mDeviceAttr;


	if(NULL == pTsk)
	{
		return ret;
	}

	TimerExe_mng("timer_single_control mac: %s .......deviceParamInfo.dev_type:%d .....................", pTsk->mac, deviceParamInfo.dev_type);

	memset(&mDeviceAttr, 0, sizeof(DEVICE_ATTRI_T));

	RS485_CTRL_COMMAND_PARAM_T  ctrlparam;	
	memset(&ctrlparam, USER_NUMBER_VAL_0, sizeof(RS485_CTRL_COMMAND_PARAM_T));
	memset(ctrlparam.cmd, USER_NUMBER_VAL_0, sizeof(ctrlparam.cmd));

	memset(&deviceParamInfo, 0, sizeof(struct device_param_info_struct));
	memcpy(deviceParamInfo.mac, pTsk->mac, DEVICE_MAC_ADDRESS_LENGHT);
	
	end_point = pTsk->obj_ctrl_id;

	deviceParamInfo.endpoint_id = end_point+1;
	get_device_param_info(&deviceParamInfo);

	if(deviceParamInfo.riu == 0)
	{
		rs485_device_scene_control(&deviceParamInfo);	
	}
	else if(deviceParamInfo.riu == 1)
	{
		zigbee_send_scene_control(&deviceParamInfo);
	}			



	return ret;
}

