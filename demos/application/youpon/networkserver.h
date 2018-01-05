#ifndef __NETWORKSERVER_H__
#define __NETWORKSERVER_H__

#include "mico.h"
#include "cJSON.h"

#define GATEWAY_TYPE							0     //  0:���ߴ��� 1: ���ߴ���  2: ���߰��� 3: ���߰���
#define SYSTEM_APP_FIRMWAR_VERSION                ("YA01_YXCK_EMW3031_10_TS_170206")      

typedef enum{
	GET_GATEWAY_INFO		= 600,
	GET_SCENE_LISTS		= 99,
	GET_ALL_SUBDEVICE_LIST	= 100,
	GET_ALL_PANEL_LIST	   	 = 110,
	GET_ALL_SENSOR_LIST	 	 = 111,
	GET_SCENE_ACTIONS_LIST	 = 112,
	DELETE_DEVICE	 = 113,
	SET_PANEL_CONNECT_WITH_DEVICE	 = 122,
	SET_SCENE_ACTIONS			=123,

	TIMER_CTRL_REQUEST		=139,
	TIMER_CTRL_NEW_ADD		=141,
	TIMER_CTRL_MODIFY		=142,
	TIMER_CTRL_DELETE			=143,
	TIMER_CTRL_START			=144,
	TIMER_CTRL_STOP			=145,

	LINKAGE_NEW_ADD			=146,
	LINKAGE_MODIFY			=147,
	LINKAGE_DELETE				=148,
	LINKAGE_START				=149,
	LINKAGE_STOP				=150,
	LINKAGE_REQUEST			=151,

	NETWORK_CMD_MAX	 = 1000
}NETWORK_SERVER_COMM_CMD;


int parseServerMessage(int8_t *recvbuff, int32_t BufferLen, uint32_t device_id);

#endif

