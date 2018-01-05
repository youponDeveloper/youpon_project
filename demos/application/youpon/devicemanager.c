
#include "devicemanager.h"
#include "user_config.h"
#include "rs485parse.h"
#include "system_internal.h"
#include "timercontrol.h"
#include "sensormanager.h"
#include "user_main.h"

#define device_mng(M, ...) custom_log("device_mng", M, ##__VA_ARGS__)

//#define   DEVICE_BIND_TEST	1
static void integrade_device_report_attribute_state(app_context_t* app_context, uint32_t device_type, int i, ZB_DEV_JOINED_REQUEST_T *pDevInfo);
static void light_device_report_attribute_state(app_context_t* app_context, uint32_t device_type, int i, ZB_DEV_JOINED_REQUEST_T *pDevInfo);

extern app_context_t*  get_app_context();
extern void HexDump(uint8_t * pdata, uint32_t data_len);
extern system_context_t* get_sys_context();
extern unsigned short ChangeEndian16(unsigned short value);
extern OSStatus getDeviceListInfo(uint32_t device_id);
extern OSStatus pushAppMainRecvMsgProcess(unsigned char *msg, unsigned int inBufLen, int riu);
extern int rs485_device_single_control(struct device_param_info_struct * deviceParamInfo);
extern system_context_t* get_sys_context();
extern uint8_t* getMacAddress();
extern void rs485_request_get_device(uint16_t devAddr);
extern int updateKitchenSensorDeviceNwk( uint8_t dev_mac[], uint16_t nwkaddr);
int find_device(uint16_t dev_netaddr)
{
	int ret = -1;
	int i;
	uint8_t is_used;
	uint16_t dev_addr;

	app_context_t* app_context = get_app_context();
	
	for(i = 0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);	
		is_used= app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_used == 1 && dev_addr == dev_netaddr)
		{
			ret = i;
			device_mng("find out device dev_addr:0x%04x  i:%d\n",dev_addr, i);
			break;
		}
	}

	return ret;
}

uint32_t get_device_class(uint16_t dev_netaddr)
{
	uint32_t dev_class = 0;
	int i;
	uint8_t is_used;
	uint16_t dev_addr;

	app_context_t* app_context = get_app_context();
	
	for(i = 0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);	
		is_used    = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_addr  = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		dev_class = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_used == 1 && dev_addr == dev_netaddr)
		{
			device_mng("dev_class:%d", dev_class);
			return dev_class ;
		}
	}

	return 0;

}

int zigbee_sensor_linkage_control_airchange(linkage_param_t  *plinkage_param)
{
	int ret = -1;
	int i,j;
	uint16_t dev_addr,clu_id;
	uint8_t is_can_add,endpoint_id,dev_type;
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	app_context_t* app_context = get_app_context();
	struct device_param_info_struct dev_param;
	
	if(NULL == app_context || NULL == plinkage_param)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	//HexDump(plinkage_param->mac, DEVICE_MAC_ADDRESS_LENGHT);
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		memset(mac, 0, sizeof(mac));
		memcpy(mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[i].mac));
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//HexDump(mac, sizeof(mac));
		//device_mng(" dev_addr:%04x \n", dev_addr);
		if(is_can_add == 1 && !memcmp(plinkage_param->mac, mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			for(j=0; j < DEVICE_MAC_ENDPOINT; j++)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				endpoint_id = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id;
				clu_id = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].clu_id;
				dev_type = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].dev_type;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				device_mng(" endpoint_id:%d obj_id:%d \n", endpoint_id, plinkage_param->obj_id);
				if(plinkage_param->ctrl_type == 1 && endpoint_id == plinkage_param->obj_id )
				{
					memset(&dev_param, 0, sizeof(struct device_param_info_struct));
					dev_param.dev_addr = dev_addr;
					dev_param.dev_type = dev_type;
					dev_param.endpoint_id = endpoint_id;
					dev_param.nclu = clu_id;
					if((dev_type == 1 && endpoint_id == 2) || dev_type == 4 || dev_type == 2)
						dev_param.attrcmd = RS485_DATA_PARAM_TYPE;
					else
						dev_param.attrcmd = RS485_DATA_PARAM_TYPE2;
					
					dev_param.value = plinkage_param->param;
					zigbee_send_airchange_control(&dev_param);
					
				}
			}
			
			return 0;
		}
	}
        
        return ret;
}

int zigbee_sensor_linkage_turnon_light_device(linkage_param_t  *plinkage_param)
{
	int ret = -1;
	int i,j;
	uint16_t dev_addr;
	uint8_t is_can_add,endpoint_id;
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	app_context_t* app_context = get_app_context();
	system_context_t* mico_context = (system_context_t*)get_sys_context();
	struct device_param_info_struct dev_param;
	
	if(NULL == app_context || NULL == plinkage_param)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}
	
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		memset(mac, 0, sizeof(mac));
		memcpy(mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[i].mac));
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//HexDump(mac, sizeof(mac));
		//device_mng(" dev_addr:%04x \n", dev_addr);
		if(is_can_add == 1 && !memcmp(plinkage_param->mac, mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			for(j=0; j < DEVICE_MAC_ENDPOINT; j++)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				endpoint_id = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				//device_mng(" endpoint_id:%d \n", endpoint_id);
				if(endpoint_id == 4)// || endpoint_id == 5 
				{
					memset(&dev_param, 0, sizeof(struct device_param_info_struct));
					dev_param.dev_addr = dev_addr;
					dev_param.dev_type = 4;
					dev_param.endpoint_id = 4;
					dev_param.nclu = 0x0006;
					dev_param.value = plinkage_param->param;
					dev_param.attrcmd = RS485_DATA_PARAM_TYPE;
					zigbee_send_light_single_control(&dev_param);
					
					//mico_rtos_thread_msleep(120);
					//dev_param.endpoint_id = 5;
					//zigbee_send_light_single_control(&dev_param);

					return 0;
				}
			}
			
			return 0;
		}
	}
        
        return ret;
}

void subdevice_clear_invalid_record()
{
	int i;
	uint8_t is_can_add;
	uint32_t dev_class;
	uint16_t net_addr;
	
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_class = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		net_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		if((is_can_add == 1 && dev_class == 0) || (is_can_add == 1 && dev_class == 2048) ||  net_addr == 0xFFFF)
		{
			memset(&(app_context->userDataStore->userData.deviceinfo.device_info[i]), 0, sizeof(device_info_list_t));			
		}
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
	}

}


int sensor_linkage_control_device(linkage_dev_param_t *pLinkageParam)
{
	int ret = -1;
	int i,j;
	uint16_t dev_addr,clu_id;
	uint8_t is_can_add,endpoint_id,dev_type,riu=0;
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	app_context_t* app_context = get_app_context();
	struct device_param_info_struct dev_param;
	
	if(NULL == app_context || NULL == pLinkageParam)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	//memcpy(pLinkageParam->dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		riu = app_context->userDataStore->userData.deviceinfo.device_info[i].riu;
		memset(mac, 0, sizeof(mac));
		memcpy(mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[i].mac));
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		HexDump(mac, sizeof(mac));
		device_mng(" dev_addr:%04x \n", dev_addr);
		if(is_can_add == 1 && !memcmp(pLinkageParam->dev_mac, mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			for(j=0; j < DEVICE_MAC_ENDPOINT; j++)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				endpoint_id = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id;
				clu_id = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].clu_id;
				dev_type = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].dev_type;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
#ifdef USER_DEBUG_
				device_mng(" endpoint_id:%d obj_id:%d \n", endpoint_id, pLinkageParam->obj_id);
#endif

				if(riu == 0 &&  endpoint_id == pLinkageParam->obj_id )
				{
					memset(&dev_param, 0, sizeof(struct device_param_info_struct));
					dev_param.dev_addr = dev_addr;
					dev_param.dev_type = dev_type;
					dev_param.endpoint_id = endpoint_id;
					dev_param.nclu = clu_id;
					if((dev_type == 2 && dev_param.nclu == 0x0006) || dev_type == 3 || dev_type == 10)
						dev_param.attrcmd = RS485_DATA_PARAM_TYPE;
					else
						dev_param.attrcmd = RS485_DATA_PARAM_TYPE2;
					
					dev_param.value = pLinkageParam->param;
					rs485_device_single_control(&dev_param);
				}
				else if(riu == 1 &&  endpoint_id == pLinkageParam->obj_id )
				{
					memset(&dev_param, 0, sizeof(struct device_param_info_struct));
					dev_param.dev_addr = dev_addr;
					dev_param.dev_type = dev_type;
					dev_param.endpoint_id = endpoint_id;
					dev_param.nclu = clu_id;
					if((dev_type == 1 && endpoint_id == 2) || dev_type == 4 || dev_type == 2 || dev_type == 10)
						dev_param.attrcmd = RS485_DATA_PARAM_TYPE;
					else
						dev_param.attrcmd = RS485_DATA_PARAM_TYPE2;
					
					dev_param.value = pLinkageParam->param;
					
					if(dev_param.nclu == 0x0008)
						zigbee_send_airchange_control(&dev_param);
					else
						zigbee_send_light_single_control(&dev_param);
					
				}
			}
			
			return 0;
		}
	}
        
        return ret;
}


