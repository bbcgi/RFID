#include "EPC_STATE.h"

void SecuredStateFunction(){

	bit securedSateKillFirstDone=0;	
	unsigned short FirstKillPwd=0x0000;
	byte * data_pointer=0x00;
	byte * temp=0x00;
	unsigned char tempChar;
	unsigned char i;
	unsigned short *word_pointer=0x00;

	YLED=LED_ON;
	BLED=LED_ON;
	halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //打開RX

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
			case QUERY_ADJUST_HEADER :
			case QUERY_REP_HEADER :
				state=ReadyState;
				return ;		
			case ACK_HEADER:
				/*判斷Reader送回的ACK中RN16是否和Handler相同*/
				Ack=(MyACK*)buffer;
				if(Ack->RN_ACK==Handler)/*若相同則TAG反應PC,EPC,CRC-16*/
				{
					halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
					halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&EpcMB,12);
					halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); 
					continue;
				}
				else
				{
					state=ArbitrateState;
					return ;
				}	
				break;	
			case REQ_RN_HEADER :
				Ack=(MyACK*)buffer;
				if(Ack->RN_ACK==Handler)
				{
					halRandomNumberGen((byte*)&RN_16 , 2);
					halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
					halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&RN_16,2);
					halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //打開RX
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ;
				}
				else 
				{
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ;
				}
				break;
			case READ_HEADER :
				ReadCommand=(MyRead*)buffer;
				if(ReadCommand->RN_Read==Handler)//&& check /*valid memory access*/
				{
					//BackScatter Data :Not finished
					halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
					switch(ReadCommand->MenBank_Read){
						case RESERVED_MEM_BANK :
							data_pointer=(byte*)&ReceivedMB;
							break;
						case EPC_MEM_BANK :
							data_pointer=(byte*)&EpcMB;
							break;
						case TID_MEM_BANK:
							data_pointer=(byte*)&TidMB;
							break;
						case USER_MEM_BANK:
							data_pointer=(byte*)&UserMB;
							break;
					}
					temp=buffer;
					*temp=0;
					temp++;
					tempChar=ReadCommand->WordCount*2;
					for(i=0;i<tempChar;i++){
						*temp=*data_pointer;
						temp++;
						data_pointer++;
					}
					*temp=(Handler)>>8;
					temp++;
					*temp=((Handler)&0xff);

					halRFSendPacket(PREAMBLE_BYTE_COUNT,buffer,tempChar+3);
					halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //打開RX
					data_pointer=0x00;
				}
				break;
			case WRITE_HEADER :
				WriteCommand=(MyWrite*)buffer;
				if(WriteCommand->RN_Write==Handler)// check && /*valid memory access*/
				{
					
					halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
					switch(WriteCommand->ManBank_Write){
						case RESERVED_MEM_BANK :
							word_pointer=(unsigned short*)&ReceivedMB;
							break;
						case EPC_MEM_BANK :
							word_pointer=(unsigned short*)&EpcMB;
							break;
						case TID_MEM_BANK:
							word_pointer=(unsigned short*)&TidMB;
							break;
						case USER_MEM_BANK:
							word_pointer=(unsigned short*)&UserMB;
							break;
					}
					word_pointer+=(WriteCommand->WordPtr_Write);
					*word_pointer=(WriteCommand->Data_Write);
					word_pointer=0x00;

					Write_Response.Header=0x00;
					Write_Response.RN=Handler;
					halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&Write_Response,3);
					halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //打開RX
					//WriteTagData2Flash();
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return  ;
				}
				//else if(WriteCommand->RN_Write==Handler)// check && /*Invalid memory access*/
				//{
				//	MyTagErrorCode* TagErrorCodeCommand = (MyTagErrorCode*)buffer;
					//halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
					//halRFSendPacket(PREAMBLE_BYTE_COUNT,TagErrorCodeCommand->ErrorCode,1);
				//	continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ;
				//}
				//else
				//{
				//	continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ;
				//}	
				break;
			case KILL_HEADER:
				KillCommand = (MyKill*)buffer;
				if(KillCommand->RN_Kill==Handler)
				{
					if(securedSateKillFirstDone){
						securedSateKillFirstDone=0;
						if(KillCommand->pwd_Kill == ReceivedMB.KillPwdB && FirstKillPwd==ReceivedMB.KillPwdA){
							state=KilledState;
							halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
							halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&Handler,2);
							halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //打開RX
							return ; 
						}else if(KillCommand->pwd_Kill==0){
							/* Backscatter Error Code*/
							TagErrorCode.Header=0x01;
							TagErrorCode.ErrorCode=0x00;
							TagErrorCode.RN_TagErrorCode=Handler;
							
							halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
							halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&TagErrorCode,6);
							halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
							state=OpenState;
							return ; 
						}else{
							state=ArbitrateState;
						}
					}else{
						securedSateKillFirstDone =1;
						FirstKillPwd=KillCommand->pwd_Kill;
						halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
						halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&Handler,2);
						halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
						continue;
					}
				}else{
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ; 
				}
				break;
			case LOCK_HEADER :
				state=OpenState;
				return ; 
			case ACCESS_HEADER :
				AccessCommand = (MyAccess*)buffer;
				if(AccessCommand->RN_Access==Handler &&  AccessCommand->pwd_Access==0xACEE)
				{
					halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
					halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&RN_16,2);
					halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA); //打開RX
					state=SecuredState;
					return ; 
				}
				else if(AccessCommand->RN_Access==Handler &&  AccessCommand->pwd_Access!=0xACEE)
				{
					state=ArbitrateState;
					return ; 
				}
				else
				{
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ; 
				}
				break;
			case BLOCK_WRITE_HEADER :
				BlockWriteCommand=(MyBlockWrite*)buffer ;
				if(BlockWriteCommand->RN_BlockWrite==Handler)//check &&/*valid*/ 
				{
					//halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
					//halRFSendPacket(PREAMBLE_BYTE_COUNT,BlockWriteCommand->RN_16,2);
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ; 
				}
				else if(BlockWriteCommand->RN_BlockWrite==Handler)//check &&/*Invalid*/
				{  
					//not finished 
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ; 
				}
				else
				{
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ;  
				}
				break;
			case BLOCK_ERASE_HEADER:
				BlockEraseCommand=(MyBlockErase*)buffer ;
				if(BlockEraseCommand->RN_BlockErase==Handler)//check &&/*valid*/ 
				{
					//halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
					//halRFSendPacket(PREAMBLE_BYTE_COUNT,BlockEraseCommand->RN_16,2);
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ; 
				}
				else if(BlockWriteCommand->RN_BlockWrite==Handler)//check &&/*Invalid*/
				{  
					//not finished 
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ; 
				}
				else
				{
					continue;
					//mainFunctionPointer= &OpenStateFunction;
					//return ;  
				}
				break;
			default:
				state=ArbitrateState;
				return ; 
		}
	}
}