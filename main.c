#include "EPC_STATE.h"
#include <stdlib.h>
#include <string.h>

byte command ;
xdata byte buffer[64];

unsigned short RN_16;
unsigned short CRC_16;
unsigned short Handler;
unsigned short SlotCounter;
unsigned char ReciveByte ;
unsigned char BackupQ;

/*
unsigned short KillPwd;
unsigned short AccessPwd;
unsigned short PC;
unsigned short EPC_CRC;
unsigned short EPC_DATA[4];
unsigned short TID_DATA[2];
unsigned short USER_DATA[2];
*/

xdata ReceivedMemBank ReceivedMB;
xdata EpcMemBank EpcMB;
xdata TidMemBank TidMB;
xdata UserMemBank UserMB;

MyQueryRep* QueryRepCommand;
MyQueryAdjust* QueryAdjustCommand;
MyQuery* QueryCommand;

MySelect* SelectCommand;
MyKill* KillCommand ;
MyAccess* AccessCommand ;
MyRead* ReadCommand ;
MyWrite* WriteCommand ;
MyBlockWrite* BlockWriteCommand ;
MyBlockErase* BlockEraseCommand ; 
MyACK* Ack;
MyReq_RN* ReqRNCommand;

WRITE_RESPONSE Write_Response;
MyTagErrorCode TagErrorCode;

unsigned char state;

#define RAMBUF_ADDRESS      0x0000
#define FLASHPAGE_ADDRESS   0x4500
byte xdata tagData[128];
byte xdata ramBuf[128] _at_ RAMBUF_ADDRESS;
byte code flashPage[128] _at_ FLASHPAGE_ADDRESS;

bit IsSelected;
unsigned char isPowerDownKilled=0;
// X-tal frequency: 14.745600 MHz
// RF frequency A: 927.968523 MHz	Rx
// RF frequency B: 927.968523 MHz	Tx
// RX Mode: Low side LO
// Frequency separation: 64 kHz
// Data rate: 38.4 kBaud
// Data Format: Manchester
// RF output power: 0 dBm
// IF/RSSI: RSSI Enabled

RF_RXTXPAIR_SETTINGS code RF_SETTINGS = {
    0xA9, 0x2F, 0x2A,    // Modem 0, 1 and 2
    0xCC, 0x60, 0x00,    // Freq A
    0x5E, 0x45, 0x03,    // Freq B
    0x01, 0xAB,          // FSEP 1 and 0
    0x68,                // PLL_RX
    0x30,                // PLL_TX
    0x6C,                // CURRENT_RX
    0xF3,                // CURRENT_TX
    0x32,                // FREND
    0x0A,                // PA_POW
    0x00,                // MATCH
    0x00,                // PRESCALER
    };
RF_RXTXPAIR_CALDATA xdata RF_CALDATA;

void init(){
	//mainFunctionPointer=&ReadyStateFunction;
	//mainFunctionPointer=OpenStateFunction;
	state=ReadyState;

	// Initialize peripherals
	WDT_ENABLE(FALSE);
	// Set optimum settings for speed and low power consumption
	MEM_NO_WAIT_STATES();
	FLASH_SET_POWER_MODE(FLASH_STANDBY_BETWEEN_READS);
	// Calibration
	halRFCalib(&RF_SETTINGS, &RF_CALDATA);
	
	RLED_OE(TRUE);
	GLED_OE(TRUE);
	BLED_OE(TRUE);
	YLED_OE(TRUE);

	TurnOffAllLed();
	ReadTagDataFromFlash();
	EpcMB.EPC_CRC=culSmallCRC16Block((byte*)(EpcMB.EPC_DATA),4,CRC16_INIT);

	IsSelected=0;

	//SlotCounter=3;//for test
	//BackupQ=5;//for test
	//RN_16=0xABCD;//for test
	//Handler=0xABCD;//for test
	//ReceivedMB.KillPwdA=0x1234;//for test open state
	//ReceivedMB.KillPwdB=0x5678;//for test secured state
	//ReceivedMB.AccessPwdA=0xff00;//for test open state
	//ReceivedMB.AccessPwdB=0x00ff;//for test secured state
	//ReceivedMB.AccessPwdA=0x0000;//for test secured state
	//ReceivedMB.AccessPwdB=0x0000;//for test secured state
}

