#include "rs485sensorparse.h"
#include "user_config.h"

#define rs485SensorParse(M, ...) custom_log("uart_485Sensor", M, ##__VA_ARGS__)

SENSOR_LINKAGE_PARAM_T glinkage_param;

extern app_context_t* app_context_global;
extern unsigned short ChangeEndian16(unsigned short value);
extern int rs485_update_device_state(uint16_t clu_id, uint16_t dev_addr, uint8_t endpoint, int32_t value);
//extern int sensor_linkage_turnon_light_device(int flag);
extern uint8_t* getMacAddress();
extern int check_linakge_ctrl_condistions(linkage_param_t  *plinkage_param);
extern int check_ir_lightray_linakge_ctrl_condistions(linkage_param_t  *plinkage_param,uint8_t mac[]);
extern void sensor_device_reset_init();
extern int32_t getSensorDeviceId(uint16_t dev_addr);
extern void subdevice_clear_invalid_record();
extern void scene_panel_clear_invalid_record();
extern void zigbee_sensor_linkage_param_init();
extern void sensor_ir_timerStop();
extern void sensor_ir_timerStart();
app_context_t*  get_app_context();
uint8_t scenectrl_flag = 0;

void set_scenectrlflag(uint8_t flag)
{
	rs485SensorParse(" set scenectrl_flag=%d \n", flag);

	scenectrl_flag =  flag;
}

uint8_t get_scenectrlflag()
{
	return scenectrl_flag;
}

OSStatus sensor_linkage_init()
{
	OSStatus err;
	err = mico_rtos_init_mutex(&glinkage_param.sensorparam_mutex);	
	clear_sensor_linkage_flag();
	subdevice_clear_invalid_record();
	//scene_panel_clear_invalid_record();
	zigbee_sensor_linkage_param_init();
	sensor_device_reset_init();
	
	return err;
}

OSStatus clear_sensor_linkage_flag()
{
	mico_rtos_lock_mutex(&glinkage_param.sensorparam_mutex);
	glinkage_param.co2_linkage_flag = 0;
	glinkage_param.tvoc_linkage_flag = 0;
	glinkage_param.ir_linkage_flag = 0;
	glinkage_param.smoke_linkage_flag = 0;
	glinkage_param.combustible_linkage_flag = 0;
	glinkage_param.lightrayvalue = 0;
	glinkage_param.ir_value = 0;
	mico_rtos_unlock_mutex(&glinkage_param.sensorparam_mutex);
	
      return 0;
}


int rs485_scene_handle(uint8_t cmd, uint16_t groupid, uint8_t sceneid)
{
	
	if(cmd ==0x05  && groupid == 0x0000&& sceneid == 2)
	{
		set_scenectrlflag(1);
	}
	else if(cmd ==0x07  && groupid == 0x0000&& sceneid == 2)
	{
		set_scenectrlflag(0);
	}

	return 0;
}


int rs485_DeviceAllIntegrateStatusEx(uint16_t dev_addr, uint32_t data_len, RS485_DEVICE_REPORT_STATE3_T *sensorStatus)
{
	int ret=0;
	uint8_t param_len;
	uint16_t parsed_data_len = 0;
	uint8_t *pData = NULL;
	uint16_t clu_id, wValue;
	RS485_DEV_SENSOR_REQ_STATE_T *pDevSensorReqState = NULL;

	rs485SensorParse("data_len=%d \n", data_len);
	data_len -= 11;
	
	if(data_len < sizeof(RS485_DEV_SENSOR_REQ_STATE_T))
	{
		rs485SensorParse("data is invalid.\n");
		return -1;
	}

	pData = (uint8_t *)&sensorStatus->endport[0];
	while(parsed_data_len < data_len)
	{
		pDevSensorReqState = (RS485_DEV_SENSOR_REQ_STATE_T *)(&pData[parsed_data_len]);
		clu_id = ChangeEndian16(pDevSensorReqState->pileId);
		param_len = 1;
		parsed_data_len += sizeof(RS485_DEV_SENSOR_REQ_STATE_T);

		if(pDevSensorReqState->attrType == 0x21 || pDevSensorReqState->attrType == 0x29)
			param_len = 2;

		wValue = 0;
		memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);

		parsed_data_len += param_len;
		if(pDevSensorReqState->endpoint > 0)
			rs485_update_device_state(clu_id, dev_addr,  pDevSensorReqState->endpoint, wValue);
	}
	
	return ret;
}

