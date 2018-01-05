/*
 * xlinkHttp.c
 *
 *  Created on: 2015年11月24日
 *      Author: john
 */

#include "xlinkHttp.h"
#include <stdio.h>
#include "../xlinksrc/xlink_md5.h"
//#include "xlink_md5.h"

#define DEBUG_HTTP 
//typedef struct sockaddr_in xlink_addr;

typedef struct {
	char ipaddr[64];
	unsigned int port;
	char path[256];
	int sockfd;
} ServerInfo;

static void xlinkHttpLoop(ServerInfo *info, XLINK_HTTPDOWN *phttp);
static int HTTP_GetPath(char *url, char *path);
static int HTTP_Get_IP_PORT(char *url, char *ip, char *port);
static int HTTP_ConnectHost(ServerInfo *info, XLINK_HTTPDOWN *param);
static int is_ip_Addr(const char *ip);
static int HTTP_getHosntByName(const char *hostName);
static int IPStringtoAddr(const char *ip);
static int HTTP_find_start(char *buf, int size);
static unsigned int HttpGetFileSize(char *buffer, int buflen);

int xlinkHttpStartDownFile(XLINK_HTTPDOWN *phttpParam) {

	int error = 0, retValue = 0;
	ServerInfo Info;
	char portBuf[32];
	char tempBuf[256];
	memset(portBuf, 0, 32);
	memset(&Info, 0, sizeof(Info));
        
        printf("\r\n---set http parameter callback---------\r\n");
         
	if (phttpParam == NULL)
		return -100;
	if (phttpParam->m_errCallBack == NULL)
		return -101;
	if (phttpParam->m_recvCallBack == NULL)
		return -102;
	if (phttpParam->m_staCallBack == NULL)
		return -103;
	if (strlen(phttpParam->m_url) == 0)
		return -104;
        printf("\r\n---set http parameter callback  done---------\r\n");
        
	memset(tempBuf, 0, 256);
	memcpy(tempBuf, phttpParam->m_url, phttpParam->m_urlLength);

        
        printf("\r\n---http get path---------\r\n");
	error = HTTP_GetPath(tempBuf, Info.path);
	if (error < 0)
		return -105;

	memset(tempBuf, 0, 256);
	memcpy(tempBuf, phttpParam->m_url, phttpParam->m_urlLength);

        
        printf("\r\n------http get ip port---------\r\n");
	error = HTTP_Get_IP_PORT(tempBuf, Info.ipaddr, portBuf);
	if (error < 0)
		return -106;

	Info.port = atoi(portBuf);

	printf("ip  %s    port %d\r\n", Info.ipaddr, Info.port);
	error = 0;

        printf("\r\n------http connect host---------\r\n");
	retValue = HTTP_ConnectHost(&Info, phttpParam);
	if (retValue != 0) {
		return retValue;
	}
        printf("\r\n---------------star xlink http loop------------------\r\n");
	xlinkHttpLoop(&Info, phttpParam);

	return 0;
}

#define XLINK_HTTP_BUFFER_SIZE 1024

