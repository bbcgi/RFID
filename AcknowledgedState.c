#include "EPC_STATE.h"

void AcknowledgedStateFunction(){

	YLED=LED_ON;
	
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
			AnswerToQuery((MyQuery*)&buffer) ;
			return;
		case QUERY_ADJUST_HEADER :
		case QUERY_REP_HEADER :
			state=ReadyState;
			return ;		
		case ACK_HEADER:
			/*判斷Reader送回的ACK中RN16是否和之前TAG送出的相同*/
			Ack=(MyACK*)buffer;
			if(Ack->RN_ACK==RN_16)/*若相同則TAG反應PC,EPC,CRC-16*/
			{
				halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
				halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&EpcMB,12);
				halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); 
				continue;
				//mainFunctionPointer=&AcknowledgedStateFunction;
				//return ;
			}
			else
			{
				state=ArbitrateState;
				return ;
			}	
			break;
		case REQ_RN_HEADER :
			ReqRNCommand=(MyReq_RN*)buffer;
			if(ReqRNCommand->RN_Req_RN==RN_16)
			{
				if(ReceivedMB.AccessPwdA !=0 || ReceivedMB.AccessPwdB !=0)
				{
					state=OpenState;
				}
				else
				{
					state=SecuredState;
				}
				
			}else{
				state=AcknowledgedState;
				return ;
			}	
			//for "if(Ack->RN_ACK==RN_16 && ReceivedMB.AccessPwd !=0)"
			// and "else if(Ack->RN_ACK==RN_16 && ReceivedMB.AccessPwd ==0)"
			halRandomNumberGen((byte*)&Handler,2);
			halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
			halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&Handler,2);
			halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); 
			return;

		default :
			state=ArbitrateState;   
			return ;
		} 
	}	
}
