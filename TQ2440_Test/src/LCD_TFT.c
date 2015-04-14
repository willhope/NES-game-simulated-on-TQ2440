/**************************************************************
The initial and control for 16Bpp TFT LCD
**************************************************************/

#include "def.h"
#include "option.h"
#include "2440addr.h"
#include "2440lib.h"
#include "2440slib.h" 
#include "LCD_TFT.h"


#define M5D(n)				((n) & 0x1fffff)	// To get lower 21bits

extern void Uart_Printf(char *f, ...) ;

extern unsigned char __CHS[];
extern unsigned char __VGA[];

volatile unsigned short LCD_BUFFER[SCR_YSIZE_TFT][SCR_XSIZE_TFT];

U8 cursor_x,cursor_y,cursor_mode = 1;

/**************************************************************
TFT LCD功能模块初始化
**************************************************************/
void Lcd_PowerEnable(int invpwren,int pwren);
void Lcd_Init(void)
{
	rGPCUP  = 0x00000000;
	rGPCCON = 0xaaaa02a9; 
	 
	rGPDUP  = 0x00000000;
	rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]

	rLCDCON1=(CLKVAL_TFT<<8)|(MVAL_USED<<7)|(3<<5)|(12<<1)|0;
    	// TFT LCD panel,12bpp TFT,ENVID=off
	rLCDCON2=(VBPD<<24)|(LINEVAL_TFT<<14)|(VFPD<<6)|(VSPW);
	rLCDCON3=(HBPD<<19)|(HOZVAL_TFT<<8)|(HFPD);
	rLCDCON4=(MVAL<<8)|(HSPW);
	rLCDCON5 = (1<<11) | (0<<10) | (1<<9) | (1<<8) | (0<<7) | (0<<6) | (1<<3)  |(BSWP<<1) | (HWSWP);

	rLCDSADDR1=(((U32)LCD_BUFFER>>22)<<21)|M5D((U32)LCD_BUFFER>>1);
	rLCDSADDR2=M5D( ((U32)LCD_BUFFER+(SCR_XSIZE_TFT*LCD_YSIZE_TFT*2))>>1 );
	rLCDSADDR3=(((SCR_XSIZE_TFT-LCD_XSIZE_TFT)/1)<<11)|(LCD_XSIZE_TFT/1);
	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	rTCONSEL &= (~7) ;     // Disable LPC3480
	rTPAL=0; // Disable Temp Palette
}

/**************************************************************
LCD视频和控制信号输出或者停止，1开启视频输出
**************************************************************/
void Lcd_EnvidOnOff(int onoff)
{
	if(onoff==1)
		rLCDCON1|=1; // ENVID=ON
	else
		rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}

/**************************************************************
TFT LCD 电源控制引脚使能
**************************************************************/
void Lcd_PowerEnable(int invpwren,int pwren)
{
	//GPG4 is setted as LCD_PWREN
	rGPGUP=rGPGUP&(~(1<<4))|(1<<4); // Pull-up disable
	rGPGCON=rGPGCON&(~(3<<8))|(3<<8); //GPG4=LCD_PWREN
	rGPGDAT = rGPGDAT | (1<<4) ;
	//invpwren=pwren;
	//Enable LCD POWER ENABLE Function
	rLCDCON5=rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
	rLCDCON5=rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}

/**************************************************************
TFT LCD移动观察窗口
**************************************************************/
void Lcd_MoveViewPort(int vx,int vy)
{
	U32 addr;

	SET_IF(); 
	#if (LCD_XSIZE_TFT<32)
		while((rLCDCON1>>18)<=1); // if x<32
	#else	
		while((rLCDCON1>>18)==0); // if x>32
	#endif
    
	addr=(U32)LCD_BUFFER+(vx*2)+vy*(SCR_XSIZE_TFT*2);
	rLCDSADDR1= ( (addr>>22)<<21 ) | M5D(addr>>1);
	rLCDSADDR2= M5D(((addr+(SCR_XSIZE_TFT*LCD_YSIZE_TFT*2))>>1));
	CLR_IF();
}    

