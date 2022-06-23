#include "EPC_STATE.h"

void ReadyStateFunction(){

	RLED=LED_ON;
	
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
			default :
				state=ReadyState;    //Remain in Ready 
				return ;
		}
	}
}


