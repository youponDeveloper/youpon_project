/**
******************************************************************************
* @file    user_uart.c
* @author  hehx_ybzn@163.com
* @version V1.0.0
* @date    12-oct-2016
* @brief   Define uart communition to zigbee coordinator interface.
******************************************************************************
* @attention
*
******************************************************************************
*/ 

#include <stdio.h>

#include "mico.h"
#include "user_uart.h"
#include "user_config.h"

#define user_uart_log(M, ...) custom_log("USER_UART", M, ##__VA_ARGS__)
#define user_uart_log_trace() custom_log_trace("USER_UART")

#define	STEP_SUCCESS	0xFFFF
#define	STEP_FAILURE	0xEEEE

static volatile ring_buffer_t  rx_buffer;
static volatile uint8_t        rx_data[USER_UART_BUFFER_LENGTH];


static struct appuart_rcvst appuart_rcvbuf={0};

void HexDump(uint8_t * pdata, uint32_t data_len)
{
	#if USER_DEBUG_
	uint32_t i;

	for (i=0; i<data_len; i++)
	{
		printf("%02x%s",*(pdata+i),(i%16 == 15)?"\n" : " ");
	}
	user_uart_log("\r\n");
	#endif
}

OSStatus user_uartInit(void)
{
	OSStatus err = kUnknownErr;
	mico_uart_config_t uart_config;

	//USART init
	uart_config.baud_rate    = 115200;
	uart_config.data_width   = DATA_WIDTH_8BIT;
	uart_config.parity       = NO_PARITY;
	uart_config.stop_bits    = STOP_BITS_1;
	uart_config.flow_control = FLOW_CONTROL_DISABLED;
	uart_config.flags = UART_WAKEUP_DISABLE;
	ring_buffer_init  ( (ring_buffer_t *)&rx_buffer, (uint8_t *)rx_data, USER_UART_BUFFER_LENGTH );

	err = MicoUartInitialize( USER_UART, &uart_config, (ring_buffer_t *)&rx_buffer );
	user_uart_log("err=%d \n", err);
  
  	return kNoErr;
}

OSStatus user_uartSend(unsigned char *inBuf, unsigned int inBufLen)
{
	OSStatus err = kUnknownErr;
	
	if( (NULL == inBuf) || ( 0 == inBufLen) )
	{
		err = kParamErr;
		user_uart_log("ERROR: user_uartSend input params error!");
		goto exit;
	}

#ifdef USER_DEBUG_
	user_uart_log("sent to zigbee uart2:[%d]=%s",  inBufLen, inBuf);
#endif

	err = MicoUartSend(USER_UART, inBuf, inBufLen);
	//mico_rtos_thread_msleep(5);
	require_noerr_action( err, exit, user_uart_log("ERROR: send to USART error! err=%d", err) );

	return kNoErr;

exit:
	return err;
}


static  void clear_appuart_rcvbuf()
{
 	memset(&appuart_rcvbuf.buf, 0, sizeof(appuart_rcvbuf.buf));
 	appuart_rcvbuf.len = 0;
	appuart_rcvbuf.head = 0;
}

static  int appuart_getch(uint8_t *buf)
{
	uint32_t data_len = USER_NUMBER_VAL_0, getLen = 1024;
	
	if(appuart_rcvbuf.len <= 0) 
	{
		clear_appuart_rcvbuf();
		if( MicoUartRecv( USER_UART, appuart_rcvbuf.buf, getLen, USER_UART_RECV_TIMEOUT) == kNoErr)
		{
			data_len = getLen;
		}
		else
		{
			data_len = MicoUartGetLengthInBuffer( USER_UART );
			if(data_len)
			{
				if(data_len > 1024)data_len=1024;

				if(MicoUartRecv(USER_UART, appuart_rcvbuf.buf, data_len, USER_UART_RECV_TIMEOUT)==kNoErr)
				{
					//user_uart_log("appuart_rcvbuf.buf[data_len=%d][%s]", data_len, appuart_rcvbuf.buf);
					//HexDump(appuart_rcvbuf.buf, data_len);
				}
				else
					user_uart_log("MicoUartRecv timeout");
			}
			else
			{
				data_len = USER_NUMBER_VAL_0;
			}
		}
		
		if((data_len > 1024)) 
		{
			clear_appuart_rcvbuf();
			return 0;
		}
#if 0
		if(data_len > 0)
		{
			user_uart_log("appuart_rcvbuf.buf[data_len=%d][%s]", data_len, appuart_rcvbuf.buf);
			//HexDump(appuart_rcvbuf.buf, data_len);
		}
#endif

		appuart_rcvbuf.len = data_len;	
	}

	if(appuart_rcvbuf.len > 0) 
	{
		*buf = appuart_rcvbuf.buf[appuart_rcvbuf.head++];
		appuart_rcvbuf.len --;
		return 1;
	}
	
	return -1;
}

int RecvUartPkt(char *buffer, uint32_t buf_len)
{
	int step=USER_NUMBER_VAL_0;
	uint8_t uc;
	uint32_t len=USER_NUMBER_VAL_0;
	char *pData = (char *)buffer;

	while(appuart_getch(&uc)==USER_NUMBER_VAL_1) 
	{		
		switch(step) 
		{
		case USER_NUMBER_VAL_0:
			if(uc==USER_JSON_BEGIN_FLAG)
			{	 
				step=USER_NUMBER_VAL_1;
				pData[len++] = uc;
			}
			else
			{
				len=USER_NUMBER_VAL_0;
			}
			break;	
		case USER_NUMBER_VAL_1:
			if(uc==USER_JSON_END_FLAG)
			{
				step=USER_NUMBER_VAL_2;
				//user_uart_log("uc=%c", uc);
				pData[len++] = uc;
			}
			else
			{
				//user_uart_log("index=%d uc=%c", len, uc);
				if(len < buf_len && uc != '\r' && uc != '\n')
					pData[len++] = uc;

			}
			break;
		case USER_NUMBER_VAL_2:
			if(uc==USER_JSON_PROTOCON_TAB)
			{
				step=USER_NUMBER_VAL_3;
				if(len < buf_len)
					pData[len++] = uc;
			}
			else
			{
				step = USER_NUMBER_VAL_0;
				len = USER_NUMBER_VAL_0;
				memset(buffer, 0, buf_len);
				pData = buffer;
			}
			break;
		case USER_NUMBER_VAL_3:
			if(uc==USER_JSON_PROTOCON_TAB2)
			{
				if(len < buf_len)
					pData[len++] = uc;
				
				step = STEP_SUCCESS;
			}
			else
			{
				step = USER_NUMBER_VAL_0;
				len = USER_NUMBER_VAL_0;
				memset(buffer, 0, buf_len);
				pData = buffer;
			}
			break;
		default:
			step = USER_NUMBER_VAL_0;
			len = USER_NUMBER_VAL_0;
			memset(buffer, 0, buf_len);
			pData = buffer;
			break;
		}

		if(step == STEP_SUCCESS)
		{
			return len;
		}
		
	}

	return USER_NUMBER_VAL_0;
}

