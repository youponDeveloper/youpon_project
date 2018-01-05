/*
 * xlink_MXCHIP_mico.h
 *
 *  Created on: 2015å¹?æœ?æ—
 *      Author: XT800
 */

#ifndef XLINK_MXCHIP_MICO_H_
#define XLINK_MXCHIP_MICO_H_


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "mico.h"
#include "mico_socket.h"
#include "MICOAppDefine.h"
#include "platform.h"
#include "platform_config.h"
#include "moc_api.h"


#define  XLINK_FUNC

#define   xlink_malloc(x)     malloc(x)
#define   xlink_free(x)        free(x)

#define   xlink_strlen(x)      strlen(x)
#define   xlink_strcmp(x,t)    strcmp((char*)(x),(char*)(t))
#define   xlink_strncmp(x,t,z) strncmp((char*)(x),(char*)(t),z)
#define   xlink_memset(x,d,t)  memset(x,d,t)
#define   xlink_memcpy(x,d,l)  memcpy(x,d,l)
#define   xlink_msleep(n)      mico_thread_msleep(n)
#define   xlink_time(n)        xlinkGetTime(n)
#define   xlink_sprintf        sprintf
#define   xlink_printf         printf

#define   xlink_socket         	socket
#define   xlink_bind           	bind
#define   xlink_connect        	connect
#define   xlink_setsockopt     	setsockopt
#define   xlink_recv           	recv
#define   xlink_recvfrom       	recvfrom
#define   xlink_sendto         	sendto
#define   xlink_send           	send
#define   xlink_close(x)      	close(x)
#define   xlink_set_fd        	fd_set
#define   xlink_select(a,b,c,d,e)  select(a,b,c,d,e)

typedef time_t xsdk_time_t;


struct in_address {
	unsigned int s_addr;
};

//struct sockaddr_in {
//	void *data;
//	unsigned short type;
//	unsigned char sin_family;
//	unsigned short sin_port;
//	struct in_address sin_addr;
//	unsigned short spares[6];
//};


typedef struct sockaddr_in xlink_addr;

#endif /* XLINK_MXCHIP_MICO_H_ */