int sensor_linkage_turnon_light_device(int flag, uint8_t dev_mac[], uint8_t edp_id)
{
	int ret = -1;
	int i,j;
	uint16_t dev_addr, clu_id;
	uint8_t is_can_add,endpoint_id,dev_type=0,riu;
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	app_context_t* app_context = get_app_context();
	system_context_t* mico_context = (system_context_t*)get_sys_context();
	struct device_param_info_struct dev_param;
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}
	
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		riu = app_context->userDataStore->userData.deviceinfo.device_info[i].riu;
		memset(mac, 0, sizeof(mac));
		memcpy(mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[i].mac));
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1 && !memcmp(dev_mac, mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			for(j=0; j < DEVICE_MAC_ENDPOINT; j++)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				endpoint_id = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id;
				dev_type = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].dev_type;
				clu_id = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].clu_id;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
#ifdef USER_DEBUG_
			//	device_mng(" endpoint_id:%d dev_type:%d riu:%d \n", endpoint_id, dev_type, riu);
#endif
				if((edp_id == endpoint_id ) && (endpoint_id == 4) )// || endpoint_id == 5
				{
					if(riu == 0)//485
					{
						memset(&dev_param, 0, sizeof(struct device_param_info_struct));
						dev_param.dev_addr = dev_addr;
						dev_param.dev_type = 4;
						dev_param.endpoint_id = 4;
						dev_param.nclu = 0x0006;
						dev_param.value = flag;
						dev_param.attrcmd = RS485_DATA_PARAM_TYPE;
						rs485_device_single_control(&dev_param);
						
						//mico_rtos_thread_msleep(120);
						//dev_param.endpoint_id = 5;
						//rs485_device_single_control(&dev_param);
					}
					else if(riu == 1)//zigbee
					{
						memset(&dev_param, 0, sizeof(struct device_param_info_struct));
						dev_param.dev_addr = dev_addr;
						dev_param.dev_type = 4;
						dev_param.endpoint_id = 4;
						dev_param.nclu = 0x0006;
						dev_param.value = flag;
						dev_param.attrcmd = RS485_DATA_PARAM_TYPE;
						if(dev_param.nclu == 0x0008)
							zigbee_send_airchange_control(&dev_param);
						else
							zigbee_send_light_single_control(&dev_param);
						
						//mico_rtos_thread_msleep(120);
						//dev_param.endpoint_id = 5;
						//if(dev_param.nclu == 0x0008)
						//	zigbee_send_airchange_control(&dev_param);
						//else
						//	zigbee_send_light_single_control(&dev_param);
					}
					return 0;
				}
				else
				{
					if(riu == 0)//485
					{
						if(endpoint_id == edp_id )
						{
							memset(&dev_param, 0, sizeof(struct device_param_info_struct));
							dev_param.dev_addr = dev_addr;
							dev_param.dev_type = dev_type;
							dev_param.endpoint_id = endpoint_id;
							dev_param.nclu = clu_id;
							if((dev_type == 1 && endpoint_id == 2) || dev_type == 4 || dev_type == 2|| dev_type == 10)
								dev_param.attrcmd = RS485_DATA_PARAM_TYPE;
							else
								dev_param.attrcmd = RS485_DATA_PARAM_TYPE2;
							
							dev_param.value = flag;
							rs485_device_single_control(&dev_param);
						}
					}
					else if(riu == 1)//zigbee
					{
						if( endpoint_id == edp_id )
						{
							memset(&dev_param, 0, sizeof(struct device_param_info_struct));
							dev_param.dev_addr = dev_addr;
							dev_param.dev_type = dev_type;
							dev_param.endpoint_id = endpoint_id;
							dev_param.nclu = clu_id;
							if((dev_type == 1 && endpoint_id == 2) || dev_type == 4 || dev_type == 2|| dev_type == 10)
								dev_param.attrcmd = RS485_DATA_PARAM_TYPE;
							else
								dev_param.attrcmd = RS485_DATA_PARAM_TYPE2;

							device_mng("dev_addr=%04X dev_param.nclu=%04x flag=%d endpoint_id=%d ", dev_param.dev_addr, dev_param.nclu, flag, endpoint_id);
							dev_param.value = flag;
							if(dev_param.nclu == 0x0008)
								zigbee_send_airchange_control(&dev_param);
							else
								zigbee_send_light_single_control(&dev_param);
							
						}
					}
				}
			}
			
			return 0;
		}
	}
        
        return ret;
}


int get_device_param_info(struct device_param_info_struct *pDeviceInfo)
{
	int ret = -1;
	int i,j;
	uint8_t is_can_add;
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	app_context_t* app_context = get_app_context();
	

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		memset(mac, 0, sizeof(mac));
		memcpy(mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[i].mac));
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//HexDump(mac, sizeof(mac));
		if(is_can_add == 1 && !memcmp(pDeviceInfo->mac, mac, sizeof(pDeviceInfo->mac)))
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			pDeviceInfo->dev_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
			pDeviceInfo->riu = app_context->userDataStore->userData.deviceinfo.device_info[i].riu;
			pDeviceInfo->devclass = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
			
			for(j=0; j < DEVICE_MAC_ENDPOINT; j++)
			{
				device_mng("pDeviceInfo->endpoint_id:%d   endpoint_id:%d \n", pDeviceInfo->endpoint_id, app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id);
				if(app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id == pDeviceInfo->endpoint_id)
				{
					device_mng("dev_type=%d ",app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].dev_type);
					pDeviceInfo->dev_type = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].dev_type;
					pDeviceInfo->nclu = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].clu_id;
					break;
				}
			}
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
			return 0;
		}
	}

	return ret;
}

void clear_all_device_recode_inflash() //cli test
{
	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		
	}
	else
	{
		device_mng("******************* clear_all_device_recode_inflash ,len:%d**************************\n",sizeof(user_data_in_flash_t));
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);	
			memset((void *)&(app_context->userDataStore->userData),0, sizeof(user_data_in_flash_t));
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		}
		user_data_store_update();
	}
}

void  print_device_infomation()
{
#if USER_DEBUG_
	app_context_t* app_context = get_app_context();
	int i;
	uint8_t id;
	uint16_t net_addr;
	uint32_t dev_class;
	uint8_t  is_used;  
	uint8_t dev_onoffline; 
	uint8_t riu;
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT]={0};

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		
	}
	else
	{
		printf("******************* device list **************************\n");
		for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			is_used = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
			id = app_context->userDataStore->userData.deviceinfo.device_info[i].id;
			net_addr= app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
			dev_class = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
			dev_onoffline = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline;
			riu = app_context->userDataStore->userData.deviceinfo.device_info[i].riu;
			memcpy(mac,app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

			
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			#ifdef USER_DEBUG_
			printf("%d<==> id:%d  net_addr:0x%04x  dev_class:0x%08x  is_used:%d  dev_onoffline:%d   riu:%d",i, id,net_addr,dev_class,is_used,dev_onoffline,riu);
			printf("mac:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],mac[6],mac[7],mac[8],mac[9],mac[10],mac[11]);		
			#endif
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
		}
		
	}
#endif
}

/*
	Description:  this device exist return 1, else return 0.
*/
int checkDeviceExist(uint8_t mac[])
{
	int ret = 0;
	int i;
	uint8_t is_can_add;
	//uint16_t nwkaddr=0;
	uint8_t dev_mac[32];
	
	app_context_t* app_context = get_app_context();
	

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1 && !memcmp(dev_mac, mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			return 1;
		}
	}

	return ret;
}

int getGatewayRs485Device(uint8_t mac[])
{
	int ret = 0;
	int i;
	uint8_t is_can_add;
	uint32_t dev_class;
	
	app_context_t* app_context = get_app_context();
	

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_class = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;

		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1 && dev_class == 299)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			memcpy(mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
			return 1;
		}
	}

	return ret;
}


/*
	Description: 
	Param: mac[]  size must > DEVICE_MAC_ADDRESS_LENGHT bytes.
*/
int get_device_mac(uint16_t nwk_addr, int8_t mac[])
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr=0;
	
	app_context_t* app_context = get_app_context();
	

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//device_mng("i=%d  nwkaddr=%04x nwk_addr=%04x",i, nwkaddr, nwk_addr);
		if(is_can_add == 1 && nwkaddr == nwk_addr)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			memcpy(mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			return 0;
		}
	}

	return ret;
}

int initDeviceListOnlineStatus()
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 0;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		}
	}
	
	ret =  user_data_store_update(); //no use
	return ret;
}


void rs485_request_subdevice_state()
{
	device_mng("rs485_request_subdevice_state.\n");
	uint16_t dev_addr;
	uint8_t is_can_add;
	uint8_t riu;
	int i;
	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ;
	}
	
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		riu = app_context->userDataStore->userData.deviceinfo.device_info[i].riu;
		dev_addr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1 && riu == 0 && dev_addr != 0xFFFF )
		{
			rs485_request_get_device(dev_addr);
		}
	}
}


