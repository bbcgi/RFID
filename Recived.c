#include <chipcon/hal.h>
#include <chipcon/reg1010.h>
#include <chipcon/cc1010eb.h>

#define CRC16_POLY  0x1021
#define CRC16_INIT  0xFFFF
#define CRC_OK       0
#define PREAMBLE_BYTE_COUNT 7

xdata byte buffer[100];

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

    // Calibration data
    RF_RXTXPAIR_CALDATA xdata RF_CALDATA;

void main()
{
   char i ;
   char PtrRssiByte=0;
   byte command ;
   //char *RSIBYTE = 1 ; 
   
   byte *RN16 ;
   byte ReciveByte ;     
  for(i=0;i<100;i++)
          buffer[i]=0xff ;
    // Initialize peripherals
    WDT_ENABLE(FALSE);
    RLED_OE(TRUE);
    YLED_OE(TRUE);
    BLED_OE(TRUE);

    // Set optimum settings for speed and low power consumption
    MEM_NO_WAIT_STATES();
    FLASH_SET_POWER_MODE(FLASH_STANDBY_BETWEEN_READS);

    // Calibration
    halRFCalib(&RF_SETTINGS, &RF_CALDATA);
RLED_OE(TRUE);
RLED=LED_ON ;
BLED_OE(TRUE);
BLED=LED_ON ;
YLED_OE(TRUE);
YLED=LED_ON ;

//BLED_OE(TRUE);
//BLED=LED_ON;
    // Turn on RF for RX
    halRFSetRxTxOff(RF_RX, &RF_SETTINGS, &RF_CALDATA);
while(1){
      
    ReciveByte = halRFReceivePacket(10,&buffer,25,0,CC1010EB_CLKFREQ) ;
    command = buffer[0];    
	switch (command){
	    case 0x08:  //Query
               BLED=~BLED;
			   halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
			   halRandomNumberGen(RN16 , 2);  //generate RN16
               halRFSendPacket(PREAMBLE_BYTE_COUNT,RN16,2);
			   break ;
        case 0x0A:  //Select
	           YLED=~YLED ;
               break;
		 default:
		       RLED=~RLED;
		 }
  /* if(ReciveByte!= 0 )
	{
	   // Turn on RF for TX
	   halRFSetRxTxOff(RF_TX, &RF_SETTINGS, &RF_CALDATA);
	   halRFSendPacket(PREAMBLE_BYTE_COUNT,&RN16,2);
	   YLED=~YLED;
	   RLED=LED_OFF ;
     }
	 else
	 {
	    RLED=~RLED ;
		YLED=LED_OFF ;
	  }*/
    
  //  BLED=~BLED;
   
	
   }

}
