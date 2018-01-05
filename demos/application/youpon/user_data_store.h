
#ifndef _USER_DATA_STORE_H_
#define _USER_DATA_STORE_H_

#include "xlink_demo.h"
#include "user_main.h"

void * userDataStoreInit();
extern int XLINK_FUNC user_data_store_update();
extern user_data_store_t *get_user_data_store_context();
int XLINK_FUNC xlink_config_update() ;

#endif
