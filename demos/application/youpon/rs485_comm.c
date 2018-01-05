#include "rs485_comm.h"

#define rs485_log(M, ...) custom_log("rs485", M, ##__VA_ARGS__)
#define rs485_log_trace() custom_log_trace("rs485")

static uint8_t gsequenceNumber;

extern mico_semaphore_t uart_idle_irq_sem2;

extern void HexDump(uint8_t * pdata, uint32_t data_len);

inline uint8_t getNextOutingSequenceNumber() 
{
	return gsequenceNumber++;
}


uint8_t getCRC8(uint8_t *pdata, uint16_t length)
{
	uint8_t i;
	uint8_t crc = USER_NUMBER_VAL_0;
	while(length-- != USER_NUMBER_VAL_0)
	{
	    for(i = USER_NUMBER_VAL_1; i != USER_NUMBER_VAL_0; i *= USER_NUMBER_VAL_2)
	    {
	        if((crc & USER_NUMBER_VAL_1) != USER_NUMBER_VAL_0)
		 {
				crc /= USER_NUMBER_VAL_2;
				crc ^= USER_NUMBER_VAL_0X8C;
		 }
	        else
		 {
				crc /= USER_NUMBER_VAL_2;
	        }
			
	        if((*pdata & i) != USER_NUMBER_VAL_0)
		 {
				crc^=USER_NUMBER_VAL_0X8C;
	        }
	   }
		
	   pdata++;
	}
	return(crc);
}

int32_t Rs485_SendDataPackageEx(RS485_CTRL_COMMAND_PARAM_T *pDevParam)//no used
{
	if(NULL == pDevParam)
	{
		return USER_NUMBER_VAL_INIT;
	}
	
	uint8_t buffer[64];
	uint16_t totalLength = pDevParam->cmd_length + FieldIndex_MinNumber;
	uint16_t payload_len = pDevParam->cmd_length;

	rs485_log("totalLength=%02x %d\n", totalLength, pDevParam->cmd_length);
	memset(buffer, USER_NUMBER_VAL_0, sizeof(buffer));
	buffer[FieldIndex_FrameHeadH] = RS485_FrameHeadH;
	buffer[FieldIndex_FrameHeadM] = RS485_FrameHeadM;
	buffer[FieldIndex_FrameHeadL] = RS485_FrameHeadL;
	pDevParam->cmd_length = totalLength - USER_NUMBER_VAL_4;
	buffer[FieldIndex_LengthH] = pDevParam->cmd_length >> USER_NUMBER_VAL_8;
	buffer[FieldIndex_LengthL] = pDevParam->cmd_length & USER_NUMBER_VAL_FF;
	buffer[FieldIndex_CtrlNumber] = pDevParam->frametype;
	buffer[FieldIndex_SequenceNumber] =  getNextOutingSequenceNumber();
	buffer[FieldIndex_DestinationAddressH] = pDevParam->destinationAddress >> USER_NUMBER_VAL_8;
	buffer[FieldIndex_DestinationAddressL] = pDevParam->destinationAddress & USER_NUMBER_VAL_FF;
	buffer[FieldIndex_SourceAddressH] = pDevParam->srcAddress >> USER_NUMBER_VAL_8;
	buffer[FieldIndex_SourceAddressL] = pDevParam->srcAddress & USER_NUMBER_VAL_FF;
	buffer[FieldIndex_DestinationEndpoint] = pDevParam->destEndpoit;
	buffer[FieldIndex_SourceEndpoint] = pDevParam->srcEndpoit;
	buffer[FieldIndex_ClusterIdH] = pDevParam->pileId >> USER_NUMBER_VAL_8;
	buffer[FieldIndex_ClusterIdL] = pDevParam->pileId;
	buffer[FieldIndex_CommandId] = pDevParam->cmdId;

	if(payload_len > USER_NUMBER_VAL_0 && payload_len < USER_NUMBER_VAL_96)
	{
		memcpy((char*)&buffer[FieldIndex_Payload], pDevParam->cmd, payload_len);
	}
	buffer[totalLength - USER_NUMBER_VAL_1] = USER_NUMBER_VAL_FF;
	return pushAppMainRecvMsgProcess( (unsigned char *)buffer, (uint32_t)totalLength, 0);
}

