/*-------------------------------------------------------------------*/
/*  InfoNES_system.c                                                 */
/*-------------------------------------------------------------------*/
#include "InfoNES.h"
#include "def.h"
#include <string.h>
#include <stdio.h>
#include "LCD_TFT.h"
#include "InfoNES_Types.h"
#include "ff.h"	
#include "2440lib.h"

unsigned char FrameCount;
unsigned char afps;
struct NesHeader_tag NesHeader;

extern unsigned short LCD_BUFFER[SCR_YSIZE_TFT][SCR_XSIZE_TFT];
extern void PutPixel(U32 x,U32 y, U32 c );

unsigned char buffer[512*1024];
int secondCount = 0;

extern int joy;
int joy2;
int mm;
FATFS  FS;


/*-------------------------------------------------------------------*/
/*  Palette data                                                     */
/*-------------------------------------------------------------------*/
WORD NesPalette[64]={
#if 0
  0x738E,0x88C4,0xA800,0x9808,0x7011,0x1015,0x0014,0x004F,
  0x0148,0x0200,0x0280,0x11C0,0x59C3,0x0000,0x0000,0x0000,
  0xBDD7,0xEB80,0xE9C4,0xF010,0xB817,0x581C,0x015B,0x0A59,
  0x0391,0x0480,0x0540,0x3C80,0x8C00,0x0000,0x0000,0x0000,
  0xFFDF,0xFDC7,0xFC8B,0xFC48,0xFBDE,0xB39F,0x639F,0x3CDF,
  0x3DDE,0x1690,0x4EC9,0x9FCB,0xDF40,0x0000,0x0000,0x0000,
  0xFFDF,0xFF15,0xFE98,0xFE5A,0xFE1F,0xDE1F,0xB5DF,0xAEDF,
  0xA71F,0xA7DC,0xBF95,0xCFD6,0xF7D3,0x0000,0x0000,0x0000,
#else
  0x738E,0x20D1,0x0015,0x4013,0x880E,0xA802,0xA000,0x7840,
  0x4140,0x0200,0x0280,0x01C2,0x19CB,0x0000,0x0000,0x0000,
  0xBDD7,0x039D,0x21DD,0x801E,0xB817,0xE00B,0xD940,0xCA41,
  0x8B80,0x0480,0x0540,0x0487,0x0411,0x0000,0x0000,0x0000,
  0xFFDF,0x3DDF,0x5C9F,0x445F,0xF3DF,0xFB96,0xFB8C,0xFCC7,
  0xF5C7,0x8682,0x4EC9,0x5FD3,0x075B,0x0000,0x0000,0x0000,
  0xFFDF,0xAF1F,0xC69F,0xD65F,0xFE1F,0xFE1B,0xFDD6,0xFED5,
  0xFF14,0xE7D4,0xAF97,0xB7D9,0x9FDE,0x0000,0x0000,0x0000,
#endif
};


extern void Uart_Printf(char *f, ...) ;
extern void Uart_SendByte(int data);


