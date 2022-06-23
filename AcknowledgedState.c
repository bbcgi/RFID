#include "EPC_STATE.h"

void AcknowledgedStateFunction(){

	YLED=LED_ON;
	
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
			AnswerToQuery((MyQuery*)&buffer) ;
			return;
		case QUERY_ADJUST_HEADER :
		case QUERY_REP_HEADER :
			state=ReadyState;
			return ;		
		case ACK_HEADER:
			/*�P�_Reader�e�^��ACK��RN16�O�_�M���eTAG�e�X���ۦP*/
			Ack=(MyACK*)buffer;
			if(Ack->RN_ACK==RN_16)/*�Y�ۦP�hTAG����PC,EPC,CRC-16*/
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
