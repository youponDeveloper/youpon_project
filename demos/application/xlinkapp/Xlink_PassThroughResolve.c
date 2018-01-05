
#include "Xlink_PassThroughResolve.h"
#include "Xlink_PassThroughProtocol.h"
#include "xlink_DataPointProcess.h"

#include "xlink_demo.h"
#include "xlink_config.h"
#include "xlink_system.h"
#include "user_config.h"

#define XlinkPass_log(M, ...) custom_log("XlinkPass_log", M, ##__VA_ARGS__)

#define TRAN_MAX_DP_LEN   	1000

extern XLINK_USER_CONFIG  	user_config;
extern mico_mutex_t wifi_connect_sta_mutex;
volatile int get_dp_len = 0;
volatile unsigned short udp_dpmessage_id=0,tcp_dpmessage_id=0;
volatile unsigned short udp_m_id = 0,tcp_m_id=0;
XLINK_CMD_VALUE cmd_type = 0;

extern int XlinkReadProductIDKEY(unsigned char *flag,char *proid, char *proKey);
extern void set_broadcast_flag(uint8_t flag);
XLINK_FUNC void xlink_call_reboot(void )
{
	 XlinkPass_log("xlink_call_reboot in ........");
	  MicoSystemReboot();
}


XLINK_FUNC void xlink_getmac_resp(void)
{	
  XLINK_PASSTHROUGHPROTOCOLPACKET respack;
	
//	unsigned char mac_len=0;	
	memset(&respack,'\0',sizeof(respack));
	
#if XLINK_DEBUG_ON	
	 xlink_printf("wifi get mac start");
#endif
	
	respack.en = 1;
	respack.cmd = XLINK_PTR_CHECK_MAC;
	respack.data = user_config.mac;

	respack.alldatalen = 2 + 6;
	respack.datalen = 6;

	Xlink_PassThroughProtolBuildSendData(&respack);
}


XLINK_FUNC void xlink_getwifi_status_resp(void)
{
    XLINK_PASSTHROUGHPROTOCOLPACKET respack;
	 
	unsigned char net_state[3];
	unsigned char temp_buf[3];
	memset(&respack,'\0',sizeof(respack));	
	memset(net_state,0,sizeof(net_state));
	
	respack.en = 1;
	respack.cmd = XLINK_PTR_CHECK_NET;
	respack.alldatalen = 4;	
	respack.datalen = 2;
	respack.data = temp_buf;

	mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
	net_state[0] = g_AllSta.bit.isConnectWIFI;
	net_state[1] = g_AllSta.bit.isConnectedServer;
	mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);
	
	memcpy(respack.data,net_state,respack.datalen);
	
	Xlink_PassThroughProtolBuildSendData(&respack);
}



XLINK_FUNC void on_power_status_resp(void)
{
    XLINK_PASSTHROUGHPROTOCOLPACKET respack;

	unsigned char net_state[3];
	unsigned char temp_buf[3];
	memset(&respack,'\0',sizeof(respack));
	memset(net_state,0,sizeof(net_state));

	respack.en = 1;
	respack.cmd = XLINK_PTR_CHECK_NET;
	respack.alldatalen = 4;
	respack.datalen = 2;
	respack.data = temp_buf;

	net_state[0] = 2;
	net_state[1] = 0;

	memcpy(respack.data,net_state,respack.datalen);

	Xlink_PassThroughProtolBuildSendData(&respack);
}


XLINK_FUNC void xlink_get_pidkey_resp(void)
{
	
	XLINK_PASSTHROUGHPROTOCOLPACKET respack; 
	 
	char pid[33] = {0};
	char key[33] = {0};
	unsigned char temp_buf[65];
	unsigned char flag = 0;

	memset(&respack,'\0',sizeof(respack));	
        XlinkReadProductIDKEY(&flag,pid, key);

	respack.en=1;
	respack.cmd = XLINK_PTR_CHECK_PIDKEY;
	
	if(flag != 0x55){
		respack.alldatalen = 0x02;
		respack.datalen = 0;
		respack.data = NULL;
	}else{		
		respack.alldatalen = 0x42;
		respack.datalen = 0x40;
		respack.data = temp_buf; 
		memcpy(respack.data,pid,32);
		memcpy(respack.data+32,key,32);
	}

	Xlink_PassThroughProtolBuildSendData(&respack);
}


