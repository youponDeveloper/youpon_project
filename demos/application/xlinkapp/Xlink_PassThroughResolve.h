#ifndef __XLINK_PASSTHROUGHRESOLVE_H__
#define __XLINK_PASSTHROUGHRESOLVE_H__


#include "Xlink_PassThroughProtocol.h"



typedef enum{
		XLINK_PTR_CHECK_MAC = 0X00, //mcu check wifi mac
		XLINK_PTR_CHECK_NET = 0x01,      //mcu check wifi net
		XLINK_PTR_CHECK_PIDKEY = 0x02,   //mcu check wifi pid and key
		XLINK_PTR_SET_PIDKEY = 0x03,     //mcu set wifi pid and key
		XLINK_PTR_ENTER_SMARTLINK = 0x04, // mcu set wifi into smartlink
		XLINK_PTR_REBOOT_WIFI = 0x05,     //mcu reboot wifi
		XLINK_PTR_REPASS_WIFI = 0x06,     //mcu repass wifi
		XLINK_PTR_CHECK_WIFI_VER = 0x07,  //mcu check wifi version
		XLINK_PTR_GET_WIFI_TIME = 0x08,    //mcu get wifi time
		XLINK_PTR_SET_WIFI_VISIBLE = 0x09, //mcu set wifi in visiable

		XLINK_PTR_WIFI_TO_MCU_TRAN_T = 0X80,  //wifi transfer passthrough data to mcu
		XLINK_PTR_MCU_TO_WIFI_TRAN_T = 0x81,        //mcu transfer passthrough data to wifi
		XLINK_PTR_WIFI_TO_MCU_TRAN_DP = 0x82,       //wifi transfer app or server datapionts data to mcu
		XLINK_PTR_MCU_TO_WIFI_TRAN_DP = 0x83,	     //mcu transfer datapoints data to wifi
		XLINK_PTR_MCU_TO_WIFI_TRAN_ALLDP = 0x84,     //mcu tranfer all datapoints data to wifi and for web refresh
		XLINK_PTR_MCU_TO_WIFI_TRAN_ALLDP_NO_ALARM = 0x85
}XLINK_CMD_VALUE;


extern void xlink_call_reboot(void );
extern void xlink_resolve_process(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt);
extern void xlink_wifi_to_mcu_passthrough_process(unsigned char *data,unsigned int datalen);
extern void xlink_wifi_to_mcu_tran_dp(unsigned char *data, int datalen);
extern void xlink_getwifi_status_resp(void);
extern void xlink_send_cb_status(XLINK_CMD_VALUE cmd_type);
extern void on_power_status_resp(void);

#endif