int rs485_update_device_state(uint16_t clu_id, uint16_t dev_addr, uint8_t endpoint, int32_t value)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t dev_class;
	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_class = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//device_mng("update state::::  is_can_add=%d dev_class=%04x clu_id=%04x endpoint=%d  src=%04x  value=%d",is_can_add, dev_class, clu_id, endpoint,dev_addr, value);
		if(is_can_add == 1 && dev_class == 299) //nwkaddr == dev_addr, dev_addr =0xfffff
		{
			uint8_t chnl = endpoint;
			if(chnl > 0)chnl -= 1;
			if(chnl >= DEVICE_MAC_ENDPOINT){
				device_mng("endpoint id is out of range.");
				return ret;
			}
			
			device_mng("chnl:%d, attri_value=%d\n",chnl,  value);
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 1;
			if(clu_id== 0x0008 && chnl == 2)
			{
				if(value != 0)
				{
					// on level
					app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value &= 0x0000FFFF; 
					app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value  |= value<<16;
				}

				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value &= 0xFFFF0000; 
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value  |= value;
				
				//device_mng("endpoint.cur_value:%d, %08x",value,  app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value);
			}
			else if(clu_id== 0x0006 && chnl == 2)
			{
				if(value == 0)
				{
					//current level
					app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value &= 0xFFFF0000; 
					//app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value  |=value;
				}
				else
				{
					uint32_t onLevel = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value & 0xFFFF0000;
					onLevel >>= 16;
					app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value &= 0xFFFF0000; 
					app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value  |= onLevel;
				}
				//device_mng("endpoint.cur_value:%d, %08x",value,  app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value);
			}
			else
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value  = value;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

			return ret;
		}
	}

	return ret;
}

int rs485_sensor_device_onoffline_update_flag(uint16_t nwk_addr, uint8_t flag)
{
	int ret = -1;
	int i;
	//uint8_t is_can_add;
	uint16_t nwkaddr=0;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		//is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

#ifdef USER_DEBUG_
		//device_mng("device_mng=%04x nwk_addr=%04x flag=%d ",nwkaddr, nwk_addr, flag);
#endif

		if(nwkaddr == nwk_addr)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			if(flag == 2){
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr = 0;
				memset(&(app_context->userDataStore->userData.deviceinfo.sensor_info[i]), 0, sizeof(sensor_info_list_t));
			}
			else{
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_onoffline = flag;
			}
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
			
			device_mng("update sensor_device_onoffline info \n");
			ret =  user_data_store_update();
			return ret;
		}
	}

	return ret;
}

int device_onoffline_update_flag(uint16_t nwk_addr, uint8_t flag)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr=0;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

#ifdef USER_DEBUG_
		//device_mng("device_mng=%04x nwk_addr=%04x flag=%d ",nwkaddr, nwk_addr, flag);
#endif

		if(is_can_add == 1 && nwkaddr == nwk_addr)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			if(flag == 2)
				app_context->userDataStore->userData.deviceinfo.device_info[i].is_used = 0;

			app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = flag;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
			device_mng("update device_onoffline info \n");
			ret =  user_data_store_update();
			return ret;
		}
	}

	return ret;
}

int panel_device_logout_update_flag(uint16_t nwk_addr)
{
	int ret = -1;
#if 0
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr=0;
	
	app_context_t* app_context = get_app_context();
	

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < PANEL_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->panel_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.panel_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.panel_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->userData.deviceinfo.panel_info_mutex);

		//device_mng("is_can_add=%d device_mng=%04x nwk_addr=%04x",is_can_add, nwkaddr, nwk_addr);
		if(is_can_add == 1 && nwkaddr == nwk_addr)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->userData.deviceinfo.panel_info_mutex);
			app_context->userDataStore->userData.deviceinfo.panel_info[i].dev_onoffline = 0;
			app_context->userDataStore->userData.deviceinfo.panel_info[i].is_used = 0;
			memset(&(app_context->userDataStore->userData.deviceinfo.panel_info[i]), 0, sizeof(device_info_list_t));
			mico_rtos_unlock_mutex(&app_context->panel_info_mutex);
			device_mng("update device info \n");
			ret =  mico_system_context_update( mico_system_context_get( ) );
			return ret;
		}
	}
#endif
	return ret;
}

int device_logout_update_flag(uint16_t nwk_addr)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr=0;
	uint8_t dev_mac[32];
	
	app_context_t* app_context = get_app_context();
	

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		device_mng("is_can_add=%d device_mng=%04x nwk_addr=%04x",is_can_add, nwkaddr, nwk_addr);
		if(is_can_add == 1 && nwkaddr == nwk_addr)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 0;
			app_context->userDataStore->userData.deviceinfo.device_info[i].is_used = 0;
			memset(dev_mac, 0, sizeof(dev_mac));
			memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
			memset(&(app_context->userDataStore->userData.deviceinfo.device_info[i]), 0, sizeof(device_info_list_t));
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			device_mng("device logout:%d\n", i);

			removeInvalidTimerItem(dev_mac);
			ret =  user_data_store_update();
			
			return ret;
		}
	}

	panel_device_logout_update_flag(nwkaddr);

	return ret;
}

int update_device_state(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr;
	DEV_CTRL_ENDPOINT_STATE_T *pDepStateInfo;
	app_context_t* app_context = get_app_context();
	//uint16_t device_type;
	
	if(NULL == pDevInfo)
	{
		device_mng("pDevInfo is NULL.\n");
		return ret;
	}

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	pDepStateInfo = (DEV_CTRL_ENDPOINT_STATE_T *)(pDevInfo->payload);

#ifdef USER_DEBUG_
	//device_mng("src:%x sep:%d attri_id:%d msgflag:%d pDepStateInfo->state:%02X attri_value:%d  ", pDevInfo->src, pDevInfo->sep, pDepStateInfo->attri_id, pDepStateInfo->msgflag, pDepStateInfo->state, pDepStateInfo->attri_value );
#endif

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1 && nwkaddr == pDevInfo->src && pDepStateInfo->msgflag == ZIGBEE_RESP_EVENT_FLAG_A &&
			pDepStateInfo->attri_id == ZIGBEE_ATTRI_ID_0X0000)
		{
			uint8_t chnl = pDevInfo->sep;
			if(chnl > 0)chnl -= 1;
			if(chnl >= DEVICE_MAC_ENDPOINT){
				device_mng("endpoint id is out of range.");
				return ret;
			}
			
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			//device_type = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
			app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 1;
			app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[chnl].cur_value  = pDepStateInfo->attri_value;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			ret = 0;
			return ret;
		}
	}

	return ret;
}

void integrade_kitchen_device_report_attribute_state(app_context_t* app_context, uint32_t device_type, int i, ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].dev_type = 1;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].endpoint_id = 8;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].clu_id = 0x0008;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].cur_value = 0;
	mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

	mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].dev_type = 1;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].clu_id = 0x0006;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].endpoint_id = 9;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].cur_value = 0;
	mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
					

	mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].dev_type = 4;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].endpoint_id = 4;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].clu_id = 0x0006;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].cur_value = 0;
	mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

	mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].dev_type = 4;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].endpoint_id = 5;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].clu_id = 0x0006;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].cur_value = 0;
	mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

	mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].dev_type = 8;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].endpoint_id = 6;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].clu_id = 0x0008;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].cur_value = 0;
	mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);


	mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].dev_type = RS485_TYPE_BACKUSE;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].endpoint_id = 7;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].clu_id = 0x0006;
	app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].cur_value = 0;
	mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

}

void integrade_device_report_attribute_state(app_context_t* app_context, uint32_t device_type, int i, ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int j;
	uint32_t type;

	for(j=0; j < DEVICE_MAC_ENDPOINT; j++)
	{
		type = device_type & (USER_NUMBER_VAL_1 << j);
	
		switch(type)
		{
			case 1:
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].dev_type = type;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].endpoint_id = 1;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);


				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].dev_type = type;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].endpoint_id = 2;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
								
			}
			break;
			case 2:
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[2].dev_type = type;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[2].endpoint_id = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[2].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[2].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

			}
			break;
			case 4:
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].dev_type = type;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].endpoint_id = pDevInfo->sep;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].dev_type = type;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].endpoint_id = 5;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			}
			break;
			case 8:
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].dev_type = type;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].endpoint_id = 6;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

			}	
			break;
		}
	}	

	if(15 == device_type)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].dev_type = RS485_TYPE_BACKUSE;
		app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].endpoint_id = 7;
		app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].clu_id = 0x0006;
		app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].cur_value = 0;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);	
	}
	
}

void light_device_report_attribute_state(app_context_t* app_context, uint32_t device_type, int i, ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	uint8_t index = pDevInfo->sep;
	index -= 1;
	
	switch(device_type)
	{
		case ZB_TYPE_SINGLE_LIGHT:
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[index].dev_type = device_type;
			app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[index].endpoint_id = pDevInfo->sep;
			app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[index].clu_id = 0x0006;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		}
		break;
	}

}

