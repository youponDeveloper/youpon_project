#include "xlink_DataPointProcess.h"


dp_int_fn  datap_int_buf[MAX_DP_NUM];
dp_str_tn  datap_str_buf[MAX_DP_STR_NUM];


void XLINK_FUNC dp_mem_init(void)
{
	unsigned char i=0,j=0;

	for(i=0; i<MAX_DP_NUM; i++){
		xlink_memset(&datap_int_buf[i],0,sizeof(dp_int_fn));
		datap_int_buf[i].i_buf[0]=0xff;
	}

	for(j=0; i<MAX_DP_STR_NUM; j++){
		xlink_memset(&datap_str_buf[i],0,sizeof(dp_str_tn));
	}
}

XLINK_FUNC unsigned char dp_check_type(unsigned char type)
{
	unsigned char res = 0;

	switch(type)
	{
		case DP_BYTE:
		case DP_INT16:
		case DP_UINT16:
		case DP_INT32:
		case DP_UINT32:
		case DP_STRING:
		case DP_BINARY:
		case DP_FLOAT:
			res = 1;
			break;
		default:
			res = 0;
			break;
	}
	return res;
}



void XLINK_FUNC dp_push_process(unsigned char *data,unsigned short datalen)
{
    unsigned char i = 0;
    unsigned short j = 0;
    unsigned short temp = 0;

    if(datalen < 4)return;

#if DATAPOINT_DEBUG_ON
    xlink_printf("resolve datapoint start.....\r\n");
#endif

    for(j = 0; j< datalen;)
    {
       if(data[j] >= MAX_DP_NUM)return;
       if(dp_check_type(data[j+1]) == 0){
    	   return;
       }
       switch(data[j+1] & 0xf0){
			case DP_BYTE:

				datap_int_buf[data[j]].i_buf[0]=data[j+3];
				datap_int_buf[data[j]].isuse=1;
				datap_int_buf[data[j]].dp_type = DP_BYTE;
				j+=4;
#if DATAPOINT_DEBUG_ON
				xlink_printf("resolve byte j=%d.......\r\n",j);
#endif
				break;
			case DP_INT16:

				datap_int_buf[data[j]].i_buf[0]=data[j+3];
				datap_int_buf[data[j]].i_buf[1]=data[j+4];
				datap_int_buf[data[j]].isuse=1;
				datap_int_buf[data[j]].dp_type = DP_INT16;
				j+=5;

#if DATAPOINT_DEBUG_ON
				xlink_printf("resolve DP_INT16 j=%d.......\r\n",j);
#endif
				break;

			case DP_UINT16:

				datap_int_buf[data[j]].i_buf[0]=data[j+3];
				datap_int_buf[data[j]].i_buf[1]=data[j+4];
				datap_int_buf[data[j]].isuse=1;
				datap_int_buf[data[j]].dp_type = DP_UINT16;
				j+=5;

#if DATAPOINT_DEBUG_ON
				xlink_printf("resolve DP_UINT16 j=%d.......\r\n",j);
#endif
				break;
			case DP_INT32:

				datap_int_buf[data[j]].i_buf[0]=data[j+3];
				datap_int_buf[data[j]].i_buf[1]=data[j+4];
				datap_int_buf[data[j]].i_buf[2]=data[j+5];
				datap_int_buf[data[j]].i_buf[3]=data[j+6];
				datap_int_buf[data[j]].isuse=1;
				datap_int_buf[data[j]].dp_type = DP_INT32;
				j+=7;

#if DATAPOINT_DEBUG_ON
				xlink_printf("resolve DP_INT32 j=%d.......\r\n",j);
#endif
				break;
			case DP_UINT32:

				datap_int_buf[data[j]].i_buf[0]=data[j+3];
				datap_int_buf[data[j]].i_buf[1]=data[j+4];
				datap_int_buf[data[j]].i_buf[2]=data[j+5];
				datap_int_buf[data[j]].i_buf[3]=data[j+6];
				datap_int_buf[data[j]].isuse=1;
				datap_int_buf[data[j]].dp_type = DP_UINT32;
				j+=7;

#if DATAPOINT_DEBUG_ON
				xlink_printf("resolve DP_UINT32 j=%d.......\r\n",j);
#endif
				break;
//			case DP_INT64:
//			case DP_UINT64:
//			case DP_DOUBLE:
			case DP_FLOAT:
				datap_int_buf[data[j]].i_buf[0]=data[j+3];
				datap_int_buf[data[j]].i_buf[1]=data[j+4];
				datap_int_buf[data[j]].i_buf[2]=data[j+5];
				datap_int_buf[data[j]].i_buf[3]=data[j+6];
				datap_int_buf[data[j]].isuse=1;
				datap_int_buf[data[j]].dp_type = DP_FLOAT;
				j+=7;

#if DATAPOINT_DEBUG_ON
				xlink_printf("resolve DP_UINT32 j=%d.......\r\n",j);
#endif
				 break;
			case DP_STRING:

				if(datap_int_buf[data[j]].i_buf[0] == 0xff){
					for(i = 0; i < MAX_DP_STR_NUM; i++){
						if(datap_str_buf[i].isuse == 0){
							datap_str_buf[i].isuse = 1;
							datap_int_buf[data[j]].i_buf[0]=i;
							break;
						}
					}
					if(i>=MAX_DP_STR_NUM){
						return;
					}
				}

				if(datap_int_buf[data[j]].i_buf[0] >= MAX_DP_STR_NUM)return;
				temp = data[j+1] & 0x0f;
				temp <<= 8;
				temp += data[j+2];

				if((datalen-j-3) < temp)return;

				//xlink_printf("datalen len=%d.....\r\n",datalen);

				xlink_memcpy(datap_str_buf[datap_int_buf[data[j]].i_buf[0]].s_buf,&data[j+3],temp);


				datap_int_buf[data[j]].isuse = 1;
				datap_int_buf[data[j]].dp_type = DP_STRING;
				datap_str_buf[datap_int_buf[data[j]].i_buf[0]].str_len = temp;

				j+=3+temp;

#if DATAPOINT_DEBUG_ON
				xlink_printf("resolve DP_STRING j=%d.......\r\n",j);
#endif
				break;

			case DP_BINARY:

				if(datap_int_buf[data[j]].i_buf[0] == 0xff){
					for(i = 0; i < MAX_DP_STR_NUM; i++){
						if(datap_str_buf[i].isuse == 0){
							datap_str_buf[i].isuse = 1;
							datap_int_buf[data[j]].i_buf[0]=i;
							break;
						}
					}
					if(i >= MAX_DP_STR_NUM){
						return;
					}
				}

				if(datap_int_buf[data[j]].i_buf[0] >= MAX_DP_STR_NUM)return;
				temp = data[j+1] & 0x0f;
				temp <<= 8;
				temp |= data[j+2];

				if((datalen-j-3) < temp)return;

				xlink_memcpy(datap_str_buf[datap_int_buf[data[j]].i_buf[0]].s_buf,&data[j+3],temp);

				datap_int_buf[data[j]].isuse=1;
				datap_int_buf[data[j]].dp_type = DP_BINARY;
				datap_str_buf[datap_int_buf[data[j]].i_buf[0]].str_len = temp;
				j+=3+temp;

#if DATAPOINT_DEBUG_ON
				xlink_printf("resolve DP_BINARY j=%d.......\r\n",j);
#endif
				break;
			default:
				break;
       }
    }
}














