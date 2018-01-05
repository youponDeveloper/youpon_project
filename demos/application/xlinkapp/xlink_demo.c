#include <time.h>

#include "xlink_demo.h"
#include "xlink_config.h"
#include "MICOAppDefine.h"

#include "../xlinksrc/xlink_data.h"

#include "xlink_ringbuf.h"


#include "xlinkHttp.h"
#include "OTAupgrade.h"
#include "CheckSumUtils.h"
#include "mico_system.h"

#include "Xlink_PassThroughProtocol.h"
#include "Xlink_PassThroughResolve.h"
#include "xlink_DataPointProcess.h"
#include "MICO.h"
#include "common.h"

#include "wifi_led.h"
#include "user_config.h"
#ifndef XLINK_FUNC
#define XLINK_FUNC
#endif

//#define XLINK_PRODUCT_ID  "160fa2ae97f8e000160fa2ae97f8e001"
//#define XLINK_PRODUCT_KEY "5c674ba78c346797bb613d11f7b04e84"      

#define xlink_log(M, ...) custom_log("xlink demo", M, ##__VA_ARGS__)
  

XLINK_LOCAL_STA    g_AllSta;
mico_mutex_t  wifi_connect_sta_mutex;
static mico_mutex_t  tcp_send_sta_mutex;
static int server_tcp_send_time_out=0;
	
XLINK_USER_CONFIG  user_config;
unsigned char      get_mac_buf[6]={0};
char               m_pID[33];
int                fileLength = 0;
int                file_totallen = 0;
CRC16_Context      CrcContex;
unsigned int       targtime=0;
static char        urlbuf[256];
static char        mdbbuf[32];
static unsigned char csta = 0;
static unsigned char  cstaCount = 0;
static unsigned char  upMd5IsOk = 0;
static int isupgrade = 0;
static int  is_recalctime = 0;
	
volatile unsigned int  TempGLen = 0;
char m_pID[33];
uint8_t IsSendDataFlag = 0;
int IsSendDataLen = 0;
int IsSendDataMode = 0;
unsigned char xlink_start_flag = 0;
uint8_t  broadcasterr_flag = 0;   // 1: err, 0: no err
uint8_t  checkneedreboot_cnt = 0;

static  uint8_t server_set_dp[SET_DP_MAX_LEN];
static  uint8_t udp_data_cach[SET_DP_MAX_LEN];
static  uint8_t pass_pipe_cach[SET_DP_MAX_LEN];

extern  XLINK_CMD_VALUE cmd_type;
extern  dp_int_fn  datap_int_buf[MAX_DP_NUM];
extern  dp_str_tn  datap_str_buf[MAX_DP_STR_NUM];
static XLINK_FUNC void xlink_demo_work_init(void);

unsigned int lop = 0;
extern int socket_queue_create(app_context_t * const inContext, mico_queue_t *queue);
extern int socket_queue_delete(app_context_t * const inContext, mico_queue_t *queue);
extern void socket_msg_free(socket_msg_t*msg);
extern OSStatus user_uartSend(unsigned char *inBuf, unsigned int inBufLen);
extern int parseServerMessage(int8_t *recvbuff, int32_t BufferLen, uint32_t device_id);
extern int timer_control_init();
extern void setOtaFileMd5Str(uint8_t md5[], int len);
extern void popAppMainSendMsgThread(mico_thread_arg_t arg);
extern void reCalcTime();
extern void wifi_ota_glitter_start(mico_gpio_t gpio);
extern void wifi_lighten_led_color(bool onoff, int color);
extern OSStatus pushFlogMsg2FlogCloudQueue(unsigned char *msg, unsigned int inBufLen, uint32_t device_id);

void set_broadcast_flag(uint8_t flag)
{
	//xlink_log("flag = %d", flag);
	broadcasterr_flag = flag;
}

uint8_t get_broadcast_flag()
{
	return broadcasterr_flag;
}

void init_wifi_connect_status()
{
	mico_rtos_init_mutex( &wifi_connect_sta_mutex);
	mico_rtos_init_mutex( &tcp_send_sta_mutex);	
}

void xlink_PacketCallBack(XLINK_PASSTHROUGHPROTOCOLPACKET *PTP_pkt)
{
		xlink_log("rec one packet ........");
		xlink_resolve_process(PTP_pkt);	
}