int kitchen_panel_device_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr;
	uint32_t device_type, dev_class;
	uint8_t dev_mac[DEVICE_MAC_ADDRESS_LENGHT];
	
	DEV_ENDPOINT_JOIN_REPORT_T *pDepStateInfo;
	PANEL_BINDING_DEVICE_INFO_T panel_dev_info;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pDevInfo)
	{
		device_mng("pDevInfo is NULL.\n");
		return ret;
	}

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	memset(&panel_dev_info, 0, sizeof(PANEL_BINDING_DEVICE_INFO_T));

	pDepStateInfo = (DEV_ENDPOINT_JOIN_REPORT_T *)(pDevInfo->payload);
	device_type = atoi((char const *)pDepStateInfo->attri_value);

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		dev_class   = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);

		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

	//	device_mng("attribute report:::: dev_class:%d device_type=%d is_can_add=%d nwkaddr=%04x pDevInfo->src=%04x pDevInfo->sep=%d",dev_class,device_type, is_can_add, nwkaddr, pDevInfo->src, pDevInfo->sep);
		if(is_can_add == 1 && nwkaddr == pDevInfo->src)
		{	
			if(dev_class == device_type)
				break;
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class = device_type;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].dev_type = 1;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].endpoint_id = 1;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].dev_type = 1;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].endpoint_id = 2;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
								

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].dev_type = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].endpoint_id = 4;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[3].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].dev_type = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].endpoint_id = 5;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[4].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].dev_type = 4;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].endpoint_id = 6;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[5].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);


				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].dev_type = RS485_TYPE_BACKUSE;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].endpoint_id = 7;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[6].cur_value = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[7].dev_type = 0;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[7].endpoint_id = 8;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[8].dev_type = 0;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[8].endpoint_id = 9;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				device_mng("update kitchen_panel device info \n");
				updateKitchenSensorDeviceNwk(dev_mac, nwkaddr);
				return 0;
			}
		}
	}

	return ret;
}

int scenepanel_device_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr;
	uint32_t device_type, dev_class;
	DEV_ENDPOINT_JOIN_REPORT_T *pDepStateInfo;
	PANEL_BINDING_DEVICE_INFO_T panel_dev_info;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pDevInfo)
	{
		device_mng("pDevInfo is NULL.\n");
		return ret;
	}

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	memset(&panel_dev_info, 0, sizeof(PANEL_BINDING_DEVICE_INFO_T));

	pDepStateInfo = (DEV_ENDPOINT_JOIN_REPORT_T *)(pDevInfo->payload);
	device_type = atoi((char const *)pDepStateInfo->attri_value);

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		dev_class   = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//device_mng("attribute report::::dev_class=%d  device_type=%d is_can_add=%d nwkaddr=%04x pDevInfo->src=%04x pDevInfo->sep=%d",dev_class,device_type, is_can_add, nwkaddr, pDevInfo->src, pDevInfo->sep);
		if(is_can_add == 1 && nwkaddr == pDevInfo->src)
		{			
			if(dev_class == device_type)
				break;
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class = device_type;
		  		for(int j=0; j < 4; j++)
		  		{
					app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id = j+1;
					app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].dev_type = 0;
				}
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				device_mng("scenepanel update device info.\n");
				return 0;
			}
		}
	}

	return ret;
}
/*
int scenepanel_device_attribute_update(uint8_t cmd,uint16_t groupid, uint8_t sceneid)
{    
	int ret = -1;    
	int i;    
	uint8_t is_can_add;    
	uint16_t nwkaddr;    
	uint32_t device_type, dev_class;    
	
	app_context_t* app_context = get_app_context();        

	if(NULL == app_context)    
	{        
		device_mng("app_context is NULL.\n");       
		return ret;    
	}            

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)   
	{        
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);        
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;        		 
		dev_class = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);                
		device_mng("scenepanel sttribute update :dev_class=%d  is_can_add=%d nwkaddr=%04x ",dev_class,is_can_add, nwkaddr);        
		if(is_can_add == 1 && dev_class == 8)        
		{               
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);               			              
			if(cmd == 5)                    
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[sceneid].cur_value = 1;               
			else if(cmd == 7)                   
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[sceneid].cur_value = 0;                
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);                                
			device_mng("scenepanel update device info.\n");                
			return 0;            
		}        
	}    
	        
	return ret;
}
*/
int device_light_one_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr;
	uint32_t device_type,dev_class;
	DEV_ENDPOINT_JOIN_REPORT_T *pDepStateInfo;
	PANEL_BINDING_DEVICE_INFO_T panel_dev_info;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pDevInfo)
	{
		device_mng("pDevInfo is NULL.\n");
		return ret;
	}

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	memset(&panel_dev_info, 0, sizeof(PANEL_BINDING_DEVICE_INFO_T));

	pDepStateInfo = (DEV_ENDPOINT_JOIN_REPORT_T *)(pDevInfo->payload);
	device_type = atoi((char const *)pDepStateInfo->attri_value);

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		dev_class   = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;

		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//device_mng("attribute report::::dev_class=%d device_type=%d is_can_add=%d nwkaddr=%04x pDevInfo->src=%04x pDevInfo->sep=%d",dev_class,device_type, is_can_add, nwkaddr, pDevInfo->src, pDevInfo->sep);
		if(is_can_add == 1 && nwkaddr == pDevInfo->src)
		{
			if(dev_class == device_type)
				break;
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class = device_type;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].dev_type = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].endpoint_id = 1;	
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
				
				device_mng("light_one update device info \n");
				return 0;
			}
		}
	}

	return ret;
}


int device_light_two_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr;
	uint32_t device_type, dev_class;
	DEV_ENDPOINT_JOIN_REPORT_T *pDepStateInfo;
	PANEL_BINDING_DEVICE_INFO_T panel_dev_info;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pDevInfo)
	{
		device_mng("pDevInfo is NULL.\n");
		return ret;
	}

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	memset(&panel_dev_info, 0, sizeof(PANEL_BINDING_DEVICE_INFO_T));

	pDepStateInfo = (DEV_ENDPOINT_JOIN_REPORT_T *)(pDevInfo->payload);
	device_type = atoi((char const *)pDepStateInfo->attri_value);

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		dev_class   = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//device_mng("attribute report::::dev_class=%d device_type=%d is_can_add=%d nwkaddr=%04x pDevInfo->src=%04x pDevInfo->sep=%d",dev_class,device_type, is_can_add, nwkaddr, pDevInfo->src, pDevInfo->sep);
		if(is_can_add == 1 && nwkaddr == pDevInfo->src)
		{
			if(dev_class == device_type)
				break;
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class = device_type;

				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].dev_type = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].endpoint_id = 1;
				
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].dev_type = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].endpoint_id = 2;
										
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
				
				device_mng("light_two update device info \n");
				return 0;
			}
		}
	}

	return ret;
}

int device_light_three_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr;
	uint32_t device_type, dev_class;
	DEV_ENDPOINT_JOIN_REPORT_T *pDepStateInfo;
	PANEL_BINDING_DEVICE_INFO_T panel_dev_info;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pDevInfo)
	{
		device_mng("pDevInfo is NULL.\n");
		return ret;
	}

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	memset(&panel_dev_info, 0, sizeof(PANEL_BINDING_DEVICE_INFO_T));

	pDepStateInfo = (DEV_ENDPOINT_JOIN_REPORT_T *)(pDevInfo->payload);
	device_type = atoi((char const *)pDepStateInfo->attri_value);

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr     = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		dev_class   = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//device_mng("attribute report::::dev_class=%d device_type=%d is_can_add=%d nwkaddr=%04x pDevInfo->src=%04x pDevInfo->sep=%d",dev_class,device_type, is_can_add, nwkaddr, pDevInfo->src, pDevInfo->sep);
		if(is_can_add == 1 && nwkaddr == pDevInfo->src)
		{
			if(dev_class == device_type)
				break;
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class = device_type;

				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].dev_type = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[0].endpoint_id = 1;
				
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].dev_type = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[1].endpoint_id = 2;
				
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[2].dev_type = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[2].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[2].endpoint_id = 3;					
			
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
				
				device_mng("light_three update device info \n");
				return 0;
			}
		}
	}

	return ret;
}

int device_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	uint16_t nwkaddr;
	uint32_t device_type;
	DEV_ENDPOINT_JOIN_REPORT_T *pDepStateInfo;
	PANEL_BINDING_DEVICE_INFO_T panel_dev_info;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pDevInfo)
	{
		device_mng("pDevInfo is NULL.\n");
		return ret;
	}

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	memset(&panel_dev_info, 0, sizeof(PANEL_BINDING_DEVICE_INFO_T));

	pDepStateInfo = (DEV_ENDPOINT_JOIN_REPORT_T *)(pDevInfo->payload);
	device_type = atoi((char const *)pDepStateInfo->attri_value);

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		nwkaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		//device_mng("attribute report:::: device_type=%d is_can_add=%d nwkaddr=%04x pDevInfo->src=%04x pDevInfo->sep=%d",device_type, is_can_add, nwkaddr, pDevInfo->src, pDevInfo->sep);
		if(is_can_add == 1 && nwkaddr == pDevInfo->src)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 1;
			app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class = device_type;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

			switch(device_type)
			{
				case ZB_TYPE_INTEGRATE:
					integrade_device_report_attribute_state(app_context, device_type, i, pDevInfo);
				break;
				case ZB_TYPE_KITCHEN_PANEL:
					integrade_kitchen_device_report_attribute_state(app_context, device_type, i, pDevInfo);
				break;
				case ZB_TYPE_LIGHT:
					light_device_report_attribute_state(app_context, device_type, i, pDevInfo);
				break;
				default:
					continue;
				break;
			}

			device_mng("attribute_update  device info \n");
			//ret =  user_data_store_update();//use timer delay save
			device_delay_save_timerStart();
			return ret;
		}
	}

	return ret;
}


