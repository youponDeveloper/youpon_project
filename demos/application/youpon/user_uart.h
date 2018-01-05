/**
******************************************************************************
* @file    user_uart.h
* @author  hehx_ybzn@163.com
* @version V1.0.0
* @date    12-oct-2016
* @brief   Define uart communition to zigbee coordinator interface .
******************************************************************************
* @attention
*
******************************************************************************
*/ 

#ifndef __USER_UART_H_
#define __USER_UART_H_

#include "mico.h"

/*******************************************************************************
 * DEFINES
 ******************************************************************************/

 #define USER_UART                    MICO_UART_2// (MICO_UART_NONE)

#define 		USER_UART_RECV_TIMEOUT              	300
#define 		USER_UART_BUFFER_LENGTH             	1024
#define 		READ_UART_BUFFER_LENGTH			256


/*******************************************************************************
 * INTERFACES
 ******************************************************************************/
#pragma pack(1)
struct appuart_rcvst {
	uint8_t buf[1024+1];
	uint32_t len;
	uint32_t head;
};
#pragma pack()

OSStatus user_uartInit(void);
OSStatus user_uartSend(unsigned char *inBuf, unsigned int inBufLen);
//uint32_t user_uartRecv(char *outBuf, unsigned int getLen);
int RecvUartPkt(char *buffer, uint32_t buf_len);

#endif  // __USER_UART_H_
