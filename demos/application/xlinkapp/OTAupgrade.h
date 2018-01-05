#ifndef __OTAUPGRADE_H__
#define __OTAUPGRADE_H__


#include "mico.h"

extern int upgrade_process(char *get_data,int data_len);
extern  int ready_upgrade(void);
extern void upgrade_done(uint16_t crc ,int filelen );

#endif