void xlinkHttpLoop(ServerInfo *info, XLINK_HTTPDOWN *phttp) {

	int retSelect = 0, datalen = 0, retValue = 0, first = 0;
	int mfd = info->sockfd;
	int start_text;
	unsigned int fileTotalSize = 0, fileoffset = 0;
	xlink_set_fd m_fd_set;
	char buffer[XLINK_HTTP_BUFFER_SIZE];
	//char sendBuff[XLINK_HTTP_BUFFER_SIZE];
	char md5CheckStr[50];
	MD5_CTX ctx;
#if __MXCHIP__
	static struct timeval timeoutal;
#else
	static struct timeval timeoutal;
#endif
	timeoutal.tv_sec = 20;
	timeoutal.tv_usec = 100;
       
	xlinkHttpMd5Init(&ctx);
       
	fileoffset = 0;
             
	retValue = xlink_sprintf(buffer, "GET %s  HTTP/1.1\r\nHost: %s:%d\r\nConnection:close\r\n\r\n", info->path,
			info->ipaddr, info->port);

	retSelect = xlink_send(mfd, buffer, retValue, 0);
  
	if (retSelect <= 0) {
		phttp->m_errCallBack(HTTP_STA_SEND_REQUEST_FAILED);
		return;
	}
      
	first = 1;
	do {
		FD_ZERO(&m_fd_set);
		FD_SET(mfd, &m_fd_set);
		memset(buffer, 0, XLINK_HTTP_BUFFER_SIZE);
		retSelect = xlink_select(mfd + 1, &m_fd_set, NULL, NULL, &timeoutal);
		if (retSelect <= 0) {
			retValue = phttp->m_errCallBack(HTTP_STA_DOWN_TIMEOUT);
                        if(retValue==0)
                          continue;
                        else
                          break;
		}
		if (FD_ISSET(mfd, &m_fd_set)) {
			if (first) {
				first = 0;
				datalen = xlink_recv(mfd, buffer, XLINK_HTTP_BUFFER_SIZE, 0);
				printf("recv len %d=%s", datalen, buffer);
				
				start_text = HTTP_find_start(buffer, datalen);
				if (start_text < 0) {
					if (start_text == -2) {
						fileTotalSize = HttpGetFileSize(buffer, datalen);
						printf("****************file size %d\r\n", fileTotalSize);
						continue;
					}
					printf("\r\nstart_text--------------------------=%d\r\n", start_text);
					phttp->m_errCallBack(HTTP_STA_REMOTE_CLOSE_SOCKET);
					xlink_close(mfd);
					return;
				}
				datalen -= start_text;								
				XlinkHttpMd5Update(&ctx, (unsigned char *)(buffer + start_text), datalen);			
				fileoffset += datalen;
				phttp->m_recvCallBack((buffer + start_text), datalen, fileTotalSize, fileoffset);
			} else {
				datalen = xlink_recv(mfd, buffer, XLINK_HTTP_BUFFER_SIZE, 0);
				if (datalen == 0) { //socket close
                                        printf("\r\n-------------receive data length = 0----------\r\n");
					phttp->m_errCallBack(HTTP_STA_REMOTE_CLOSE_SOCKET);
					xlink_close(mfd);
					break;
				} else if (datalen < 0) {
					continue;
				} else {
					timeoutal.tv_sec = 15;
				
					XlinkHttpMd5Update(&ctx, (unsigned char *) buffer, datalen);
				
					fileoffset += datalen;
					phttp->m_recvCallBack(buffer, datalen, fileTotalSize, fileoffset);
				}
			}
		}
	} while (1);
	
        XlinkHttpM5dFinalString(&ctx, md5CheckStr);
        retValue = strncmp(md5CheckStr, phttp->md5str, 32);
        printf("\r\nthe final md5CheckStr=%s\r\n",md5CheckStr);
        if (retValue == 0) {
             phttp->m_errCallBack(HTTP_STA_MD5_CHECK_SUCCESS);
        } else {
             phttp->m_errCallBack(HTTP_STA_MD5_CHECK_FAILED);
        }
	phttp->m_errCallBack(HTTP_STA_DOWN_COMPLETE);
	xlink_close(mfd);
}

static int HTTP_GetPath(char *url, char *path) {
	char *p;
	p = strstr(url, "http://");
	if (p == NULL) {
		p = strchr(url, '/');
		if (p == NULL)
			return -1;
		else {
			strcpy(path, p);
			return 0;
		}
	} else {
		p = strchr(url + strlen("http://"), '/');
		if (p == NULL)
			return -1;
		else {
			strcpy(path, p);
			return 0;
		}
	}
}

static int HTTP_Get_IP_PORT(char *url, char *ip, char *port) {
	char *p = NULL;
	int offset = 0;
	char DOMAIN_NAME[128];
	p = strstr(url, "http://");
	if (p == NULL) {
		offset = 0;
	} else {
		offset = strlen("http://");
	}
	p = strchr(url + offset, '/');
	if (p == NULL) {
		printf("url:%s format error\n", url);
		return -1;
	} else {
		memset(DOMAIN_NAME, 0x0, sizeof(DOMAIN_NAME));
		memcpy(DOMAIN_NAME, url + offset, (p - url - offset));
		p = strchr(DOMAIN_NAME, ':');
		if (p == NULL) {
			strcpy(ip, DOMAIN_NAME);
			strcpy(port, "80");
#ifdef DEBUG_HTTP
			printf("ip=%s,port=%s\n",ip,port); //debug info
#endif
			return 1;
		} else {
			*p = '\0';
			strcpy(ip, DOMAIN_NAME);
			strcpy(port, p + 1);
#ifdef DEBUG_HTTP
			printf("ip=%s,port=%s\n",ip,port); //debug info
#endif
			return 2;
		}
		return 0;
	}
}

