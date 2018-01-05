
#include "Xlink_PassThroughProtocol.h"
#include "xlink_demo.h"


#define XLINKPASSTP_log(M, ...) custom_log("XLINKPASSTP_log", M, ##__VA_ARGS__)

XLINK_FUNC int Xlink_PassThroughProtolInit(XLINK_PASSTHROUGHPROTOCOL *PTP_pck)
{
	if(PTP_pck->PacketCallBack == NULL)return -1;
	if(PTP_pck->pktbuf == NULL)return -2;
	if(PTP_pck->pktbuflen < 7)return -3;
	PTP_pck->pktpos = 0;
	return 0;
}


XLINK_FUNC void Xlink_PassThroughProtolPutData(XLINK_PASSTHROUGHPROTOCOL *PTP_pck,unsigned char *data,unsigned short datalen)
{
	unsigned short i = 0;
	for (i = 0; i < datalen; i++) {
		if(PTP_pck->pktpos >= PTP_pck->pktbuflen) {
			PTP_pck->pktpos = 0;
			PTP_pck->pktbuf[0] = 0;
		}
		if((PTP_pck->pktpos >= 2) && (PTP_pck->pktbuf[0] == XLINK_PTP_HEAD)) {
			PTP_pck->pktlentmp = PTP_pck->pktbuf[1];
			PTP_pck->pktlentmp <<= 8;
			PTP_pck->pktlentmp += PTP_pck->pktbuf[2];
		}
		else {
			PTP_pck->pktlentmp = 0;
		}
		switch(data[i]) {
			case XLINK_PTP_HEAD:
				PTP_pck->pktpos = 0;
				PTP_pck->pktbuf[PTP_pck->pktpos++] = XLINK_PTP_HEAD;
				break;
			case XLINK_PTP_TAIL:
				if((PTP_pck->pktpos >= 5) && (PTP_pck->pktbuf[0] == XLINK_PTP_HEAD)) {
					//a packet 
					PTP_pck->pkt->alldatalen = PTP_pck->pktbuf[1];
					PTP_pck->pkt->alldatalen <<= 8;
					PTP_pck->pkt->alldatalen += PTP_pck->pktbuf[2];
					if(PTP_pck->pkt->alldatalen == (PTP_pck->pktpos - 3)) {
						PTP_pck->pkt->cmd = PTP_pck->pktbuf[3];
						PTP_pck->pkt->datalen = PTP_pck->pkt->alldatalen - 2;
						if (PTP_pck->pkt->datalen > 0) {
							PTP_pck->pkt->data = &PTP_pck->pktbuf[4];
						}
						else {
							PTP_pck->pkt->data = NULL;
						}
						if(PTP_pck->pktbuf[PTP_pck->pktpos -1] == Xlink_PassThroughProtolBuildXor(0,&PTP_pck->pktbuf[1],PTP_pck->pktpos -2)) {
							PTP_pck->pkt->en = 1;
							PTP_pck->PacketCallBack(PTP_pck->pkt);
						}
					}
				}
				PTP_pck->pktpos = 0;
				PTP_pck->pktbuf[0] = 0;
				break;
			case XLINK_PTP_CHANGE:
				if((PTP_pck->pktpos >= 1) && (PTP_pck->pktbuf[0] == XLINK_PTP_HEAD)) {
					 PTP_pck->pktbuf[PTP_pck->pktpos - 1] |= XLINK_PTP_OR;					
				}
				else {
					PTP_pck->pktpos = 0;
					PTP_pck->pktbuf[0] = 0;
				}
				break;
			default:
				if((PTP_pck->pktpos > 0) && (PTP_pck->pktbuf[0] == XLINK_PTP_HEAD)) {
					 PTP_pck->pktbuf[PTP_pck->pktpos++] = data[i];					
				}
				else {
					PTP_pck->pktpos = 0;
					PTP_pck->pktbuf[0] = 0;
				}
				break;
		}
	}
}