//{"t":2,"ids":"D054000000130000","pay":"8154D0B2CB3F0D006F0D008E"}
//8154D0B2CB3F0D006F0D008E
int zigbee_new_device_add_report(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int ret = -1;
	int i;
	int new_add_device_index = -1;
	uint8_t is_can_add;
	uint8_t dev_mac[32+1];
	uint16_t netaddr = 0;
	ZIGBEE_DEV_JOIN_PARAM_T *pDepStateInfo = NULL;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == pDevInfo)
	{
		device_mng("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	pDepStateInfo = (ZIGBEE_DEV_JOIN_PARAM_T *)(pDevInfo->payload);

#ifdef USER_DEBUG_
	//device_mng("mac:");
	//HexDump(pDepStateInfo->mac, ZIGBEE_DEVICE_MAC_ADDRESS_LENGHT);
#endif	
	memset(dev_mac, 0, sizeof(dev_mac));
	
	if(!memcmp(pDepStateInfo->mac, dev_mac, ZIGBEE_DEVICE_MAC_ADDRESS_LENGHT))
	{
		device_mng("INVALID MAC ADDRESS.\n");
		return ret;
	}
	
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, ZIGBEE_DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

#ifdef USER_DEBUG_
		//device_mng("is_can_add=%d \n", is_can_add);
#endif

		
		if(is_can_add == 1 && !memcmp(dev_mac, pDepStateInfo->mac, ZIGBEE_DEVICE_MAC_ADDRESS_LENGHT))
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);			
			netaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			//device_mng("netaddr =%04x, pDevInfo->src=%04x \n", netaddr, pDevInfo->src);

			
			if(netaddr != pDevInfo->src)
			{
			//	device_mng("update zigbee device info i=%d \n", i);
				new_add_device_index = i;
				break;
			}
			else
			{
			//	device_mng("netaddr is same, no need update,i:%d\n", i);
				return 0;
			}
		}
		
		if(new_add_device_index == -1 && is_can_add != 1)
		{	
			new_add_device_index = i;
		}
		
	}

	if(new_add_device_index >= 0)
	{
		device_mng("new_add_device_index=%d pDevInfo->sep=%d pDevInfo->src=%04x[nwk_id=%04x] pDevInfo->clu=%04x\n", new_add_device_index, pDevInfo->sep, pDevInfo->src,pDepStateInfo->nwk_id,pDevInfo->clu);
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		memset(&app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index], 0, sizeof(device_info_list_t));
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].id = new_add_device_index;
		
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].is_used = 1;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].riu = 1;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].net_addr = pDevInfo->src;

		//HexDump(pDepStateInfo->mac, sizeof(pDepStateInfo->mac));
		memset(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, 0, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac));
		memcpy(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, pDepStateInfo->mac, sizeof(pDepStateInfo->mac));
		device_delay_save_timerStart();
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		//HexDump(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac));
		//user_data_store_update();	//use timer delay save	
		ret =  0;			
	}
	else
	{
		device_mng("No space to add new device..............!\n");
	}
	
	return ret;
}


int zigbee_init_device_chnl_params(app_context_t* app_context, uint8_t idx, uint8_t dev_type,  uint8_t *payload, int len)
{
	int j, ret = 1;
	uint8_t chnl;

	chnl_status_record_t * chnlrecord= NULL;

	if(len <=0)
	{
		for(j = 0; j < DEVICE_MAC_ENDPOINT; j++)
		{	
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);				
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[j].cur_value    = 0;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		}
		return -1;
	}



	switch(dev_type)
	{
		case ZB_TYPE_SINGLE_LIGHT:
		case ZB_TYPE_DOUBLE_LIGHT:
		case ZB_TYPE_THREE_LIGHT:

			for(j = 0; j < len; )
			{	
				chnlrecord  = (chnl_status_record_t * )(payload+j);
				chnl = chnlrecord->endpoint_id -1;
				j += sizeof(chnl_status_record_t);
				
				if(chnl < 10)
				{
					mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
					app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[chnl].dev_type    = 3;
					app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[chnl].clu_id         = 0x0006;
					app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[chnl].endpoint_id = chnlrecord->endpoint_id;	
					app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[chnl].cur_value    = chnlrecord->cur_value;
					mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
				}
			}
			
			break;
		case ZB_TYPE_SCENEPANEL:	

			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			app_context->userDataStore->userData.deviceinfo.device_info[idx].dev_class = dev_type;
	  		for(j=0; j < 4; j++)
	  		{	
				app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[j].endpoint_id = j+1;
				app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[j].dev_type = 0;
			}		
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			break;

		case ZB_TYPE_KITCHEN_PANEL:
			for(j = 0; j < len; )
			{	
				chnlrecord  = (chnl_status_record_t * )(payload+j);
				chnl = chnlrecord->endpoint_id -1;
				j += sizeof(chnl_status_record_t);
				//device_mng("end_point:0x%02x , clu_id:0x%04x attri_id:0x%04x attri_type:0x%02x  value:0x%02x", chnlrecord->endpoint_id,chnlrecord->clu_id,chnlrecord->attri_id,chnlrecord->attri_type, chnlrecord->cur_value);									

				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[chnl].cur_value    = chnlrecord->cur_value;
				app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[chnl].endpoint_id = chnlrecord->endpoint_id;
				app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[chnl].clu_id         = chnlrecord->clu_id;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

			}
			
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[0].dev_type = 1;
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[1].dev_type = 1;
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[3].dev_type = 3;
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[4].dev_type = 3;
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[5].dev_type = 4;
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[6].dev_type = RS485_TYPE_BACKUSE;
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[7].dev_type = 0;
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[8].dev_type = 0;

			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[7].endpoint_id = 8;
			app_context->userDataStore->userData.deviceinfo.device_info[idx].chnl[8].endpoint_id = 9;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
			break;
		default:
			device_mng("unkown dev_type Joined");	
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			memset(&app_context->userDataStore->userData.deviceinfo.device_info[idx], 0, sizeof(device_info_list_t));
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			ret = -1;
			break;
			
	}

#if USER_DEBUG_
	if(dev_type == ZB_TYPE_SINGLE_LIGHT)
		device_mng("ZB_TYPE_SINGLE_LIGHT Joined");
	else if(dev_type == ZB_TYPE_DOUBLE_LIGHT)
		device_mng("ZB_TYPE_DOUBLE_LIGHT Joined");
	else if(dev_type == ZB_TYPE_THREE_LIGHT)
		device_mng("ZB_TYPE_THREE_LIGHT Joined");
	else if(dev_type == ZB_TYPE_SCENEPANEL)
		device_mng("ZB_TYPE_SCENEPANEL Joined");
	else if(dev_type == ZB_TYPE_KITCHEN_PANEL)
		device_mng("ZB_TYPE_KITCHEN_PANEL Joined");	
	else 
		device_mng("UNKOWN  ZB_TYPE Joined");
#endif	

	return ret;
}


//{"t":2,"ids":"D054000000130000","pay":"8154D0B2CB3F0D006F0D008E"}
//8154D0B2CB3F0D006F0D008E
int zigbee_new_device_add_report2(ZB_DEV_JOINED_REQUEST_T *pDevInfo)
{
	int i, ret = -1;
	int new_add_device_index = -1;
	uint8_t is_can_add;
	uint32_t dev_class = 0;

	uint8_t dev_mac[DEVICE_MAC_ADDRESS_LENGHT];
	uint16_t netaddr = 0;
	ZIGBEE_DEV_JOIN_PARAM_T2 *pDepStateInfo = NULL;


	app_context_t* app_context = get_app_context();
	
	if(NULL == pDevInfo ||NULL == app_context)
	{
		device_mng("pDevInfo is NULL or app_context is NULL.\n");
		return ret;
	}

	pDepStateInfo = (ZIGBEE_DEV_JOIN_PARAM_T2 *)(pDevInfo->payload);

	device_mng("dev_cnt:%d,  dev_type:%d",pDepStateInfo->dev_cnt, pDepStateInfo->dev_type);
	memset(dev_mac, 0, sizeof(dev_mac));		
	if(!memcmp(pDepStateInfo->mac, dev_mac, ZIGBEE_DEVICE_MAC_ADDRESS_LENGHT) || pDepStateInfo->cmd != 1)	
	{		
		device_mng("INVALID MAC ADDRESS!!!!!!\n");		
		return ret;	
	}
	
	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);		
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;		
		memset(dev_mac, 0, sizeof(dev_mac));		
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, ZIGBEE_DEVICE_MAC_ADDRESS_LENGHT);		
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

		if(is_can_add == 1 && !memcmp(dev_mac, pDepStateInfo->mac, ZIGBEE_DEVICE_MAC_ADDRESS_LENGHT))
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			netaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;	
			dev_class = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			device_mng("netaddr =%04x, pDevInfo->src=%04x \n", netaddr, pDevInfo->src);
			if(netaddr != pDevInfo->src || dev_class != pDepStateInfo->dev_type)
			{
				device_mng("update zigbee device info i=%d \n", i);	
				new_add_device_index = i;
				break;	
			}			
			else		
			{	
				device_mng("netaddr is same, no need update,i:%d\n", i);

				zigbee_init_device_chnl_params(app_context, i, pDepStateInfo->dev_type, pDepStateInfo->endpiont_payload, pDevInfo->len - 15);
				
				return 0;			
			}
		}
		
		if(new_add_device_index == -1 && is_can_add != 1)
		{	
			new_add_device_index = i;
		}
	}

	if(new_add_device_index >= 0)
	{
		device_mng("new_add_device_index=%d  pDevInfo->src=%04x  devtype= %d ", new_add_device_index,  pDevInfo->src, pDepStateInfo->dev_type);
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		memset(&app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index], 0, sizeof(device_info_list_t));
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].id = new_add_device_index;
		
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].is_used = 1;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].riu = 1;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].net_addr = pDevInfo->src;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].dev_class = pDepStateInfo->dev_type;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].dev_onoffline = 1;

		memset(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, 0, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac));
		memcpy(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, pDepStateInfo->mac, sizeof(pDepStateInfo->mac));

		memset(dev_mac, 0, sizeof(dev_mac));		
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);		
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		ret = zigbee_init_device_chnl_params(app_context,new_add_device_index, pDepStateInfo->dev_type, pDepStateInfo->endpiont_payload,pDevInfo->len - 15);
		updateKitchenSensorDeviceNwk(dev_mac, pDevInfo->src);
		if(ret)
			device_delay_save_timerStart();
		ret =  0;			
	}
	else
	{
		device_mng("No space to add new device..............!\n");
	}
	
	return ret;
}


