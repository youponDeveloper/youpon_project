
#include "mico.h"
#include "CheckSumUtils.h"
#include "mico_system.h"

#include "OTAupgrade.h"
#include "xlink_MXCHIP_mico.h"

#define xlink_update_log(M, ...)  custom_log("XLINK_UPDATE", M, ##__VA_ARGS__)
#define xlink_update_log_trace()  custom_log_trace("XLINK_UPDATE")

mico_logic_partition_t *ota_partition ;
//md5_context ctx;

//CRC16_Context contex;
#define BIN_BUFF_LEN                (2048)

uint32_t read_offset = 0x00;
uint32_t write_offset = 0x00;
static unsigned char ota_file_md5[128] = {0};

void setOtaFileMd5Str(uint8_t md5[], int len)
{
	if(len > 128)len=128;
	memcpy(ota_file_md5, md5, len);
}

static void FOTA_WifiStatusHandler_t(WiFiEvent event, void * arg)
{
    UNUSED_PARAMETER(arg);
    switch (event) {
    case NOTIFY_STATION_UP:
      //wifi_up = 1;
      break;
    case NOTIFY_STATION_DOWN:
      //wifi_up = 0;
      break;
   
    default:
      break;
    }
    return;
}

int ready_upgrade(void)
{
    OSStatus err = kNoErr;    
      
    ///network_InitTypeDef_st conf;
    ota_partition = MicoFlashGetInfo(MICO_PARTITION_OTA_TEMP ); 

    mico_system_notify_remove_all(mico_notify_WIFI_STATUS_CHANGED);
    mico_system_notify_remove_all(mico_notify_WiFI_PARA_CHANGED);
    mico_system_notify_remove_all(mico_notify_DHCP_COMPLETED);
    mico_system_notify_remove_all(mico_notify_WIFI_CONNECT_FAILED);
    mico_system_notify_remove_all(mico_notify_EASYLINK_WPS_COMPLETED);
    mico_system_notify_register( mico_notify_WIFI_STATUS_CHANGED, (void *)FOTA_WifiStatusHandler_t, NULL );
    micoWlanStopEasyLink();
    micoWlanStopEasyLinkPlus();
    micoWlanStopAirkiss();
    mico_thread_msleep(10);
         
   // write_offset = 0;
    err = MicoFlashErase(MICO_PARTITION_OTA_TEMP, '\0', ota_partition->partition_length);
    require_noerr(err, exit);
    
    printf("ready upgrade firmware start---------------\r\n");

exit: 
  if(err!=kNoErr) xlink_update_log("ready_upgrade err");
  return err;  
  
}

int upgrade_process(char *get_data,int data_len)
{
    OSStatus err = kNoErr;    
   
    err = MicoFlashWrite(MICO_PARTITION_OTA_TEMP, &read_offset, (uint8_t*)get_data, data_len);
    xlink_msleep(20);
    xlink_update_log("read_offset = %d\r\n",read_offset);

    require_noerr(err, exit);      
exit: 
  if(err!=kNoErr) xlink_update_log("upgrade_process err=%d\r\n",err);
  return err;          
}

static unsigned char* user_strupr(unsigned char* szMsg)
{
    unsigned char *pcMsg = NULL;

    for (pcMsg = szMsg; ('\0' != *pcMsg); pcMsg++)
    {
        if (('a' <= *pcMsg) && (*pcMsg <= 'z'))
        {
            *pcMsg += ('A' - 'a');
        }
    }

    return szMsg;
}

/*
static bool user_str2hex(unsigned char *src, uint8_t *dest, uint32_t dest_size)
{
    unsigned char hb = 0;
    unsigned char lb = 0;
    uint32_t i = 0, j = 0;
    uint32_t src_size = strlen((const char *)src);

    if ( (src_size % 2 != 0) || (src_size <= 0))
        return false;

    src = user_strupr( src );

    for ( i = 0; i < src_size; i ++ )
    {
        if(i > dest_size * 2)
            return false;

        hb = src[i];
        if ( hb >= 'A' && hb <= 'F' )
            hb = hb - 'A' + 10;
        else if ( hb >= '0' && hb <= '9' )
            hb = hb - '0';
        else
            return false;

        i++;
        lb = src[i];
        if ( lb >= 'A' && lb <= 'F' )
            lb = lb - 'A' + 10;
        else if ( lb >= '0' && lb <= '9' )
            lb = lb - '0';
        else
            return false;

        dest[j++] = (hb << 4) | (lb);
    }

    return true;
}
*/
extern void zigbee_reset(void);
void upgrade_done(uint16_t crc ,int filelen ){

   //server_ota_finish( filelen);
#if 0
    mico_logic_partition_t* ota_partition = MicoFlashGetInfo( MICO_PARTITION_OTA_TEMP );
    mico_Context_t* mico_ctx = mico_system_context_get();
	
    memset(&mico_ctx->bootTable, 0, sizeof(boot_table_t));
    mico_ctx->bootTable.length = filelen;
    mico_ctx->bootTable.start_address = ota_partition->partition_start_addr;
    mico_ctx->bootTable.type = 'A';
    mico_ctx->bootTable.upgrade_type = 'U';
    mico_ctx->bootTable.crc = crc;  // pass crc16 to bootloader to check update
    mico_system_context_update( mico_ctx );

    xlink_update_log("start to reboot........\r\n");
    mico_system_power_perform( mico_ctx, eState_Software_Reset );
    mico_thread_sleep( MICO_WAIT_FOREVER );
#endif


    mico_ota_switch_to_new_fw( filelen, crc );

    xlink_update_log("MicoSystemReboot before ....................!\n");


    zigbee_reset();
    mico_thread_msleep(1000);

    MicoSystemReboot();
}
