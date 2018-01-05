
#include "panelmanager.h"
#include "user_config.h"
#include "system_internal.h"
#include "rs485parse.h"

#define panel_mng(M, ...) custom_log("panel_mng", M, ##__VA_ARGS__)

extern app_context_t*  get_app_context();
extern void HexDump(uint8_t * pdata, uint32_t data_len);
extern system_context_t* get_sys_context();
extern uint8_t* getMacAddress();