static int  rs485_add_device2list(uint16_t dev_addr, uint16_t devType, uint8_t rs485dev_mac[])
{
	int ret=-1;
	int i;//,j;
	int new_add_device_index = -1;
	uint8_t is_can_add;
	uint8_t dev_mac[32+1];
	uint8_t  chnl_index = 0;
       uint16_t netaddr = 0;
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}


	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

#ifdef USER_DEBUG_
		device_mng("is_can_add=%d \n", is_can_add);
#endif

		
		if(is_can_add == 1 && !memcmp(dev_mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			netaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			device_mng("netaddr =%04x, pDevInfo->src=%04x \n", netaddr,dev_addr);
			
			if(netaddr != dev_addr)
			{
				device_mng("update 485 device info i=%d \n", i);
				new_add_device_index = i;
				break;
			}
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 1;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				device_mng("netaddr is same, no need update,i:%d\n", i);
				return 0;
			}
		}
				
		if(new_add_device_index == -1 && is_can_add != 1)
		{	
			new_add_device_index = i;
		}

	}
	
	device_mng("tmp_devicetype=%d new_add_device_index=%d \n", devType, new_add_device_index);

	if(new_add_device_index >= 0)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		memset(&app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index], 0, sizeof(device_info_list_t));
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].id = new_add_device_index;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].dev_class = devType;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].is_used = 1;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].net_addr = dev_addr;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].dev_onoffline = 1;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].riu = 0;

		//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		memset(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, 0, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac));
		memcpy(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		//HexDump(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac));

		
		device_mng("devicetype=%d \n", devType);
		switch(devType)
		{
			case RS485_TYPE_WARM_WIND:
			{
				chnl_index = 0;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_WARM_WIND;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_1;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_WARM_WIND;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_2;
				device_mng("DEV_WARM_WIND joined.......................!");		
			}
			break;
			case RS485_TYPE_LIGHT_WARM:
			{
				chnl_index = 2;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_LIGHT_WARM;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].endpoint_id = ENDPOIT_3;
			}
			break;
			case RS485_TYPE_LIGHT:
			{
				chnl_index = 3;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_LIGHT;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].endpoint_id = ENDPOIT_4;

				chnl_index = 4;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0006;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_LIGHT;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].endpoint_id = ENDPOIT_5;
				
			}
			break;
			case RS485_TYPE_CHANGEAIR:
			{
				chnl_index = 5;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0008;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_CHANGEAIR;
				app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].endpoint_id = ENDPOIT_6;
			}
			break;
			default:
				device_mng("unkown device type.\n");
				break;
		}

		//ret =  user_data_store_update();//use timer delay save
		device_delay_save_timerStart();
	}
	else
	{
		device_mng("No space to add new device..............!\n");
	}
        
        return 0;
}

static int  rs485_add_integradedevice2list(uint16_t dev_addr, uint8_t devType[], uint8_t device_type_count, uint8_t rs485dev_mac[])
{
	int ret=-1;
	int i,j;
	int new_add_device_index = -1;
	uint8_t is_can_add;
	uint32_t dev_class=0;
	uint32_t devicetype;
	uint16_t netaddr = 0;
	uint8_t dev_mac[32+1];
	uint8_t  chnl_index = 0;
	
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}
	

	for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
		dev_class  = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
#ifdef USER_DEBUG_
		device_mng(" i: %d   is_can_add=%d \n", i,is_can_add);
#endif

		
		if(is_can_add == 1 && !memcmp(dev_mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			netaddr = app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			new_add_device_index = i;
			if(netaddr!= dev_addr)
			{	
				device_mng("netaddr =0x%04x, dev_addr= %0x4x\n", netaddr, dev_addr);
				device_mng("update 485 device info i=%d \n", i);
				break;
			}
			else
			{
				device_mng(" netaddr is ==, no update device info i=%d,dev_addr:%04x \n", i,dev_addr);
				return 0;
			}
		}
		else if((is_can_add == 1) && memcmp(dev_mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT) && (dev_class == 299))// new ctr 485
		{
			//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			memset((void *)&app_context->userDataStore->userData.deviceinfo.device_info[i], USER_NUMBER_VAL_0, sizeof(device_info_list_t));
			app_context->userDataStore->userData.deviceinfo.device_info[i].dev_onoffline = 1;
			app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr = dev_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			new_add_device_index = i;
			device_mng("new 485 box info i=%d \n", i);
			break;
		}
	
		if(new_add_device_index == -1 && is_can_add != 1)
		{	
			new_add_device_index = i;			
		}
		
	}
	
	device_mng("new_add_device_index=%d \n",  new_add_device_index);

	if(new_add_device_index >= 0)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
		memset(&app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index], 0, sizeof(device_info_list_t));
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].id = new_add_device_index;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].is_used = 1;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].net_addr = dev_addr;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].dev_onoffline = 1;
		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].riu = 0;

		//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		memset(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, 0, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac));
		memcpy(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		//HexDump(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac, sizeof(app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].mac));

		
		for(j=0; j < device_type_count; j++)
		{
			devicetype = devType[j];
			dev_class += devicetype;
			
			device_mng("devicetype=%d dev_class=%d \n", devicetype, dev_class);
			
			switch(devicetype)
			{
				case RS485_TYPE_WARM_WIND:
				{
					chnl_index = 0;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_WARM_WIND;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0008;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_1;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_WARM_WIND;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0008;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_2;

				}
				break;
				case RS485_TYPE_LIGHT_WARM:
				{
					chnl_index = 2;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0008;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_LIGHT_WARM;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_3;
				}
				break;
				case RS485_TYPE_LIGHT:
				{
					chnl_index = 3;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0006;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_LIGHT;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_4;

					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0006;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_LIGHT;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_5;
					
				}
				break;
				case RS485_TYPE_CHANGEAIR:
				{
					chnl_index = 5;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0008;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_CHANGEAIR;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_6;
				}
				break;
				case RS485_TYPE_BACKUSE:
				{
					chnl_index = 6;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].cur_value = 0;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].clu_id = 0x0006;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index].dev_type = RS485_TYPE_BACKUSE;
					app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[chnl_index++].endpoint_id = ENDPOIT_7;
				}
				break;
				default:
					device_mng("unkown device type.\n");
					break;
			}
		}

		app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].dev_class = 299;

		//add scene id
		uint8_t  scene_id=ENDPOIT_8;
		for(j=chnl_index, scene_id=ENDPOIT_8; j < DEVICE_MAC_ENDPOINT && scene_id <= ENDPOIT_10; j++, scene_id++)
		{
			device_mng("j: %d scene_id:%d \n", j, scene_id);
			app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[j].endpoint_id = scene_id;
			app_context->userDataStore->userData.deviceinfo.device_info[new_add_device_index].chnl[j].dev_type = 0;
		}
		
		//ret =  user_data_store_update();		//use timer delay save
		device_delay_save_timerStart();
		setMacAddress();
	}
	else
	{
		device_mng("No space to add new device..............!\n");
	}
        
        return 0;
}
//18 00 FFFF 1234 00 00 0013 00 34004C0011514E5636383920 01020304 00 FF
int rs485_new_device_joined_success_ack(RS485_DATA_FORMAT_T *pRs485Data)
{
	int ret = -1;
	uint16_t devicetype;
	uint16_t dev_addr;
	uint8_t device_type_count;
	uint8_t rs485dev_mac[32+1];
	
	RS485_REQUEST_JOIN_T *pDepStateInfo = NULL;
	
	app_context_t* app_context = get_app_context();
	system_context_t* mico_context = (system_context_t*)get_sys_context();

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	pDepStateInfo = (RS485_REQUEST_JOIN_T *)(pRs485Data->payload);

	device_type_count = (pRs485Data->data_len - sizeof(RS485_DATA_FORMAT_HEADER_T)-2); //00 FF
	
	devicetype = pDepStateInfo->devicetype[0];
	dev_addr = ChangeEndian16(pRs485Data->dataheader.srcAddr);

	if(dev_addr == 0xFFFF)
	{
		device_mng("invalid short address.\n");
		return ret;
	}

	memset(rs485dev_mac, 0, sizeof(rs485dev_mac));
	memcpy(rs485dev_mac, pRs485Data->dataheader.mac_addr, DEVICE_MAC_ADDRESS_LENGHT);

	device_mng("@@@@@@@@@@@@ rs485_dev_joined_request device_type_count=%d devicetype=%d dev_addr=%04x ,srcAddr=%04x \n", device_type_count, devicetype,dev_addr,pRs485Data->dataheader.srcAddr);

	if(device_type_count == 1)
	{
		devicetype = pDepStateInfo->devicetype[0];
		switch(devicetype)
		{
			case RS485_TYPE_WARM_WIND:
			{
				rs485_add_device2list(dev_addr, devicetype, rs485dev_mac);		
			}
			break;
			case RS485_TYPE_LIGHT_WARM:
			{
				rs485_add_device2list(dev_addr, devicetype, rs485dev_mac);
			}
			break;
			case RS485_TYPE_LIGHT:
			{
				rs485_add_device2list(dev_addr, devicetype, rs485dev_mac);
			}
			break;
			case RS485_TYPE_CHANGEAIR:
			{
				rs485_add_device2list(dev_addr, devicetype, rs485dev_mac);
			}
			break;
			case RS485_TYPE_SENSOR_LIGHTRAY:
			case RS485_TYPE_SENSOR_GAS:
			case RS485_TYPE_SENSOR_TEMPR_HUMITURE:
			case RS485_TYPE_SENSOR_IR:
			case RS485_TYPE_SENSOR_COMBUSTIBLE:
			case RS485_TYPE_SENSOR_SMOKE:
			{
				rs485_add_sensor_device2list(dev_addr, devicetype, rs485dev_mac);
			}
			break;
			default:
			{
				device_mng("unkown device type.\n");
			}
			break;
		}
	}
	else if(device_type_count > 1)
	{		
		if((device_type_count == CTRL_BOX_DEVICE_ENDPOINT_COUNT)||(device_type_count == 3))
		{
			rs485_add_integradedevice2list(dev_addr, pDepStateInfo->devicetype, device_type_count, rs485dev_mac);
		}
		else
		{
			rs485_sensor_add_integradedevice2list(dev_addr, pDepStateInfo->devicetype, device_type_count, rs485dev_mac);
		}
	}
	else
	{
		device_mng("unkown device type.\n");
	}
	
	return ret;
}