void process_dp_pass_data(void)
{
	if(IsSendDataFlag){
		switch(IsSendDataMode){
			case 0:
				xlink_wifi_to_mcu_passthrough_process(pass_pipe_cach,IsSendDataLen);
			break;
			case 1:
				xlink_wifi_to_mcu_passthrough_process(udp_data_cach,IsSendDataLen);
				break;
			case 2:
				xlink_wifi_to_mcu_tran_dp(server_set_dp, IsSendDataLen);
				break;
			default:
				break;
		}
		IsSendDataFlag = 0;
	}
}

int Xlink_PTP_Init(void)
{
	return 0;
}


void Send_wifi_status(unsigned char sta)
{
	mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
	g_AllSta.bit.isConnectWIFI = sta;	
	mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);
	
	wifi_lighten_led_color(sta, 0);
}

#if 0
void XlinkUartSend(unsigned char *Buffer, unsigned short BufferLen, x_uint32 device_id) {
  	g_AllSta.bit.isSendUartData = 1;
	xlink_log("XlinkUartSend**********************************device_id: %d Buffer:%s  Buffer[0]=%02x  BufferLen=%d ", device_id, Buffer, Buffer[0], BufferLen);
	if(BufferLen > 1)
	{
		parseServerMessage(Buffer, BufferLen, device_id);
	}
}
#endif

void broadcast_server_data_to_allapp(unsigned char * data, x_uint32 datalen,x_uint32 device_id)
{
	 if(NULL == data || datalen < 1)
	 {
	 	xlink_log("NULL == data");
		return;
	 }	
	pushSend2ServerMsgProcess((uint8_t *)data, datalen, device_id);	
		 
}

void check_network_state_is_traffic_block(void)
{

	if(get_broadcast_flag()){
		checkneedreboot_cnt++;
		if(checkneedreboot_cnt ==5)
		{
			checkneedreboot_cnt = 0;
			xlink_log("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			xlink_call_reboot();			
		}
	}
	else
	{
		checkneedreboot_cnt = 0;
	}
}

void pipe1_call(unsigned char * data, x_uint32 datalen, x_uint32 device_id, x_uint8 *opt) 
{
#ifdef USER_DEBUG_
	xlink_log("recv tcp data[%d]:%s",datalen, data);
#endif
	if(datalen > 1)
	{
		pushFlogMsg2FlogCloudQueue(data,datalen,device_id);
		//parseServerMessage((int8_t *)data, datalen, device_id);		
	}
}

static void udp_pipe_call(unsigned char *data, x_uint32 datalen, xlink_addr *addr) 
{
#ifdef USER_DEBUG_
	xlink_log("recv udp data[%d]: %s  \r\n",datalen, data);
#endif

	if(datalen > 1)
	{
		//parseServerMessage((int8_t *)data, datalen, 0);
		pushFlogMsg2FlogCloudQueue(data,datalen,0);
		
	}
}

XLINK_FUNC int xlink_demo_init(void) 
{
	xlink_demo_work_init();
	dp_mem_init();

	return 1;
}

static XLINK_FUNC void app_status(XLINK_APP_STATUS status) {

	switch (status) {
	case XLINK_WIFI_STA_APP_CONNECT:
		xlink_log("****new app connect****\r\n");
		break;
	case XLINK_WIFI_STA_APP_DISCONNECT:
		xlink_log("**** app disconnect****\r\n");
		break;
	case XLINK_WIFI_STA_APP_TIMEOUT:
		xlink_log("**** app timeout****\r\n");
		break;
	case XLINK_WIFI_STA_CONNECT_SERVER:
		xlink_log("**** tcp connect to server success****\r\n");
		break;
	case XLINK_WIFI_STA_DISCONNCT_SERVER:
	{
		xlink_log("**** tcp disconnect to server ****\r\n"); 
		mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
	        g_AllSta.bit.isConnectedServer = 0;
	        if (g_AllSta.bit.islogined == 1) {
	            //xlink_getwifi_status_resp();
	        }
	        g_AllSta.bit.islogined = 0;
		mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);
	}
	break;
	case XLINK_WIFI_STA_LOGIN_SUCCESS:
		xlink_log("**** tcp login server success****\r\n");
		mico_rtos_lock_mutex(&wifi_connect_sta_mutex);
		g_AllSta.bit.isConnectedServer = 1;
		g_AllSta.bit.isNeedSend = 1;
		g_AllSta.bit.islogined = 1;
		g_AllSta.bit.isNeedSendServerSta = 1;
		mico_rtos_unlock_mutex(&wifi_connect_sta_mutex);
		break;
	}

}

