#ifndef __APPMAINMSGQUEUE_H__
#define __APPMAINMSGQUEUE_H__

#include "mico.h"
#include "user_main.h"
#include "cJSON.h"

typedef enum
{
    APP_RECV_MSG_QUEUE,//this queue message will send to uart
    APP_SEND_MSG_QUEUE,//the queue message will send to fogcloud
    APP_RECV_FOGCLOUD_MSG_QUEUE,//the queue message recv from fogcloud
    APP_MSG_QUEUE_MAX,
} app_msg_queue_count_t;


typedef struct _app_main_msg_t {
	uint32_t deviceid;
	int32_t riu;
	uint32_t data_len;
	char data[1];  // 1 byte fake msg payload, real msg memory will be reallocate when received
} app_main_msg_t ;


typedef struct _app_msg_queue_s
{
	mico_mutex_t  msg_mutex;
	mico_queue_t  msg_queue;
} app_msg_queue_t;

typedef struct _FOG_MSG_PACKAGED_S
{	
	char *pBuffer;
	char *out;
	uint32_t  outlen;
	cJSON *root;
}FOG_MSG_PACKAGED_T;


OSStatus app_recv_send_msg_queue_init(app_context_t * const app_context);
OSStatus pushAppMainRecvMsgProcess(unsigned char *msg, unsigned int inBufLen, int riu);
OSStatus pushSend2ServerMsgProcess(unsigned char *msg, unsigned int inBufLen,uint32_t device_id);


#endif

