#include <chipcon/hal.h>
#include <chipcon/cul.h>
#include <chipcon/reg1010.h>
#include <chipcon/cc1010eb.h>
#include <stdio.h>
#include <string.h>
#include "EPC_COMMAND.h"

#ifndef __EPC_UTIL_COMMAND_SET__
#define __EPC_UTIL_COMMAND_SET__


extern RF_RXTXPAIR_CALDATA xdata RF_CALDATA;

#define PREAMBLE_BYTE_COUNT 25
extern xdata byte buffer[100];
extern MySelect select;
extern MyQuery query;
extern MyQueryRep queryRep;

void init_command();
void deSelect();
void select_by_mask(unsigned char bank,unsigned short*mask,unsigned char maskLen,unsigned char Q);
void kill_tag(unsigned short* epc_data_4_word,unsigned short* pwd);
unsigned short select_single_tag(unsigned short* epc_data_4_word,unsigned char Q);
unsigned char get_data_by_epc(unsigned short* epc_data_4_word,unsigned char bank,unsigned char* output,unsigned char len);
unsigned char put_data_by_epc(unsigned short* epc_data_4_word,unsigned char bank,unsigned char* input,unsigned char offset,unsigned char len);

void send_rs232_sync_bytes();
void send_rs232_finish_bytes();
void send_data_frame(unsigned char* data_pointer,unsigned char len);

#endif
