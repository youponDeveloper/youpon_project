#ifndef  __LINKAGEMANAGER_H__
#define  __LINKAGEMANAGER_H__

#include "MICO.h"
#include "cJSON.h"
#include "rs485sensorparse.h"

typedef struct _LINKAGE_CTRL_CONDS_S
{
	uint8_t mac[16];
	int8_t env_cond[4];
	int32_t value;
	uint8_t sensor_type;
}LINKAGE_CTRL_CONDS_T;


typedef struct _LINKAGE_CTRL_INFO_S
{
	LINKAGE_CTRL_CONDS_T linkageCond[2];
	uint8_t ctrl_id;
	int8_t ctrlname[32];
	uint8_t dev_mac[16];
	uint8_t status;
	int8_t env_cond[4];
	uint8_t ctrl_type;
	uint8_t obj_id;
	int32_t cmd;
	int32_t value;
	uint8_t conds_size;
}LINKAGE_CTRL_INFO_T;

int linkage_ctrl_new_add_request(cJSON *root);
int linkage_ctrl_modify_request(cJSON *root);
int linkage_ctrl_delete_request(cJSON *root);
int linkage_ctrl_start_request(cJSON *root);
int linkage_ctrl_stop_request(cJSON *root);
int linkage_list_request();
int linkage_control_update_status_request(LINKAGE_CTRL_INFO_T *pLinkageInfo);
int zigbee_check_linakge_airchange_ctrl_condistions(linkage_param_t  *plinkage_param);
int zigbee_check_ir_lightray_linakge_ctrl_condistions(linkage_param_t  *plinkage_param );
int linkage_deleteFixedMacItem(char *pMac);

#endif

