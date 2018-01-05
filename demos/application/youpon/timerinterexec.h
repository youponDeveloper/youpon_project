#ifndef __TIMERINTEREXC_H__
#define __TIMERINTEREXC_H__

#include "MICO.h"
#include "user_main.h"
#include "timer_task.h"

struct device_param_info_struct {
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	int   endpoint_id;
	int   value;
	uint16_t nclu;
	uint16_t dev_addr;
	uint32_t dev_type;
	uint32_t attrcmd;
	uint32_t riu;
	uint32_t devclass;
};

void zigbee_send_airchange_control(struct device_param_info_struct *pDevParam);
void zigbee_send_light_single_control(struct device_param_info_struct *pDevParam);

#endif

