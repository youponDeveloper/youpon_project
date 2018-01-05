/*
 * xlinkHttp.h
 *
 *  Created on: 2015年11月24日
 *      Author: john
 */

#ifndef XLINKHTTP_H_
#define XLINKHTTP_H_

#include "xlink_demo.h"

//文件总大小
//已经下载的大小
//当前状态 0 正常
typedef void (*xlinkhttpStaCallBack)(const unsigned int fileSize, const unsigned int rate, const unsigned int sta);
typedef void (*xlinkhttpRecvDataCallBack)(const char *data, unsigned int datalen, const unsigned int fileSize,
		const unsigned int offset);
typedef int (*xlinkhttpErrorCallBack)(const unsigned char errSta);

enum {
	HTTP_STA_GETHOST_FAILED, //
	HTTP_STA_GETHOST_SUCCESS, //
	HTTP_STA_CREATESOCKET_FAILED, //
	HTTP_STA_CREATESOCKET_SUCCESS, //
	HTTP_STA_SOCKET_CLOSE, //
	HTTP_STA_REMOTE_CLOSE_SOCKET, //
	HTTP_STA_CONECT_FAILED, //
	HTTP_STA_CONECT_SUCCESS, //
	HTTP_STA_MD5_CHECK_SUCCESS, //
	HTTP_STA_MD5_CHECK_FAILED,
	HTTP_STA_DOWN_COMPLETE, //
	HTTP_STA_SEND_REQUEST_FAILED,//
	HTTP_STA_DOWN_TIMEOUT,
};

typedef struct XLINKHTTP {
	char m_url[256];
	char md5str[33];
	unsigned int m_urlLength;
	xlinkhttpStaCallBack m_staCallBack;
	xlinkhttpRecvDataCallBack m_recvCallBack;
	xlinkhttpErrorCallBack m_errCallBack;
} XLINK_HTTPDOWN;

extern int xlinkHttpStartDownFile(XLINK_HTTPDOWN *phttpParam);

#endif /* XLINKHTTP_H_ */
