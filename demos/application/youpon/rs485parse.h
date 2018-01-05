#ifndef _RS485PARSE_H_
#define _RS485PARSE_H_

#include "user_main.h"
#include "zigbeeparse.h"


#define 	ZB_DEST_ID_WARM_WIND	1
#define 	ZB_DEST_ID_COOL_WIND		3
#define 	ZB_DEST_ID_SWING_WIND	2
#define 	ZB_DEST_ID_LIGHT_WARM     1
#define 	ZB_DEST_ID_LIGHT_WARM_SWITCH     2
#define	ZB_DEST_ID_LIGHT1			1
#define	ZB_DEST_ID_LIGHT2			2
#define	ZB_DEST_ID_CHANGEAIR        1

typedef enum
{
    RS485_SENSOR_DEV_LIGHTRAY=0,
    RS485_SENSOR_DEV_GAS,
    RS485_SENSOR_DEV_TEMPR_HUMITURE,
    RS485_SENSOR_DEV_IR,
    RS485_SENSOR_DEV_COMBUSTIBLE,
    RS485_SENSOR_DEV_SMOKE,
    RS485_SENSOR_DEV_MAX,
} sensor_device_id_t;


typedef enum
{
	DEV_ADDR_WARMWIND=0x1001,
	DEV_ADDR_BACKUSE_DEV=0x1005,
	DEV_ADDR_LIGHTWARM=0x1010,
	DEV_ADDR_LIGHT=0x1020,
	DEV_ADDR_AIRCHANGE=0x1030,
	DEV_ADDR_INTEGRADE=0x1040,
	DEV_ADDR_SENSOR_LIGHTRAY=0x6000,
	DEV_ADDR_SENSOR_GAS=0x6010,
	DEV_ADDR_SENSOR_HUMITURE=0x6020,
	DEV_ADDR_SENSOR_IR=0x6030,
	DEV_ADDR_SENSOR_COMBUSTIBLE=0x6040,
	DEV_ADDR_SENSOR_SMOKE=0x6050,
	DEV_ADDR_SENSOR_ALL_INTEGRATE=0x6060,
	DEV_ADDR_MAX,
	
} DEVICE_ADDR_ALLOCATION_E;

typedef enum{
	ENDPOIT_1		 = 0x1,
	ENDPOIT_2	   	 = 0x2,
	ENDPOIT_3	 	 = 0x3,
	ENDPOIT_4		 = 0x4,
	ENDPOIT_5		 = 0x5,
	ENDPOIT_6		 = 0x6,
	ENDPOIT_7		 = 0x7,
	ENDPOIT_8		 = 0x8,
	ENDPOIT_9		 = 0x9,
	ENDPOIT_10		 = 0x0a,
	ENDPOIT_MAX	 = 0x99
}DEST_DEVICE_ENDPOIT_ID;


typedef enum
{
    RS485_TYPE_WARM_WIND=1,
    RS485_TYPE_LIGHT_WARM=2,
    RS485_TYPE_LIGHT=3,
    RS485_TYPE_CHANGEAIR=4,
    RS485_TYPE_BACKUSE=10,
    RS485_TYPE_INTEGRADE=299,
    RS485_TYPE_SENSOR_LIGHTRAY=13,
    RS485_TYPE_SENSOR_GAS=14,
    RS485_TYPE_SENSOR_TEMPR_HUMITURE=15,
    RS485_TYPE_SENSOR_IR=16,
    RS485_TYPE_SENSOR_IR_LIGHTRAY=0x90,
    RS485_TYPE_SENSOR_COMBUSTIBLE=17,
    RS485_TYPE_SENSOR_SMOKE=18,
    RS485_TYPE_SENSOR_ALL_INTEGRADE=0x00f0,
    RS485_TYPE_DEVICE_TYPE_MAX
} RS485_DEVICE_TYPE_E;

#pragma pack(1)
typedef struct _RS485_REQUEST_JOIN_MSG_S
{
	unsigned char devicetype[6];
	unsigned char joinRequestFlag[6];
	unsigned char state;
	unsigned short devAddr;
}RS485_REQUEST_JOIN_T;

