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

#ifndef __RS485_UART_H_
#define __RS485_UART_H_

#include "mico.h"
#include "cJSON.h"

/*******************************************************************************
 * DEFINES
 ******************************************************************************/

 #define RS485_UART                    MICO_UART_3// (MICO_UART_NONE)

#define RS485_UART_RECV_TIMEOUT              60
#define RS485_UART_BUFFER_LENGTH             512
   
#define RS485_READ_UART_BUFFER_LENGTH		260


#define  RS485_HEADER1		0x55
#define  RS485_HEADER2		0xAA
#define  RS485_HEADER3		0x55

#pragma pack(1)
struct rs485_parse_param {
	int step;
	uint16_t  data_len;
	uint16_t  recv_len;
	uint8_t wCrc;
};
#pragma pack()

/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

OSStatus rs485_uartInit(void);
OSStatus rs485_uartSend(unsigned char *inBuf, unsigned int inBufLen);
uint32_t rs485_uartRecv(unsigned char *outBuf, unsigned int getLen);
int Rs485RecvUartPkt(uint8_t *buffer, uint32_t* buf_len);
void rs485_gpio_send_enable(int enable_flag);
void rs485_uart_data_recv(mico_thread_arg_t arg );

#endif  // __USER_UART_H_