/**************************************************************
TFT LCD移动观察窗口
**************************************************************/
void MoveViewPort(void)
{
    int vx=0,vy=0,vd=1;

    Uart_Printf("\n*Move the LCD view windos:\n");
    Uart_Printf(" press 8 is up\n");
    Uart_Printf(" press 2 is down\n");
    Uart_Printf(" press 4 is left\n");
    Uart_Printf(" press 6 is right\n");
    Uart_Printf(" press Enter to exit!\n");

    while(1)
    {
    	switch( Uart_GetKey() )
    	{
    	case '8':
	    if(vy>=vd)vy-=vd;    	   	
        break;

    	case '4':
    	    if(vx>=vd)vx-=vd;
    	break;

    	case '6':
                if(vx<=(SCR_XSIZE_TFT-LCD_XSIZE_TFT-vd))vx+=vd;   	    
   	    break;

    	case '2':
                if(vy<=(SCR_YSIZE_TFT-LCD_YSIZE_TFT-vd))vy+=vd;   	    
   	    break;

    	case '\r':
   	    return;

    	default:
	    break;
		}
	Uart_Printf("vx=%3d,vy=%3d\n",vx,vy);
	Lcd_MoveViewPort(vx,vy);
    }
}


//三角形向上         
void triangle_up(int x,int y,int n,int color)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0-i;j<i+1;j++)
			LCD_BUFFER[y+i][x+j] = color;
	}
}

void triangle_up2(int x,int y,int n,int color)
{
	int i,j;
	for(i=0;i<2*n;i++)
	{
		for(j=0-i;j<i+1;j++)
			LCD_BUFFER[y+i][x+j] = color;
	}
}


//三角形向下          
void triangle_down(int x,int y,int n,int color)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0-i;j<i+1;j++)
			LCD_BUFFER[y-i][x+j] = color;
	}
}

void triangle_down2(int x,int y,int n,int color)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0-i;j<i+1;j++)
			LCD_BUFFER[y-i][x+j] = color;
	}
}


//三角形向左           
void triangle_left(int x,int y,int n,int color)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0-i;j<i+1;j++)
			LCD_BUFFER[y+j][x+i] = color;
	}
}
 
//三角形向右           
void triangle_right(int x,int y,int n,int color)
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0-i;j<i+1;j++)
			LCD_BUFFER[y+j][x-i] = color;
	}
}


//画圆环           
void Glib_circle(int x,int y,int r,int r0,int color)
{
	int i,j,r2;
	for(i=-r;i<=r;i++)
		for(j=-r;j<=r;j++)
		{
			r2=i*i+j*j; 
			if(r2<r*r && r2>=r0*r0)
			{
				LCD_BUFFER[y+j][x+i] = color;
			}	
		}
}


/**************************************************************
TFT LCD单个象素的显示数据输出
**************************************************************/
void PutPixel(U32 x,U32 y, U32 c )
{
	if ( (x < SCR_XSIZE_TFT) && (y < SCR_YSIZE_TFT) )
	{
		
		LCD_BUFFER[(y)][(x)] = c;
	}
}

/**************************************************************
TFT LCD全屏填充特定颜色单元或清屏
**************************************************************/
void Lcd_ClearScr( U32 c)
{
	unsigned int x,y ;
		
    for( y = 0 ; y < SCR_YSIZE_TFT ; y++ )
    {
    	for( x = 0 ; x < SCR_XSIZE_TFT ; x++ )
    	{
			
			LCD_BUFFER[y][x] = c ;
    	}
    }
}

void Lcd_Clear( U32 x0, U32 y0,U32 x1, U32 y1, U32 c)
{

	unsigned int x,y ;
		
    for( y = y0 ; y < y1 ; y++ )
    {
    	for( x = x0 ; x < x1 ; x++ )
    	{
			
			LCD_BUFFER[y][x] = c ;
    	}
    }
    	
    
}

