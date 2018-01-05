#include "sensormanager.h"
#include "user_config.h"
#include "system_internal.h"
#include "rs485parse.h"
#include "rs485sensorparse.h"
#include "linkagemanager.h"

#define sensor_mng(M, ...) custom_log("sensor_mng", M, ##__VA_ARGS__)

ZIGBEE_SENSOR_LINKAGE_PARAM_T gzigbee_sensor_linkage_param;

extern app_context_t*  get_app_context();
extern void HexDump(uint8_t * pdata, uint32_t data_len);
extern unsigned int charchangehex( unsigned char *data , unsigned char* value );
extern int get_device_mac(uint16_t nwk_addr, int8_t mac[]);
extern system_context_t* get_sys_context();
extern uint8_t* getMacAddress();
extern int parseServerMessage(int8_t *recvbuff, int32_t BufferLen, uint32_t device_id);

static int g_cur_zb_sensor_had_joined_flag = 0;

void zigbee_sensor_linkage_param_init()
{
	int j;
	uint8_t is_used;
		
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return;
	}
	
	memset(&gzigbee_sensor_linkage_param, 0, sizeof(ZIGBEE_SENSOR_LINKAGE_PARAM_T));
	mico_rtos_init_mutex(&gzigbee_sensor_linkage_param.sensorparam_mutex);	

	for(j=0; j < SENSOR_INFO_LIST_MAX; j++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_used = app_context->userDataStore->userData.deviceinfo.sensor_info[j].is_used;
		if(is_used == 1)
		{
			app_context->userDataStore->userData.deviceinfo.sensor_info[j].dev_offine_times=0;
		}
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
	}
	
}

static inline unsigned short ChangeEndian16Ex1(unsigned short value)
{
	unsigned short out_value = USER_NUMBER_VAL_0;
	out_value = (unsigned char)(value>>USER_NUMBER_VAL_8);
	out_value |= (value<<USER_NUMBER_VAL_8)&USER_NUMBER_VAL_0XFF00;
	return out_value;
}


/*
	Description: return current address of device in list id.
	Param: return device id in flash list,sucess return id >=0, failed return -1.
*/
int32_t getSensorDeviceId(uint16_t dev_addr)
{
	int j;
	//int i;
	int32_t id = -1;
	uint16_t addr;
	app_context_t* app_context = get_app_context();

	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return id;
	}

	if(dev_addr == 0 && dev_addr == 0xFFFF )
	{
		return id;
	}
	
	for(j=0; j < SENSOR_INFO_LIST_MAX; j++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		addr = app_context->userDataStore->userData.deviceinfo.sensor_info[j].net_addr;
		id = app_context->userDataStore->userData.deviceinfo.sensor_info[j].id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
		
		if(addr == dev_addr)
		{
			return id;
		}
	}

	return -1;
}


int32_t isExistSensorDevice(uint8_t mac[])
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	app_context_t* app_context = (app_context_t* )get_app_context();
	char dev_mac[32];

	sensor_mng("isExistSensorDevice in\r\nmac:");
	HexDump(mac, DEVICE_MAC_ADDRESS_LENGHT);

	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return ret;
	}
	
	for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

		if( is_can_add == 1 )
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			memset(dev_mac, 0, sizeof(dev_mac));			
			memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);			
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
			sensor_mng("sensorMac:");
			HexDump(dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
				
			if(!memcmp(dev_mac, mac, DEVICE_MAC_ADDRESS_LENGHT))
			{
				sensor_mng("find sensor in %d", i);
				ret = 1;
				break;
			}
		}
	}
	
	return ret;
}

int isKitchenSensorDevice(uint8_t mac[])
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	app_context_t* app_context = (app_context_t* )get_app_context();
	char dev_mac[32];
	uint32_t dev_class;
	
	sensor_mng("isKitchenSensorDevice in\r\nmac:");
	HexDump(mac, DEVICE_MAC_ADDRESS_LENGHT);

	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return ret;
	}
	
	for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		dev_class = app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

		if( is_can_add == 1 )
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			memset(dev_mac, 0, sizeof(dev_mac));			
			memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);			
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
			sensor_mng("sensorMac:");
			HexDump(dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
				
			if(!memcmp(dev_mac, mac, DEVICE_MAC_ADDRESS_LENGHT) && dev_class == 300)
			{
				sensor_mng("find sensor in %d", i);
				ret = 1;
				break;
			}
		}
	}
	
	return ret;
}

int updateKitchenSensorDeviceNwk( uint8_t dev_mac[], uint16_t nwkaddr)
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	app_context_t* app_context = (app_context_t* )get_app_context();
	char sensor_mac[32];

	sensor_mng("updateKitchenSensorDeviceNwk in\r\ndev_mac:");
	HexDump(dev_mac, DEVICE_MAC_ADDRESS_LENGHT);

	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return ret;
	}
	
	for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

		if( is_can_add == 1 )
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			memset(sensor_mac, 0, sizeof(dev_mac));			
			memcpy(sensor_mac, app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);			
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
			sensor_mng("sensorMac:");
			HexDump(sensor_mac, DEVICE_MAC_ADDRESS_LENGHT);
			dev_mac[9]=0x12;
			dev_mac[10]=0x13;
			dev_mac[11]=0x14;	
			if(!memcmp(dev_mac, sensor_mac, DEVICE_MAC_ADDRESS_LENGHT))
			{
				sensor_mng("updateKitchenSensorDeviceNwk , %04x-->%04x\n", app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr, nwkaddr);
				mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr = nwkaddr;
				mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
				ret = 1;
				break;
			}
		}
	}	

	return ret;
}