int get_mac(unsigned char *buf,unsigned char len) {

    if(buf == NULL){
      return 0;
    }
    xlink_memcpy(user_config.mac,buf,len);

#ifdef USER_DEBUG_
    xlink_log("have got address is==%2x:%2x:%2x:%2x:%2x:%2x\r\n",user_config.mac[0],user_config.mac[1],\
            user_config.mac[2],user_config.mac[3],user_config.mac[4],user_config.mac[5] );
#endif

    return 1;
}


XLINK_FUNC void static time_callback(XLINK_SYS_TIME *time) 
{
	xlink_log("xlink updata current time\r\n");
	
	struct tm  utc_time;
	mico_utc_time_ms_t utc_time_ms;

	memset(&utc_time, 0, sizeof(struct tm));
	utc_time.tm_sec = time->sec;
	utc_time.tm_min = time->min;
	utc_time.tm_hour = time->hour;
	utc_time.tm_mday = time->day;
	utc_time.tm_wday = time->week;
	utc_time.tm_mon = time->mon - 1;
	utc_time.tm_year = time->year - 1900;
	utc_time.tm_isdst = -1;
	xlink_log("time->year: %d time->mon:%d Current RTC Time: %s", time->year, time->mon, asctime(&utc_time));

	/* Set UTC time to MiCO system */
	utc_time_ms = ((uint64_t) mktime( &utc_time ) * (uint64_t) 1000);
	mico_time_set_utc_time_ms( &utc_time_ms );


#ifdef DEBUG1
	iso8601_time_t iso8601_time;
	mico_time_get_iso8601_time( &iso8601_time );
	xlink_log("Current time is: %.26s\n", (char*)&iso8601_time);
#endif

	if(0 == is_recalctime)
	{	
		is_recalctime =1;
		reCalcTime();
	}
	
}

XLINK_FUNC void static updateSystemTime() 
{
//	xlink_log("updateSystemTime\r\n");
	XLINK_SYS_TIME time;
	memset(&time, 0, sizeof(XLINK_SYS_TIME));
	XlinkGetServerTime();
	XlinkGetSystemTime(&time);
	
	struct tm  utc_time;
	mico_utc_time_ms_t utc_time_ms;

	memset(&utc_time, 0, sizeof(struct tm));
	utc_time.tm_sec = time.sec;
	utc_time.tm_min = time.min;
	utc_time.tm_hour = time.hour;
	utc_time.tm_mday = time.day;
	utc_time.tm_wday = time.week;
	utc_time.tm_mon = time.mon - 1;
	utc_time.tm_year = time.year - 1900;
	utc_time.tm_isdst = -1;
//	xlink_log("time->year: %d time->mon:%d Current RTC Time: %s", time.year, time.mon, asctime(&utc_time));

	/* Set UTC time to MiCO system */
	utc_time_ms = ((uint64_t) mktime( &utc_time ) * (uint64_t) 1000);
	mico_time_set_utc_time_ms( &utc_time_ms );



#ifdef DEBUG1
	iso8601_time_t iso8601_time;
	mico_time_get_iso8601_time( &iso8601_time );
	xlink_log("Current time is: %.26s\n", (char*)&iso8601_time);
#endif

}


void xlinkhttpStaCallBack_t(const unsigned int fileSize, const unsigned int rate, const unsigned int sta) {

}


void xlinkhttpRecvDataCallBack_t(const char *data, unsigned int datalen, const unsigned int fileSize,
		const unsigned int offset) {
        
    printf("http total %d  offset %d  datalen %d\r\n", fileSize,offset, datalen);
    
    fileLength =  fileSize;  
    file_totallen += datalen;
    upgrade_process((char *)data,datalen);
    
    CRC16_Update( &CrcContex, (unsigned char*)data,datalen);
    
    unsigned int t = mico_rtos_get_time();

    if(t > targtime){
        targtime  = t+30;
      //  MicoGpioOutputTrigger((mico_gpio_t)MICO_SYS_LED); 
    }
          
}