XLINK_FUNC void xlink_set_pidkey_resp(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;

//	char pid[33] = {0};
//	char key[33] = {0};

	int ret = -1;
	unsigned char i=0;
	unsigned char temp;
	unsigned char flag = 0x55;
	
	//memset(pid,0,33);
	//memset(key,0,33);

        for (i = 0; i < 32; i++) {
			temp = pkt->data[i];
			if (temp <= '9' && temp >= '0') {
			} else if (temp > 'f' || temp < 'a') {
				return;
			}
	}
	for (i = 0; i < 32; i++) {
			temp = pkt->data[i+32];
			if (temp <= '9' && temp >= '0') {
			} else if (temp > 'f' || temp < 'a') {
				return;
			}
	}
	if(pkt->datalen == 64) {
		ret = XlinkWriteProductIDKEY(&flag,(char*)pkt->data,(char*)pkt->data + 32);
		if(ret < 0){
			return;
		}

		respack.en = 1;
		respack.cmd = XLINK_PTR_SET_PIDKEY;
		respack.alldatalen = 0x02;
		respack.datalen = 0;
		respack.data = NULL;

		Xlink_PassThroughProtolBuildSendData(&respack);

		mico_thread_msleep(100);
		xlink_call_reboot( );
	}
}


XLINK_FUNC void xlink_enter_smartlink_resp(void)
{

	XLINK_PASSTHROUGHPROTOCOLPACKET respack;


	memset(&respack,'\0',sizeof(respack));

	respack.en = 1;
	respack.cmd = XLINK_PTR_ENTER_SMARTLINK;
	respack.alldatalen = 2;
	respack.datalen = 0;
	respack.data = NULL;
	Xlink_PassThroughProtolBuildSendData(&respack);

	mico_thread_msleep(100);
	extern  void PlatformEasyLinkButtonClickedCallback(void);
    PlatformEasyLinkButtonClickedCallback();

}


XLINK_FUNC void xlink_reboot_wifi_resp(void)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;

	memset(&respack,'\0',sizeof(respack));

	respack.en = 1;
	respack.cmd = XLINK_PTR_REBOOT_WIFI;
	respack.alldatalen = 2;
	respack.datalen = 0;
	respack.data = NULL;
	Xlink_PassThroughProtolBuildSendData(&respack);

	mico_thread_msleep(100);
	xlink_call_reboot();
}



XLINK_FUNC void xlink_repass_wifi_resp(void)
{
	char buf[32];
	unsigned char flag = 0;
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;

	memset(&respack,'\0',sizeof(respack));
	memset(buf,0,32);

	XlinkWriteProductIDKEY(&flag,buf,buf);
	mico_thread_msleep(10);
	XlinkReSetSDK();
	mico_thread_msleep(10);
	respack.en = 1;
	respack.cmd = XLINK_PTR_REPASS_WIFI;
	respack.alldatalen = 2;
	respack.datalen = 0;
	respack.data = NULL;
	Xlink_PassThroughProtolBuildSendData(&respack);
	
	mico_thread_msleep(100);
	xlink_call_reboot();
}



XLINK_FUNC void xlink_getwifi_ver_resp(void)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;

	unsigned char buf[3]={0};

	memset(&respack,'\0',sizeof(respack));
	
	respack.en = 1;
	respack.cmd = XLINK_PTR_CHECK_WIFI_VER;
	respack.alldatalen = 0x04;
	respack.datalen = 0x02;
	
	buf[0] = (unsigned char)((user_config.wifisoftVersion & 0xff00)>>8);
	buf[1] = (unsigned char)(user_config.wifisoftVersion & 0x00ff);
	
	respack.data = buf;
	Xlink_PassThroughProtolBuildSendData(&respack);
}


XLINK_FUNC void xlink_getwifi_time_resp(void)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;
	XLINK_SYS_TIME m_c_time;
	unsigned char retBuffer[10];

	memset(retBuffer,'\0',sizeof(retBuffer));
	memset(&respack,'\0',sizeof(respack));

	XlinkGetSystemTime(&m_c_time);

	retBuffer[0]=(unsigned char)((m_c_time.year &0xff00)>>8);
	retBuffer[1]=(unsigned char)((m_c_time.year &0x00ff));
	retBuffer[2]= m_c_time.mon;
	retBuffer[3]= m_c_time.day;
	retBuffer[4]= m_c_time.hour;
	retBuffer[5]= m_c_time.min;
	retBuffer[6]= m_c_time.sec;

	retBuffer[7]= ((unsigned char)((char)(m_c_time.zones/100)));

	respack.en = 1;
	respack.cmd = XLINK_PTR_GET_WIFI_TIME;
	respack.alldatalen = 0x0A;
	respack.datalen = 8;
	
	respack.data = retBuffer;
	
	Xlink_PassThroughProtolBuildSendData(&respack);

}