void TurnOffAllLed(){
	RLED=LED_OFF;
	GLED=LED_OFF;
	BLED=LED_OFF;
	YLED=LED_OFF;
}
void TurnOnAllLed(){
	RLED=LED_ON;
	GLED=LED_ON;
	BLED=LED_ON;
	YLED=LED_ON;
}

unsigned short GeneratorSlotCounter(unsigned char q){
	unsigned short temp=0;
	unsigned short mod=1;
	mod<<=q;

	halRandomNumberGen((byte*)&temp ,2);
	temp%=mod;

	return temp;
}
void AnswerToSingleSelect(MySelect* scmd)
{
	unsigned char i;
	unsigned short* data_pointer;
	unsigned short mCount=0;
	bit result=0;

	switch(scmd->MenBank){
		case RESERVED_MEM_BANK:
			data_pointer=(unsigned short*)&ReceivedMB;
			break;
		case EPC_MEM_BANK:
			data_pointer=(unsigned short*)&EpcMB;
			data_pointer+=2;
			break;
		case TID_MEM_BANK:
			data_pointer=(unsigned short*)&TidMB;
			break;
		case USER_MEM_BANK:
			data_pointer=(unsigned short*)&UserMB;
			break;
	}
	scmd->Length/=2;
	for(i=0;i<scmd->Length;i++){
		if(scmd->Mask[i]==*data_pointer){
			mCount++;
		}
		data_pointer++;
	}

	if(mCount==scmd->Length){		
		state=ReadyState;
		IsSelected=1;
		GLED=LED_ON;
	}else{
		IsSelected=0;
		GLED=LED_OFF;
	}

	return;
}

void AnswerToSelect(MySelect* scmd)
{
	unsigned char i;
	unsigned short* data_pointer;
	unsigned short mask_result;
	unsigned short mCount=0;
	bit result=0;

	switch(scmd->MenBank){
		case RESERVED_MEM_BANK:
			data_pointer=(unsigned short*)&ReceivedMB;
			break;
		case EPC_MEM_BANK:
			data_pointer=(unsigned short*)&EpcMB;
			data_pointer+=2;
			break;
		case TID_MEM_BANK:
			data_pointer=(unsigned short*)&TidMB;
			break;
		case USER_MEM_BANK:
			data_pointer=(unsigned short*)&UserMB;
			break;
	}
	scmd->Length/=2;
	for(i=0;i<scmd->Length;i++){
		mask_result=*data_pointer; 
		mask_result&=scmd->Mask[i];
		if(mask_result==*data_pointer){
			mCount++;
		}
		data_pointer++;
	}

	if(mCount==scmd->Length){		
		state=ReadyState;
		IsSelected=1;
		GLED=LED_ON;
	}else{
		IsSelected=0;
		GLED=LED_OFF;
	}
	return;
}

void AnswerToQuery(MyQuery* qcmd)
{

	//have to check IsSelected
	if(!IsSelected)return;

	//依照Q值產生該範圍的變數
	BackupQ=qcmd->Q;
	SlotCounter=GeneratorSlotCounter(BackupQ);

	//if slot=0 backscatter RN16 and go to Reply
	if( SlotCounter == 0)
	{
		halRandomNumberGen((byte*)&RN_16 , 2);
		halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
		halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&RN_16,2);	
	    halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
		state=ReplyState;    //transition to reply
		return ;
	}
	
	//if slot>0 go to Arbitrate and reply none
	else if (SlotCounter >0 )
	{
		state=ArbitrateState;    //transition to Arbitrate
		return ; 
	}
	else
	{ 
		state=ReadyState;    //Remain in Ready 
		return ;
	}

}