int32_t Rs485_SendDataPackage(RS485_CTRL_COMMAND_PARAM_T *pDevParam/*uint16_t destinationAddress, uint16_t clusterId, uint16_t commandId, uint16_t length*/)
{
	if(NULL == pDevParam)
	{
		return  USER_NUMBER_VAL_INIT;
	}
	
	uint8_t buffer[64];
	uint16_t totalLength = pDevParam->cmd_length + FieldIndex_MinNumber;
	uint16_t payload_len = pDevParam->cmd_length;
	char tembuffer[120]={0};
	char recvbuff[138]={0};

	memset(buffer, USER_NUMBER_VAL_0, sizeof(buffer));
	buffer[FieldIndex_FrameHeadH] = RS485_FrameHeadH;
	buffer[FieldIndex_FrameHeadM] = RS485_FrameHeadM;
	buffer[FieldIndex_FrameHeadL] = RS485_FrameHeadL;
	pDevParam->cmd_length = totalLength - USER_NUMBER_VAL_4;
	buffer[FieldIndex_LengthH] = pDevParam->cmd_length >> USER_NUMBER_VAL_8;
	buffer[FieldIndex_LengthL] = pDevParam->cmd_length & USER_NUMBER_VAL_FF;
	buffer[FieldIndex_CtrlNumber] = pDevParam->frametype;
	buffer[FieldIndex_SequenceNumber] = getNextOutingSequenceNumber();
	buffer[FieldIndex_DestinationAddressH] = pDevParam->destinationAddress >> USER_NUMBER_VAL_8;
	buffer[FieldIndex_DestinationAddressL] = pDevParam->destinationAddress & USER_NUMBER_VAL_FF;
	buffer[FieldIndex_SourceAddressH] = pDevParam->srcAddress >> USER_NUMBER_VAL_8;
	buffer[FieldIndex_SourceAddressL] = pDevParam->srcAddress & USER_NUMBER_VAL_FF;
	buffer[FieldIndex_DestinationEndpoint] = pDevParam->destEndpoit;
	buffer[FieldIndex_SourceEndpoint] = pDevParam->srcEndpoit;
	buffer[FieldIndex_ClusterIdH] = pDevParam->pileId >> USER_NUMBER_VAL_8;
	buffer[FieldIndex_ClusterIdL] = pDevParam->pileId;
	buffer[FieldIndex_CommandId] = pDevParam->cmdId;

	if(payload_len > USER_NUMBER_VAL_0 && payload_len < USER_NUMBER_VAL_96)
	{
		memcpy((char*)&buffer[FieldIndex_Payload], pDevParam->cmd, payload_len);
	}
	buffer[totalLength - USER_NUMBER_VAL_1] = getCRC8(buffer + FieldIndex_LengthH, pDevParam->cmd_length);

	if((pDevParam->pileId == 0x0008) ||(pDevParam->pileId == 0x0006) )
	{				

		int i;
		
		for(i= 0; i< totalLength-5; i++)
			sprintf(tembuffer+i*2, "%02X", buffer[i+5]);
		tembuffer[totalLength*2] = '\0';

		sprintf(recvbuff, "cus rs485 tx {%s}\r\n", tembuffer);
		#ifdef USER_DEBUG_
		//rs485_log("485 ctr cmd send to uart2=%s\n", recvbuff);
		#endif
		pushAppMainRecvMsgProcess( (unsigned char *)recvbuff, strlen(recvbuff), 1);//send to zigbee queue
 	}
	
	return pushAppMainRecvMsgProcess( (unsigned char *)buffer, (uint32_t)totalLength, 0);
}

int32_t Rs485_SendData(uint8_t *data, uint16_t length)
{
	int32_t result = USER_NUMBER_VAL_INIT;
	
	// 2.·¢ËÍ
	rs485_uartSend(data, length);
	result = length;
	return result;
}

static void rs485_gpio_init()
{
	 MicoGpioInitialize( RS485_RXTX_ENABLE, OUTPUT_PUSH_PULL );
	 MicoGpioOutputLow( RS485_RXTX_ENABLE);
}
OSStatus Rs485_Init()
{
	OSStatus err = kNoErr;
	rs485_gpio_init();
	rs485_uartInit();
	
	return err;
}
