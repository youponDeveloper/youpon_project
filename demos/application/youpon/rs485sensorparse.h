#ifndef _RS485SENSORPARSE_H__
#define _RS485SENSORPARSE_H__

#include "mico.h"
#include "rs485parse.h"

#define SENSOR_LIGHTRAY_PILE_ID			(0x0400)
#define SENSOR_TEMPERETURE_PILE_ID		(0x0402)
#define SENSOR_HUMIDITY_PILE_ID			(0x0405)
#define SENSOR_IR_PILE_ID					(0x0406)
#define SENSOR_GAS_PILE_ID					(0xFC01)


typedef struct linkage_param_struct{
	uint8_t  mac[DEVICE_MAC_ADDRESS_LENGHT];
	int32_t value;
	int32_t value2;
	uint8_t sensor_type;
	uint8_t sensor_type2;

	uint8_t ctrl_type; //  1->single control  2->scene control
	uint8_t obj_id;
	int32_t param;

}linkage_param_t;

typedef struct sensor_type_param_s{
	uint8_t  dev_mac[DEVICE_MAC_ADDRESS_LENGHT];
	int32_t device_index;
	int32_t wValue;
	uint16_t dev_addr;
	uint16_t pileId;
	uint16_t attrId;
	uint16_t chn_id;
	uint32_t sensor_type;
}sensor_type_param_t;


typedef enum
{
	SENSOR_ID_IR=0,
	SENSOR_ID_LIGHTRAY,
	SENSOR_ID_CO2,
	SENSOR_ID_TVOC,
	SENSOR_ID_SMOKE,
	SENSOR_ID_COMBUSTIBLE,
	SENSOR_ID_TEMPR,
	SENSOR_ID_HUMIDITY,
	SENSOR_ID_MAX
} SENSOR_TYPE_CHANL_INDEX;

OSStatus clear_sensor_linkage_flag();

#endif