int removeSensorDevice( char mac_str2[], char nwk_str2[] )
{
	int ret = -1;
	int i;
	uint8_t is_can_add;
	app_context_t* app_context = (app_context_t* )get_app_context();
	char mac_str[32];
	char dev_mac[32];
	char nwk_str[16];
	uint32_t dev_class;
	
	sensor_mng("removeSensorDevice in");

	for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		dev_class = app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_class;
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

		if( is_can_add == 1 )
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			memset(mac_str, 0, sizeof(mac_str));
			memset(dev_mac, 0, sizeof(dev_mac));
			
			memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
			sprintf(mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[0],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[1],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[2],
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[3],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[4],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[5],
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[6],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[7],
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[8],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[9],
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[10],app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac[11] );
			memset(nwk_str, 0, sizeof(nwk_str));
			sprintf(nwk_str, "%04X", app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr);
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

			//sensor_mng("mac_str mac:%s",mac_str);
			
			//sensor_mng("nwk addr:%s", nwk_str);
			
			if(!memcmp(mac_str, mac_str2, sizeof(mac_str)) || !memcmp(nwk_str2, nwk_str, sizeof(nwk_str)))
			{
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used = 0;
				memset(&app_context->userDataStore->userData.deviceinfo.sensor_info[i], 0, sizeof(sensor_info_list_t));
				ret = 0;
				linkage_deleteFixedMacItem(dev_mac);

				if(dev_class == 300)
					g_cur_zb_sensor_had_joined_flag = 0;


				
				user_data_store_update();
				break;
			}
		}
	}	

	return ret;
}
	

void sensor_device_reset_init()
{
	int j;
	uint8_t  is_used;
	uint16_t net_addr;
	uint32_t dev_class;
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return ;
	}


	for(j=0; j < SENSOR_INFO_LIST_MAX; j++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_used = app_context->userDataStore->userData.deviceinfo.sensor_info[j].is_used;
		net_addr = app_context->userDataStore->userData.deviceinfo.sensor_info[j].net_addr;
		dev_class = app_context->userDataStore->userData.deviceinfo.sensor_info[j].dev_class;

		if(is_used == 1 && (net_addr == 0xFFFF || dev_class == 10))
		{
			memset(&(app_context->userDataStore->userData.deviceinfo.sensor_info[j]), 0, sizeof(sensor_info_list_t));
		}
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
	}

#if 0
	for(i=0; i < LINKAGE_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->userData.linkagectrl_mutex);
		memset(&(app_context->userDataStore->userData.linkagectrl[i]), 0, sizeof(linkage_dev_param_t));
		mico_rtos_unlock_mutex(&app_context->userDataStore->userData.linkagectrl_mutex);
	}
#endif
}

#if 0
static inline void sensor_update_value(app_context_t* app_context, uint8_t dev_index, int16_t value, uint8_t sensor_type)
{
	int j;
	uint8_t alloc_flag;
	uint8_t dev_type;
	
	for(j=0; j < SENSOR_CHANL_MAX; j++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->userData.deviceinfo.sensor_info_mutex);
		alloc_flag = app_context->userDataStore->userData.deviceinfo.sensor_info[dev_index].chnl[j].alloc_flag;
		dev_type = app_context->userDataStore->userData.deviceinfo.sensor_info[dev_index].chnl[j].dev_type;
		mico_rtos_unlock_mutex(&app_context->userDataStore->userData.deviceinfo.sensor_info_mutex);

		if(alloc_flag == 1 && dev_type == sensor_type)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->userData.deviceinfo.sensor_info_mutex);
			app_context->userDataStore->userData.deviceinfo.sensor_info[dev_index].chnl[j].value = value;
			mico_rtos_unlock_mutex(&app_context->userDataStore->userData.deviceinfo.sensor_info_mutex);
			sensor_mng("**********: %d \n", value);
			break;
		}
	}	
}
#endif