int xlinkhttpErrorCallBack_t(const unsigned char errSta) {
	switch (errSta) {
	case HTTP_STA_GETHOST_FAILED: 
		printf("DNS Resolve failed\r\n");
		break;
	case HTTP_STA_GETHOST_SUCCESS:
		printf("DNS Resolve successed\r\n");
		break;
	case HTTP_STA_CREATESOCKET_FAILED:
		printf("create socket falied\r\n");
		break;
	case HTTP_STA_CREATESOCKET_SUCCESS://
		printf("create socket succesfully\r\n");
		break;
	case HTTP_STA_SOCKET_CLOSE:
		printf("socket close\r\n");
		break;
	case HTTP_STA_REMOTE_CLOSE_SOCKET:
		printf("remote close socket \r\n");
		break;
	case HTTP_STA_CONECT_FAILED:
		printf("connect server failed\r\n");
		break;
	case HTTP_STA_CONECT_SUCCESS:
		printf("connect server successfully\r\n");
		break;
	case HTTP_STA_MD5_CHECK_SUCCESS:
		printf("md5 check ok\r\n");
                upMd5IsOk = 1;
		break;
	case HTTP_STA_MD5_CHECK_FAILED:
		printf("md5 check failed\r\n");
                upMd5IsOk=0;
		break;
	case HTTP_STA_DOWN_COMPLETE:
        printf("download finished\r\n");
		break;
	case HTTP_STA_SEND_REQUEST_FAILED:
		printf(" send request failed\r\n");
		break;
	case HTTP_STA_DOWN_TIMEOUT:
		printf(" download out of time\r\n");
		break;
	default:
		break;
	}
    if(csta == errSta){
        cstaCount++;
        if(cstaCount > 5)
        {
          return 1;
        }
    }else{
        cstaCount = 0;
        csta = errSta;
    }
	return 0;
}


void ota_function_thread(uint32_t inContext)
{
      (void)inContext;
     
      uint16_t crc = 0;
      XLINK_HTTPDOWN down;
      
      memset(&down,0,sizeof(down));
      down.m_errCallBack = xlinkhttpErrorCallBack_t;
      down.m_recvCallBack = xlinkhttpRecvDataCallBack_t;
      down.m_staCallBack = xlinkhttpStaCallBack_t;
      
      down.m_urlLength = sprintf(down.m_url, "%s",  urlbuf);
      sprintf(down.md5str, "%s", mdbbuf);
           
      ready_upgrade();
     
      CRC16_Init( &CrcContex );

      upMd5IsOk = 0;
      wifi_ota_glitter_start((int)0);
	  
      xlinkHttpStartDownFile(&down);
     
      CRC16_Final( &CrcContex, &crc);       
      if(upMd5IsOk == 1){
         printf("----the total file len=%d\r\n",file_totallen);

         upgrade_done(crc,file_totallen);
      }  
      isupgrade = 0;
      xlink_msleep(3000);

      mico_rtos_delete_thread( NULL );   
}


void upgrade_task(XLINK_UPGRADE *data)
{

    printf("%s in, isupgrade=%d, urlLength=%d\n",__FUNCTION__, isupgrade,data->urlLength );

    if(isupgrade==1)
      return;
    isupgrade = 1;
    
    if(data->urlLength == 0 || data->urlstr==NULL){
	 printf("pramar is err...%d\n");
        return;
    	}
    memset(urlbuf,0,256);
    memcpy(urlbuf,data->urlstr,data->urlLength);

    memcpy(mdbbuf,data->checkStr,data->checkStrLength);
    
    printf("get urlength=%d\n",data->urlLength);
    printf("get mCheckFlag=%d  md5lenth=%d md5str=%s\n",data->mCheckFlag,data->checkStrLength,mdbbuf);
    setOtaFileMd5Str(mdbbuf, data->checkStrLength);

    //xlink_msleep(1000);
    OSStatus err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "ota_function_thread", ota_function_thread, 0xC00, 0);
    require_noerr_string( err, exit, "ERROR: Unable to start the xlink thread" );

    return ;
	
exit:
  mico_rtos_delete_thread( NULL );
  return;
}


static void SetDataPiont(unsigned char *data, int datalen)
{
#if 0
	if(datalen <= SET_DP_MAX_LEN) {
		memcpy(server_set_dp,data,datalen);
		IsSendDataLen = datalen;
		IsSendDataFlag = 1;
		IsSendDataMode = 2;
	}
#endif

}


