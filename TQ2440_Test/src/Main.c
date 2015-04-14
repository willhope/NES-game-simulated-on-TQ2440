/****************************************************************
 NAME: u2440mon.c
 DESC: u2440mon entry point,menu,download
 ****************************************************************/
#define	GLOBAL_CLK		1

#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h"
#include "mmu.h"
#include "profile.h"
#include "memtest.h"
#include "InfoNES.h"
#include "SD_MMC.h"
#include "InfoNES_System.h"
#include "ff.h"	
#include "DisplayBmp.h"




extern char Image$$ER_ROM1$$RO$$Limit[]; 
extern char Image$$ER_ROM1$$RO$$Base[]; 
extern char Image$$RW_RAM1$$RW$$Limit[]; 
extern char Image$$RW_RAM1$$RW$$Base[]; 
extern char Image$$RW_RAM1$$ZI$$Limit[]; 
extern char Image$$RW_RAM1$$ZI$$Base[]; 

void Isr_Init(void);
void HaltUndef(void);
void HaltSwi(void);
void HaltPabort(void);
void HaltDabort(void);
void ClearMemory(void);


void Clk0_Enable(int clock_sel);	
void Clk1_Enable(int clock_sel);
void Clk0_Disable(void);
void Clk1_Disable(void);

extern void Lcd_TFT_Init(void);




extern void PlayMusicTest(void) ;
extern void RecordTest( void ) ;

extern void Open_Touchpanel(void);



FRESULT scan_files (char* path);


volatile U32 downloadAddress;

void (*restart)(void)=(void (*)(void))0x0;

volatile unsigned char *downPt;
volatile U32 downloadFileSize;
volatile U16 checkSum;
volatile unsigned int err=0;
volatile U32 totalDmaCount;

volatile int isUsbdSetConfiguration;

extern U8   func_ptr,func1_ptr;   // 功能指针,子功能指针
extern U8   disp_ptr;             // 第一显示项指针
extern U8   sel_ptr;              // 当前选择项指针
extern U16  para_val,edit_val;
extern U8   Menu_times_count;     // 菜单操作计数器
U16  column;               // 文件个数
extern char  menu0[256][20];

extern volatile int xdata, ydata;


char path[512]="0:";  

int download_run=0;
U32 tempDownloadAddress;
int menuUsed=0;

extern char Image$$RW_RAM1$$RW$$Limit[]; 
U32 *pMagicNum=(U32 *)Image$$RW_RAM1$$RW$$Limit; 
int consoleNum;
int keynum;
extern int joy;

extern FATFS  FS;


static U32 cpu_freq;
static U32 UPLL;





void delay_ms(volatile int time)
{
	volatile U32 val;
	val=(PCLK>>3)/1000-1;
	rTCNTB4=val;
	rTCFG0=rTCFG0&~(0XFF<<8)|(0X3<<8);
	rTCFG1=rTCFG1&~(0XF<<16)|(0X0<<16);
	rTCON=rTCON&~(0X7<<20)|(0X6<<20);
	rTCON=rTCON&~(0x3<<20)|(0X1<<20);
	while(time--)
	{
		while(rTCNTO4>=val>>1);
		while(rTCNTO4<val>>1);
				      
	}
}


static void cal_cpu_bus_clk(void)
{
	U32 val;
	U8 m, p, s;
	
	val = rMPLLCON;
	m = (val>>12)&0xff;
	p = (val>>4)&0x3f;
	s = val&3;

	//(m+8)*FIN*2 不要超出32位数!
	FCLK = ((m+8)*(FIN/100)*2)/((p+2)*(1<<s))*100;
	
	val = rCLKDIVN;
	m = (val>>1)&3;
	p = val&1;	
	val = rCAMDIVN;
	s = val>>8;
	
	switch (m) {
	case 0:
		HCLK = FCLK;
		break;
	case 1:
		HCLK = FCLK>>1;
		break;
	case 2:
		if(s&2)
			HCLK = FCLK>>3;
		else
			HCLK = FCLK>>2;
		break;
	case 3:
		if(s&1)
			HCLK = FCLK/6;
		else
			HCLK = FCLK/3;
		break;
	}
	
	if(p)
		PCLK = HCLK>>1;
	else
		PCLK = HCLK;
	
	if(s&0x10)
		cpu_freq = HCLK;
	else
		cpu_freq = FCLK;
		
	val = rUPLLCON;
	m = (val>>12)&0xff;
	p = (val>>4)&0x3f;
	s = val&3;
	UPLL = ((m+8)*FIN)/((p+2)*(1<<s));
	UCLK = (rCLKDIVN&8)?(UPLL>>1):UPLL;
}









