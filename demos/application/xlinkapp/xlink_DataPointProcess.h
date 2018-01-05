#ifndef __XLINK_DATAPOINTPROCESS_H__
#define __XLINK_DATAPOINTPROCESS_H__

#include "../xlinksrc/xlink_data.h"
//#include "xlink_data.h"

#define DP_STRBUF_LEN   64  //string buf len
#define MAX_DP_STR_NUM  32  //string type max num

#define DP_INTBUF_LEN   4   //bool,byte int16,uint16,int32,uint32 buf len
#define MAX_DP_NUM      200 //bool,byte int16,uint16,int32,uint32 max num

typedef struct{
	unsigned char isuse;   					//check buf if have use
	unsigned char dp_type;                 //data point type
    unsigned char i_buf[DP_INTBUF_LEN];    //char,short,int buf len
}dp_int_fn;


typedef struct {
	unsigned char isuse;   				  //check buf if have use

#if DP_STRBUF_LEN > 255
    unsigned short str_len;
#else
	unsigned char str_len;
#endif
	unsigned char s_buf[DP_STRBUF_LEN];   //string buf max len
}dp_str_tn;


typedef enum
{
	DP_BYTE  = 0x00,
	DP_INT16 = 0x10,
	DP_UINT16 = 0x20,
	DP_INT32 = 0x30,
	DP_UINT32 = 0x40,
	DP_INT64 = 0x50,
	DP_UINT64 = 0x60,
	DP_FLOAT = 0x70,
	DP_DOUBLE = 0x80,
	DP_STRING = 0x90,
	DP_BINARY = 0XA0
}DATAPOINTTYPE;

extern void dp_mem_init(void);
extern void dp_push_process(unsigned char *data,unsigned short datalen);

#endif





