//12 11 00 00 00 01  01 01  FC00   01  00    01 0800 0000 20 00  02 0600 0000 10 00 03080000002032 04060000001000 05060000001000 06080000002000
int rs485_DeviceAllIntegrateStatus(uint16_t dev_addr, uint32_t data_len, RS485_DEVICE_REPORT_STATE2_T *sensorStatus)
{
	int ret=0;
	uint8_t param_len;
	uint16_t parsed_data_len = 0;
	uint8_t *pData = NULL;
	uint16_t clu_id, wValue;
	RS485_DEV_SENSOR_REQ_STATE_T *pDevSensorReqState = NULL;

	rs485SensorParse("data_len=%d \n", data_len);
	data_len -= 11;
	
	if(data_len < sizeof(RS485_DEV_SENSOR_REQ_STATE_T))
	{
		rs485SensorParse("data is invalid.\n");
		return -1;
	}

	pData = (uint8_t *)&sensorStatus->endport[0];
	while(parsed_data_len < data_len)
	{
		pDevSensorReqState = (RS485_DEV_SENSOR_REQ_STATE_T *)(&pData[parsed_data_len]);
		clu_id = ChangeEndian16(pDevSensorReqState->pileId);

		
		param_len = 1;
		parsed_data_len += sizeof(RS485_DEV_SENSOR_REQ_STATE_T);
		if(pDevSensorReqState->attrType == 0x21 || pDevSensorReqState->attrType == 0x29)
			param_len = 2;

		wValue = 0;
		memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);

		parsed_data_len += param_len;
		if(pDevSensorReqState->endpoint > 0)
			rs485_update_device_state(clu_id, dev_addr,  pDevSensorReqState->endpoint, wValue);
	}
	
	return ret;
}