void Main(void)
{
	char *mode;
	int i;
	U8 key;
	U32 mpll_val = 0 ;

	FIL fsrc;            // file objects
	FRESULT res;         // FatFs function common result code

	#if ADS10   
//	__rt_lib_init(); //for ADS 1.0
	#endif

	Port_Init();
	
	Isr_Init();
	
	i = 2 ;	//don't use 100M!
	switch ( i ) {
	case 0:	//200
		key = 12;
		mpll_val = (92<<12)|(4<<4)|(1);
		break;
	case 1:	//300
		key = 13;
		mpll_val = (67<<12)|(1<<4)|(1);
		break;
	case 2:	//400
		key = 14;
		mpll_val = (92<<12)|(1<<4)|(1);
		break;
	case 3:	//440!!!
		key = 14;
		mpll_val = (102<<12)|(1<<4)|(1);
		break;
	default:
		key = 14;
		mpll_val = (92<<12)|(1<<4)|(1);
		break;
	}
	
	//init FCLK=400M, so change MPLL first
	ChangeMPllValue((mpll_val>>12)&0xff, (mpll_val>>4)&0x3f, mpll_val&3);
	ChangeClockDivider(key, 12);
	cal_cpu_bus_clk();
	
	consoleNum = 0;	// Uart 1 select for debug.
	Uart_Init( 0,115200 );
	Uart_Select( consoleNum );
	
//	Beep(2000, 100);
	
	Open_Touchpanel();
	Uart0_irq_init();
	Uart1_irq_init();


	rMISCCR=rMISCCR&~(1<<3); // USBD is selected instead of USBH1 
	rMISCCR=rMISCCR&~(1<<13); // USB port 1 is enabled.


	rDSC0 = 0x2aa;
	rDSC1 = 0x2aaaaaaa;
	//Enable NAND, USBD, PWM TImer, UART0,1 and GPIO clock,
	//the others must be enabled in OS!!!
	rCLKCON = 0xfffff0;

	MMU_Init();	//

	pISR_SWI=(_ISR_STARTADDRESS+0xf0);	//for pSOS

	Led_Display(0x66);

	mode="DMA";

	Clk0_Disable();
	Clk1_Disable();
	
	mpll_val = rMPLLCON;

	Lcd_TFT_Init() ;		// LCD initial

	//KeyScan_Test();
	 
	if(SDinit())
	{
		
		Uart_Printf("卡初始化完毕！\n");
		
	}
	else
	{
		
		Uart_Printf("卡初始化失败！\n");
		
		return;
	}
	download_run=1; //The default menu is the Download & Run mode.


	res = f_mount(0,&FS);	//  初始化分区结构，它不初始化SD卡
	if(res == FR_OK)
    {
		Uart_Printf("\n\nATA mounrt OK, fs_type =  %d",FS.fs_type);
	}
    else
    {
		Uart_Printf("\nATA mounrt Error!!!\n%d",res);	 
	}

	scan_files(path);
	
	
	Glib_logo();
	delay_ms(3000);
	Lcd_ClearScr(0);
	Glib_Buttom2();
//	Glib_gezi();
//	Open_Touchpanel();
	
	while(1) 
	{
		keynum =  ReadMenuKey (&xdata,&ydata);
		process_key(keynum);
		display_menu();	  	
	}		


}

/*******************************************************************************
* Function Name  : scan_files
* Description    : 搜索文件目录下所有文件
* Input          : - path: 根目录
* Output         : None
* Return         : FRESULT
* Attention		 : 不支持长文件名
*******************************************************************************/
FRESULT scan_files (char* path)
{
    FILINFO fno;
    DIR dir;
    int i,len;
    char *fn;
	int res;

#if _USE_LFN
    static char lfn[_MAX_LFN * (0 ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) 
			{
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } 
			else 
			{
               // Uart_Printf("%s/%s \r\n", path, fn);
				if((strstr(fn,".Nes") != NULL))
				{
					len = strstr(fn,".Nes") - fn + 4;

					memset(menu0[column],' ',20);
					strncpy(menu0[column++],fn,len);
				}
				else if((strstr(fn,".nes") != NULL))
				{
					len = strstr(fn,".nes") - fn + 4;

					memset(menu0[column],' ',20);
					strncpy(menu0[column++],fn,len);
				}
				else if((strstr(fn,".NES") != NULL))
				{
					len = strstr(fn,".NES") - fn + 4;

					memset(menu0[column],' ',20);
					strncpy(menu0[column++],fn,len);
				}
			}
        }
    }

    return res;
}





void Isr_Init(void)
{
	pISR_UNDEF=(unsigned)HaltUndef;
	pISR_SWI  =(unsigned)HaltSwi;
	pISR_PABORT=(unsigned)HaltPabort;
	pISR_DABORT=(unsigned)HaltDabort;
	rINTMOD=0x0;	  // All=IRQ mode
	rINTMSK=BIT_ALLMSK;	  // All interrupt is masked.
}


void HaltUndef(void)
{
	Uart_Printf("Undefined instruction exception!!!\n");
	while(1);
}

void HaltSwi(void)
{
	Uart_Printf("SWI exception!!!\n");
	while(1);
}

void HaltPabort(void)
{
	Uart_Printf("Pabort exception!!!\n");
	while(1);
}

void HaltDabort(void)
{
	Uart_Printf("Dabort exception!!!\n");
	while(1);
}


void ClearMemory(void)
{
	int memError=0;
	U32 *pt;
	
	Uart_Printf("Clear Memory (%xh-%xh):WR",_RAM_STARTADDRESS,HEAPEND);

	pt=(U32 *)_RAM_STARTADDRESS;
	while((U32)pt < HEAPEND)
	{
		*pt=(U32)0x0;
		pt++;
	}
	
	if(memError==0)Uart_Printf("\b\bO.K.\n");
}

void Clk0_Enable(int clock_sel)	
{	// 0:MPLLin, 1:UPLL, 2:FCLK, 3:HCLK, 4:PCLK, 5:DCLK0
	rMISCCR = rMISCCR&~(7<<4) | (clock_sel<<4);
	rGPHCON = rGPHCON&~(3<<18) | (2<<18);
}
void Clk1_Enable(int clock_sel)
{	// 0:MPLLout, 1:UPLL, 2:RTC, 3:HCLK, 4:PCLK, 5:DCLK1	
	rMISCCR = rMISCCR&~(7<<8) | (clock_sel<<8);
	rGPHCON = rGPHCON&~(3<<20) | (2<<20);
}
void Clk0_Disable(void)
{
	rGPHCON = rGPHCON&~(3<<18);	// GPH9 Input
}
void Clk1_Disable(void)
{
	rGPHCON = rGPHCON&~(3<<20);	// GPH10 Input
}

