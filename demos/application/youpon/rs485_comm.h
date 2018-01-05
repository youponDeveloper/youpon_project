#ifndef __RS485_COMM_H__
#define __RS485_COMM_H__

#include "MICO.h"
#include "mico_system.h"

#include "appmainmsgqueue.h"
#include "user_config.h"
#include "rs485_uart.h"

#define  MAX_TIME_CONVERT	(1000)         // 1 us

// RS485 传输一个字节时间
#define RS485_ByteTimeUs	40  //us

// RS485 帧头
#define RS485_FrameHeadH			0x55
#define RS485_FrameHeadM			0xAA
#define RS485_FrameHeadL			0x55

// RS485 广播地址
#define RS485_BroadcastAddress 		0xffff

// RS485 接收缓存长度
//#define RS485_BufferLength	512

// 回调函数ID
#define CallbackId_UartIRQ (1)
#define CallbackId_UartDMA (2)

// 发送数据时，总线忙检测与冲突即 CSMA/CD
#define SendData_RepeatCount 10					// 重发次数
#define SendData_ListenBusyCount 16				// 监听总线忙次数
#define SendData_Jamming		0x01			// 干扰信号数据
#define SendData_JammingLength	16				// 干扰信息长度

#define RS485_RECV_BACKUSE_BUF_LEN	6	//max device  endpoint is 3

typedef enum
{
	FieldIndex_FrameHeadH = 0,
	FieldIndex_FrameHeadM,
	FieldIndex_FrameHeadL,
	FieldIndex_LengthH,
	FieldIndex_LengthL,
	FieldIndex_CtrlNumber,
	FieldIndex_SequenceNumber,
	FieldIndex_DestinationAddressH,
	FieldIndex_DestinationAddressL,
	FieldIndex_SourceAddressH,
	FieldIndex_SourceAddressL,
	FieldIndex_DestinationEndpoint,
	FieldIndex_SourceEndpoint,
	FieldIndex_ClusterIdH,
	FieldIndex_ClusterIdL,
	FieldIndex_CommandId,
	FieldIndex_Payload,
	FieldIndex_CRC8,
	FieldIndex_MinNumber = FieldIndex_CRC8
}FieldIndex;

typedef struct _RS485_CTRL_COMMAND_PARAM_S
{
       char cmd[96];
	unsigned int inBufLen;
	int dev_stata;
	uint16_t cmd_length;
	uint16_t destinationAddress;
	uint16_t srcAddress ;
	char *pJsonStr;
	int i;
	int brightnessvalue;
	unsigned short netaddr;
	unsigned char  joined_st; 
	unsigned char scene_config;
	unsigned char  dev_dest_id; 
	unsigned char dev_onoffline;
	unsigned char key_id;
	unsigned char zb_logout_allow_st;
	unsigned char tmp;
	unsigned char light_onoff_flag;
	unsigned int device_type;
	unsigned char frametype;
	uint8_t  destEndpoit;
	uint8_t  srcEndpoit;
	uint16_t pileId;
	uint8_t cmdId;
}RS485_CTRL_COMMAND_PARAM_T;

typedef struct _DEVICE_ATTRI_S
{
	uint16_t attriId;
	uint8_t attriCmd;
	uint8_t attriValue;
}DEVICE_ATTRI_T;

#pragma pack(1)
typedef struct _SCENE_ATTRI_S
{
	uint16_t attriId;
	uint8_t attriValue;
}SCENE_ATTRI_T;
#pragma pack()

//#pragma pack(1)
// 收发器数据结构
typedef struct
{
	//qm_uart_transfer_t transfer;
	mico_mutex_t  msg_mutex;
	uint8_t buffer[RS485_UART_BUFFER_LENGTH+1];
	union
	{
		volatile uint16_t count;
		volatile uint16_t length;
	};
	uint8_t sequenceNumber;
	volatile uint8_t sendsuccessflag;
}Transceiver ;
//#pragma pack()

int Rs485_Init();
OSStatus pushRs485RecvMsgProcess(unsigned char *msg, unsigned int inBufLen);
OSStatus rs485QueueMsgRecv(app_main_msg_t **msg, uint32_t timeout_ms);
int32_t Rs485_SendDataPackage(RS485_CTRL_COMMAND_PARAM_T *pDevParam);
int32_t Rs485_SendData(uint8_t *data, uint16_t length);
uint8_t getCRC8(uint8_t *pdata, uint16_t length);
int32_t Rs485_SendDataPackageEx(RS485_CTRL_COMMAND_PARAM_T *pDevParam);

#endif

