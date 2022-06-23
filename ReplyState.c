#include "EPC_STATE.h"

void ReplyStateFunction(){
	
	BLED=LED_ON;
	
		while(1){
		//�������
		halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //���}RX
		ReciveByte = halRFReceivePacket(10,&buffer,64,0,CC1010EB_CLKFREQ) ;
		if(ReciveByte==0)continue;
		
		
		command = buffer[0] ;
		//������O
		switch(command){
			case SINGLE_SELECT_HEADER:
				AnswerToSingleSelect((MySelect*)&buffer);
				return;
			case SELECT_HEADER :
				AnswerToSelect((MySelect*)&buffer);
				return;
			case QUERY_HEADER  :
			case QUERY_ADJUST_HEADER :
				halRandomNumberGen((byte*)&RN_16,2);
				halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
				halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&RN_16,2);
				halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); 
				break;
			case ACK_HEADER:
				/*�P�_Reader�e�^��ACK��RN16�O�_�M���eTAG�e�X���ۦP*/
				Ack=(MyACK*)buffer;
				if(Ack->RN_ACK==RN_16)/*�Y�ۦP�hTAG����PC,EPC,CRC-16*/
				{
				/*PC,EPC,CRC16 backscatter (not finished)*/
				halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
				halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&EpcMB,12);
				halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); 

				state=AcknowledgedState;
				return ;
				}
				else
				{
				state=ArbitrateState;
				return ;
				}	
				break;	 
			default:
				state=ArbitrateState;    //Remain in Arbitrate 
				return ;
		}
	}
}