XLINK_FUNC void Xlink_PassThroughProtolBuildSendData(XLINK_PASSTHROUGHPROTOCOLPACKET *pkt)
{
#if 0
	unsigned char tmp = 0,res = 0;
	unsigned short i = 0;
	
	pkt->PacketSend = XlinkUartSend;
	if(pkt->en == 0)return;
	if(pkt->PacketSend == NULL)return;
	
	tmp = XLINK_PTP_HEAD;
	pkt->PacketSend(&tmp,1);
	
	tmp = pkt->alldatalen >> 8;
	res = Xlink_PassThroughProtolBuildXor(res,&tmp,1);
	if((tmp == XLINK_PTP_HEAD) || (tmp == XLINK_PTP_CHANGE) || (tmp == XLINK_PTP_TAIL)) {
		tmp = tmp & 0x7f;
		pkt->PacketSend(&tmp,1);
		tmp = XLINK_PTP_CHANGE;
		pkt->PacketSend(&tmp,1);
	}
	else {
		pkt->PacketSend(&tmp,1);
	}
	
	tmp = pkt->alldatalen >> 0;
	res = Xlink_PassThroughProtolBuildXor(res,&tmp,1);
	if((tmp == XLINK_PTP_HEAD) || (tmp == XLINK_PTP_CHANGE) || (tmp == XLINK_PTP_TAIL)) {
		tmp = tmp & 0x7f;
		pkt->PacketSend(&tmp,1);
		tmp = XLINK_PTP_CHANGE;
		pkt->PacketSend(&tmp,1);
	}
	else {
		pkt->PacketSend(&tmp,1);
	}	
	
	tmp = pkt->cmd;
	res = Xlink_PassThroughProtolBuildXor(res,&tmp,1);
	if((tmp == XLINK_PTP_HEAD) || (tmp == XLINK_PTP_CHANGE) || (tmp == XLINK_PTP_TAIL)) {
		tmp = tmp & 0x7f;
		pkt->PacketSend(&tmp,1);
		tmp = XLINK_PTP_CHANGE;
		pkt->PacketSend(&tmp,1);
	}
	else {
		pkt->PacketSend(&tmp,1);
	}	
	
	for(i = 0; i < pkt->datalen; i++) {
		res = Xlink_PassThroughProtolBuildXor(res,&pkt->data[i],1);
		switch(pkt->data[i]) {
			case XLINK_PTP_HEAD:
				tmp = XLINK_PTP_HEAD & 0x7f;
				pkt->PacketSend(&tmp,1);
				tmp = XLINK_PTP_CHANGE;
				pkt->PacketSend(&tmp,1);
				break;
			case XLINK_PTP_CHANGE:
				tmp = XLINK_PTP_CHANGE & 0x7f;
				pkt->PacketSend(&tmp,1);
				tmp = XLINK_PTP_CHANGE;
				pkt->PacketSend(&tmp,1);
				break;
			case XLINK_PTP_TAIL:
				tmp = XLINK_PTP_TAIL & 0x7f;
				pkt->PacketSend(&tmp,1);
				tmp = XLINK_PTP_CHANGE;
				pkt->PacketSend(&tmp,1);
				break;
			default:
				pkt->PacketSend(&pkt->data[i],1);
				break;
		}
	}
	
	tmp =res;
	if((tmp == XLINK_PTP_HEAD) || (tmp == XLINK_PTP_CHANGE) || (tmp == XLINK_PTP_TAIL)) {
		tmp = tmp & 0x7f;
		pkt->PacketSend(&tmp,1);
		tmp = XLINK_PTP_CHANGE;
		pkt->PacketSend(&tmp,1);
	}
	else {
		pkt->PacketSend(&tmp,1);
	}	
	
	tmp = XLINK_PTP_TAIL;
	pkt->PacketSend(&tmp,1);
#endif
	
}



XLINK_FUNC unsigned char Xlink_PassThroughProtolBuildXor(unsigned char resold,unsigned char *data,unsigned int datalen)
{
	unsigned char res = 0;
	unsigned int  i = 0;
	res = resold;
	for(i = 0; i < datalen; i++) {
		res ^=data[i];
	}
	return res;
}