static void sensor_add_ir_lightray_default_linkage(uint8_t sensor_mac[], uint8_t dev_mac[], int linkage_value,  int sensor_type, int dev_endpoint, int actions_val)
{
	char linkage[512];
	uint8_t sensor_mac_str[32];
	uint8_t dev_mac_str[32];
	memset(sensor_mac_str, 0, sizeof(sensor_mac_str));
	sprintf((char *)sensor_mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", sensor_mac[0], sensor_mac[1], sensor_mac[2], sensor_mac[3], sensor_mac[4], sensor_mac[5], sensor_mac[6], sensor_mac[7], sensor_mac[8], sensor_mac[9], sensor_mac[10], sensor_mac[11]);
	memset(dev_mac_str, 0, sizeof(dev_mac_str));
	sprintf((char *)dev_mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", dev_mac[0], dev_mac[1], dev_mac[2], dev_mac[3], dev_mac[4], dev_mac[5], dev_mac[6], dev_mac[7],dev_mac[8], dev_mac[9], dev_mac[10], dev_mac[11]);

	memset(linkage, 0, sizeof(linkage));
	sprintf(linkage, "{\"cmd_id\": 15,\"CMD\": 146,\"list\": [{\"ctrl_n\": \"linkage%d\", \"mac\":\"%s\", \"status\": 1,\"env_cond\": \"&&\",\"env_paras\": [{\"mac\": \"%s\", \"sensor_type\":%d, \"cond\": \"==\",\"val\": %d },{\"mac\": \"%s\", \"sensor_type\":%d, \"cond\": \">\",\"val\": %d }],\"ctrl_type\":1,\"obj_id\": %d,\"cmd\": 1,\"value\": %d}]}", sensor_type, dev_mac_str,sensor_mac_str, sensor_type, linkage_value, sensor_mac_str, SENSOR_TYPE_BEAM, BEAM_OFFICE|BEAM_DARK<<16, dev_endpoint, actions_val);
	//sensor_mng("linkage=%s", linkage);
	parseServerMessage((int8_t *)linkage, strlen(linkage), 0);
}


static void sensor_add_default_linkage(uint8_t sensor_mac[], uint8_t dev_mac[], int linkage_value,  int sensor_type, int dev_endpoint, int actions_val)
{
	char linkage[512];
	uint8_t sensor_mac_str[32];
	uint8_t dev_mac_str[32];
	memset(sensor_mac_str, 0, sizeof(sensor_mac_str));
	sprintf((char *)sensor_mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", sensor_mac[0], sensor_mac[1], sensor_mac[2], sensor_mac[3], sensor_mac[4], sensor_mac[5], sensor_mac[6], sensor_mac[7],sensor_mac[8], sensor_mac[9], sensor_mac[10], sensor_mac[11]);
	memset(dev_mac_str, 0, sizeof(dev_mac_str));
	sprintf((char *)dev_mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", dev_mac[0], dev_mac[1], dev_mac[2], dev_mac[3], dev_mac[4], dev_mac[5], dev_mac[6], dev_mac[7],dev_mac[8], dev_mac[9], dev_mac[10], dev_mac[11]);

	memset(linkage, 0, sizeof(linkage));
	sprintf(linkage, "{\"cmd_id\": 15,\"CMD\": 146,\"list\": [{\"ctrl_n\": \"linkage%d\", \"mac\":\"%s\", \"status\": 1,\"env_cond\": \"&&\",\"env_paras\": [{\"mac\": \"%s\", \"sensor_type\":%d, \"cond\": \">\",\"val\": %d }],\"ctrl_type\":1,\"obj_id\": %d,\"cmd\": 1,\"value\": %d}]}", sensor_type, dev_mac_str,sensor_mac_str, sensor_type, linkage_value, dev_endpoint, actions_val);
	//sensor_mng("linkage=%s", linkage);
	parseServerMessage((int8_t *)linkage, strlen(linkage), 0);
}

#if 0
static void sensor_modify_default_linkage(uint8_t sensor_mac[], uint8_t dev_mac[], int linkage_value,  int sensor_type, int dev_endpoint, int actions_val)
{
	char linkage[512];
	uint8_t sensor_mac_str[32];
	uint8_t dev_mac_str[32];
	memset(sensor_mac_str, 0, sizeof(sensor_mac_str));
	sprintf((char *)sensor_mac_str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", sensor_mac[0], sensor_mac[1], sensor_mac[2], sensor_mac[3], sensor_mac[4], sensor_mac[5], sensor_mac[6], sensor_mac[7], sensor_mac[8], sensor_mac[9], sensor_mac[10], sensor_mac[11]);
	memset(dev_mac_str, 0, sizeof(dev_mac_str));
	sprintf((char *)dev_mac_str,   "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", dev_mac[0], dev_mac[1], dev_mac[2], dev_mac[3], dev_mac[4], dev_mac[5], dev_mac[6], dev_mac[7], dev_mac[8], dev_mac[9], dev_mac[10], dev_mac[11]);

	memset(linkage, 0, sizeof(linkage));
	sprintf(linkage, "{\"cmd_id\": 15,\"CMD\": 147,\"list\": [{\"ctrl_id\":5, \"ctrl_n\": \"linkage%d\", \"mac\":\"%s\", \"status\": 1,\"env_cond\": \"&&\",\"env_paras\": [{\"mac\": \"%s\", \"sensor_type\":%d, \"cond\": \">=\",\"val\": %d }],\"ctrl_type\":1,\"obj_id\": %d,\"cmd\": 1,\"value\": %d}]}", sensor_type, dev_mac_str, sensor_mac_str, sensor_type, linkage_value, dev_endpoint, actions_val);
	sensor_mng("linkage=%s", linkage);
	parseServerMessage((int8_t *)linkage, strlen(linkage), 0);
}
#endif
          
int  zigbee_add_sensor_device2list(uint16_t dev_addr, uint8_t rs485dev_mac[], uint8_t sensor_mac[])
{
	int ret=-1;
	int i;
	int new_add_device_index = -1;
	uint8_t is_can_add;
	uint8_t dev_mac[32+1];
	uint8_t  chnl_index = 0;
	uint16_t netaddr =0;

	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return ret;
	}

	sensor_mng("zigbee_add_sensor_device2list in.");

	for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

		sensor_mng("is_can_add=%d \n", is_can_add);


		if(is_can_add == 1 && !memcmp(dev_mac, sensor_mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			netaddr =app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
			if(netaddr != dev_addr)
			{
				sensor_mng("update zigbee sensor info i=%d \n", i);
				new_add_device_index = i;
				break;
			}
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_offine_times = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
				sensor_mng("netaddr is same, no need update,i:%d\n", i);
				return 0;
			}

		}


		if(new_add_device_index == -1 && is_can_add != 1)
		{	
			new_add_device_index = i;
		}
	}
	
	sensor_mng(" new_add_device_index=%d \n",  new_add_device_index);

	if(new_add_device_index >= 0)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].id = new_add_device_index;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_class = 300;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].is_used = 1;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].riu = 1;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_offine_times = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].net_addr = dev_addr;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_onoffline = 1;

		//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		memset(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, 0, sizeof(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac));
		memcpy(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, sensor_mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
		//HexDump(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, sizeof(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac));

		
		chnl_index = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_BEAM;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 0;

		sensor_add_ir_lightray_default_linkage(sensor_mac, rs485dev_mac, 1, SENSOR_TYPE_IR, 4, 1);
		sensor_mng("ZIGBEE_TYPE_SENSOR_LIGHTRAY joined.......................!");		

		chnl_index = 1;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_CO2;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 0;

		sensor_add_default_linkage(sensor_mac, rs485dev_mac,  CO2_HIGHT|CO2_MID<<16, SENSOR_TYPE_CO2, 6, 1);

		chnl_index = 2;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_TVOC;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 0;

		sensor_add_default_linkage(sensor_mac, rs485dev_mac, TVOC_BAD|TVOC_MID<<16, SENSOR_TYPE_TVOC, 6, 1);

		chnl_index = 3;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_TEMPR;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 0;
		
		//sensor_add_default_linkage(sensor_mac, rs485dev_mac, TEMP_WARM|TEMP_COOL<< 16, SENSOR_TYPE_TEMPR, 1 , 3);

		chnl_index = 4;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_HUMIDITY;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 0;

		sensor_add_default_linkage(sensor_mac, rs485dev_mac, HUMI_MAX|HUMI_MOIST<< 16, SENSOR_TYPE_HUMIDITY, 6, 1);


		chnl_index = 5;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_IR;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 0;

		chnl_index = 6;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_COMBUSTIBLE;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 0;

		sensor_add_default_linkage(sensor_mac, rs485dev_mac,  COMBUSTIBLE_MID|COMBUSTIBLE_NORMAL<<16, SENSOR_TYPE_COMBUSTIBLE, 6, 1);
		

		chnl_index = 7;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_SMOKE;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 0;

		sensor_add_default_linkage(sensor_mac, rs485dev_mac,  SMOKE_BAD|SMOKE_MID<<16, SENSOR_TYPE_SMOKE, 6, 1);

		//user_data_store_update(); in add linkage save
		ret =  0;
	}
	else
	{
		sensor_mng("No space to add new device..............!\n");
	}
        
        return ret;
}

