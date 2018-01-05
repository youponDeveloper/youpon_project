#include "user_data_store.h"

#include "mico.h"
#include "platform.h"
#include "platform_config.h"


#define user_data_store(M, ...) custom_log("user_data_store", M, ##__VA_ARGS__)

user_data_store_t  *puser_data_in_flash = NULL; 

static int XLINK_FUNC user_data_flash_write_process( mico_partition_t section_index, 
                 volatile uint32_t* off_set, uint8_t* inBuffer ,uint32_t inBufferLength);

extern app_context_t*  get_app_context();

user_data_store_t *get_user_data_store_context()
{
	return puser_data_in_flash;
}

void * userDataStoreInit() 
{
	uint32_t offset=0;
	int ret =0;
	user_data_store("************************** size:%d ", sizeof(user_data_store_t));
	puser_data_in_flash = calloc(1, sizeof(user_data_store_t));

	ret = MicoFlashRead(USER_DATA_STORE_PARAMETER_1, &offset, (uint8_t *)&puser_data_in_flash->userData, sizeof(user_data_in_flash_t));
	return puser_data_in_flash;
}

static int XLINK_FUNC user_data_flash_write_process( mico_partition_t section_index,
             volatile uint32_t* off_set, uint8_t* inBuffer ,uint32_t inBufferLength)
{
        OSStatus err = kNoErr;
        mico_logic_partition_t *partition;
	 uint32_t erase_offset = *off_set;

        partition = MicoFlashGetInfo(section_index);
	//	user_data_store("************************** partition->partition_length:%d ", partition->partition_length);
        err = MicoFlashErase(section_index, erase_offset, inBufferLength);
        require_noerr(err, exit);

        err = MicoFlashWrite(section_index, off_set, inBuffer, inBufferLength);
        require_noerr(err, exit);

        return inBufferLength;
exit:
  return err;
}


int XLINK_FUNC user_data_store_update() 
{

       int ret=0;
       uint32_t offset=0;
       ret = user_data_flash_write_process(USER_DATA_STORE_PARAMETER_1,
             &offset, (uint8_t*)&puser_data_in_flash->userData, sizeof(user_data_in_flash_t));

	 user_data_store("************************** user_data_store_update ret:%d ", ret);
       return ret;
}

int XLINK_FUNC xlink_config_update() 
{

	int ret=0;
	//uint32_t offset=0x0;
	//  ret = user_data_flash_write_process(USER_DATA_STORE_PARAMETER_1,
	//      &offset, (uint8_t*)&puser_data_in_flash->xlinkconfig, sizeof(xlink_config_info));
	//
	//	 user_data_store("************************** xlink_config_update ret:%d ", ret);
       return ret;
}

