#include"svr_frame.h"
#define SVR_FRAME_DEBUG_MODE 
#ifdef SVR_FRAME_DEBUG_MODE 
  #define SVR_FRAME_printf(...)  do { printf("\nSVR_FRAMR:" __VA_ARGS__); fflush(stdout); } while(0)
#else 
  #define SVR_FRAME_printf(...) 
#endif


/************************************************************
WV_S32  SVR_FRAME_Print(SVR_FRAME_HEAD_E * pHead);
************************************************************/
WV_S32  SVR_FRAME_Print(SVR_FRAME_HEAD_E * pHead)
{
   SVR_FRAME_printf("sync  = [0x%x]\r\n", pHead -> sync);
   SVR_FRAME_printf("cmdL0 = [0x%x]\r\n", pHead -> cmdL0); 
   SVR_FRAME_printf("cmdL1 = [0x%x]\r\n", pHead -> cmdL1); 
   SVR_FRAME_printf("cnt   = [0x%x]\r\n", pHead -> cnt); 
   SVR_FRAME_printf("arg1  = [0x%x]\r\n", pHead -> arg1); 
   SVR_FRAME_printf("arg2  = [0x%x]\r\n", pHead -> arg2); 
   SVR_FRAME_printf("dNum  = [0x%x]\r\n", pHead -> dataNum);  
  return WV_SOK;
}

/************************************************************
WV_S32  SVR_FRAME_Analyze(SVR_FRAME_HEAD_E * pHead , WV_U16 cnt);
************************************************************/

WV_S32  SVR_FRAME_Analyze(SVR_FRAME_HEAD_E * pHead, WV_U16 *pCnt)
{
	static WV_U32 cntErro = 0;
   //check  sysnc 
	if(pHead -> sync  != SVR_SYNC_WORD)
	{
	   SVR_FRAME_printf("not fined sysnc ERORR!!!!!!!!");
	   cntErro++;
	   return WV_EFAIL;
	}  
	//check sysnc 
 	if((pHead->cmdL0  & 0x80) == 0x00){
     
     // SVR_FRAME_printf("It is ACK frame  %d[%d]  eroo[%d]!!!!!!!!",pHead -> cnt,*pCnt,cntErro);
		if (pHead -> cnt  != *pCnt )
		{ 
		   SVR_FRAME_printf("111cnt is not conntine get %d[ local %d] erro = %d!!!!!!!!",pHead -> cnt,*pCnt,cntErro);
		  *pCnt = pHead -> cnt;
		   cntErro++;
	    	   return WV_EFAIL;
		}   
 	}else{	
    // SVR_FRAME_printf("It is cmd  frame  %d[%d]eroo[%d]!!!!!!!!",pHead -> cnt,*pCnt,cntErro);
	 
		   SVR_FRAME_printf("get cmdL0 = %02x!!!!!!!!",pHead->cmdL0);
		   *pCnt = pHead -> cnt;
		   cntErro++;
		   return WV_EFAIL;
	

	
   } 	
	//printf("cnt is [%d],pcnt is [%d]\n",pHead -> cnt,(*pCnt));
	(*pCnt)++;
  return WV_SOK;
}

/***********************************************************
WV_S32  SVR_FRAME_SyncPre(WV_U16  sync);
***********************************************************/

WV_S32  SVR_FRAME_SyncPre(WV_U16  sync)
{
  if(sync == 0X5a)
  {
    return WV_SOK;
  }
  SVR_FRAME_printf("pre sysnc ERRO %d!!!!!!!!",sync);  
  return WV_EFAIL;
}


/***********************************************************
WV_S32  SVR_FRAME_SyncPre(WV_U16  sync);
***********************************************************/

WV_S32  SVR_FRAME_Sync(WV_U16  sync)
{
  if(sync == 0X5aa5)
  {
    return WV_SOK;
  }
  SVR_FRAME_printf("sysnc ERRO %d!!!!!!!!",sync);  
  return WV_EFAIL;
}

