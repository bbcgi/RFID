/***********************************************************************/
/*  This file is part of the C51 Compiler package                      */
/*  Copyright KEIL ELEKTRONIK GmbH 1990 - 2002                         */
/***********************************************************************/
/*                                                                     */
/*  PUTCHAR.C:  This routine is the general character output of C51.   */
/*  You may add this file to a uVision2 project.                       */
/*                                                                     */
/*  To translate this file use C51 with the following invocation:      */
/*     C51 PUTCHAR.C <memory model>                                    */
/*                                                                     */
/*  To link the modified PUTCHAR.OBJ file to your application use the  */
/*  following Lx51 invocation:                                         */
/*     Lx51 <your object file list>, PUTCHAR.OBJ <controls>            */
/*                                                                     */
/***********************************************************************/

#include <Chipcon/reg1010.h>

#define XON  0x11
#define XOFF 0x13
#define MINI;


/*
 * putchar (full version):  expands '\n' into CR LF and handles
 *                          XON/XOFF (Ctrl+S/Ctrl+Q) protocol
 */
#ifdef FULL

#ifdef UART0
	char putchar (char c)  {
	
	  if (c == '\n')  {
	    if (RI_0)  {
	      if (SBUF0 == XOFF)  {
	        do  {
	          RI_0 = 0;
	          while (!RI_0);
	        }
	        while (SBUF0 != XON);
	        RI_0 = 0; 
	      }
	    }
	    while (!TI_0);
	    TI_0 = 0;
	    SBUF0 = 0x0d;                         /* output CR  */
	  }
	  if (RI_0)  {
	    if (SBUF0 == XOFF)  {
	      do  {
	        RI_ = 0;
	        while (!RI_0);
	      }
	      while (SBUF0 != XON);
	      RI_0 = 0; 
	    }
	  }
	  while (!TI_0);
	  TI_0 = 0;
	  return (SBUF0 = c);
	}
#else
	char putchar (char c)  {
	  if (c == '\n')  {
	    if (RI_1)  {
	      if (SBUF1 == XOFF)  {
	        do  {
	          RI_1 = 0;
	          while (!RI_1);
	        }
	        while (SBUF1 != XON);
	        RI_1 = 0; 
	      }
	    }
	    while (!TI_1);
	    TI_1 = 0;
	    SBUF1 = 0x0d;                         /* output CR  */
	  }
	  if (RI_1)  {
	    if (SBUF1 == XOFF)  {
	      do  {
	        RI_1 = 0;
	        while (!RI_1);
	      }
	      while (SBUF1 != XON);
	      RI_1 = 0; 
	    }
	  }
	  while (!TI_1);
	  TI_1 = 0;
	  return (SBUF1 = c);
	}
#endif

#endif

/*
 * putchar (basic version): expands '\n' into CR LF
 */
#ifdef BASIC

#ifdef UART0
	char putchar (char c)  {
	  if (c == '\n')  {
	    while (!TI_0);
	    TI_0 = 0;
	    SBUF0 = 0x0d;                         /* output CR  */
	  }
	  while (!TI_0);
	  TI_0 = 0;
	  return (SBUF0 = c);
	}
#else
	char putchar (char c)  {
	  if (c == '\n')  {
	    while (!TI_1);
	    TI_1 = 0;
	    SBUF1 = 0x0d;                         /* output CR  */
	  }
	  while (!TI_1);
	  TI_1 = 0;
	  return (SBUF1 = c);
	}
#endif

#endif

/*
 * putchar (mini version): outputs charcter only
 */
#ifdef MINI

#ifdef UART0
	char putchar (char c)  {
	  while (!TI_0);
	  TI_0 = 0;
	  return (SBUF0 = c);
	}
#else
	char putchar (char c)  {
	  while (!TI_1);
	  TI_1 = 0;
	  return (SBUF1 = c);
	}
#endif

#endif
