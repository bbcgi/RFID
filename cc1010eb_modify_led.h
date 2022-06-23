/*****************************************************************************
 *                                                                           *
 *        **********                                                         *
 *       ************                                                        *
 *      ***        ***                                                       *
 *      ***   +++   ***                                                      *
 *      ***   + +   ***                                                      *
 *      ***   +                            CHIPCON CC1010                    *
 *      ***   + +   ***                   EVALUATION BOARD                   *
 *      ***   +++   ***               HARDWARE DEFINITION FILE               *
 *      ***       ***                                                        *
 *       ***********                                                         *
 *        *********                                                          *
 *                                                                           *
 *****************************************************************************
 * The CC1010EB Evaluation Board has a number of buttons and LEDS which can  *
 * be accessed more easily using the macros in this files.                   *
 *****************************************************************************
 * Author:              ROH                                                  *
 *****************************************************************************
 * Revision history:                                                         *
 * 1.0  2002/04/01      First Public Release                                 *
 *                                                                           *
 * $Log: cc1010eb.h,v $
 * Revision 1.2  2003/01/27 08:02:30  tos
 * Corrected LED macro (disable): Blue LED = Port2 Pin5.
 *
 * Revision 1.1  2002/10/14 11:49:07  tos
 * Initial version in CVS.
 *
 *                                                                           *
 ****************************************************************************/

#ifndef CC1010EB_H
#define CC1010EB_H

#include <Chipcon/reg1010.h>    // Include register definitions

//********** Constants ************
#define CC1010EB_CLKFREQ 14746

//********** LED macros ***********

/*
#define RLED P1_2
#define YLED P1_3
#define GLED P1_4
#define BLED P2_5
*/

#define RLED P2_0
#define YLED P2_1
#define GLED P1_4
#define BLED P0_2

//modify
#define LED_ON 	1
#define LED_OFF 0

// LED pin output enable macros
//modify by aaa
#define RLED_OE(x) {P2DIR=(x) ? P2DIR&~0x01 : P2DIR|0x01;}
#define YLED_OE(x) {P2DIR=(x) ? P2DIR&~0x02 : P2DIR|0x02;}
#define GLED_OE(x) {P1DIR=(x) ? P1DIR&~0x10 : P1DIR|0x10;}
#define BLED_OE(x) {P0DIR=(x) ? P0DIR&~0x04 : P0DIR|0x04;}


//********** Switch macros ***********
#define SW1_PRESSED (!P1_7)
#define SW2_PRESSED (!P3_2)
#define SW3_PRESSED (!P3_3)
#define SW4_PRESSED (!P2_4)

//********** Misc macros ************
#define DCLK            P0_2
#define DIO             P0_1
#define DCLKIO_OE(b)    {P0DIR=(P0DIR&~0x06)|((b)?0x00:0x06);}

// PUT IN EXAMPLE RF_RXTXPAIR STRUCTURES FOR 434/868/915 !!!


#endif //CC1010EB_H