//55 aa 55 00 36 12 25 00 00 60 06  01 01  fc 00  01   00  01  04 00  00 00 21 02 2b   01 fc 01 00 00 21 01 dc   01 fc 01 00 01 21 00 0b   01 04 02 00 00 29 0b 04  01 04 05 00 00 29 0f a8  d6
int rs485_SensorAllIntegrateStatus(uint16_t dev_addr, uint8_t dev_mac[], uint16_t data_len, RS485_DEVICE_REPORT_STATE2_T *sensorStatus)
{
	int ret=0;
	uint8_t param_len;
	uint16_t pileId;
	uint16_t parsed_data_len = 0;
	uint8_t *pData = NULL;
	uint16_t wValue;
	int32_t  device_index;
	uint8_t ir_flag;
	linkage_param_t  linkageparam;
	RS485_DEV_SENSOR_REQ_STATE_T *pDevSensorReqState = NULL;
	sensor_type_param_t  sensor_type_param;
	app_context_t* app_context = NULL;
	uint8_t mac[12];
	
	data_len -= 11;

	device_index = getSensorDeviceId(dev_addr);

	if(device_index < 0)
	{
		rs485SensorParse("current address device not exist.\n");
		return -3;
	}

	if(NULL == app_context)
		app_context = get_app_context();

	mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
	app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
	app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_offine_times = 1;
	mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

	//rs485SensorParse("##############device_index=%d \n", device_index );
	
	if(data_len < sizeof(RS485_DEV_SENSOR_REQ_STATE_T))
	{
		//rs485SensorParse("data is invalid.\n");
		return -1;
	}

	pData = (uint8_t *)&sensorStatus->endport[0];
	while(parsed_data_len < data_len)
	{
		pDevSensorReqState = (RS485_DEV_SENSOR_REQ_STATE_T *)(pData+parsed_data_len);

		memset(&sensor_type_param, 0, sizeof(sensor_type_param_t));
		param_len = 1;
		pileId = ChangeEndian16(pDevSensorReqState->pileId);
		pDevSensorReqState->attrId = ChangeEndian16(pDevSensorReqState->attrId);
		parsed_data_len += sizeof(RS485_DEV_SENSOR_REQ_STATE_T);
		//rs485SensorParse("pileId=%04x \n", pileId);
		switch(pileId)
		{
			case SENSOR_IR_PILE_ID:
			{
				param_len = 1;
				wValue = 0;
				
				memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);
				rs485SensorParse("========ir_val=%d \n", wValue);

				parsed_data_len += param_len;

				memset(&linkageparam, 0, sizeof(linkage_param_t));
				glinkage_param.ir_value = wValue;
				linkageparam.value = wValue;
				linkageparam.sensor_type = SENSOR_TYPE_IR;
				linkageparam.value2 = glinkage_param.lightrayvalue;
				linkageparam.sensor_type2 = SENSOR_TYPE_BEAM;
				memset(linkageparam.mac, 0, sizeof(linkageparam.mac));
				//HexDump(getMacAddress(), 12);

				memcpy(linkageparam.mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
				
				check_ir_lightray_linakge_ctrl_condistions(&linkageparam,mac);
				
				if(1 == wValue)
				{
					 ir_flag = 1;
				 	sensor_ir_timerStop();
				}
				else if(0 == wValue && 1 == ir_flag)
				{
					ir_flag = 0;					
					sensor_ir_timerStart(mac);
				}
				glinkage_param.lightrayvalue = 0;
			}
			break;
			
			case SENSOR_LIGHTRAY_PILE_ID:
			{
				if(pDevSensorReqState->attrType == 0x21)
					param_len = 2;
				
				memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);
				wValue = ChangeEndian16(wValue);

				glinkage_param.lightrayvalue = wValue;
				rs485SensorParse("========lightray_val=%d \n", wValue);
				
				parsed_data_len += param_len;

				memset(&linkageparam, 0, sizeof(linkage_param_t));
				linkageparam.value = glinkage_param.ir_value;
				linkageparam.sensor_type = SENSOR_TYPE_IR;
				linkageparam.value2 = wValue;
				linkageparam.sensor_type2 = SENSOR_TYPE_BEAM;
				memset(linkageparam.mac, 0, sizeof(linkageparam.mac));
				memcpy(linkageparam.mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
				check_ir_lightray_linakge_ctrl_condistions(&linkageparam,mac);
			}
			break;
			case SENSOR_TEMPERETURE_PILE_ID:
			{
				if(pDevSensorReqState->attrType == 0x21 || pDevSensorReqState->attrType == 0x29)
					param_len = 2;

				memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);
				wValue = ChangeEndian16(wValue);

				//rs485SensorParse("========tmpr_val=%d \n", wValue);

				parsed_data_len += param_len;
			}
			break;
			case SENSOR_HUMIDITY_PILE_ID:
			{
				double reall_val = 0.0;
				if(pDevSensorReqState->attrType == 0x21 || pDevSensorReqState->attrType == 0x29)
					param_len = 2;
				
				memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);
				wValue = ChangeEndian16(wValue);
				reall_val = wValue * 0.01;

				parsed_data_len += param_len;

				memset(&linkageparam, 0, sizeof(linkage_param_t));
				linkageparam.value =(int) reall_val;
				linkageparam.sensor_type = SENSOR_TYPE_HUMIDITY;
				memset(linkageparam.mac, 0, sizeof(linkageparam.mac));
				memcpy(linkageparam.mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
				
				rs485SensorParse("========humidity_val=%d  value= %d\n", wValue,linkageparam.value);

				if(!get_scenectrlflag()){
					rs485SensorParse("check_linakge_ctrl_condistions.........\n");

					check_linakge_ctrl_condistions(&linkageparam);
				}
			}
			break;
			case SENSOR_GAS_PILE_ID:
			{
				uint16_t attridd = pDevSensorReqState->attrId;
				//rs485SensorParse("attridd=%d \n", attridd);
				switch(attridd)
				{
					case 0:
					{
						if(pDevSensorReqState->attrType == 0x21)
							param_len = 2;

						memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);
						wValue = ChangeEndian16(wValue);
					
						//rs485SensorParse("========co2_val=%d \n", wValue);	
						parsed_data_len += param_len;

						memset(&linkageparam, 0, sizeof(linkage_param_t));
						linkageparam.value = wValue;
						linkageparam.sensor_type = SENSOR_TYPE_CO2;
						memset(linkageparam.mac, 0, sizeof(linkageparam.mac));
						memcpy(linkageparam.mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
						check_linakge_ctrl_condistions(&linkageparam);
					}
					break;
					case 1:
					{
						if(pDevSensorReqState->attrType == 0x21)
							param_len = 2;
						
						memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);
						wValue = ChangeEndian16(wValue);

						//rs485SensorParse("========voc_val=%d \n", wValue);
						parsed_data_len += param_len;

						memset(&linkageparam, 0, sizeof(linkage_param_t));
						linkageparam.value = wValue;
						linkageparam.sensor_type = SENSOR_TYPE_TVOC;
						memset(linkageparam.mac, 0, sizeof(linkageparam.mac));
						memcpy(linkageparam.mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
						if( wValue>= 0 && wValue<= 1000 )
							check_linakge_ctrl_condistions(&linkageparam);
						else
							ret = -1;
					}
					break;
					case 2:
					{
						if(pDevSensorReqState->attrType == 0x21 || pDevSensorReqState->attrType == 0x29)
							param_len = 2;
						
						memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);
						wValue = ChangeEndian16(wValue);
					
						rs485SensorParse("========combustible_val=%d \n", wValue);	

						parsed_data_len += param_len;

						memset(&linkageparam, 0, sizeof(linkage_param_t));
						linkageparam.value = wValue;
						linkageparam.sensor_type = SENSOR_TYPE_COMBUSTIBLE;
						memset(linkageparam.mac, 0, sizeof(linkageparam.mac));
						memcpy(linkageparam.mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
						check_linakge_ctrl_condistions(&linkageparam);
					}
					break;
					case 3:
					{
						if(pDevSensorReqState->attrType == 0x21 || pDevSensorReqState->attrType == 0x29)
							param_len = 2;
						
						memcpy(&wValue, &pDevSensorReqState->attrType+1, param_len);
						wValue = ChangeEndian16(wValue);
											
						//rs485SensorParse("========smoke_val=%d \n", wValue);
						parsed_data_len += param_len;

						memset(&linkageparam, 0, sizeof(linkage_param_t));
						linkageparam.value = wValue;
						linkageparam.sensor_type = SENSOR_TYPE_SMOKE;
						memset(linkageparam.mac, 0, sizeof(linkageparam.mac));
						memcpy(linkageparam.mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
						check_linakge_ctrl_condistions(&linkageparam);
					}
					break;
				}
			}
			break;
		}
	}
	
	return ret;
}

