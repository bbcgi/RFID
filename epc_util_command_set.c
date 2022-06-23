#include "epc_util_command_set.h"

xdata byte buffer[100];
xdata MySelect select;
xdata MyQuery query;
xdata MyQueryRep queryRep;
xdata MyACK ack;
xdata MyReq_RN reqRn;
xdata MyKill kill;
xdata MyRead read;
xdata MyWrite write;

typedef struct{
	unsigned char header;
	unsigned short slot;
}QUERY_SLOT_RESPONSE;

typedef struct{
	unsigned char header;
	unsigned char epc_data[12];
}QUERY_TAG_DATA_RESPONSE;

QUERY_SLOT_RESPONSE query_slot_response;
QUERY_TAG_DATA_RESPONSE query_tag_data_response;

void init_command(){
	select.cmd_sel=SELECT_HEADER;
	select.target=0x00;
	select.action=0x00;
	select.MenBank=EPC_MEM_BANK;
	select.EBV=0x00;
	//select.Length=0x08;//by byte 
	select.Mask[0]=0xffff;
	select.Mask[1]=0xffff;
	select.Mask[2]=0xffff;
	select.Mask[3]=0xffff;	
	select.Truncate=0x00;

	query.cmd_query=QUERY_HEADER;
	query.DR=0x00;
	query.M=0x00;
	query.TRext=0x00;
	query.sel=0x00;
	query.session_query=0x00;
	query.tgt=0x00;
	query.Q=5;

	queryRep.cmd_QR=QUERY_REP_HEADER;
	queryRep.seesion_QR=0x00;

	ack.cmd_ACK=ACK_HEADER;

	reqRn.cmd_Req_RN=REQ_RN_HEADER;

	kill.cmd_Kill=KILL_HEADER;
	kill.RFU_Kill=0x00;
	
	read.cmd_Read=READ_HEADER;
	//read.MenBank_Read=USER_MEM_BANK;
	read.WordPtr=0x00;
	//read.WordCount=2;
	//read.RN_Read=0xABCD;
	//read.CRC16_Read=0x0000;

	write.cmd_Write=WRITE_HEADER;
	//write.ManBank_Write=USER_MEM_BANK;
	//write.WordPtr_Write=0x00;
	//write.Data_Write=0xAACC;
	//write.RN_Write=0xABCD;
}
void deSelect(){
	unsigned short temp_short;

	select.MenBank=EPC_MEM_BANK;	
	select.Length=0x04;

	select.Mask[0]=0xffff;
	select.Mask[1]=0xffff;
	select.Mask[2]=0xffff;
	select.Mask[3]=0xffff;

	temp_short=culSmallCRC16Block((byte*)&select,15,CRC16_INIT);
	select.CRC16_select=temp_short;

	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&select,17);
}
void select_by_mask(unsigned char bank,unsigned short*mask,unsigned char maskLen,unsigned char Q){//until scan all slotcounter
	unsigned short temp_short;
	unsigned short i;	
	unsigned char temp_byte;

	select.MenBank=bank;	
	select.Length=maskLen;

	select.Mask[0]=*mask;mask++;
	select.Mask[1]=*mask;mask++;
	select.Mask[2]=*mask;mask++;
	select.Mask[3]=*mask;

	temp_short=culSmallCRC16Block((byte*)&select,15,CRC16_INIT);
	select.CRC16_select=temp_short;

	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&select,17);

	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&select,17);

	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&select,17);

	query.Q=Q;
	temp_short=culSmallCRC16Block((byte*)&query,8,CRC16_INIT);
	query.CRC16_query=temp_short;
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&query,10);

	temp_short=1;
	temp_short<<=Q;

	for(i=0;i<temp_short;i++){

		halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
		halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&queryRep,2);

RFCON=0x0E;		
		halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
		temp_byte=halRFReceivePacket(7,&buffer,5,0,CC1010EB_CLKFREQ) ;
