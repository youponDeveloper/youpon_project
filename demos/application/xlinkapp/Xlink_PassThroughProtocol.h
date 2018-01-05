#ifndef __XLINK_PASSTHROUGHPROTOCOL_H__
#define __XLINK_PASSTHROUGHPROTOCOL_H__


#include "../xlinksrc/xlink_data.h"
//#include "xlink_data.h"

#define XLINK_PTP_HEAD   0xFF
#define XLINK_PTP_CHANGE 0xFD
#define XLINK_PTP_TAIL   0xFE
#define XLINK_PTP_OR     0x80


typedef struct{
	unsigned char en;
	unsigned char cmd;
	unsigned short alldatalen;
	unsigned char *data;
	unsigned short datalen;
	unsigned char flag;
	void (*PacketSend)(unsigned char *data,unsigned short datalen, x_uint32 device_id);
}XLINK_PASSTHROUGHPROTOCOLPACKET;

typedef struct{
	unsigned char *pktbuf;
	unsigned short pktbuflen;
	unsigned short pktpos;

	unsigned short pktlentmp;
	XLINK_PASSTHROUGHPROTOCOLPACKET *pkt;
	void (*PacketCallBack)(XLINK_PASSTHROUGHPROTOCOLPACKET *PTP_pkt);
	
}XLINK_PASSTHROUGHPROTOCOL;



extern int Xlink_PassThroughProtolInit(XLINK_PASSTHROUGHPROTOCOL *PTP_pck);
extern void Xlink_PassThroughProtolPutData(XLINK_PASSTHROUGHPROTOCOL *PTP_pck,unsigned char *data,unsigned short datalen);
extern void Xlink_PassThroughProtolBuildSendData(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt);
extern unsigned char Xlink_PassThroughProtolBuildXor(unsigned char resold,unsigned char *data,unsigned int datalen);


#endif
