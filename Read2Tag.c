#include <chipcon/hal.h>
#include <chipcon/cul.h>
#include <chipcon/reg1010.h>
#include <chipcon/cc1010eb_modify_led.h>
#include "EPC_COMMAND.h"
#include "epc_util_command_set.h"

xdata unsigned short tag_change_data[6];
//#define SET_TARGET(t,x) (t->target=x) 


// X-tal frequency: 14.745600 MHz
// RF frequency A: 433.302000 MHz	Rx
// RF frequency B: 433.302000 MHz	Tx
// RX Mode: Low side LO
// Frequency separation: 64 kHz
// Data rate: 2.4 kBaud
// Data Format: Manchester
// RF output power: 10 dBm
// IF/RSSI: RSSI Enabled


/*
RF_RXTXPAIR_SETTINGS code RF_SETTINGS = {
    0x4B, 0x2F, 0x0E,    // Modem 0, 1 and 2
    0x58, 0x00, 0x00,    // Freq A
    0x41, 0xFC, 0x9C,    // Freq B
    0x02, 0x80,          // FSEP 1 and 0
    0x60,                // PLL_RX
    0x48,                // PLL_TX
    0x44,                // CURRENT_RX
    0x81,                // CURRENT_TX
    0x0A,                // FREND
    0xFF,                // PA_POW
    0xC0,                // MATCH
    0x00,                // PRESCALER
    };
*/
// X-tal frequency: 14.745600 MHz
// RF frequency A: 433.038686 MHz	Rx
// RF frequency B: 433.038686 MHz	Tx
// RX Mode: Low side LO
// Frequency separation: 64 kHz
// Data rate: 38.4 kBaud
// Data Format: Manchester
// RF output power: 0 dBm
// IF/RSSI: RSSI Enabled

RF_RXTXPAIR_SETTINGS code RF_SETTINGS = {
    0xA9, 0x2F, 0x1C,    // Modem 0, 1 and 2
    0x66, 0xA0, 0x00,    // Freq A
    0x41, 0xF2, 0x53,    // Freq B
    0x02, 0x80,          // FSEP 1 and 0
    0x70,                // PLL_RX
    0x48,                // PLL_TX
    0x44,                // CURRENT_RX
    0x81,                // CURRENT_TX
    0x0A,                // FREND
    0x40,                // PA_POW
    0xC0,                // MATCH
    0x00,                // PRESCALER
    };

    // Calibration data
    RF_RXTXPAIR_CALDATA xdata RF_CALDATA;

unsigned char receivedByte;
unsigned char memBank;
unsigned char Q;
unsigned char maskLen;
unsigned char mask[8];
unsigned char memData[8];
unsigned char* char_pointer;
unsigned result;

int uart0_wait_and_receive(unsigned char *cp,unsigned short timeout){
	data unsigned char i;
	for(i=0;i<100;i++){
	    if (RI_0) {
	        RI_0=0;
			*cp=UART0_RECEIVE();
	        return 0;         
	    }
	}

	halConfigTimer23(TIMER3|TIMER23_NO_INT_TIMER, 10000, CC1010EB_CLKFREQ);   
    INT_SETFLAG(INUM_TIMER3, INT_CLR);
    TIMER3_RUN(TRUE);

    while (1) {
        // Check if 10 ms have passed
        if (INT_GETFLAG(INUM_TIMER3)) {
            // Clear interrupt flag and decrement timeout value
            INT_SETFLAG(INUM_TIMER3, INT_CLR);
			if (timeout && !--timeout) {
				timeout=255;
                return 1;                      // Timeout
			}
        }
        if (RI_0) {
            RI_0=0;
			*cp=UART0_RECEIVE();
            return 0;         
        }
    }
}

int uart1_wait_and_receive(unsigned char *cp,unsigned short timeout){
	data unsigned char i;
	for(i=0;i<100;i++){
	    if (RI_1) {
	        RI_1=0;
			*cp=UART1_RECEIVE();
	        return 0;         
	    }
	}

	halConfigTimer23(TIMER3|TIMER23_NO_INT_TIMER, 10000, CC1010EB_CLKFREQ);   
    INT_SETFLAG(INUM_TIMER3, INT_CLR);
    TIMER3_RUN(TRUE);

    while (1) {
        // Check if 10 ms have passed
        if (INT_GETFLAG(INUM_TIMER3)) {
            // Clear interrupt flag and decrement timeout value
            INT_SETFLAG(INUM_TIMER3, INT_CLR);
			if (timeout && !--timeout) {
				timeout=255;
                return 1;                      // Timeout
			}
        }
        if (RI_1) {
            RI_1=0;
			*cp=UART1_RECEIVE();
            return 0;         
        }
    }
}

