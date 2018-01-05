#ifndef PANELMANAGER_H
#define PANELMANAGER_H

#include "MICO.h"
#include "cJSON.h"

#include "zigbeeparse.h"

typedef struct _DEVICE_INFO_S
{
	uint8_t panel_mac[DEVICE_MAC_ADDRESS_LENGHT];
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	uint32_t dev_type;
	uint16_t panel_nwk;

	uint16_t dev_nwkid;
	uint16_t dev_nclu;
	uint8_t  endpoit_id;
	uint8_t  chnl;
	uint32_t  dev_val;
		
	uint8_t  panel_chnl;
	uint8_t  action_cmd;
}DEVICE_INFO_T;

typedef struct _PANEL_BINDING_DEVICE_INFO_S
{
	uint8_t mac[DEVICE_MAC_ADDRESS_LENGHT];
	uint32_t dev_type;
	uint8_t  chnl;
	uint8_t  endpoit_id;
}PANEL_BINDING_DEVICE_INFO_T;

#endif