static void GetAllDataPiont(unsigned char *data, int *datalen)
{
#if 0
    unsigned char i=0;
    unsigned short j=0;
    int len_dp = 0;

    len_dp = *datalen;
    *datalen = 0;

    for(i=0; i < MAX_DP_NUM ;i++){
        if(datap_int_buf[i].isuse == 1){
           switch(datap_int_buf[i].dp_type){
                case DP_BYTE:
                    if((len_dp-j) < 4){
                        return;
                    }
                    data[j++] = i;
                    data[j++] = DP_BYTE;
                    data[j++] = 1;
                    data[j++] = datap_int_buf[i].i_buf[0];
#if DATAPOINT_DEBUG_ON
                    xlink_printf("get datap byte  j=%d........\r\n",j);
#endif
                    break;
                case DP_INT16:
                    if((len_dp-j) < 5){
                        return;
                    }
                    data[j++] = i;
                    data[j++] = DP_INT16;
                    data[j++] = 2;
                    data[j++] = datap_int_buf[i].i_buf[0];
                    data[j++] = datap_int_buf[i].i_buf[1];
#if DATAPOINT_DEBUG_ON
                    xlink_printf("get datap DP_INT16 j=%d........\r\n",j);
#endif
                    break;
                case DP_UINT16:
                    if((len_dp-j) < 5){
                        return;
                    }
                    data[j++] = i;
                    data[j++] = DP_UINT16;
                    data[j++] = 2;
                    data[j++] = datap_int_buf[i].i_buf[0];
                    data[j++] = datap_int_buf[i].i_buf[1];
#if DATAPOINT_DEBUG_ON
                    xlink_printf("get datap DP_UINT16 j=%d........\r\n",j);
#endif
                    break;
                case DP_INT32:
                    if((len_dp-j) < 7){
                        return;
                    }
                    data[j++] = i;
                    data[j++] = DP_INT32;
                    data[j++] = 4;
                    data[j++] = datap_int_buf[i].i_buf[0];
                    data[j++] = datap_int_buf[i].i_buf[1];
                    data[j++] = datap_int_buf[i].i_buf[2];
                    data[j++] = datap_int_buf[i].i_buf[3];
#if DATAPOINT_DEBUG_ON
                    xlink_printf("get datap DP_INT32 j=%d........\r\n",j);
#endif
                    break;
                case DP_UINT32:
                    if((len_dp-j) < 7){
                        return;
                    }
                    data[j++] = i;
                    data[j++] = DP_UINT32;
                    data[j++] = 4;
                    data[j++] = datap_int_buf[i].i_buf[0];
                    data[j++] = datap_int_buf[i].i_buf[1];
                    data[j++] = datap_int_buf[i].i_buf[2];
                    data[j++] = datap_int_buf[i].i_buf[3];
#if DATAPOINT_DEBUG_ON
                     xlink_printf("get datap DP_UINT32 j=%d........\r\n",j);
#endif
                    break;
                case DP_FLOAT:
                    if((len_dp-j) < 7){
                        return;
                    }
                    data[j++] = i;
                    data[j++] = DP_FLOAT;
                    data[j++] = 4;
                    data[j++] = datap_int_buf[i].i_buf[0];
                    data[j++] = datap_int_buf[i].i_buf[1];
                    data[j++] = datap_int_buf[i].i_buf[2];
                    data[j++] = datap_int_buf[i].i_buf[3];
#if DATAPOINT_DEBUG_ON
                     xlink_printf("get datap DP_FLOAT j=%d........\r\n",j);
#endif
                    break;


                case DP_STRING:
                    if((len_dp-j) < (datap_str_buf[datap_int_buf[i].i_buf[0]].str_len+3)){
                        return;
                    }
                    data[j++] = i;
                    data[j++] = DP_STRING;
                    data[j++] = datap_str_buf[datap_int_buf[i].i_buf[0]].str_len;

                    memcpy(&data[j],datap_str_buf[datap_int_buf[i].i_buf[0]].s_buf,
                            datap_str_buf[datap_int_buf[i].i_buf[0]].str_len);

                    j += datap_str_buf[datap_int_buf[i].i_buf[0]].str_len;

#if DATAPOINT_DEBUG_ON
                    xlink_printf("get datap DP_STRING. j=%d........\r\n",j);
#endif
                    break;

                case DP_BINARY:
                    if((len_dp-j) < (datap_str_buf[datap_int_buf[i].i_buf[0]].str_len+3)){
                        return;
                    }
                    data[j++] = i;
                    data[j++] = DP_BINARY;
                    data[j++] = datap_str_buf[datap_int_buf[i].i_buf[0]].str_len;
                    memcpy(&data[j],datap_str_buf[datap_int_buf[i].i_buf[0]].s_buf,
                            datap_str_buf[datap_int_buf[i].i_buf[0]].str_len);
                    j += datap_str_buf[datap_int_buf[i].i_buf[0]].str_len;
#if DATAPOINT_DEBUG_ON
                    xlink_printf("get datap DP_BINARY j=%d........\r\n",j);
#endif
                    break;

                default:
#if DATAPOINT_DEBUG_ON
                   xlink_printf("get datap default error\r\n",j);
#endif
                   return;
           }
        }
    }
    *datalen = j;

#if DATAPOINT_DEBUG_ON
    xlink_printf(" get datapoint len=%d....\r\n",*datalen);
#endif
#endif

}



