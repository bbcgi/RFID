#include "EPC_STATE.h"

void ReplyStateFunction(){
	
	BLED=LED_ON;
	
		while(1){
		//接收資料
		halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //打開RX
		ReciveByte = halRFReceivePacket(10,&buffer,64,0,CC1010EB_CLKFREQ) ;
		if(ReciveByte==0)continue;
		
		
		command = buffer[0] ;
		//分辨指令
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
				/*判斷Reader送回的ACK中RN16是否和之前TAG送出的相同*/
				Ack=(MyACK*)buffer;
				if(Ack->RN_ACK==RN_16)/*若相同則TAG反應PC,EPC,CRC-16*/
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