int  rs485_add_sensor_device2list(uint16_t dev_addr, uint16_t devType, uint8_t rs485dev_mac[])
{
	int ret=-1;
	int i;
	int new_add_device_index = -1;
	uint8_t is_can_add;
	uint8_t dev_mac[32+1];
	uint8_t linkage_dev_mac[32+1];
	uint8_t  chnl_index = 0;
	uint16_t netaddr =0;

	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return ret;
	}


	for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

#ifdef USER_DEBUG_
		sensor_mng("is_can_add=%d \n", is_can_add);
#endif

		if(is_can_add == 1 && !memcmp(dev_mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			netaddr =app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
			if(netaddr != dev_addr)
			{
				sensor_mng("update 485 sensor info i=%d \n", i);
				new_add_device_index = i;
				break;
			}
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_offine_times = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
				sensor_mng("netaddr is same, no need update,i:%d\n", i);
				return 0;
			}
		}


		if(new_add_device_index == -1 && is_can_add != 1)
		{	
			new_add_device_index = i;		
		}
	}
	
	sensor_mng("tmp_devicetype=%d new_add_device_index=%d \n", devType, new_add_device_index);

	if(new_add_device_index >= 0)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		memset(&app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index], 0, sizeof(sensor_info_list_t));
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].id = new_add_device_index;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_class = devType;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].is_used = 1;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_offine_times = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].net_addr = dev_addr;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_onoffline = 1;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].riu = 0;

		//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		memset(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, 0, sizeof(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac));
		memcpy(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
		//HexDump(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, sizeof(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac));

		
		sensor_mng("devicetype=%d \n", devType);
		switch(devType)
		{
			case RS485_TYPE_SENSOR_LIGHTRAY:
			{
				chnl_index = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_BEAM;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

				memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
				memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
				sensor_add_ir_lightray_default_linkage(rs485dev_mac, linkage_dev_mac, 1, SENSOR_TYPE_IR, 4, 1);
				sensor_mng("RS485_TYPE_SENSOR_LIGHTRAY joined.......................!");		
			}
			break;
			case RS485_TYPE_SENSOR_GAS:
			{
				chnl_index = 1;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_CO2;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

				memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
				memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
				sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, CO2_HIGHT|CO2_MID<<16, SENSOR_TYPE_CO2, 6, 1);

				chnl_index = 2;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_TVOC;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

				sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, TVOC_BAD|TVOC_MID<<16, SENSOR_TYPE_TVOC, 6, 1);
			}
			break;
			case RS485_TYPE_SENSOR_TEMPR_HUMITURE:
			{
				memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
				memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
				chnl_index = 3;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_TEMPR;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;
			//	sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, TEMP_COOL|TEMP_COLD << 16, SENSOR_TYPE_TEMPR, 3, 1);

				chnl_index = 4;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_HUMIDITY;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;
				sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, HUMI_MAX|HUMI_MOIST<< 16, SENSOR_TYPE_HUMIDITY, 6, 1);

			}
			break;
			case RS485_TYPE_SENSOR_IR:
			{
				chnl_index = 5;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_IR;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;
			}
			break;
			case RS485_TYPE_SENSOR_COMBUSTIBLE:
			{
				chnl_index = 6;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_COMBUSTIBLE;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

				memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
				memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
				sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, COMBUSTIBLE_MID|COMBUSTIBLE_NORMAL<<16, SENSOR_TYPE_COMBUSTIBLE, 6, 1);
				
			}
			break;
			case RS485_TYPE_SENSOR_SMOKE:
			{
				chnl_index = 7;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_SMOKE;
				app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

				memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
				memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
				sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, SMOKE_BAD|SMOKE_MID<<16, SENSOR_TYPE_SMOKE, 6, 1);
			}
			break;
			
			default:
				sensor_mng("unkown device type.\n");
				break;
		}

		//ret =  user_data_store_update();//use timer delay save
		device_delay_save_timerStart();
	}
	else
	{
		sensor_mng("No space to add new device..............!\n");
	}
        
        return 0;
}