void Xlink_tcp_pipe_send_cb(unsigned short handle,unsigned char val)
{
    	//xlink_log("****tcp_pipe_send_cb handle=%d,val=%d****\r\n",handle,val);
    	//if(val)
}

void Xlink_tcp_pipe2_send_cb(unsigned short handle,unsigned char val)
{
      //xlink_log("****tcp_pipe2_send_cb handle=%d,val=%d****\r\n",handle,val);
     // if(val)
   // if(val){
    	//xlink_send_cb_status(XLINK_PTR_MCU_TO_WIFI_TRAN_T);
    //}
}

void Xlink_udp_pipe_send_cb(unsigned short handle,unsigned char val)
{
    //xlink_log("****udp_pipe_send_cb handle=%d,val=%d****\r\n",handle,val);
}

void Xlink_tcp_datapoint_send_cb(unsigned short handle,unsigned char val)
{
    // printf("****tcp_datapoint_send_cb handle=%d,val=%d****\r\n",handle,val);

 #if 0
     if(val){
      switch(cmd_type)
      {
		  case XLINK_PTR_MCU_TO_WIFI_TRAN_DP:
		  case XLINK_PTR_MCU_TO_WIFI_TRAN_ALLDP_NO_ALARM:
			  xlink_send_cb_status(cmd_type);
			  break;
		  default:
			  break;
      }
    }
#endif
	 
}

void xlink_notify_cb(unsigned short messagetpye,unsigned char * data, unsigned int datalen)
{
	//   int i=0;
	//   printf("----notify----messagetpye=%d-----------\r\n",messagetpye);
	//   printf("----notify----datalen=%d-----------\r\n",datalen);
	//   printf("notify data=");
	//   for(i=0;i<datalen;i++){
	//	   printf("%02X",data[i]);
	//  }
	//  printf("\r\n");
}

XLINK_FUNC void xlink_resolve_mcu_cmd(void)
{

}



XLINK_FUNC void xlink_demo_work_init(void) {
  
	char proKey[33];
        
	user_config.tcp_pipe2 = NULL;
	user_config.tcp_pipe = pipe1_call;
	user_config.udp_pipe = udp_pipe_call;
	user_config.status = app_status;
	user_config.DebugPrintf = NULL;//printf;
	user_config.upgrade = upgrade_task;
	user_config.server_time = time_callback;
	user_config.wifi_type = 1;
	user_config.wifisoftVersion = 17;
	user_config.in_internet = 1;
	//user_config.pipetype = 1;

	user_config.mcuHardwareVersion  =user_config.wifi_type;
	user_config.mcuHardwareSoftVersion =user_config.wifisoftVersion;
        
	user_config.Xlink_GetAllDataPoint = GetAllDataPiont;
	user_config.Xlink_SetDataPoint = SetDataPiont;
    
	user_config.tcp_notify = xlink_notify_cb;
	user_config.tcp_datapoint_send_cb = Xlink_tcp_datapoint_send_cb;
	user_config.tcp_pipe2_send_cb = Xlink_tcp_pipe2_send_cb;
	user_config.tcp_pipe_send_cb = Xlink_tcp_pipe_send_cb;
	user_config.udp_pipe_send_cb = Xlink_udp_pipe_send_cb;	
        	
       user_config.maclen = 6;
        
	xlinkConfigInit(&user_config);
	XlinkReadProductIDKEY(&xlink_start_flag,m_pID, proKey);
	m_pID[32] = '\0';
	proKey[32] = '\0';
	sprintf(m_pID, "160fa6b1b8a903e9160fa6b1b8a93201");
	sprintf(proKey, "04570425e2d50581e9020a43c31c73ff");
	
	if(Xlink_PTP_Init() == 0) {

	}


	xlink_log("XlinkSystemInit start--user_config.wifisoftVersion:%d ----\r\n", user_config.wifisoftVersion);
	if (XlinkSystemInit(m_pID, proKey, &user_config) == 0) {
		xlink_log("XlinkSystemInit failed------\r\n");
	}else{
		XlinkGetServerTime();
	}

	xlink_log("m_pID=%s proKey=%s\n", m_pID, proKey);
	xlink_log("Bootloader version: %s\r\n", mico_get_bootloader_ver());
	
	XlinkPorcess_UDP_Enable();

}