if(RFCON&0x10){
	halWait(25,CC1010EB_CLKFREQ);
	query_slot_response.header=0xFA;
	query_slot_response.slot=i;
	send_data_frame((unsigned char*)&query_slot_response,1);	
}
		if(temp_byte>0){
			ack.RN_ACK=*((unsigned short*)buffer);
			halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
			halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&ack,3);

			halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
			temp_byte=halRFReceivePacket(10,query_tag_data_response.epc_data,100,0,CC1010EB_CLKFREQ) ;//receive epc

			query_tag_data_response.header=0x0B;
			send_data_frame((unsigned char*)&query_tag_data_response,13);
			query_tag_data_response.header=0x00;
		}else{
			query_slot_response.header=0x0A;
			query_slot_response.slot=i;
			send_data_frame((unsigned char*)&query_slot_response,3);
		}
	}

halWait(25,CC1010EB_CLKFREQ);

	query_slot_response.header=0xCA;
	send_data_frame((unsigned char*)&query_slot_response,1);
halWait(25,CC1010EB_CLKFREQ);

	query_slot_response.header=0xCA;
	send_data_frame((unsigned char*)&query_slot_response,1);
halWait(25,CC1010EB_CLKFREQ);

	query_slot_response.header=0xCA;
	send_data_frame((unsigned char*)&query_slot_response,1);
}
void kill_tag(unsigned short* epc_data_4_word,unsigned short* pwd){
	unsigned short handler;
	unsigned short i;
	unsigned short GetFromTag;
	handler=select_single_tag(epc_data_4_word,1);
	if(handler==0x00){
	RLED=LED_ON;
	return;//fail!
	}

	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	reqRn.RN_Req_RN=handler;
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&reqRn,3);
	
	halWait(5,CC1010EB_CLKFREQ);

	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&reqRn,5);
	halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
	for(i=1;i<3;i++){
		GetFromTag=halRFReceivePacket(10,&buffer,25,0,CC1010EB_CLKFREQ) ;
		if(GetFromTag>0)break;
	}
	halWait(10,CC1010EB_CLKFREQ);

	kill.RN_Kill=handler;
	kill.pwd_Kill=*pwd;pwd++;
	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&kill,8);
	halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
	
	for(i=1;i<3;i++){
		GetFromTag=halRFReceivePacket(10,&buffer,25,0,CC1010EB_CLKFREQ) ;
		if(GetFromTag>0)break;
	}
	halWait(10,CC1010EB_CLKFREQ);
	

	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&reqRn,5);
	halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
	for(i=1;i<3;i++){
		GetFromTag=halRFReceivePacket(10,&buffer,25,0,CC1010EB_CLKFREQ) ;
		if(GetFromTag>0)break;
	}
	halWait(10,CC1010EB_CLKFREQ);

	kill.pwd_Kill=*pwd;
	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&kill,8);
	halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
	
	for(i=1;i<3;i++){
		GetFromTag=halRFReceivePacket(10,&buffer,25,0,CC1010EB_CLKFREQ) ;
		if(GetFromTag>0)break;
	}
}
//select_single_tag will return handler
unsigned short select_single_tag(unsigned short* epc_data_4_word,unsigned char Q){//set tag to open or secured
	unsigned short temp_short;
	unsigned short *short_pointer;
	unsigned short i;
	unsigned char temp_byte;
	unsigned char compare_count;
	unsigned short* epc_data_4_word_backup=epc_data_4_word;

	//init_command();
select.cmd_sel=0xDD;

	select.MenBank=EPC_MEM_BANK;	
	select.Length=0x08;
	select.Mask[0]=*epc_data_4_word;
	epc_data_4_word++;
	select.Mask[1]=*epc_data_4_word;
	epc_data_4_word++;
	select.Mask[2]=*epc_data_4_word;
	epc_data_4_word++;
	select.Mask[3]=*epc_data_4_word;
	
	temp_short=culSmallCRC16Block((byte*)&select,15,CRC16_INIT);
	select.CRC16_select=temp_short;

	
	for(i=0;i<7;i++){
		halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
		halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&select,17);
	}