int  rs485_sensor_add_integradedevice2list(uint16_t dev_addr, uint8_t devType[], uint8_t device_type_count, uint8_t rs485dev_mac[])
{
	int ret=-1;
	int i,j;
	int new_add_device_index = -1;
	uint8_t is_can_add;
	uint32_t dev_class=0;
	uint32_t devicetype;

	uint8_t dev_mac[32+1];
	uint8_t linkage_dev_mac[32+1];
	uint8_t  chnl_index = 0;
	uint16_t netaddr =0;
	app_context_t* app_context = get_app_context();
	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return ret;
	}


	for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		is_can_add = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
		memset(dev_mac, 0, sizeof(dev_mac));
		memcpy(dev_mac, app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

#ifdef USER_DEBUG_
		sensor_mng("is_can_add=%d \n", is_can_add);
#endif


		
		if(is_can_add == 1 && !memcmp(dev_mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT))
		{
			//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
			mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
			netaddr =app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr;
			mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
			if(netaddr != dev_addr)
			{
				sensor_mng("update 485 sensor info i=%d \n", i);
				new_add_device_index = i;
				break;
			}
			else
			{
				mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_offine_times = 0;
				mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
				sensor_mng("netaddr is same, no need update,i:%d\n", i);
				return 0;
			}
		}

		if(new_add_device_index == -1 && is_can_add != 1)
		{	
			new_add_device_index = i;
		}
		
	}

	sensor_mng("new_add_device_index=%d \n",  new_add_device_index);

	if(new_add_device_index >= 0)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
		memset(&app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index], 0, sizeof(sensor_info_list_t));
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].id = new_add_device_index;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].is_used = 1;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].net_addr = dev_addr;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_onoffline = 1;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_offine_times = 0;
		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].riu =0;

		//HexDump(rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		memset(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, 0, sizeof(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac));
		memcpy(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, rs485dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
		mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
		//HexDump(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac, sizeof(app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].mac));

		
		for(j=0; j < device_type_count; j++)
		{
			devicetype = devType[j];
			dev_class += devicetype;
			
			sensor_mng("devicetype=%d dev_class=%d \n", devicetype, dev_class);

			switch(devicetype)
			{
				case RS485_TYPE_SENSOR_LIGHTRAY:
				{
					chnl_index = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_BEAM;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

					memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
					memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
					sensor_add_ir_lightray_default_linkage(rs485dev_mac, linkage_dev_mac, 1, SENSOR_TYPE_IR, 4, 1);
					sensor_mng("RS485_TYPE_SENSOR_LIGHTRAY joined.......................!");		
				}
				break;
				case RS485_TYPE_SENSOR_GAS:
				{
					chnl_index = 1;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_CO2;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

					memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
					memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
					sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, CO2_HIGHT|CO2_MID << 16, SENSOR_TYPE_CO2, 6, 1);

					chnl_index = 2;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_TVOC;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

					sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, TVOC_BAD|TVOC_MID << 16, SENSOR_TYPE_TVOC, 6, 1);
				}
				break;
				case RS485_TYPE_SENSOR_TEMPR_HUMITURE:
				{
					memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
					memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
					chnl_index = 3;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_TEMPR;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;
					//sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, TEMP_COOL|TEMP_COLD << 16, SENSOR_TYPE_TEMPR, 3, 1);


					chnl_index = 4;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_HUMIDITY;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;
					sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, HUMI_MOIST|HUMI_MAX<< 16, SENSOR_TYPE_HUMIDITY, 6, 1);
					
				}
				break;
				case RS485_TYPE_SENSOR_IR:
				{
					chnl_index = 5;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_IR;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;
				}
				break;
				case RS485_TYPE_SENSOR_COMBUSTIBLE:
				{
					chnl_index = 6;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_COMBUSTIBLE;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

					memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
					memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
					sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, COMBUSTIBLE_MID|COMBUSTIBLE_NORMAL<<16, SENSOR_TYPE_COMBUSTIBLE, 6, 1);
					
				}
				break;
				case RS485_TYPE_SENSOR_SMOKE:
				{
					chnl_index = 7;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].value = 0;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].dev_type = SENSOR_TYPE_SMOKE;
					app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].chnl[chnl_index].alloc_flag = 1;

					memset(linkage_dev_mac, 0, sizeof(linkage_dev_mac));
					memcpy(linkage_dev_mac, getMacAddress(), DEVICE_MAC_ADDRESS_LENGHT);
					sensor_add_default_linkage(rs485dev_mac, linkage_dev_mac, SMOKE_BAD|SMOKE_MID<<16, SENSOR_TYPE_SMOKE, 6, 1);
				}
				break;
				
				default:
					sensor_mng("unkown device type.\n");
					break;
			}

		}

		app_context->userDataStore->userData.deviceinfo.sensor_info[new_add_device_index].dev_class = dev_class;		
		//ret =  user_data_store_update();//use timer delay save
		device_delay_save_timerStart();
	}
	else
	{
		sensor_mng("No space to add new device..............!\n");
	}
        
        return 0;
}

