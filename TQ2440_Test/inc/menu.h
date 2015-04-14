
#include "def.h"
/*
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

//***********************************************
#define     KEY_ESC     1 //KEY_TAMPER	1
#define     KEY_ENTER   2 //KEY_WAKEUP	2
#define     KEY_USER	3
#define     KEY_UP   	4
#define     KEY_DN		5
#define     KEY_LEFT	6
#define     KEY_RIGHT	7
//************************************************




extern void process_key(U8 keycode);
extern void display_menu(void);

extern U8 ReadMenuKey( int *xdata, int *ydata);