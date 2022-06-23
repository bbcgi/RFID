#include "EPC_STATE.h"

void ArbitrateStateFunction(){
	GLED=LED_ON;
	
	halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //打開RX
	halRandomNumberGen((byte*)&RN_16,2);

	while(1){
	//接收資料
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
		case QUERY_REP_HEADER :
			if( SlotCounter==0x0000 ){
				/*if slot=0 roll over 7fffh ,else decrement */
				SlotCounter = 0x7FFF ;
				/*if slot=0 go to Reply,reply new RN16 */
				
				halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
				halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&RN_16,2);
				halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); 
				state=ReplyState; 
				return ; 
			}else{
				SlotCounter--;
				/*if slot<>0 remain in Arbitrate Reply None*/
				continue;
				//mainFunctionPointer=&ArbitrateStateFunction; 
				//return ; 
			}
			break;
		case QUERY_ADJUST_HEADER :
			QueryAdjustCommand=(MyQueryAdjust*)&buffer;
			switch(QueryAdjustCommand->UpDn){
				case 0x06:
					BackupQ++;
					break;
				case 0x03:
					if(BackupQ>=2){
						BackupQ--;
					}
					break;
				default:
					break;
			}
			//halRandomNumberGen((byte*)&SlotCounter ,2 );
			SlotCounter=GeneratorSlotCounter(BackupQ);

			if(SlotCounter != 0)
			{
				state=ArbitrateState;
				return ;
			}
			else
			{
				state=ReplyState;
				return ;
			}
			break;
		default:
			state=ArbitrateState;    //Remain in Arbitrate 
			return ;
		}
	}
}