int zigbee_sensor_new_device_add_report(ZB_DEV_JOINED_REQUEST_T *pDevInfo, uint8_t mac[])
{
	int ret = -1;
	int32_t device_index;
	DEV_SENSOR_REPORT_STATE_T *pDepStateInfo;
	float reall_temp;
	app_context_t* app_context = get_app_context();
	linkage_param_t  linkageparam;
	uint8_t chnl_index;
	uint8_t ir_flag;

	if(NULL == pDevInfo)
	{
		sensor_mng("pDevInfo is NULL.\n");
		return ret;
	}

	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
		return ret;
	}

	pDepStateInfo = (DEV_SENSOR_REPORT_STATE_T *)(pDevInfo->payload+3);


	device_index = getSensorDeviceId(pDevInfo->src);
	if(device_index < 0)
	{
		sensor_mng("current address device not exist.\n");
		g_cur_zb_sensor_had_joined_flag = 0;
		return -3;
	}

	if(NULL == app_context)
		app_context = get_app_context();
	


	sensor_mng("##############device_index=%d \n", device_index );

	switch(pDevInfo->clu)
	{
		case ZIGBEE_NCLU_SENSOR_ATTR_REPORT://红外感应
		{
			if(pDepStateInfo->attri_id == ZIGBEE_ATTRIBUTE_STATE)
			{
				//sensor_mng("IR value:%d ", pDepStateInfo->attri_value);

				gzigbee_sensor_linkage_param.ir_value = pDepStateInfo->attri_value;

				chnl_index = 5;
				mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].value = pDepStateInfo->attri_value;
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].alloc_flag = 1;
				mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

				memset(&linkageparam, 0, sizeof(linkage_param_t));
				linkageparam.value = pDepStateInfo->attri_value;
				linkageparam.sensor_type = SENSOR_TYPE_IR;
				linkageparam.value2 = gzigbee_sensor_linkage_param.lightrayvalue;
				linkageparam.sensor_type2 = SENSOR_TYPE_BEAM;
				memcpy(linkageparam.mac, mac, DEVICE_MAC_ADDRESS_LENGHT);
				zigbee_check_ir_lightray_linakge_ctrl_condistions(&linkageparam);

				if(1 == pDepStateInfo->attri_value)
				{
					 ir_flag = 1;
				 	zigbee_ir_lightray_timerStop();
				}
				else if(0 == pDepStateInfo->attri_value && 1 == ir_flag)
				{
					ir_flag = 0;		
					zigbee_ir_lightray_timerStart(linkageparam.mac);			
				}				
			}
		}
		break;
		case ZIGBEE_NCLU_SENSOR_BEAM_ATTR://光线感应
		{
			if(pDepStateInfo->attri_id == ZIGBEE_ATTRIBUTE_STATE)
			{
				pDepStateInfo->attri_value = ChangeEndian16Ex1(pDepStateInfo->attri_value);

				chnl_index = 0;
				mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].value = pDepStateInfo->attri_value;
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].alloc_flag = 1;
				mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
				
				gzigbee_sensor_linkage_param.lightrayvalue = pDepStateInfo->attri_value;

				memset(&linkageparam, 0, sizeof(linkage_param_t));
				linkageparam.value = gzigbee_sensor_linkage_param.ir_value;
				linkageparam.sensor_type = SENSOR_TYPE_IR;
				linkageparam.value2 = pDepStateInfo->attri_value;
				linkageparam.sensor_type2 = SENSOR_TYPE_BEAM;
				memcpy(linkageparam.mac, mac, DEVICE_MAC_ADDRESS_LENGHT);
				//sensor_mng("lightray sensor attri_id=%04x, value:%d ", pDepStateInfo->attri_id, pDepStateInfo->attri_value);
				zigbee_check_ir_lightray_linakge_ctrl_condistions(&linkageparam);
			}
		}
		break;
		case ZIGBEE_NCLU_SENSOR_TEMP_ATTR://温度
		{
			if(pDepStateInfo->attri_id == ZIGBEE_ATTRIBUTE_STATE)
			{
				pDepStateInfo->attri_value = ChangeEndian16Ex1(pDepStateInfo->attri_value);
				reall_temp = pDepStateInfo->attri_value*0.01;

				chnl_index = 3;
				mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].value = (int)reall_temp;
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].alloc_flag = 1;
				mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
					
				sensor_mng("TEMPR_SENSOR attri_id=%04x, value:%d reall_temp=%f ", pDepStateInfo->attri_id, pDepStateInfo->attri_value, reall_temp);

			}
		}
		break;
		case ZIGBEE_NCLU_SENSOR_HUMIDITY_ATTR://湿度
		{
			if(pDepStateInfo->attri_id == ZIGBEE_ATTRIBUTE_STATE)
			{
				float reall_val;
				
				pDepStateInfo->attri_value = ChangeEndian16Ex1(pDepStateInfo->attri_value);
				reall_val = pDepStateInfo->attri_value*0.01;

				chnl_index = 4;
				mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].value = (int)reall_val;
				app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].alloc_flag = 1;
				mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
				
				sensor_mng("HUMIDITY_SENSOR attri_id=%04x, value:%d    %f", pDepStateInfo->attri_id, pDepStateInfo->attri_value, reall_val);

				memset(&linkageparam, 0, sizeof(linkage_param_t));
				linkageparam.value = (int)reall_val;
				linkageparam.sensor_type = SENSOR_TYPE_HUMIDITY;
				memcpy(linkageparam.mac, mac, DEVICE_MAC_ADDRESS_LENGHT);
				zigbee_check_linakge_airchange_ctrl_condistions(&linkageparam);
			}
		}
		break;	
		case SENSOR_GAS_PILE_ID:
		{
			switch(pDepStateInfo->attri_id)
			{
				case 0:
				{
					pDepStateInfo->attri_value = ChangeEndian16Ex1(pDepStateInfo->attri_value);
					sensor_mng("gas co2 value:%d", pDepStateInfo->attri_value);

					chnl_index = 1;
					mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].value = pDepStateInfo->attri_value;
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].alloc_flag = 1;
					mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
				
					memset(&linkageparam, 0, sizeof(linkage_param_t));
					linkageparam.value = pDepStateInfo->attri_value;
					linkageparam.sensor_type = SENSOR_TYPE_CO2;
					memcpy(linkageparam.mac, mac, DEVICE_MAC_ADDRESS_LENGHT);	
					zigbee_check_linakge_airchange_ctrl_condistions(&linkageparam);
				}
				break;
				case 1:
				{
					pDepStateInfo->attri_value = ChangeEndian16Ex1(pDepStateInfo->attri_value);
					//sensor_mng("gas voc value:%d", pDepStateInfo->attri_value);

					chnl_index = 2;
					mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].value = pDepStateInfo->attri_value;
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].alloc_flag = 1;
					mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);

					memset(&linkageparam, 0, sizeof(linkage_param_t));
					linkageparam.value = pDepStateInfo->attri_value;
					linkageparam.sensor_type = SENSOR_TYPE_TVOC;
					memcpy(linkageparam.mac, mac, DEVICE_MAC_ADDRESS_LENGHT);
					zigbee_check_linakge_airchange_ctrl_condistions(&linkageparam);
				}		
				break;
				case 2:
				{
					pDepStateInfo->attri_value = ChangeEndian16Ex1(pDepStateInfo->attri_value);
					//sensor_mng("gas combustible value:%d", pDepStateInfo->attri_value);

					chnl_index = 6;
					mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].value = pDepStateInfo->attri_value;
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].alloc_flag = 1;
					mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
					
					memset(&linkageparam, 0, sizeof(linkage_param_t));
					linkageparam.value = pDepStateInfo->attri_value;
					linkageparam.sensor_type = SENSOR_TYPE_COMBUSTIBLE;
					memcpy(linkageparam.mac, mac, DEVICE_MAC_ADDRESS_LENGHT);
					zigbee_check_linakge_airchange_ctrl_condistions(&linkageparam);
				}		
				break;
				case 3:
				{
					pDepStateInfo->attri_value = ChangeEndian16Ex1(pDepStateInfo->attri_value);
					//sensor_mng("gas smoke value:%d", pDepStateInfo->attri_value);

					chnl_index = 7;
					mico_rtos_lock_mutex(&app_context->userDataStore->sensor_info_mutex);
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].dev_onoffline = 1;
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].value = pDepStateInfo->attri_value;
					app_context->userDataStore->userData.deviceinfo.sensor_info[device_index].chnl[chnl_index].alloc_flag = 1;
					mico_rtos_unlock_mutex(&app_context->userDataStore->sensor_info_mutex);
					
					memset(&linkageparam, 0, sizeof(linkage_param_t));
					linkageparam.value = pDepStateInfo->attri_value;
					linkageparam.sensor_type = SENSOR_TYPE_SMOKE;
					memcpy(linkageparam.mac, mac, DEVICE_MAC_ADDRESS_LENGHT);
					zigbee_check_linakge_airchange_ctrl_condistions(&linkageparam);
				}		
				break;
				default:
				{
					return ret;
				}	
				break;
			}
		}
		break;
		default:
		{
			return ret;
		}
		break;
	}
			
  return 0;
}