XLINK_FUNC void xlink_setwifi_visiable_resp(unsigned char flag)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;

	memset(&respack,'\0',sizeof(respack));

	if(flag){
		XlinkPorcess_UDP_Enable();
	}else{
		XlinkPorcess_UDP_Disable();
	}
	respack.en = 1;
	respack.cmd = XLINK_PTR_SET_WIFI_VISIBLE;
	respack.alldatalen = 0x02;
	respack.datalen = 0;
	respack.data = NULL;
	
	Xlink_PassThroughProtolBuildSendData(&respack);
}


XLINK_FUNC void xlink_wifi_to_mcu_tran_dp(unsigned char *data, int datalen)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;

	memset(&respack,'0',sizeof(respack));

	respack.en = 1;
	respack.cmd = XLINK_PTR_WIFI_TO_MCU_TRAN_DP;
	respack.alldatalen = datalen+2;
	respack.datalen = datalen;
	respack.data = data;

	Xlink_PassThroughProtolBuildSendData(&respack);
}

void  sendMessageToServer(char *pbuffer, int32_t buffer_len, uint32_t deviceid)
{
	x_int32 ret=0;
	uint8_t isConnectWIFI = 0;
	uint8_t isConnectedServer = 0;
	
	 if(NULL == pbuffer || buffer_len < 1||buffer_len>1024)
	 {
	 	XlinkPass_log("NULL == pbuffer,buffer_len:%d",buffer_len);
		return;
	 }

	mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
	isConnectWIFI = g_AllSta.bit.isConnectWIFI;
	isConnectedServer = g_AllSta.bit.isConnectedServer;
	mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);
	
	 if(isConnectWIFI || isConnectedServer)
	 {   
		//XlinkPass_log("######sendMessageToServer in ######### \n");
		if(isConnectedServer)
		{
			set_broadcast_flag(1);
			if(deviceid == 0)
			{
				ret = XlinkSendTcpPipe2_cb(tcp_m_id, (uint8_t *)pbuffer, buffer_len);
			}
			else
			{
				ret = XlinkSendTcpPipe_cb(tcp_m_id, (uint8_t *)pbuffer, buffer_len, deviceid);
			}
			set_broadcast_flag(0);

			if(ret < 0)
			{
				XlinkPass_log("^^^^^ sendMessageToServer failed , resend once again^^^^^^^^\n");
				set_broadcast_flag(1);
				if(deviceid == 0)
				{
					ret = XlinkSendTcpPipe2_cb(tcp_m_id, (uint8_t *)pbuffer, buffer_len);
				}
				else
				{
					ret = XlinkSendTcpPipe_cb(tcp_m_id, (uint8_t *)pbuffer, buffer_len, deviceid);
				}
				set_broadcast_flag(0);

				if(ret < 0)
					XlinkPass_log("^^^^^^^^^^^^^^^^^^^^^^^ sendMessageToServer failed ^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
			}
		}
		else
		{
			ret = XlinkSendUdpPipe_cb(udp_m_id, pbuffer, buffer_len, NULL);
			udp_m_id++;			
		}

		#ifdef USER_DEBUG_
		//XlinkPass_log("######sendMessageToServer: %d ######### ret: %d \n", deviceid, ret);
		#endif
		

		tcp_m_id++;
	 }
	 else
	 	{
		XlinkPass_log("isConnectWIFI :%d , isConnectedServer:%d",isConnectWIFI,isConnectedServer);
	 	}
}

XLINK_FUNC void xlink_mcu_to_wifi_passthrough_resp(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;	

	unsigned char flag = 0;
	uint8_t isConnectWIFI = 0;
	uint8_t isConnectedServer = 0;

	mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
	isConnectWIFI = g_AllSta.bit.isConnectWIFI;
	isConnectedServer = g_AllSta.bit.isConnectedServer;
	mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);

	 memset(&respack,'\0',sizeof(respack));

	if(isConnectWIFI ||isConnectedServer)
	{   
		XlinkPass_log("#########xlink_mcu_to_wifi_passthrough_resp#########\n");
		XlinkSendUdpPipe_cb(udp_m_id,pkt->data, pkt->datalen, NULL);
		udp_m_id++;
		if(isConnectedServer)
		{
		XlinkSendTcpPipe2_cb(tcp_m_id,pkt->data, pkt->datalen);
		tcp_m_id++;
		}
		flag = 0; //success
	}
	else
	{
		flag = 1;
	}

	respack.en = 1;
	respack.cmd = XLINK_PTR_MCU_TO_WIFI_TRAN_T;
	respack.alldatalen = 0x03;	
	respack.datalen = 0x01;

	respack.data= &flag;

	Xlink_PassThroughProtolBuildSendData(&respack);
}

