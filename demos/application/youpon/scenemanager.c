#include "scenemanager.h"

#define scene_mng(M, ...) custom_log("scene_mng", M, ##__VA_ARGS__)

extern app_context_t*  get_app_context();
extern  unsigned int charchangehex( unsigned char *data , unsigned char* value );
extern void HexDump(uint8_t * pdata, uint32_t data_len);