void check_soft_version_upgrade(void)
{
//xlink_log("isUpgrade:%d, isLogined:%d\r\n",g_xlink_info.config.flag.Bit.isUpgrade , g_xlink_info.net_info.tcpFlag.Bit.isLogined);
//xlink_log("CurrentSoftVersion:%d, wifisoftVersion:%d\r\n",g_xlink_info.config.CurrentSoftVersion , g_xlink_user_config->wifisoftVersion);

	if(g_xlink_info.config.flag.Bit.isUpgrade == 1 && g_xlink_info.net_info.tcpFlag.Bit.isLogined == 1 )
	{
		if(g_xlink_info.config.CurrentSoftVersion < g_xlink_user_config->wifisoftVersion){
			xlink_tcp_send_upgrade(0,g_xlink_user_config->wifisoftVersion, g_xlink_info.config.CurrentSoftVersion);
			g_xlink_info.config.flag.Bit.isUpgrade = 0;
			g_xlink_info.config.CurrentSoftVersion = g_xlink_user_config->wifisoftVersion;
			xsdk_write_config(XLINK_CONFIG_INDEX_CONFIG, (char*) &g_xlink_info.config, sizeof(xlink_SdkConfig));
			xsdk_config_save();
			
		}
	}
}

#if 0
void test_pingcmd_thread(uint32_t inContext)
{
	char *str[]={"ping", "14.215.177.39"};//14.215.177.39 192.168.3.1


	while(1)
	{	
		if(g_AllSta.bit.isConnectWIFI)
		ping_Command("ping 14.215.177.39",strlen("ping 14.215.177.39"), 2, str);

		mico_rtos_thread_sleep(1);
	}
}
#endif

void XLINK_FUNC xlink_function_thread(uint32_t inContext)
{         
	time_t t=0;

	while(1)
	{     
		t = mico_rtos_get_time()/1000;
		XlinkSystemLoop(t, 30);
		
		popAppMainSendMsgThread(0);
		
		mico_rtos_lock_mutex(&tcp_send_sta_mutex);
		server_tcp_send_time_out = 0;
		mico_rtos_unlock_mutex(&tcp_send_sta_mutex);
	}     
}


void xlink_domain_loop(uint32_t inContext)
{
	(void)inContext;
	int tcp_send_time_out;
	uint8_t  syncTimeTimes = 0;

	while(1)
	{
		XlinkSystemTcpLoop();
		mico_thread_sleep(3);
		check_soft_version_upgrade();
		mico_rtos_lock_mutex(&tcp_send_sta_mutex);
		tcp_send_time_out = server_tcp_send_time_out;
		server_tcp_send_time_out += 1;
		mico_rtos_unlock_mutex(&tcp_send_sta_mutex);

		check_network_state_is_traffic_block();
		
		if(tcp_send_time_out > 4)
		{
			xlink_log("tcp_send_time_out>4.......reboot");
			mico_system_power_perform(mico_system_context_get(), eState_Software_Reset);			
		}

		syncTimeTimes += 1;
		if(syncTimeTimes > 20)
		{
			syncTimeTimes = 0;
			updateSystemTime();
		}
	}
}


