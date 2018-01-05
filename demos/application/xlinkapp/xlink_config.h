
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "xlink_demo.h"



extern void  xlinkConfigInit(XLINK_USER_CONFIG *config);

x_int32 XlinkGetDeviceMac(x_uint8 *mac);
XLINK_FUNC void mac_to_hex(char ch_h, char ch_l, x_uint8 *for_num);

extern int XlinkReadProductIDKEY(unsigned char *flag,char *proid, char *proKey);
extern int XlinkWriteProductIDKEY(unsigned char *flag,char *proid, char *proKey);




#endif