static int HTTP_ConnectHost(ServerInfo *info, XLINK_HTTPDOWN *param) {

	int serverIp = 0;
	int retValue = 0;
	int httpfd = 0;
	xlink_addr serverAddr;
	if (is_ip_Addr(info->ipaddr) != 1) {
		do { //域名解析
			serverIp = HTTP_getHosntByName(info->ipaddr);
			if (serverIp == 0) {
				retValue = param->m_errCallBack(HTTP_STA_GETHOST_FAILED);
				if (retValue != 0) {
					return -1;
				}
				xlink_msleep(50);
			} else {
				retValue = param->m_errCallBack(HTTP_STA_GETHOST_SUCCESS);
				break;
			}
		} while (1);
	} else {
		//serverIp = IPStringtoAddr(info->ipaddr);
#if __MXCHIP__
		serverIp = inet_addr(info->ipaddr);
#else
	        serverIp = IPStringtoAddr(info->ipaddr);
#endif
	}

// create socket
	do {
		retValue = 0;
		//httpfd = xlink_socket(AF_INET, SOCK_STREAM, 0);
#if __MXCHIP__
	httpfd = xlink_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //set_tcp_keepalive(3, 60);
#else
	httpfd = xlink_socket(AF_INET, SOCK_STREAM, 0);
#endif                                                          
		if (httpfd <= 0) {
			retValue = param->m_errCallBack(HTTP_STA_CREATESOCKET_FAILED);
			if (retValue != 0) {
				return -2;
			}
			xlink_msleep(50);
		} else {
			retValue = param->m_errCallBack(HTTP_STA_CREATESOCKET_SUCCESS);
			break;
		}
	} while (1);

//connect server
	memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_addr.s_addr = serverIp;
	serverAddr.sin_port = htons(info->port);
	serverAddr.sin_family = AF_INET;
	do {
          retValue = 0;
		//retValue = xlink_connect(httpfd, (struct sockaddr *) &serverAddr, sizeof(xlink_addr));
                
//#if  __MXCHIP__
//	struct sockaddr_t mxchip_addr;
//	xlink_memset(&mxchip_addr, 0, sizeof(struct sockaddr_t));
//	mxchip_addr.s_port = info->port;
//	mxchip_addr.s_ip = serverIp;
//	retValue = xlink_connect(httpfd, &mxchip_addr, sizeof(mxchip_addr));
//#else
	retValue = xlink_connect(httpfd, (struct sockaddr *) &serverAddr, sizeof(xlink_addr));
//#endif
                
                
		if (retValue < 0) {
			retValue = param->m_errCallBack(HTTP_STA_CONECT_FAILED);
			if (retValue != 0) {
				param->m_errCallBack(HTTP_STA_SOCKET_CLOSE);
				xlink_close(httpfd);
				return -3;
			}
			xlink_msleep(50);
		} else {
			param->m_errCallBack(HTTP_STA_CONECT_SUCCESS);
			break;
		}
	} while (1);

	info->sockfd = httpfd;

	return 0;
}

static int is_ip_Addr(const char *ip) {
	int section = 0; //每一节的十进制值
	int dot = 0; //几个点分隔符
	int last = -1; //每一节中上一个字符
	while (*ip) {
		if (*ip == '.') {
			dot++;
			if (dot > 3) {
				return 0;
			}
			if (section >= 0 && section <= 255) {
				section = 0;
			} else {
				return 0;
			}
		} else if (*ip >= '0' && *ip <= '9') {
			section = section * 10 + *ip - '0';
			if (last == '0') {
				return 0;
			}
		} else {
			return 0; //不是IP
		}
		last = *ip;
		ip++;
	}

	if (section >= 0 && section <= 255) {
		if (3 == dot) {
			section = 0;
			return 1;
		}
	}
	return 0;
}

static int HTTP_getHosntByName(const char *hostName) {

       extern  int  xlinkGetHostByName(const char *hostname);
       int ret = 0;
       
       ret = xlinkGetHostByName(hostName);    
       return ret;
}

static int IPStringtoAddr(const char *ip) {
	int ret = 0;
	int buf[4];

	sscanf(ip, "%3d.%3d.%3d.%3d", &buf[0], &buf[1], &buf[2], &buf[3]);
	ret = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	return ret;
}

static int HTTP_find_start(char *buf, int size) {
	int i;
	char *p;
	p = strstr(buf, "\r\n\r\n");
	if(p){
            i = p - buf + 4;
            if (i == size) {
                    return -2;
            } else if (i > size)
                    i = -1;
	} else
		i = -1;
	return i;
}

unsigned int HttpGetFileSize(char *buffer, int buflen) {
	unsigned int filelength = 0;
	char *p;
	if ((p = strstr(buffer, "Content-Length")) || (p = strstr(buffer, "Content-length:")))
		p += 16;
	else {
		p = buffer;
		while ((p = strstr(p, "\r\n"))) { /*fixup some file don't have string "Content-Length", I hope it works ok! */
			p = p + 2;
			if ((*p <= '9') && (*p >= '0'))
				break;
		}
		if (!p)
			return 0;
	}
	filelength = atoi(p);
	return filelength;
}


