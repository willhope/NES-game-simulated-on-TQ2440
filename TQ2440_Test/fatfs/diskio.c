/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "SD_MMC.h"


static init=1;
extern sSD_struct SDCard;//identify device 得到的信息数据结构
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
//this function can only be called after power up, hardware reset not implemented
//maybe software reset can be implemented here.
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive number (0..),only 0 implemented */
)
{
	if (drv!=0)	return STA_NODISK;
	//init=SDinit();
	if(init)
	return RES_OK;
	else
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..),only 0 implemented */
)
{
	if (drv!=0) return STA_NODISK;
	
	if (init==0) return STA_NOINIT;
	
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..),only 0 implemented */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{

	DRESULT res;

	

	if ((drv!=0) || (buff==0))	return RES_PARERR;//more check needed
	
	if(count<2)
		{
		res=Read_One_Block(sector,(U32*)buff);
		
		}
	else
		res=Read_Mult_Block(sector,count,(U32*)buff);
	
	
	if(res)
		return RES_OK;
	else
		return RES_ERROR;

}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
/* The FatFs module will issue multiple sector transfer request
/  (count > 1) to the disk I/O layer. The disk function should process
/  the multiple sector transfer properly Do. not translate it into
/  multiple single sector transfers to the media, or the data read/write
/  performance may be drasticaly decreased. */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..),only 0 implemented */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	DRESULT res;
	
	
	if ((drv!=0) || (buff==0))	return RES_PARERR;//more check needed
	//ACMD23(SDCard.iCardRCA,count);
	if(count < 2)
		res=Write_One_Block(sector,(U32*)buff);
	else
		res=Write_Mult_Block(sector,count,(U32*)buff);

	
			
	if(res)
		return RES_OK;
	else
		return RES_ERROR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..),only 0 implemented */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	
	if (drv!=0) return RES_PARERR;
	
	if (ctrl==GET_SECTOR_COUNT)//获取硬盘总扇区数，使用IDENTIFY DEVICE命令获得
	{
		
		*(DWORD *)buff = (DWORD)(1024*1024*8);
		//Uart_Printf("Get Sector Numbers: %d\n", hd_id.lba_capacity);
		return RES_OK;
	}
	if (ctrl==GET_BLOCK_SIZE)//每个扇区为512个字节
	{
		*(DWORD *)buff = (DWORD)(512);
		return RES_OK;
	}
	if (ctrl==CTRL_SYNC) //每一次读写操作均等到设备空闲，因此这里直接返回OK
		return RES_OK;
		
	return RES_PARERR;
}