/**************************************************************
LCD屏幕显示垂直翻转
// LCD display is flipped vertically
// But, think the algorithm by mathematics point.
//   3I2
//   4 I 1
//  --+--   <-8 octants  mathematical cordinate
//   5 I 8
//   6I7
**************************************************************/
void Glib_Line(int x1,int y1,int x2,int y2,int color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}

/**************************************************************
在LCD屏幕上画一个矩形
**************************************************************/
void Glib_Rectangle(int x1,int y1,int x2,int y2,int color)
{
    Glib_Line(x1,y1,x2,y1,color);
    Glib_Line(x2,y1,x2,y2,color);
    Glib_Line(x1,y2,x2,y2,color);
    Glib_Line(x1,y1,x1,y2,color);
}

/**************************************************************
在LCD屏幕上用颜色填充一个矩形
**************************************************************/
void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color)
{
    int i;

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}

/**************************************************************
在LCD屏幕上指定坐标点画一个指定大小的图片
**************************************************************/
void Paint_Bmp(int x0,int y0,int h,int l,unsigned char bmp[])
{
	int x,y;
	U32 c;
	int p = 0;
	
    for( y = 0 ; y < l ; y++ )
    {
    	for( x = 0 ; x < h ; x++ )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;

			if ( ( (x0+x) < SCR_XSIZE_TFT) && ( (y0+y) < SCR_YSIZE_TFT) )
				LCD_BUFFER[y0+y][x0+x] = c ;
			
    		p = p + 2 ;
    	}
    }
}

//暂停按钮              
void Glib_pulse(void)
{
	Glib_circle(380,70,25,22,0x1f);
	Glib_FilledRectangle(368,57,376,83,0x1f);
	Glib_FilledRectangle(383,57,391,83,0x1f);
}

//退出按钮              
void Glib_quit(void)
{
	Glib_circle(445,115,15,12,0x1f);
	Glib_FilledRectangle(438,108,452,122,0x1f);

}

void Glib_start(void)
{
	Glib_circle(380,70,25,22,0x1f);
	triangle_right(395,70,22,0x1f);

}	

	
//画按钮                          
void Glib_Buttom(void)
{	
	//Glib_pulse();   //pause
	Glib_start();   //quit
	triangle_up(380,0,29,0x1f);
	triangle_left(305,70,29,0x1f);
	triangle_right(450,70,29,0x1f);
	triangle_down(380,140,29,0x1f);

/*	Glib_circle(335,170,32,29,Red);
	Glib_circle(335,240,32,29,Red);
	Glib_circle(425,170,32,29,Red);
	Glib_circle(425,240,32,29,Red);  */

	Paint_Bmp(303,138,65,65,S65);
	Paint_Bmp(303,208,65,65,q65);
	Paint_Bmp(393,138,65,65,A65);
	Paint_Bmp(393,208,65,65,B65);

	
}

void Glib_Buttom2(void)
{	
	Paint_Bmp(180,40,65,220,slect2);
	
}

void Glib_logo(void)
{	
	Paint_Bmp(0,0,480,272,logo3);
	
}

void Glib_gezi(void)
{ 
	int i, ii;
	for(ii=0;ii<24;ii++)
	{
		Glib_Line(0,20*ii,480,20*ii,0xffff);
		Glib_Line(20*ii,0,20*ii,272,0xffff);
		if(ii==5||ii==10||ii==15)
		{
			Glib_Line(0,20*ii,480,20*ii, (0x00<<11) | (0x3f<<5) | (0x00));
			Glib_Line(20*ii,0,20*ii,272, (0x00<<11) | (0x3f<<5) | (0x00));
		}
	}
}

// 设置光标
void set_cursor(U8 x,U8 y)
{
    cursor_x = x;
    cursor_y = y;
}