void ReadTagDataFromFlash(){
	unsigned short *dataPointer=(unsigned short code*)flashPage;
	unsigned char i;

	
	isPowerDownKilled=*dataPointer;
	dataPointer++;
	if(isPowerDownKilled){
	    halConfigTimer23(TIMER2 | TIMER23_INT_TIMER, 50000, CC1010EB_CLKFREQ);
	    INT_ENABLE(INUM_TIMER2, INT_ON);
	    INT_PRIORITY(INUM_TIMER2, INT_LOW);
	    INT_GLOBAL_ENABLE(INT_ON);
	    TIMER2_RUN(TRUE);
	}

	ReceivedMB.KillPwdA=*dataPointer;
	dataPointer++;
	ReceivedMB.KillPwdB=*dataPointer;
	dataPointer++;
	
	ReceivedMB.AccessPwdA=*dataPointer;
	dataPointer++;
	ReceivedMB.AccessPwdB=*dataPointer;
	dataPointer++;
	
	for(i=0;i<4;i++){
		EpcMB.EPC_DATA[i]=*dataPointer;
		dataPointer++;	
	}
	
	for(i=0;i<2;i++){
		TidMB.TID_DATA[i]=*dataPointer;
		dataPointer++;	
	}
	
	for(i=0;i<2;i++){
		UserMB.USER_DATA[i]=*dataPointer;
		dataPointer++;	
	}

	//save power down kill info to flash

	dataPointer=(unsigned short xdata*)ramBuf;
	*dataPointer=1;
	FLASH_SET_POWER_MODE(FLASH_ALWAYS_ON );
	halFlashWritePage(flashPage, ramBuf, CC1010EB_CLKFREQ);
	FLASH_SET_POWER_MODE(FLASH_STANDBY_BETWEEN_READS);
}

void WriteTagData2Flash() {
	unsigned short *dataPointer=(unsigned short xdata*)tagData;
	unsigned char i;

	*dataPointer=0;
	dataPointer++;

	*dataPointer=ReceivedMB.KillPwdA;
	dataPointer++;
	*dataPointer=ReceivedMB.KillPwdB;
	dataPointer++;
	
	*dataPointer=ReceivedMB.AccessPwdA;
	dataPointer++;
	*dataPointer=ReceivedMB.AccessPwdB;
	dataPointer++;
	
	for(i=0;i<4;i++){
		*dataPointer=EpcMB.EPC_DATA[i];
		dataPointer++;	
	}
	
	for(i=0;i<2;i++){
		*dataPointer=TidMB.TID_DATA[i];  
		dataPointer++;	
	}
	
	for(i=0;i<2;i++){
		*dataPointer=UserMB.USER_DATA[i];
		dataPointer++;	
	}
	FLASH_SET_POWER_MODE(FLASH_ALWAYS_ON );
	memcpy(ramBuf, tagData, 128);
	halFlashWritePage(flashPage, ramBuf, CC1010EB_CLKFREQ);
	FLASH_SET_POWER_MODE(FLASH_STANDBY_BETWEEN_READS);
}

/*word CRC16Check()
{
     
}*/



void main(){
	init();
	
	while(1){
		switch(state){
			case ReadyState:
				ReadyStateFunction();
				break;
			case ArbitrateState:
				ArbitrateStateFunction();
				break; 
			case ReplyState:
				ReplyStateFunction();
				break;
			case AcknowledgedState:
				AcknowledgedStateFunction();
				break;
			case OpenState:
				OpenStateFunction();
				break;
			case SecuredState:
				SecuredStateFunction();
				break;
			case KilledState:
				KilledStateFunction();
				break;
		}
		//current_mainFunctionPointer=mainFunctionPointer;
		//mainFunctionPointer=0;
		//current_mainFunctionPointer();
		TurnOffAllLed();
	}	
}

void TIMER2_ISR() interrupt INUM_TIMER2 {
    INT_SETFLAG (INUM_TIMER2, INT_CLR);
    RLED = ~RLED;
}

void FlashIntrHandler(void) interrupt INUM_FLASH {    
    INT_SETFLAG(INUM_FLASH, INT_CLR);    
    return;
}