OSStatus  sensor_attribute_report(cJSON *root,  ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo)
{
	OSStatus err = kNoErr;
	cJSON *objectValue = NULL;
	int  str_len;
	int i;
	int bytesLen;
        
	uint8_t dev_mac[DEVICE_MAC_ADDRESS_LENGHT];

	unsigned char payload[64]={0};
	ZB_DEV_JOINED_REQUEST_T  msgInfo;
		
	if(NULL == root || NULL == pZclZdoMsgInfo)
	{
		err = kParamErr;
		sensor_mng("auto_find_network error.");
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
		
		for( i = 0; i < bytesLen ; i++ )
		{
			charchangehex( &payload[i*2] , &msgInfo.payload[i] );
		}
	}

	msgInfo.dep = pZclZdoMsgInfo->dep;
	msgInfo.src = ChangeEndian16Ex1(pZclZdoMsgInfo->src);
	msgInfo.pro = ChangeEndian16Ex1(pZclZdoMsgInfo->pro);
	msgInfo.clu = ChangeEndian16Ex1(pZclZdoMsgInfo->clu);
	
	memset(dev_mac, 0, sizeof(dev_mac));
	if(get_device_mac(msgInfo.src, (int8_t *)dev_mac) >= 0)
	{		
		if(g_cur_zb_sensor_had_joined_flag == 0)
		{
			uint8_t sensor_mac[32];
			memset(sensor_mac, 0, sizeof(sensor_mac));
			memcpy(sensor_mac, dev_mac, DEVICE_MAC_ADDRESS_LENGHT);
			sensor_mac[9]=0x12;
			sensor_mac[10]=0x13;
			sensor_mac[11]=0x14;
		
			if((isExistSensorDevice(sensor_mac)==1) ||(zigbee_add_sensor_device2list(msgInfo.src,  dev_mac, sensor_mac) == 0))
			{
				g_cur_zb_sensor_had_joined_flag = 1;
				zigbee_sensor_new_device_add_report(&msgInfo, dev_mac);
			}
		}
		zigbee_sensor_new_device_add_report(&msgInfo, dev_mac);
	}
		
	return err;
}