XLINK_FUNC void xlink_wifi_to_mcu_passthrough_process(unsigned char *data,unsigned int datalen)
{
    XLINK_PASSTHROUGHPROTOCOLPACKET respack;
	
    memset(&respack,'\0',sizeof(respack));	

    respack.en = 1;
    respack.cmd = XLINK_PTR_WIFI_TO_MCU_TRAN_T;
    respack.alldatalen = datalen+2;
    respack.datalen = datalen;
    
    respack.data = data;
	
    Xlink_PassThroughProtolBuildSendData(&respack); 
}



XLINK_FUNC void xlink_mcu_to_wifi_dp_process(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;	

	unsigned char flag = 0;
	uint8_t isConnectWIFI = 0;
	uint8_t isConnectedServer = 0;

	mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
	isConnectWIFI = g_AllSta.bit.isConnectWIFI;
	isConnectedServer = g_AllSta.bit.isConnectedServer;
	mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);

	memset(&respack,'\0',sizeof(respack));

	dp_push_process(pkt->data, pkt->datalen);

	xlink_printf("proces mcu_to_wifi_dp_process.....\r\n");
	XlinkPass_log(" ************************************ xlink_mcu_to_wifi_dp_process ************** \n");

	if(isConnectWIFI || isConnectedServer)
	{
		//Xlink_UpdateDataPoint(pkt->data, pkt->datalen,0);
		Xlink_UpdateDataPoint_udp(udp_dpmessage_id,pkt->data, pkt->datalen,0);
		udp_dpmessage_id++;
		if(isConnectedServer)
		{
			Xlink_UpdateDataPoint_tcp(tcp_dpmessage_id,pkt->data, pkt->datalen,0);
			tcp_dpmessage_id++;
			cmd_type = XLINK_PTR_MCU_TO_WIFI_TRAN_DP;
		}
		flag = 0; //success

	}
	else
	{
		flag = 1;
	}

	respack.en = 1;
	respack.cmd = XLINK_PTR_MCU_TO_WIFI_TRAN_DP;
	respack.alldatalen = 0x03;	
	respack.datalen = 0x01;
	respack.data= &flag;

	Xlink_PassThroughProtolBuildSendData(&respack);
}


XLINK_FUNC void xlink_mcu_to_wifi_no_alarm_dp_process(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt)
{
	 XLINK_PASSTHROUGHPROTOCOLPACKET respack;

	 unsigned char flag = 0;
	 uint8_t isConnectWIFI = 0;
	uint8_t isConnectedServer = 0;
	
	mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
	isConnectWIFI = g_AllSta.bit.isConnectWIFI;
	isConnectedServer = g_AllSta.bit.isConnectedServer;
	mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);
	
         memset(&respack,'\0',sizeof(respack));

         dp_push_process(pkt->data, pkt->datalen);

	 if(isConnectWIFI || isConnectedServer){
		  //Xlink_UpdateDataPoint(pkt->data, pkt->datalen,1);
			Xlink_UpdateDataPoint_udp(udp_dpmessage_id,pkt->data, pkt->datalen,1);
			udp_dpmessage_id++;
			if(isConnectedServer){
				Xlink_UpdateDataPoint_tcp(tcp_dpmessage_id,pkt->data, pkt->datalen,1);
				tcp_dpmessage_id++;
				cmd_type = XLINK_PTR_MCU_TO_WIFI_TRAN_ALLDP_NO_ALARM;
			}
		  flag = 0; //success
	 }else{
	    flag = 1;
	 }

	 respack.en = 1;
	 respack.cmd = XLINK_PTR_MCU_TO_WIFI_TRAN_ALLDP_NO_ALARM;
	 respack.alldatalen = 0x03;
	 respack.datalen = 0x01;
	 respack.data= &flag;

	 Xlink_PassThroughProtolBuildSendData(&respack);
}


