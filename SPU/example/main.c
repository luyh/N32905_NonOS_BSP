/****************************************************************************
*                                                                           *
* Copyright (c) 2009 Nuvoton Tech. Corp. All rights reserved.               *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   main.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   SPU sample application using SPU library. 
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*
* HISTORY
*
* REMARK
*   None
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wblib.h"
#include "SONG.h"
#include "DrvSPU.h"
#include "spu.h"
//#include "w55fa93_gpio.h"

#define LENGTH		194540	// The length of song in unit of 16-bit

static UINT16 u16IntCount = 2;
static UINT32 u32FragSize;
static volatile UINT8 bPlaying = TRUE;

int playCallBack(UINT8 * pu8Buffer)
{	
	UINT32 u32Offset = 0;
	UINT32 len = sizeof(SPU_SOURCE);
	
	u32Offset = ( u32FragSize / 2) * u16IntCount;
	if (u32Offset >= len)		// Reach the end of the song, restart from beginning
	{		
		u16IntCount = 2;
		u32Offset = 0;
		bPlaying = FALSE;
		return TRUE;
	}	
	
	memcpy(pu8Buffer, &SPU_SOURCE[u32Offset], u32FragSize/2);
	
	u16IntCount++;
	
	return FALSE;
}


int main(void)
{
	WB_UART_T uart;
	UINT32 u32ExtFreq;	

	sysSetSystemClock(eSYS_UPLL, 	//E_SYS_SRC_CLK eSrcClk,	
						192000,		//UINT32 u32PllKHz, 	
						192000,		//UINT32 u32SysKHz,
						192000,		//UINT32 u32CpuKHz,
						  96000,		//UINT32 u32HclkKHz,
						  48000);		//UINT32 u32ApbKHz	

	u32ExtFreq = sysGetExternalClock();
	uart.uiFreq = u32ExtFreq*1000;		
    uart.uiBaudrate = 115200;
    uart.uiDataBits = WB_DATA_BITS_8;
    uart.uiStopBits = WB_STOP_BITS_1;
    uart.uiParity = WB_PARITY_NONE;
    uart.uiRxTriggerLevel = LEVEL_1_BYTE;
    sysInitializeUART(&uart);	

	sysEnableCache(I_D_CACHE);

	sysprintf("Start Playing...\n");	

	spuOpen(eDRVSPU_FREQ_8000);
	
	spuIoctl(SPU_IOCTL_SET_VOLUME, 0x3f, 0x3f);
	spuIoctl(SPU_IOCTL_GET_FRAG_SIZE, (UINT32)&u32FragSize, 0);
	
	spuEqOpen(eDRVSPU_EQBAND_2, eDRVSPU_EQGAIN_P7DB);
	spuDacOn(1);
    spuStartPlay((PFN_DRVSPU_CB_FUNC *) playCallBack, (UINT8 *)SPU_SOURCE);		

   	while(bPlaying == TRUE);	
	
    spuEqClose();	
    spuClose();
    	
    while(1);	
		
	return(0);
}