#if WORKFRAME_DEFINE == WORKFRAME_DOUBLE
extern PIXEL DoubleFrame[ 2 ][ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
extern PIXEL *WorkFrame;
extern WORD WorkFrameIdx;
#elif WORKFRAME_DEFINE == WORKFRAME_SINGLE
extern PIXEL WorkFrame[ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
#else
extern PIXEL WorkLine[ NES_DISP_WIDTH ];
#endif


extern char  menu0[256][20];
extern U8   func_ptr;

void Print(BYTE * ch,int length)
{
	int i;
	Uart_Printf("\n\n地 址   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F");
	for(i=0;i<length;i++)
	{
		if((i%16)==0) Uart_Printf("\n%4x: ",i);
		Uart_Printf(" %2X ",ch[i]);
	}
	Uart_Printf("\n\n");
}


/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/
void
SysTickIntHandler(void)
{
    //
    // Update the Systick interrupt counter.
    //
  if(secondCount++==5)
  {
    secondCount = 0;
    afps = FrameCount;
    FrameCount = 0;
//     afps++;
  }
}


/* Menu screen */
int InfoNES_Menu()
{
	return 0;
}

/* Read ROM image file */

int InfoNES_ReadRom(char *buf,int length)
{
	  
	  BYTE * rom = (BYTE*)buf;
	  //Print(buf,1024);
	  memcpy( &NesHeader, rom, sizeof(NesHeader));
	  Uart_Printf(" NesHeader.byID : %x \n",NesHeader.byID[0]);
	  Uart_Printf(" NesHeader.byID : %x \n",NesHeader.byID[1]);
	  Uart_Printf(" NesHeader.byID : %x \n",NesHeader.byID[2]);
	  Uart_Printf(" NesHeader.byID : %x \n",NesHeader.byID[3]);
	  if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
	  {
	  	
	    /* not .nes file */
	    return -1;
	  }
	  rom += sizeof(NesHeader);
	
	  /* Clear SRAM */
	  memset( SRAM, 0, SRAM_SIZE );
	
	  /* If trainer presents Read Triner at 0x7000-0x71ff */
	  if ( NesHeader.byInfo1 & 4 )
	  {
	    //memcpy( &SRAM[ 0x1000 ], rom, 512);
		rom += 512;
	  }
	  Uart_Printf(" here \n");
	  /* Allocate Memory for ROM Image */
	  ROM = rom;
	  rom += NesHeader.byRomSize * 0x4000;
	 
	  if ( NesHeader.byVRomSize > 0 )
	  {
	    /* Allocate Memory for VROM Image */
		VROM = (BYTE*)rom;
		rom += NesHeader.byVRomSize * 0x2000;
	  }


   /* Print(ROM,512);
    Print(VROM,512);   */

  /* Successful */
  return 0;
}





int LoadRomImage(const char *pszFileName)
{
	int i;
	unsigned int state =0;
	FIL  file; 

	i=f_open(&file,pszFileName,FA_READ) ;	
	Uart_Printf("\n i :%d \n!!!!!!",i);	
	if(FR_OK==i)
	{
		Uart_Printf("\nNES File Open OK!!!");
		Uart_Printf("\nNES File Length= %d",file.fsize);	 
	}
	else
	{
		Uart_Printf("\nFile Open Error!!! %d",i);	
		f_close(&file);
		return -1;
	}
	i=f_read(&file,buffer,file.fsize,&state); //  读文件数据，长度file.fsize，
//	Print(buffer,1024);
	if(FR_OK==i)
		Uart_Printf("\nFile Read OK!!!%x",state);
	else
	{
		Uart_Printf("\nFile Read Error!!!%d",i); 
		f_close(&file);
		return -1;		 
	}
	i=InfoNES_ReadRom(buffer,file.fsize);
	f_close(&file);	
	 
	return i;
}


void App_Main(void)
{
	int x;
	char  buff[20];
	
	Lcd_ClearScr(0);
	Glib_Buttom();
	memset(buff,0,20);
    strncpy(buff,menu0[func_ptr],20);
	
	
	
    for(x=0;x<20;x++)
    {
        if(buff[x] == ' ')
        {
            buff[x] = 0;
        }
    }


    InfoNES_Load(buff);   
	
	InfoNES_Main();	  
}



//
//int InfoNES_ReadRom( const char *pszFileName )
//{
///*
// *  Read ROM image file
// *
// *  Parameters
// *    const char *pszFileName          (Read)
// *
// *  Return values
// *     0 : Normally
// *    -1 : Error
// */
//
//  
//  /* Read ROM Header */
//  BYTE * rom = (BYTE*)nes_rom;
//
//  memcpy( &NesHeader, rom, sizeof(NesHeader));
//  Uart_Printf(" NesHeader.byID : %d \n",NesHeader.byID[0]);
//  Uart_Printf(" NesHeader.byID : %d \n",NesHeader.byID[1]);
//  Uart_Printf(" NesHeader.byID : %d \n",NesHeader.byID[2]);
//  Uart_Printf(" NesHeader.byID : %d \n",NesHeader.byID[3]);
//  if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
//  {
//  	
//    /* not .nes file */
//    return -1;
//  }
//  rom += sizeof(NesHeader);
//
//  /* Clear SRAM */
//  memset( SRAM, 0, SRAM_SIZE );
//
//  /* If trainer presents Read Triner at 0x7000-0x71ff */
//  if ( NesHeader.byInfo1 & 4 )
//  {
//    //memcpy( &SRAM[ 0x1000 ], rom, 512);
//	rom += 512;
//  }
//  Uart_Printf(" here \n");
//  /* Allocate Memory for ROM Image */
//  ROM = rom;
//  rom += NesHeader.byRomSize * 0x4000;
// 
//  if ( NesHeader.byVRomSize > 0 )
//  {
//    /* Allocate Memory for VROM Image */
//	VROM = (BYTE*)rom;
//	rom += NesHeader.byVRomSize * 0x2000;
//  }
//
//  /* Successful */
//  return 0;
//}
//
/* Release a memory for ROM */
void InfoNES_ReleaseRom()
{
}

/* Transfer the contents of work frame on the screen */
extern 
void InfoNES_LoadFrame()
{
	char buf[20];
	int i;
	//sprintf(buf, "%d fps",afps);
	/*for(i=0;i<5;i++)
	PutPixel(300,150,buf[i]);	   */
	//Uart_Printf(" %d fps",afps);

}

/* Transfer the contents of work line on the screen */
void InfoNES_LoadLine()
{
  int i;
  if(PPU_Scanline == 20)
  {
    FrameCount++;
	  InfoNES_LoadFrame();
  }
 // Uart_Printf(" PPU_Scanline : %d \n",PPU_Scanline);
  for(i=0;i<256;i++)
  {
    PutPixel(i,PPU_Scanline+32,WorkLine[i]);
/*	if(WorkLine[i] != 0)
    Uart_Printf(" %d ",WorkLine[i]);	  */ 
  //  LCD_BUFFER[272-i][PPU_Scanline] = WorkLine[i];
  }
}

/* Get a joypad state */
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
	
	*pdwPad1 = 0;

//	*pdwPad1 |= joy;  	
	*pdwPad2 = 0; 
	mm++;	
	if(mm%40)
	{		
	*pdwPad1=joy;
	*pdwPad2=joy2;
	}
	else
	{
	*pdwPad1=joy&0xfc;
	*pdwPad2=joy2&0xfc;	
	mm=0;
	}		
//	Uart_Printf( "joy%d is pressed!\n", *pdwPad1) ;

	//*pdwPad2 = 0;
	*pdwSystem = 0;		
}

/* memcpy */
void *InfoNES_MemoryCopy( void *dest, const void *src, int count ){return memcpy(dest,src,count);}

/* memset */
void *InfoNES_MemorySet( void *dest, int c, int count ){return memset(dest,c,count);}

/* Print debug message */
void InfoNES_DebugPrint( char *pszMsg )
{
}

/* Wait */
void InfoNES_Wait()
{
}

/* Sound Initialize */
void InfoNES_SoundInit( void )
{
}

/* Sound Open */
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate )
{
  return 0;
}

/* Sound Close */
void InfoNES_SoundClose( void )
{
}

/* Sound Output 5 Waves - 2 Pulse, 1 Triangle, 1 Noise, 1 DPCM */
void InfoNES_SoundOutput(int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5)
{
}

/* Print system message */
void InfoNES_MessageBox( char *pszMsg, ... )
{
}