/**************************************************************
在LCD屏幕上指定坐标点写ASCII码
**************************************************************/
void Lcd_PutASCII(unsigned int x,unsigned int y,unsigned char ch,unsigned int c,unsigned int bk_c)
{
       unsigned short int i,j;
       unsigned char *pZK,mask,buf;
	
	

       pZK = &__VGA[ch*16];
       for( i = 0 ; i < 16 ; i++ )
       {
              mask = 0x80;
              buf = pZK[i];
              for( j = 0 ; j < 8 ; j++ )
              {
                     if( buf & mask )
                     {
                            PutPixel(x+j,y+i,c);
                     }
                     else
                     {

                     }
            
                     mask = mask >> 1;
              }
       }
}

/**************************************************************
在LCD屏幕上指定坐标点写汉字
**************************************************************/
void Lcd_PutHZ(unsigned int x,unsigned int y,unsigned short int QW,unsigned int c,unsigned int bk_c)
{
       unsigned short int i,j;
       unsigned char *pZK,mask,buf;

		
	   
       pZK = &__CHS[ (  ( (QW >> 8) - 1 )*94 + (QW & 0x00FF)- 1 )*32 ];
       for( i = 0 ; i < 16 ; i++ )
       {
              //左
              mask = 0x80;
              buf = pZK[i*2];
              for( j = 0 ; j < 8 ; j++ )
              {
                     if( buf & mask )
                     {
                            PutPixel(x+j,y+i,c);
                     }
                     else
                     {
                       
                     }
                     mask = mask >> 1;
              } 
        
              //右
              mask = 0x80;
              buf = pZK[i*2 + 1];
              for( j = 0 ; j < 8 ; j++ )
              {
                     if( buf & mask )
                     {
                            PutPixel(x+j + 8,y+i,c);
                     }
                     else
                     {
                     
                     }
                     mask = mask >> 1;
              }                 
       }
}

//----------------------
void Lcd_printf(unsigned int x,unsigned int y,U8 *text,unsigned int Color,unsigned int bkColor)
{
       	U8 *p;
		U16	x0,y0;
		U16 i;
		U8 code0 ,code1;
		static U8 scr[15][22] = {0};
	 

	   	p = text;
	  	x0 = x / 8;
      	y0 = y / 16;
     
		 while(*p != 0 )
       {
             	code0 = *p;  
	     		code1 = *(p+1);
                if( code0 > 0xA0 & code1 > 0xA0 )  //中文输出
               {
//				if ((code0 != scr[x0][y0]) || (code1 != scr[x0+1][y0]))
//	         		{
//						//Uart_Printf("hello here \n");
//						
//               			Lcd_PutHZ( x , y , (*p - 0xA0)*0x0100 + *(p+1) - 0xA0 , Color , bkColor );
//						scr[x0][y0] = code0;
//						scr[x0+1][y0] = code1;
//					}
					Lcd_PutHZ( x , y , (*p - 0xA0)*0x0100 + *(p+1) - 0xA0 , Color , bkColor );
					x0 += 2;    
					x += 16;
					p += 2;


                }
                else // ASC 字库
				{  
//				if ((x == cursor_x) && (y == cursor_y))   // 光标位置要刷新
//				{
//					scr[x0][y0] = 0;
//				}
//					if (code0 != scr[x0][y0])
//					{
//						Lcd_PutASCII( x, y, code0, Color, bkColor ); 
//						if ((x == cursor_x) && (y == cursor_y))   // 光标位置要刷新
//						{
//							scr[x0][y0] = 1;
//						}
//						else
//						{
//						scr[x0][y0] = code0;
//						}    
//					}
					Lcd_PutASCII( x, y, code0, Color, bkColor );
					x0 += 1;
					x += 8 ;
					p++;
				}                   
 
       }
   
}
/**************************************************************
**************************************************************/
void Lcd_TFT_Init(void)
{
	Lcd_Init();
	Lcd_PowerEnable(0, 1);
	Lcd_EnvidOnOff(1);		//turn on vedio

	Lcd_ClearScr( (0x00<<11) | (0x00<<5) | (0x00) );  
	
}