//102E FFFF 0001 00 00 0013 00 0F0000000000 00
//18 00 FFFF 0000 00 00 0000 02 34004C0011514E5636383920 01020304 00 1234 FF
int rs485_new_device_add_report(RS485_DATA_FORMAT_T *pRs485Data)
{
	int ret = -1;
	uint16_t devicetype;
	uint16_t dev_addr;
	uint8_t device_type_count;
	uint8_t rs485dev_mac[32+1];
	uint8_t cmdstatus = 1;
	
	RS485_REQUEST_JOIN_T *pDepStateInfo = NULL;
	
	app_context_t* app_context = get_app_context();
	system_context_t* mico_context = (system_context_t*)get_sys_context();

	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return ret;
	}

	pDepStateInfo = (RS485_REQUEST_JOIN_T *)(pRs485Data->payload);

	device_type_count = (pRs485Data->data_len - sizeof(RS485_DATA_FORMAT_HEADER_T)-4);// 00 1234 FF
	
	devicetype = pDepStateInfo->devicetype[0];
	dev_addr = ChangeEndian16(pRs485Data->dataheader.srcAddr);

	if(dev_addr == 0xFFFF)
	{
		device_mng("invalid short address.\n");
		return ret;
	}
	else if(dev_addr == 0x0000)
	{
		cmdstatus = pRs485Data->payload[device_type_count];
		if(cmdstatus==0)
		{
			dev_addr = ((pRs485Data->payload[device_type_count+1]<<8) |pRs485Data->payload[device_type_count+2]);
			device_mng("dev_addr:%04x\n", dev_addr);
		}
		else
			return ret;
	}
	
	memset(rs485dev_mac, 0, sizeof(rs485dev_mac));
	memcpy(rs485dev_mac, pRs485Data->dataheader.mac_addr, DEVICE_MAC_ADDRESS_LENGHT);

	device_mng("@@@@@@@@@@@@ rs485_dev_joined_request device_type_count=%d devicetype=%d dev_addr=0x%04x\n", device_type_count, devicetype,dev_addr);

	if(device_type_count == 1)
	{
		devicetype = pDepStateInfo->devicetype[0];
		switch(devicetype)
		{
			case RS485_TYPE_WARM_WIND:
			{
				rs485_add_device2list(dev_addr, devicetype, rs485dev_mac);		
			}
			break;
			case RS485_TYPE_LIGHT_WARM:
			{
				rs485_add_device2list(dev_addr, devicetype, rs485dev_mac);
			}
			break;
			case RS485_TYPE_LIGHT:
			{
				rs485_add_device2list(dev_addr, devicetype, rs485dev_mac);
			}
			break;
			case RS485_TYPE_CHANGEAIR:
			{
				rs485_add_device2list(dev_addr, devicetype, rs485dev_mac);
			}
			break;
			case RS485_TYPE_SENSOR_LIGHTRAY:
			case RS485_TYPE_SENSOR_GAS:
			case RS485_TYPE_SENSOR_TEMPR_HUMITURE:
			case RS485_TYPE_SENSOR_IR:
			case RS485_TYPE_SENSOR_COMBUSTIBLE:
			case RS485_TYPE_SENSOR_SMOKE:
			{
				rs485_add_sensor_device2list(dev_addr, devicetype, rs485dev_mac);
			}
			break;
			default:
			{
				device_mng("unkown device type.\n");
			}
			break;
		}
	}
	else if(device_type_count > 1)
	{		
		if((device_type_count == CTRL_BOX_DEVICE_ENDPOINT_COUNT)||(device_type_count == 3))
		{
			rs485_add_integradedevice2list(dev_addr, pDepStateInfo->devicetype, device_type_count, rs485dev_mac);
		}
		else
		{
			rs485_sensor_add_integradedevice2list(dev_addr, pDepStateInfo->devicetype, device_type_count, rs485dev_mac);
		}
	}
	else
	{
		device_mng("unkown device type.\n");
	}
	
	return ret;
}

int clear_all_rs485_device(void)
{
	int i, ret=0;
	int  riu;
	uint8_t is_can_add;
	uint32_t dev_class;
	char mac_str[32];


	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		
	}
	else
	{			
		device_mng("******************* clear_all_rs485_device **************************\n");
		for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			riu = app_context->userDataStore->userData.deviceinfo.device_info[i].riu;
			is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
			dev_class  = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
			memset(mac_str, 0, sizeof(mac_str));
			memcpy(mac_str, app_context->userDataStore->userData.deviceinfo.device_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
			if((is_can_add == 1)&&(0 == riu)&&(dev_class==299))
			{
				device_mng("******************* clear_item i:%d **************************\n", i);
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				memset((void *)&app_context->userDataStore->userData.deviceinfo.device_info[i], USER_NUMBER_VAL_0, sizeof(device_info_list_t));				
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);	

				removeInvalidTimerItem((uint8_t *)mac_str);
				ret = user_data_store_update();
				
				break; //only one rs485 Control  box
			}
		}
				
	}
	

	return ret;
}

int close_all_rs485_device(void)
{
	int i, j, ret=0;
	int  riu;
	uint8_t is_can_add;
	uint32_t dev_class;


	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");		
	}
	else
	{			
		device_mng("******************* close_all_rs485_device **************************\n");
		for(i=0; i < DEVICE_INFO_LIST_MAX; i++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			riu = app_context->userDataStore->userData.deviceinfo.device_info[i].riu;
			is_can_add = app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;
			dev_class  = app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
			if((is_can_add == 1)&&(0 == riu)&&(dev_class==299))
			{
				for(j= 0; j <DEVICE_MAC_ENDPOINT ;j++)
				{
					mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
					{					
						if(j !=2)
							app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].cur_value = 0;
						else
						{
							app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].cur_value &= 0xFFFF0000; //RS485_TYPE_LIGHT_WARM
						}
					}				
					mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);	
				}
				
				break; //only one rs485 Control  box
			}
		}				
	}

	return ret;
}


int clear_all_device_restore_factory_set(void)
{
	int ret=0;
	app_context_t* app_context = get_app_context();

	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		ret=-1;
	}
	else
	{
		device_mng("******************* clear_all_device_restore_factory_set ,len:%d**************************\n",sizeof(user_data_in_flash_t));
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);	
			memset((void *)&(app_context->userDataStore->userData),0, sizeof(user_data_in_flash_t));
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
		}
		ret = user_data_store_update();
	}

	return ret;
}

