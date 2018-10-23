#include "tsk_checkOutLine.h"
#include "fpga_conf.h"
#include "his_spi.h"
typedef struct TSK_CHECK_OUT_DEV_E 
{
	WV_THR_HNDL_T 	thrHndl; 
	WV_U32      	open;
	WV_U32      	close;
	 
}TSK_CHECK_OUT_DEV_E;
TSK_CHECK_OUT_DEV_E  gCheckOutLine;


/******************************************************************************

void * TSK_CheckOut_Proc(void * prm);
******************************************************************************/
void * TSK_CheckOut_Proc(void * prm)
{

	TSK_CHECK_OUT_DEV_E   * pDev; 

	pDev = (TSK_CHECK_OUT_DEV_E *)prm;

	WV_U16 dataOld=0,dataNew=0;
    //TSK_FPGA_Read(0x08,&dataOld);
	//dataOld = dataOld & 0x0003;
	//dataNew = dataOld;

	pDev-> open  = 1;
	pDev-> close  = 0;  

  	while(pDev -> open == 1)
	{  
				
		TSK_FPGA_Read(0x08,&dataNew);
		dataNew = dataNew & 0x3;
		if(dataOld == dataNew  )
		{
			usleep(500000);
			continue;
		}

		if(TSK_SCENE_GetState() == 0 )//scene close
		{
			usleep(500000);
			continue;
		}
		dataOld = dataNew & 0x0003;
		TSK_SCENE_CleanWinFram();
		//FPGA_CONF_CleanWinFram();
		
	} 
	pDev-> open  = 0;
	pDev-> close = 1;
	return NULL;
} 
/***************************************************************

WV_S32 TSK_CheckOut_Open();

***************************************************************/
WV_S32 TSK_CheckOut_Open()
{

   WV_CHECK_RET( WV_THR_Create(&gCheckOutLine.thrHndl,TSK_CheckOut_Proc, WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gCheckOutLine)); 
   return  WV_SOK; 
}

/***************************************************************

WV_S32 TSK_CheckOut_Close();

***************************************************************/

WV_S32 TSK_CheckOut_Close()
{ 
   if(gCheckOutLine.open == 1)
   {
   	  gCheckOutLine.open = 0;
   	  while(gCheckOutLine.close == 1) ;
     WV_CHECK_RET( WV_THR_Destroy(&gCheckOutLine.thrHndl));	
   }  
   return  WV_SOK;
 
}

