/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER
#define _INTEGER

#if 0	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */



/* These types must be 8-bit integer */

typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types must be 16-bit integer */

typedef unsigned short	USHORT;
typedef unsigned short	WORD;


/* These types must be 32-bit integer */

typedef unsigned long	ULONG;


#endif

#endif
