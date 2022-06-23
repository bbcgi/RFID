#ifndef __EPC_COMMAND__
#define __EPC_COMMAND__

#define RESERVED_MEM_BANK	0x00
#define EPC_MEM_BANK		0x01
#define TID_MEM_BANK		0x02
#define USER_MEM_BANK		0x03

/* Command Set */
#define     QUERY_REP_HEADER     0x00 
#define      ACK_HEADER         0x40  
#define     QUERY_HEADER        0x80
#define     QUERY_ADJUST_HEADER  0x90
#define     SELECT_HEADER       0xA0
#define      RFU_HEADER         0xB0
#define      NAK_HEADER         0xC0
#define      REQ_RN_HEADER      0xC1
#define      READ_HEADER        0xC2
#define      WRITE_HEADER       0xC3
#define      KILL_HEADER        0xC4
#define      LOCK_HEADER        0xC5
#define      ACCESS_HEADER      0xC6
#define      BLOCK_WRITE_HEADER  0xC7 
#define      BLOCK_ERASE_HEADER  0xC8

#define		 SINGLE_SELECT_HEADER 0xDD

//Define Sender Command 
//Select Set
void WriteTagData2Flash();
typedef struct SelectStruct{
	    byte cmd_sel ;
		byte target ;
        byte action ;
		byte MenBank ;
		byte EBV ;
		byte Length ;// by byte in this implement
		unsigned short Mask[4] ;
		byte Truncate ;
	    unsigned short  CRC16_select ;
	}MySelect ;
    
//Inventory Set     
typedef struct QueryStruct{
        byte cmd_query ;
		byte DR ;
		byte M  ;
		byte TRext ;
		byte sel ;
		byte session_query ;
		byte tgt ;
		byte Q ;
		unsigned short CRC16_query ;
	  }MyQuery ;
typedef struct QueryAdjustStruct{
        byte cmd_QA ;
		byte seesion_QA ;
		byte UpDn ;
		}MyQueryAdjust ;
typedef struct QueryRepStruct{
        byte cmd_QR ;
		byte seesion_QR ;
		}MyQueryRep ;
typedef struct ACKStruck{
        byte cmd_ACK ;
		unsigned short RN_ACK ;
		}MyACK ;
typedef struct NAKStruck{
         byte  cmd_NAK ;
         }MyNAK ;
//Access Set
typedef struct Req_RNStruct{
         byte cmd_Req_RN ;
         unsigned short RN_Req_RN ;
         unsigned short CRC16_Req_RN;
         }MyReq_RN;  
typedef struct ReadStruct{
        byte cmd_Read ;
        byte MenBank_Read ;
        byte WordPtr ;
        byte WordCount ;
        unsigned short RN_Read ;
        unsigned short CRC16_Read ;
        }MyRead ;
typedef struct WriteStruct{
        byte cmd_Write ;
        byte ManBank_Write ;
        byte WordPtr_Write ;
        unsigned short Data_Write ;
        unsigned short RN_Write ;
        unsigned short CRC16_Write ;
        }MyWrite;
typedef struct KillStruct{
        byte cmd_Kill ;
        unsigned short pwd_Kill ;
        byte RFU_Kill ;
        unsigned short RN_Kill ;
        unsigned short CRC16_Kill ;
        }MyKill;
typedef struct LockStruct{
        byte cmd_Lock ;
        word PayLoad ;
        unsigned short RN_Lock ;
        unsigned short CRC16_Lock ;
        }MyLock ;
typedef struct AccessStruct{
        byte cmd_Access ;
        unsigned short pwd_Access ;
        unsigned short RN_Access ;
        unsigned short CRC16_Access ;
        }MyAccess ;
typedef struct BlockWriteStruct{
        byte cmd_BlockWrite ;
        byte MenBank_BlockWrite ;
        byte WordPtr_BlockWrite ;
        unsigned short Data_BlockWrite ;
        unsigned short RN_BlockWrite ;
        unsigned short CRC16_BlockWrite ;
        }MyBlockWrite ;
typedef struct BlockEraseStruct{
         byte cmd_BlockErase ;
         byte MenBank_BlockErase ;
         byte WordPtr_BlockErase ;
         byte WordCount_BlockErase;
         unsigned short  RN_BlockErase ;
         unsigned short  CRC16_BlockErase ;
        }MyBlockErase ;   
typedef struct TagErrorCode{
        byte Header ;
        byte ErrorCode ;
        unsigned short RN_TagErrorCode ;
        unsigned short CRC16_TagErrorCode ;
}MyTagErrorCode ;

typedef struct{
	byte Header;
	unsigned short RN;
}WRITE_RESPONSE;
#endif
