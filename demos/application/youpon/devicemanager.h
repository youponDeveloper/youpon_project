#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "MICO.h"
#include "cJSON.h"

#include "zigbeeparse.h"
#include "panelmanager.h"
#include "rs485parse.h"
#include "timerinterexec.h"
#include "rs485sensorparse.h"

int zigbee_new_device_add_report(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
int zigbee_new_device_add_report2(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
int update_device_state(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
int device_logout_update_flag(uint16_t nwk_addr);
int device_onoffline_update_flag(uint16_t nwk_addr, uint8_t flag);
int device_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
int initDeviceListOnlineStatus();
int get_device_mac(uint16_t nwk_addr, int8_t mac[]);
int device_connected_update_status(DEVICE_INFO_T *pDevInfo);
void print_device_infomation();
int rs485_new_device_add_report(RS485_DATA_FORMAT_T *pDevInfo);
int rs485_sensor_device_onoffline_update_flag(uint16_t nwk_addr, uint8_t flag);
void rs485_request_subdevice_state();
int get_device_param_info(struct device_param_info_struct *pDeviceInfo);
int zigbee_sensor_linkage_turnon_light_device(linkage_param_t  *plinkage_param);
int zigbee_sensor_linkage_control_airchange(linkage_param_t  *plinkage_param);
int kitchen_panel_device_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
int device_light_one_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
int device_light_two_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
int device_light_three_report_attribute_update(ZB_DEV_JOINED_REQUEST_T *pDevInfo);
extern int checkDeviceExist(uint8_t mac[]);
int rs485_new_device_joined_success_ack(RS485_DATA_FORMAT_T *pRs485Data);
int getGatewayRs485Device(uint8_t mac[]);
int clear_all_device_restore_factory_set(void);
int clear_all_rs485_device(void);
int save_scenelist_parame(uint16_t netaddr, uint16_t groupid, uint8_t sceneid);
int delet_dev_from_scenelist(uint16_t net_addr);
int close_all_rs485_device(void);
//int devicechldata_syn_scenedata(uint16_t groupid, uint8_t sceneid);
//int scenepanel_device_attribute_update(uint8_t cmd,uint16_t groupid, uint8_t sceneid);

#endif