typedef struct _RS485_CTRL_STATE_RESP_S
{
	unsigned char attrState;
	unsigned short attrId;
	unsigned char attrType;
	unsigned char value;
}RS485_CTRL_STATE_RESP_T;

typedef struct _RS485_SET_STATE
{
	unsigned short attrId;
	unsigned char attrType;
	unsigned char value;
}RS485_SET_STATE;

typedef struct _RS485_DEV_SENSOR_REQ_STATE_S
{
	unsigned char endpoint;
	unsigned short pileId;
	unsigned short attrId;
	unsigned char attrType;
}RS485_DEV_SENSOR_REQ_STATE_T;

typedef struct _RS485_DEV_POINT_STATE_ATTR_s
{
	unsigned char endpoint;
	unsigned short pileId;
	unsigned short attrId;
	unsigned char attrType;
	unsigned char value;
}RS485_DEV_POINT_STATE_ATTR_T;


typedef struct _RS485_DEVICE_REPORT_STATE2_S
{
	unsigned char attrState;
	RS485_DEV_POINT_STATE_ATTR_T endport[6];
}RS485_DEVICE_REPORT_STATE2_T;

typedef struct _RS485_DEVICE_REPORT_STATE3_S
{
	RS485_DEV_POINT_STATE_ATTR_T endport[6];
}RS485_DEVICE_REPORT_STATE3_T;

typedef struct _RS485_SENSOR_REPORT_STATE_S
{
	unsigned char attrState;
	unsigned char endpoint;
	unsigned short pileId;
	unsigned short attrId;
	unsigned char attrType;
	unsigned short value;
	unsigned char endpoint2;
	unsigned short pileId2;
	unsigned short attrId2;
	unsigned char attrType2;
	unsigned short value2;
	unsigned char endpoint3;
	unsigned short pileId3;
	unsigned short attrId3;
	unsigned char attrType3;
	unsigned short value3;
}RS485_SENSOR_REPORT_STATE_T;

typedef struct _RS485_DEVICE_REPORT_ATTRI_S
{	
	unsigned char endpoint;
	unsigned short pileId;
	unsigned short attrId;
	unsigned char attrType;
	unsigned char value;
}RS485_DEVICE_REPORT_ATTRI_T;

typedef struct _RS485_DEVICE_ATTRI_S
{
	unsigned short attrId;
	unsigned char flag;
	unsigned char attrType;
	unsigned char lenght;
	unsigned char data[1];
}RS485_DEVICE_ATTRI_T;

typedef struct _RS485_DEVICE_REPORT_STATE_S
{
	unsigned short attrId;
	unsigned char attrType;
	unsigned char value;
	unsigned short attrId2;
	unsigned char attrType2;
	unsigned short value2;
}RS485_DEVICE_REPORT_STATE_T;

typedef struct _RS485_DATA_FORMAT_HEADER_T
{
	unsigned char frametype;
	unsigned char pkno;
	unsigned short destAddr;
	unsigned short srcAddr;
	unsigned char destEendport;
	unsigned char srcEendport;
	unsigned short cluID;
	unsigned char cmdID;
	unsigned char mac_addr[12];
}RS485_DATA_FORMAT_HEADER_T;

typedef struct _RS485_DATA_FORMAT_T
{
	RS485_DATA_FORMAT_HEADER_T dataheader;
	unsigned char payload[64];
	unsigned short data_len;
}RS485_DATA_FORMAT_T;

typedef struct _RS485_SCENE_FORMAT_T
{	
	unsigned short group_id;
	unsigned char scene_id;
}RS485_SCENE_GROUP;


#pragma pack()

//11 F0 0000 0001 01 06 0008 0400000020011E

OSStatus  rs485_dev_joined_request(RS485_DATA_FORMAT_T *pRs485Data);
OSStatus  rs485_msg_state_report(RS485_DATA_FORMAT_T *pRs485Data);
OSStatus  rs485_msg_zcl_zdo(cJSON *root);

#endif