int save_scenelist_parame(uint16_t netaddr, uint16_t groupid, uint8_t sceneid)
{
	int i, j , bit=0, ret=0;
	uint8_t cnt,  is_used, updateflag = 0;// end_piont, attr_type, value;
	//uint16_t net_addr,  class_id, clu_id, attr_id;
	//uint16_t dev_addr;
	int idx;
	uint32_t dev_class;

	device_mng("save_scenelist_parame in, netaddr:0x%04x, groupid=%d, sceneid=%d",netaddr, groupid, sceneid);

	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context && sceneid>=SCENE_SCENEID_MAX)
	{
		device_mng("app_context is NULL.\n");
		return -1;	
	}


	idx = find_device(netaddr);
	dev_class = get_device_class(netaddr);
	
	if(idx>=0  &&  dev_class ==ZB_TYPE_SCENEPANEL )
	{
	
		memset((void *)&app_context->userDataStore->userData.scenectrl[sceneid], 0, sizeof(scene_ctrl_config_t));
		app_context->userDataStore->userData.scenectrl[sceneid].groupId = groupid;
		app_context->userDataStore->userData.scenectrl[sceneid].sceneId = sceneid;
		
		for(i = 0; i < DEVICE_INFO_LIST_MAX; i++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);	
			is_used= app_context->userDataStore->userData.deviceinfo.device_info[i].is_used;

			mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);
			
			if(is_used==1)
			{	
				updateflag =1;
				bit = 0;
				cnt = 0;
				mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
				app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].net_addr =  app_context->userDataStore->userData.deviceinfo.device_info[i].net_addr;
				app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].classid =  app_context->userDataStore->userData.deviceinfo.device_info[i].dev_class;
				mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);

				for(j = 0; j < 7; j++)
				{
					if(app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].dev_type)
					{		
						if((bit+6)>=SCENE_RECORD_PAYLOAD_LEN)
						{
							device_mng("save bit is > SCENE_RECORD_PAYLOAD_LEN ,out.\n");	
							break;
						}
						
						mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
						app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].payload[bit] = app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].endpoint_id;
						bit++;
						memcpy(&(app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].payload[bit]),  &(app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].clu_id), 2);
						bit += 4;//clu_id, attri_id,
						if(app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].clu_id==0x0008)
							app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].payload[bit] = 0x20 ;
						else if(app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].clu_id==0x0006)
							app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].payload[bit] = 0x10 ;
						
						bit++;
						memcpy(&(app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].payload[bit]),  &(app_context->userDataStore->userData.deviceinfo.device_info[i].chnl[j].cur_value), 1);
						mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);

						bit++;					
						cnt++;					
					}
				}
				mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
				app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].cnt = cnt;
				mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);
			}			
		}

	}
	else
	{
		device_mng("no find device or dev_class is not ZB_TYPE_SCENEPANEL");
		return  -1;
	}
	
	if(updateflag)
	{
		//ret = user_data_store_update();//use timer delay save
		device_delay_save_timerStart();
	}

	return ret;
}

#if 0
int devicechldata_syn_scenedata(uint16_t groupid, uint8_t sceneid)
{
	int i, j ,k, bit=0;
	
	uint8_t cnt,  is_used;// end_piont, attr_type, value;
	uint16_t net_addr1,net_addr2;//  class_id, clu_id, attr_id;
	uint8_t endpoint_id;
	
	uint8_t paybuf[SCENE_RECORD_PAYLOAD_LEN] = {0};	
	app_context_t* app_context = get_app_context();

	chnl_status_record_t * record=NULL;


	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return -1;
	}

	device_mng("devicechldata_syn_scenedata in, groupid=%d, sceneid=%d", groupid, sceneid);
	if(sceneid>=SCENE_SCENEID_MAX)
	{
		device_mng("sceneId =%d, is > SCENE_SCENEID_MAX.!", sceneid);
		return -1;	
	}
	
	for(i = 0; i < DEVICE_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
		net_addr1 = app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].net_addr;
		cnt         = app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].cnt;
		mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);
		if(cnt&&net_addr1)
		{
			for(j = 0; j < DEVICE_INFO_LIST_MAX; j++)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				net_addr2 = app_context->userDataStore->userData.deviceinfo.device_info[j].net_addr;
				is_used    = app_context->userDataStore->userData.deviceinfo.device_info[j].is_used;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);

				if(net_addr1 == net_addr2 && is_used ==1)
				{
					memcpy(paybuf, app_context->userDataStore->userData.scenectrl[sceneid].devlist[i].payload, SCENE_RECORD_PAYLOAD_LEN);
					
					for(k= 0, bit =0; (k <DEVICE_MAC_ENDPOINT)&&(bit<cnt*7) ;k++)
					{
						record = (chnl_status_record_t *)(paybuf+bit);
						//device_mng("%d ,%d, %d, %d, %d", record->endpoint_id, record->clu_id,record->attri_id,record->attri_type,record->cur_value);
						mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
						endpoint_id = app_context->userDataStore->userData.deviceinfo.device_info[j].chnl[k].endpoint_id;
						
						if(record->endpoint_id == endpoint_id)
						{	
							bit +=7 ;																		
							app_context->userDataStore->userData.deviceinfo.device_info[j].chnl[k].cur_value = record->cur_value;													
						}
						mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
					}
				}
			}
		}						
	}

	device_mng("devicechldata_syn_scenedata out");
	return 0;
}

#endif


int delet_dev_from_scenelist(uint16_t net_addr)
{
	int i, j;
	uint8_t updateflag = 0;
	uint16_t netaddr = 0;
	
	app_context_t* app_context = get_app_context();
	
	if(NULL == app_context)
	{
		device_mng("app_context is NULL.\n");
		return -1;
	}

	for(i  = 0 ; i < SCENE_SCENEID_MAX; i++)
	{		
		for(j = 0; j < DEVICE_INFO_LIST_MAX; j++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
			netaddr = app_context->userDataStore->userData.scenectrl[i].devlist[j].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);			

			if(netaddr == net_addr)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
				memset(&app_context->userDataStore->userData.scenectrl[i].devlist[j], 0 , sizeof(scene_dev_list_t));
				mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);	
				
				updateflag =  1;
				break;
			}
		}
	}

	if(updateflag){	
		//ret = user_data_store_update();	//use timer delay save
		device_delay_save_timerStart();
	}
	return 0;
}

int write_scene_info(uint16_t net_addr, uint8_t *value, int len)
{

	int  j = 0;
    	uint16_t netaddr;
	uint8_t sceneId;
	scene_write_info_t *pRecordData= NULL;
	
	app_context_t* app_context = get_app_context();

	
	if(NULL == app_context&& value != NULL)
	{
		device_mng("app_context is NULL.");
		return -1;
	}
 
	pRecordData = (scene_write_info_t *)value;
	
	sceneId = pRecordData->sceneId;
	if((sceneId>=SCENE_SCENEID_MAX)||((len-4)>SCENE_RECORD_PAYLOAD_LEN))
	{
		device_mng("sceneId =%d, is > SCENE_SCENEID_MAX. or paylen =%d >SCENE_RECORD_PAYLOAD_LEN.", sceneId, len-4);
		return -1;	
	}
	//device_mng("write_scene_info in, netaddr:0x%04x, groupid=%04x, sceneid=%d",net_addr, pRecordData->groupId, sceneId);

	//if(pRecordData->groupId==0x0001|| pRecordData->groupId == 0x0100)//group ,485:0001 zigbee:0100
	{
		for(j=0; j < DEVICE_INFO_LIST_MAX; j++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
			netaddr = app_context->userDataStore->userData.scenectrl[sceneId].devlist[j].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);

			if(netaddr == net_addr)
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->scene_info_mutex);
				memset(app_context->userDataStore->userData.scenectrl[sceneId].devlist[j].payload, 0 , SCENE_RECORD_PAYLOAD_LEN);
				
	            		memcpy(app_context->userDataStore->userData.scenectrl[sceneId].devlist[j].payload, value+4, len -4);
				device_mng("payload:");
				HexDump(app_context->userDataStore->userData.scenectrl[sceneId].devlist[j].payload, len-4);
				mico_rtos_unlock_mutex(&app_context->userDataStore->scene_info_mutex);

				device_delay_save_timerStart();

	            		break;
			}		
		}
	}
	
	return 0;

}
	


int parse_zigbee_write_scene_info(uint8_t *str)
{
        int j, ret=0;
        int nstrlen;

        short int net_addr;

       int bytesLen;
	uint8_t  buff[5]={0};
	uint8_t tmp[USER_NUMBER_VAL_128+11]={0};
	uint8_t ids_buff[USER_NUMBER_VAL_64+5]={0};
	
        for(; *str; str++)
        {

                if(*str== '{')
                {
                        j = 0;
                        continue;
                }
                else if(*str == '}')
                {
                        nstrlen = j;
                        break;
                }
				
		   if(j < (USER_NUMBER_VAL_128+10))
                	tmp[j++] = *str;
        }

     //   device_mng("str:%s",str+4);
        if(strncmp(str+4, "send",4)==0)
        {
                for(j = 0; j<4; j++)
                buff[j] =*(str+11+j);   //net_id string
        }
		
	bytesLen = nstrlen/USER_NUMBER_VAL_2;
	if(bytesLen>(USER_NUMBER_VAL_64+4))
		bytesLen = USER_NUMBER_VAL_64+4;
	
	for( j = USER_NUMBER_VAL_0; j < bytesLen ; j++ )
	{
		charchangehex( &tmp[j*USER_NUMBER_VAL_2] , &ids_buff[j] );

	}
		
	 net_addr = strtol((char const *)buff,NULL, 16);	
	 
        device_mng("buff:%s", buff);
        device_mng("tmp:%s",tmp);
        device_mng("net_addr:0x%x",net_addr);
	 write_scene_info(net_addr, ids_buff, bytesLen);
	 
       return ret;
}