void main()
{

	

	unsigned char i;
	// Initialize peripherals
	WDT_ENABLE(FALSE);
	// Set optimum settings for speed and low power consumption
	MEM_NO_WAIT_STATES();
	FLASH_SET_POWER_MODE(FLASH_STANDBY_BETWEEN_READS);
	
	// Calibration
	halRFCalib(&RF_SETTINGS, &RF_CALDATA);

#ifdef UART0
		UART0_SETUP(57600, CC1010EB_CLKFREQ, UART_NO_PARITY | UART_RX_TX | UART_POLLED);
#else
		UART1_SETUP(57600, CC1010EB_CLKFREQ, UART_NO_PARITY | UART_RX_TX | UART_POLLED);
#endif
	RLED_OE(TRUE);
	GLED_OE(TRUE);
	BLED_OE(TRUE);
	YLED_OE(TRUE);
	
	init_command();
		
	BLED=LED_ON;
	while(1){
		init_command();
		deSelect();
		halRFSetRxTxOff(RF_OFF, &RF_SETTINGS, &RF_CALDATA);

		receivedByte=getchar();
/*
#ifdef UART0
		if(uart0_wait_and_receive(&receivedByte,10)){
			//if time out
				UART0_WAIT_AND_SEND(0xAA);
#else
		if(uart1_wait_and_receive(&receivedByte,10)){
			//if time out
				UART1_WAIT_AND_SEND(0xAA);
#endif
			continue;
		}*/

		RLED=LED_OFF;
		switch(receivedByte){
			case 0x0a://select command
				RLED=LED_ON;
				//send_data_frame(&receivedByte,1);
				Q=getchar();
				memBank=getchar();
				maskLen=getchar();
				for(i=0;i<maskLen;i++){
					mask[i]=getchar();
				}
				select_by_mask(memBank,(unsigned short*)&mask,maskLen,Q);
				RLED=LED_OFF;
				break;
			case 0xC0://read command 
				GLED=LED_ON;
				for(i=0;i<8;i++){
					mask[i]=getchar();
				}
				memBank=getchar();
				maskLen=getchar();


				result=get_data_by_epc((unsigned short*)mask,memBank,(unsigned char*)&tag_change_data,maskLen);
				
				if(result==0){
				RLED=LED_ON;
				GLED=LED_OFF;
					return;
				}
				send_rs232_sync_bytes();

#ifdef UART0
				UART0_WAIT_AND_SEND(0xf0);
				UART0_WAIT_AND_SEND(memBank);
#else
				UART1_WAIT_AND_SEND(0xf0);
				UART1_WAIT_AND_SEND(memBank);
#endif

				for(i=0;i<8;i++){
#ifdef UART0
					UART0_WAIT_AND_SEND(mask[i]);
#else
					UART1_WAIT_AND_SEND(mask[i]);
#endif
				}
				char_pointer=(unsigned char*)&tag_change_data;
				for(i=0;i<maskLen;i++){
					
#ifdef UART0
					UART0_WAIT_AND_SEND(*char_pointer);
#else
					UART1_WAIT_AND_SEND(*char_pointer);
#endif
					char_pointer++;
				}

				send_rs232_finish_bytes();
				deSelect();
				GLED=LED_OFF;
				break;
			case 0xc1://write command
				//send_data_frame(&receivedByte,1);
				for(i=0;i<8;i++){
					mask[i]=getchar();
				}
				memBank=getchar();
				maskLen=getchar();

				for(i=0;i<maskLen;i++){
					memData[i]=getchar();
					if(memData[i]==0x0a){
							RLED=~RLED;
							GLED=~GLED;
							BLED=~BLED;
							YLED=~YLED;
					}
				}
				put_data_by_epc((unsigned short*)mask,memBank,(unsigned char*)&memData,0,maskLen/2);//len in 2 byte
				break;
			case 0xdd://kill command
				for(i=0;i<8;i++){
					mask[i]=getchar();
				}

				for(i=0;i<4;i++){
					memData[i]=getchar();
				}
				kill_tag((unsigned short*)&mask,(unsigned short*)&memData);
				break;
		}
	}
}