select.cmd_sel=SELECT_HEADER;


	


	query.Q=Q;
	temp_short=culSmallCRC16Block((byte*)&query,8,CRC16_INIT);
	query.CRC16_query=temp_short;
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&query,10);

	temp_short=1;
	temp_short<<=Q;
	for(i=0;i<temp_short;i++){
		halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
		halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&queryRep,2);
		
		halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
		temp_byte=halRFReceivePacket(7,&buffer,5,0,CC1010EB_CLKFREQ);

		if(temp_byte>0){
			ack.RN_ACK=*((unsigned short*)buffer);
			halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
			halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&ack,3);
		
			halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
			temp_byte=halRFReceivePacket(10,&buffer,100,0,CC1010EB_CLKFREQ) ;//receive epc
			if(temp_byte==0){
				continue;
			}
			
			compare_count=0;
			short_pointer=(unsigned short*)&buffer;
			short_pointer+=2;
			for(i=0;i<4;i++){
				if(epc_data_4_word_backup[i]==short_pointer[i]){
					compare_count++;
				}
			}
			if(compare_count==4){
				break;
			}
		}
	}

	if(temp_byte==0){
	return 0x00;//fail!
	}


	
	halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	reqRn.RN_Req_RN=ack.RN_ACK;
	halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&reqRn,3);

	halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
	temp_byte=halRFReceivePacket(7,(byte*)&temp_short,10,0,CC1010EB_CLKFREQ) ;//receive handler

	if(i==temp_byte)return 0x00;//fail!

	return temp_short;
}

//len in word(2 byte), return receive bytes count
unsigned char get_data_by_epc(unsigned short* epc_data_4_word,unsigned char bank,unsigned char* output,unsigned char len){
	unsigned short handler=0;
	unsigned char receiveCount=0;
	unsigned char test=0;

		handler=select_single_tag(epc_data_4_word,1);

	if(handler==0){
		return 0;
	}
		read.MenBank_Read=bank;
		read.WordCount=len;
		read.RN_Read=handler;


	test=3;

	while(test-- && receiveCount==0){
		halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
		halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&read,8);
		
		halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
		receiveCount=halRFReceivePacket(10,output,len*2,0,CC1010EB_CLKFREQ);
	}

	return receiveCount;
}

//len in word(2 byte), return write count ,return 0 if fail.
unsigned char put_data_by_epc(unsigned short* epc_data_4_word,unsigned char bank,unsigned char* input,unsigned char offset,unsigned char len){
	unsigned short temp;
	unsigned short* word_pointer;
	unsigned char i;

	temp=select_single_tag(epc_data_4_word,1);
	word_pointer=(unsigned short*)input;

	write.ManBank_Write=bank;
	write.RN_Write=temp;

	for(i=0;i<len;i++){
		write.WordPtr_Write=i+offset;
		write.Data_Write=*word_pointer;
		word_pointer++;

		halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
		halRFSendPacket(PREAMBLE_BYTE_COUNT,(byte*)&write,9);
		
		halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
		temp=halRFReceivePacket(10,&buffer,25,0,CC1010EB_CLKFREQ);
		if(temp==0)return 0;

		halWait(10,CC1010EB_CLKFREQ);
	}

	return len;
}

void send_rs232_sync_bytes(){
	unsigned char i;
	for(i=0;i<10;i++){
#ifdef UART0
		UART0_WAIT_AND_SEND(0xAA);
#else
		UART1_WAIT_AND_SEND(0xAA);
#endif
	}
#ifdef UART0
		UART0_WAIT_AND_SEND(0xCC);
#else
		UART1_WAIT_AND_SEND(0xCC);
#endif
}
void send_rs232_finish_bytes(){
	unsigned char i;
	for(i=0;i<10;i++){
#ifdef UART0
		UART0_WAIT_AND_SEND(0xAA);
#else
		UART1_WAIT_AND_SEND(0xAA);
#endif
	}	
}
void send_data_frame(unsigned char* data_pointer,unsigned char len){
	unsigned char i;
	send_rs232_sync_bytes();
	
	for(i=0;i<len;i++){
#ifdef UART0
		UART0_WAIT_AND_SEND(*data_pointer);
#else
		UART1_WAIT_AND_SEND(*data_pointer);
#endif
		data_pointer++;
	}
	send_rs232_finish_bytes();
}
