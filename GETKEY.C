/***********************************************************************/
/*  This file is part of the C51 Compiler package                      */
/*  Copyright KEIL ELEKTRONIK GmbH 1993 - 2002                         */
/***********************************************************************/
/*                                                                     */
/*  GETKEY.C:  This routine is the general character input of C51.     */
/*  You may add this file to a uVision2 project.                       */
/*                                                                     */
/*  To translate this file use C51 with the following invocation:      */
/*     C51 GETKEY.C  <memory model>                                    */
/*                                                                     */
/*  To link the modified GETKEY.OBJ file to your application use the   */
/*  following Lx51 invocation:                                         */
/*     Lx51 <your object file list>, GETKEY.OBJ <controls>             */
/*                                                                     */
/***********************************************************************/

#include <Chipcon/reg1010.h>

#ifdef UART0
	char _getkey ()  {
	  char c;
	
	  while (!RI_0);
	  c = SBUF0;
	  RI_0 = 0;
	  return (c);
	}
#else
	char _getkey ()  {
	  char c;
	
	  while (!RI_1);
	  c = SBUF1;
	  RI_1 = 0;
	  return (c);
	}
#endif
