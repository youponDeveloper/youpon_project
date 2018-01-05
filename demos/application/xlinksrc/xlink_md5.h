/*
 * md5.h
 *
 *  Created on: 2015年1月9日
 *      Author: john
 */

#ifndef _XMD5_H_
#define _XMD5_H_
#ifdef  __cplusplus
extern "C" {
#endif
//#include "Xlink_Head_Adaptation.h"

//#ifdef __alpha


//#else
//typedef unsigned long uint32;
//#endif

struct XlinkMD5Context {
	 unsigned int  buf[4];
	 unsigned int  bits[2];
	unsigned char in[64];
};
/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
typedef struct XlinkMD5Context MD5_CTX;

extern void xlinkHttpMd5Init(MD5_CTX  *ctx);
extern void XlinkHttpMd5Update(MD5_CTX  *ctx,unsigned char *data,unsigned int datalen);
extern void XlinkHttpM5dFinal(MD5_CTX  *ctx,unsigned char *RetBuffer);
extern void XlinkHttpM5dFinalString(MD5_CTX  *ctx,char *RetBuffer);


extern void xlinkGetMd5(unsigned char *RetBuffer,unsigned char *data,unsigned int datalen);

#ifdef  __cplusplus
}
#endif

#endif /* MD5_H_ */
