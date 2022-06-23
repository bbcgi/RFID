#include "EPC_STATE.h"

void KilledStateFunction(){
	TurnOffAllLed();

 	WriteTagData2Flash();
	halRFSetRxTxOff(RF_OFF, &RF_SETTINGS, &RF_CALDATA);
	while(1){	
		TurnOffAllLed();
		ENTER_SLEEP_MODE();
	}	
}