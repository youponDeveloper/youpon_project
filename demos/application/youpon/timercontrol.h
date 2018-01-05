#ifndef __TIMERCONTROL_H__
#define __TIMERCONTROL_H__

#include "MICO.h"
#include "cJSON.h"
#include "user_main.h"

typedef struct _TIMER_CTRL_PARAM_INFO_S
{
	uint8_t ctrl_id;
	int8_t ctrlname[32];
	uint8_t status;
	uint8_t ctrl_type;
	uint8_t obj_id;//scene id or device endpoint id
	int32_t cmd;
	int32_t value;
	uint8_t act_times;
	uint8_t timer_inter;
	int32_t timer_week;
	uint32_t timer_time_exe;
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
}TIMER_CTRL_PARAM_INFO_T;

int timer_ctrl_new_add_request(cJSON *root);
int timer_ctrl_modify_request(cJSON *root);
int timer_ctrl_delete_request(cJSON *root);
int timer_ctrl_start_request(cJSON *root);
int timer_ctrl_stop_request(cJSON *root);
int timer_list_request();
int timer_control_update_status_request(TIMER_CTRL_PARAM_INFO_T *pLinkageInfo);
int timer_control_delete_by_devmac(char *pMac);
extern int removeInvalidTimerItem(uint8_t mac[]);
extern int clearInvalidTimerItem();

#endif