#if 0
void print_sensor_device_info()
{
	app_context_t* app_context = get_app_context();
	int i;
	uint8_t id;
	uint16_t net_addr;
	uint32_t dev_class;
	uint8_t  is_used;  
	uint8_t dev_onoffline; 
	uint8_t dev_offine_times;
	uint8_t riu;
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT]={0};

	sensor_channel_endpoit_t chnl[SENSOR_CHANL_MAX];
	if(NULL == app_context)
	{
		sensor_mng("app_context is NULL.\n");
	}
	else
	{
		sensor_mng("******************* sensor list **************************\n");
		for(i=0; i < SENSOR_INFO_LIST_MAX; i++)
		{
			mico_rtos_lock_mutex(&app_context->userDataStore->device_info_mutex);
			id = app_context->userDataStore->userData.deviceinfo.sensor_info[i].id;
			net_addr= app_context->userDataStore->userData.deviceinfo.sensor_info[i].net_addr;
			dev_class = app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_class;
			is_used = app_context->userDataStore->userData.deviceinfo.sensor_info[i].is_used;
			dev_onoffline = app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_onoffline;
			dev_offine_times = app_context->userDataStore->userData.deviceinfo.sensor_info[i].dev_offine_times;
			riu = app_context->userDataStore->userData.deviceinfo.sensor_info[i].riu;
			memcpy(mac,app_context->userDataStore->userData.deviceinfo.sensor_info[i].mac, DEVICE_MAC_ADDRESS_LENGHT);
			memcpy(chnl,app_context->userDataStore->userData.deviceinfo.sensor_info[i].chnl, SENSOR_CHANL_MAX*sizeof(sensor_channel_endpoit_t));
			
			mico_rtos_unlock_mutex(&app_context->userDataStore->device_info_mutex);
			
			sensor_mng("%d-> id:%d  net_addr:0x%04x  dev_class:0x%08x  is_used:%d  dev_onoffline:%d  dev_offline_times:%d,  riu:%d",i, id,net_addr,dev_class,is_used,dev_onoffline,dev_offine_times,riu);
			sensor_mng("mac:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],mac[6],mac[7],mac[8],mac[9],mac[10],mac[11]);		
			sensor_mng("chl:{[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],[%d,%d,%d],[%d,%d,%d]\n}",chnl[0].dev_type,chnl[0].value,chnl[0].alloc_flag,\
			chnl[1].dev_type,chnl[1].value,chnl[1].alloc_flag,chnl[2].dev_type,chnl[2].value,chnl[2].alloc_flag,chnl[3].dev_type,chnl[3].value,chnl[3].alloc_flag,chnl[4].dev_type,chnl[4].value,chnl[4].alloc_flag,\
			chnl[5].dev_type,chnl[5].value,chnl[5].alloc_flag,chnl[6].dev_type,chnl[6].value,chnl[6].alloc_flag,chnl[7].dev_type,chnl[7].value,chnl[7].alloc_flag);
			
		}

	}
}
#endif



