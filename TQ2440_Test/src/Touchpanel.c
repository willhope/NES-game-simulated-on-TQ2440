/*****************************************
  NAME: Touchpanel.c
  DESC: ADC & Touch screen test
 *****************************************/
#include "def.h"
#include "2440addr.h"
#include "2440lib.h"
#include "InfoNES.h"

#define REQCNT 30
#define ADCPRS 9
#define LOOP 1

void __irq AdcTsAuto(void);

int count=0;
int joy;
volatile int xdata, ydata;
    
void Open_Touchpanel(void)
{

   
	rADCDLY=50000;                  //Normal conversion mode delay about (1/3.6864M)*50000=13.56ms
	rADCCON=(1<<14)+(ADCPRS<<6);   //ADCPRS En, ADCPRS Value

	Uart_Printf("\nTouch Screen test\n");

	rADCTSC=0xd3;  //Wfait,XP_PU,XP_Dis,XM_Dis,YP_Dis,YM_En

	pISR_ADC = (int)AdcTsAuto;
	rINTMSK=~BIT_ADC;       //ADC Touch Screen Mask bit clear
	rINTSUBMSK=~(BIT_SUB_TC);			
}


void close_Touchpanel(void)
{
	rINTSUBMSK|=BIT_SUB_TC;
	rINTMSK|=BIT_ADC;
}


void __irq AdcTsAuto(void)
{
	U32 saveAdcdly;
	int temp;
	if(rADCDAT0&0x8000)
	{
		//Uart_Printf("\nStylus Up!!\n");
		rADCTSC&=0xff;	// Set stylus down interrupt bit
	}
	//else 
		//Uart_Printf("\nStylus Down!!\n");

	rADCTSC=(1<<3)|(1<<2);         //Pull-up disable, Seq. X,Y postion measure.
	saveAdcdly=rADCDLY;
	rADCDLY=40000;                 //Normal conversion mode delay about (1/50M)*40000=0.8ms

	rADCCON|=0x1;                   //start ADC

	while(rADCCON & 0x1);		//check if Enable_start is low
	while(!(rADCCON & 0x8000));        //check if EC(End of Conversion) flag is high, This line is necessary~!!
		
	while(!(rSRCPND & (BIT_ADC)));  //check if ADC is finished with interrupt bit

	xdata=(rADCDAT0&0x3ff);
 	ydata=(rADCDAT1&0x3ff);

	xdata *= 0.272;
	ydata *= 0.480;
	ydata = 480-ydata;
	temp = ydata;
	ydata = xdata;
	xdata = temp;

	xdata = (xdata - 20.0303) / 0.8972;
	ydata = (ydata - 37.86667) / 0.7486;

	//check Stylus Up Interrupt.
	rSUBSRCPND|=BIT_SUB_TC;
	ClearPending(BIT_ADC);
	rINTSUBMSK &= ~(BIT_SUB_TC);
	rINTMSK &= ~(BIT_ADC);
			 
	rADCTSC =0xd3;    //Waiting for interrupt
	rADCTSC=rADCTSC|(1<<8); // Detect stylus up interrupt signal.

	while(1)		//to check Pen-up state
	{
		if(rSUBSRCPND & (BIT_SUB_TC))	//check if ADC is finished with interrupt bit
		{
			//Uart_Printf("Stylus Up Interrupt~!\n");
			break;	//if Stylus is up(1) state
		}
	}	

	//Uart_Printf("count=%03d  XP=%04d, YP=%04d\n", count++, xdata, ydata);    //X-position Conversion data            
 
	if(xdata>355 && xdata<395 && ydata>0 && ydata<30)  
		{
			
			joy = PAD_JOY_UP;  //up
		//	Uart_Printf("up\n");
		}
	else if(xdata>300 && xdata<335 && ydata>40 && ydata<100)  
		{
			
			joy = PAD_JOY_LEFT;  //left
		//	Uart_Printf("left\n");
		}
	else if(xdata>420 && xdata<450 && ydata>40 && ydata<100)  
		{
			
			joy = PAD_JOY_RIGHT;  //right
		//	Uart_Printf("right\n");
		}
	else if(xdata>355 && xdata<395 && ydata>110 && ydata<140)  
		{
			
			joy = PAD_JOY_DOWN;   //down 
		//	Uart_Printf("down\n");
		}  
	else if(xdata>430 && xdata<460 && ydata>90 && ydata<120)  
		{
		
		//	over=!over;   //quit 
		//	Uart_Printf("quit\n");
		}  
	else if(xdata>360 && xdata<400 && ydata>50 && ydata<90)  
		{
			
			joy = PAD_JOY_START;   //pause or start
		//	Uart_Printf("pause or start\n");
		} 

	 else if(xdata>300 && xdata<365 && ydata>140 && ydata<200)  
		{
			
			joy = PAD_JOY_SELECT;   //slect
		//	Uart_Printf("slect\n");
		}
	  
	 else if(xdata>395 && xdata<460 && ydata>140 && ydata<200)  
		{
			
			joy = PAD_JOY_A;   //A
		//	Uart_Printf("A\n");
		} 


	else if(xdata>300 && xdata<365 && ydata>210&& ydata<272)  
		{
			
		//	joy = 0x04;   //quit
		//	Uart_Printf("quit\n");
		}

	 else if(xdata>395 && xdata<460 && ydata>210 && ydata<272)  
		{
			
			joy = PAD_JOY_B;   //B
		//	Uart_Printf("B\n");
		}


	rADCDLY=saveAdcdly; 
	rADCTSC=rADCTSC&~(1<<8); // Detect stylus Down interrupt signal.
	rSUBSRCPND|=BIT_SUB_TC;
	rINTSUBMSK &=~(BIT_SUB_TC);	// Unmask sub interrupt (TC)     
	ClearPending(BIT_ADC);
}
