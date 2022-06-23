#include <chipcon/hal.h>
#include <chipcon/cul.h>
#include <chipcon/reg1010.h>
//#include <chipcon/cc1010eb.h>
#include "cc1010eb_modify_led.h"
#include "EPC_COMMAND.h"


#ifndef __EPC_STATE__
#define __EPC_STATE__

/*State Function */
void ReadyStateFunction();
void ArbitrateStateFunction();
void ReplyStateFunction();
void AcknowledgedStateFunction();
void OpenStateFunction();
void SecuredStateFunction();
void KilledStateFunction();


#define CRC16_POLY           0x1021
#define CRC16_INIT           0xFFFF
#define CRC_OK                 0
#define PREAMBLE_BYTE_COUNT    7 

extern bit IsSelected;
extern unsigned char BackupQ;

typedef struct {
        unsigned short KillPwdA;
		unsigned short KillPwdB;
		unsigned short AccessPwdA;
		unsigned short AccessPwdB;
		}ReceivedMemBank;

typedef struct {
		unsigned short EPC_CRC;
        unsigned short PC;
		unsigned short EPC_DATA[4];
		}EpcMemBank ;

typedef struct {
        unsigned short TID_DATA[2];
		}TidMemBank ;

typedef struct {
        unsigned short USER_DATA[2];
		}UserMemBank ;

extern xdata ReceivedMemBank ReceivedMB;
extern xdata EpcMemBank EpcMB;
extern xdata TidMemBank TidMB;
extern xdata UserMemBank UserMB;

/*
extern unsigned short KillPwd;
extern unsigned short AccessPwd;
extern unsigned short PC;
extern unsigned short EPC_CRC;
extern unsigned short EPC_DATA[4];
extern unsigned short TID_DATA[2];
extern unsigned short USER_DATA[2];
*/
#define AcknowledgedState		(0x00)
#define ArbitrateState			(0x01)
#define KilledState				(0x02)
#define OpenState				(0x03)
#define ReadyState				(0x04)
#define ReplyState				(0x05)
#define SecuredState			(0x06)
extern unsigned char state;

extern unsigned char RecieveByte ;
extern byte command ;
extern xdata byte buffer[64];
extern char  i ;
extern unsigned short RN_16;
extern unsigned short CRC_16;
extern unsigned short Handler;
extern unsigned short SlotCounter;
extern RF_RXTXPAIR_CALDATA xdata RF_CALDATA;
extern byte ReciveByte ;

extern MyQueryRep* QueryRepCommand;
extern MyQueryAdjust* QueryAdjustCommand;
extern MyQuery* QueryCommand;

extern MySelect* SelectCommand;
extern MyKill* KillCommand ;
extern MyAccess* AccessCommand ;
extern MyRead* ReadCommand ;
extern MyWrite* WriteCommand ;
extern MyBlockWrite* BlockWriteCommand ;
extern MyBlockErase* BlockEraseCommand ; 
extern MyACK* Ack;
extern MyReq_RN* ReqRNCommand;

extern WRITE_RESPONSE Write_Response;
extern MyTagErrorCode TagErrorCode;
 
void AnswerToSingleSelect(MySelect* scmd);
void AnswerToSelect(MySelect* scmd);
void AnswerToQuery(MyQuery* qcmd);

void TurnOffAllLed();
void ReadTagDataFromFlash();
void WriteTagData2Flash();

unsigned short GeneratorSlotCounter(unsigned char Q);

#endif