XLINK_FUNC void xlink_mcu_to_wifi_alldp_process(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt)
{
	XLINK_PASSTHROUGHPROTOCOLPACKET respack;	
	uint8_t isConnectWIFI = 0;
	uint8_t isConnectedServer = 0;

	mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
	isConnectWIFI = g_AllSta.bit.isConnectWIFI;
	isConnectedServer = g_AllSta.bit.isConnectedServer;
	mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);

	unsigned char flag = 0;	
	memset(&respack,'\0',sizeof(respack));

	dp_mem_init();

	xlink_printf("proces mcu_to_wifi_alldp_process.....\r\n");

	if(isConnectWIFI || isConnectedServer)
	{
		if(TRAN_MAX_DP_LEN >= pkt->datalen) 
		{
			dp_push_process(pkt->data, pkt->datalen);
			flag = 0; //success
		}
		else 
		{
			flag = 1;
		}
	}
	else
	{
		flag = 1;
	}

	respack.en = 1;
	respack.cmd = XLINK_PTR_MCU_TO_WIFI_TRAN_ALLDP;
	respack.alldatalen = 0x03;	
	respack.datalen = 0x01;
	respack.data= &flag;

	Xlink_PassThroughProtolBuildSendData(&respack);
}



XLINK_FUNC void xlink_send_cb_status(XLINK_CMD_VALUE cmd_type)
{

	XLINK_PASSTHROUGHPROTOCOLPACKET respack;
	unsigned char flag = 2;

	memset(&respack,'\0',sizeof(respack));

	respack.en = 1;
	respack.cmd = cmd_type;
	respack.alldatalen = 0x03;
	respack.datalen = 0x01;
	respack.data= &flag;

	Xlink_PassThroughProtolBuildSendData(&respack);

}

XLINK_FUNC void xlink_resolve_process(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt)
{
	
	XLINK_CMD_VALUE cmd_value;
	unsigned char temp;
	
	if(pkt == NULL)return;
	if(pkt->en == 0)return;
	
	cmd_value = pkt->cmd;

//#if XLINK_DEBUG_ON
    XlinkPass_log("get cmd value=%d,datalen=%d\r\n",cmd_value,pkt->datalen);
//#endif
	
	switch(cmd_value){
			case XLINK_PTR_CHECK_MAC:  //mcu check wifi mac
				 xlink_getmac_resp();
				 break;				
			case XLINK_PTR_CHECK_NET:  //mcu check wifi net
				 xlink_getwifi_status_resp();
				 break;
			case XLINK_PTR_CHECK_PIDKEY: //mcu check wifi pid and key
				 xlink_get_pidkey_resp();
				 break;
			case XLINK_PTR_SET_PIDKEY:  //mcu set wifi pid and key
				 xlink_set_pidkey_resp(pkt);
				 break;
			case XLINK_PTR_ENTER_SMARTLINK: // mcu set wifi into smartlink
				 xlink_enter_smartlink_resp();
				 break;
			
			case XLINK_PTR_REBOOT_WIFI:   //mcu reboot wifi
				xlink_reboot_wifi_resp();
				break;
			
			case XLINK_PTR_REPASS_WIFI:  //mcu repass wifi
				xlink_repass_wifi_resp();
				break;
			case XLINK_PTR_CHECK_WIFI_VER:   //mcu check wifi version
				xlink_getwifi_ver_resp();
				break;
			case XLINK_PTR_GET_WIFI_TIME:   //mcu get wifi time
				xlink_getwifi_time_resp();
				break;
			case XLINK_PTR_SET_WIFI_VISIBLE:  //mcu set wifi in visiable
				if(pkt->datalen == 1) {
					temp = pkt->data[0];
					xlink_setwifi_visiable_resp(temp);
				}
				break;
			case XLINK_PTR_WIFI_TO_MCU_TRAN_T:
				break;
			case XLINK_PTR_MCU_TO_WIFI_TRAN_T:   //mcu transfer passthrough data to wifi		
				xlink_mcu_to_wifi_passthrough_resp(pkt);			 
				break;
			case XLINK_PTR_WIFI_TO_MCU_TRAN_DP:
				break;
			case XLINK_PTR_MCU_TO_WIFI_TRAN_DP:  //mcu transfer datapoints data to wifi
				xlink_mcu_to_wifi_dp_process(pkt);
				break;
			case XLINK_PTR_MCU_TO_WIFI_TRAN_ALLDP:  //mcu tranfer all datapoints data to wifi and for web refresh
			   xlink_mcu_to_wifi_alldp_process(pkt);
				break;

			case XLINK_PTR_MCU_TO_WIFI_TRAN_ALLDP_NO_ALARM:  //mcu transfer datapoints data to wifi
				xlink_mcu_to_wifi_no_alarm_dp_process(pkt);
				break;

			default:
				break;		
	}		
}

