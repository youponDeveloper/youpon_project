#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include "MICO.h"
#include "cJSON.h"
#include "zigbeeparse.h"

OSStatus  sensor_attribute_report(cJSON *root,  ZB_ZCL_ZDO_MSG_T *pZclZdoMsgInfo);
int zigbee_sensor_new_device_add_report(ZB_DEV_JOINED_REQUEST_T *pDevInfo, uint8_t mac[]);
void zigbee_sensor_linkage_param_init();
int  rs485_add_sensor_device2list(uint16_t dev_addr, uint16_t devType, uint8_t rs485dev_mac[]);
int  rs485_sensor_add_integradedevice2list(uint16_t dev_addr, uint8_t devType[], uint8_t device_type_count, uint8_t rs485dev_mac[]);
int removeSensorDevice( char mac_str2[], char nwk_str2[] );

#endif

