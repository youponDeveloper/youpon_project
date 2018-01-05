/*
 * xlink_AppPassthrough.h
 *
 *  Created on:
 *      Author: john
 */

#ifndef XLINK_APPPASSTHROUGH_H_
#define XLINK_APPPASSTHROUGH_H_

#include "../xlinksrc/xlink_system.h"
//#include "xlink_system.h"

#define CMAND_INIT    0
#define AT_SUCCESS    1    //at command success
#define AT_ERROR      2    //at command error

#define XLINK_CONFIG_PRODUCTID_KEY   4
#define XLINK_ID_KEY_LENGTH          64
#define XLINK_UART_BUFFER_SIZE       512

#define XLINK_SAVE_PID_KEY_FLAG     0X55

#define SET_DP_MAX_LEN  	     1024
#define PACKAGE_BUF_LEN     	     1024
#define UART_MAX_FRAME_LEN  	     512
#define QUEUE_BUF_LEN   	1024//1024


extern unsigned char get_mac_buf[6];
extern int get_mac(unsigned char *buf,unsigned char len);
extern void xlink_function_thread(uint32_t inContext);
extern void xlink_domain_loop(uint32_t inContext);
//extern void test_pingcmd_thread(uint32_t inContext);
extern int  xlink_demo_init(void);
//extern void XlinkUartSend(unsigned char *Buffer, unsigned short BufferLen, x_uint32 device_id);
extern void  xlink_resolve_mcu_cmd(void);
extern void Send_wifi_status(unsigned char sta);

typedef union {
	unsigned char byte;
	struct {
		unsigned char isConnectedServer :1;
		unsigned char isConnectWIFI :1;
		unsigned char isNeedSend :1;
		unsigned char islogined :1;
		unsigned char isSendUartData :1;
		unsigned char isNeedSendWifiSta:1;
		unsigned char isNeedSendServerSta:1;
		unsigned char res :1;
	} bit;
} XLINK_LOCAL_STA;

extern XLINK_LOCAL_STA g_AllSta;
extern void init_wifi_connect_status();

#endif /* XLINK_APPPASSTHROUGH_H_ */
