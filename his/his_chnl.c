#include "his_chnl.h"
#include "hi_unf_common.h"
#include "hi_unf_avplay.h"
#include "hi_unf_sound.h"
#include "hi_unf_disp.h"
#include "hi_unf_vo.h"
#include "hi_unf_demux.h"
 



#define HIS_CHNL_DEBUG_MODE 
#ifdef HIS_CHNL_DEBUG_MODE 
  #define HIS_CHNL_printf(...)  do { printf("\n\rHIS-CHNL:" __VA_ARGS__); fflush(stdout); } while(0)
#else 
  #define HIS_CHNL_printf(...) 
#endif

#if 0 
/******************************************************************************

void *HIS_CHNL_EsProc(Void * prm);
******************************************************************************/
void * HIS_CHNL_EsProc(void * prm)
{
	WV_AVPLAY_CHNL_E *pAvChnl;
	WV_S8 *pEsBuf,*pUdpBuf;
	WV_S8 *pRx;
	WV_U32 esLen;
	WV_ESRX_PACK_INFO_S  esInfo;
    WV_U32 bufLen,timeOut,esPts;
    HI_UNF_STREAM_BUF_S StreamBuf;
    WV_S32 rxLen;
     WV_S32 full = 0,ret =0;
	pAvChnl = prm; 
	pAvChnl -> open  = 1;
	pAvChnl -> close = 0;
	
	bufLen = 1920*1080*2;//0x100000;//1920*1080*2;
	pEsBuf =  ( WV_S8 *)malloc(1920*1080*2); 
	pUdpBuf =  ( WV_S8 *)malloc(3048); 
	
	
	pRx = pEsBuf;
	esLen = 0;
	timeOut = 0;//0XFFFFFFFF;//0;//0XFFFFFFFF;
	
    WV_ASSERT_RET (HI_UNF_AVPLAY_GetBuf(pAvChnl->avPlayHandl,HI_UNF_AVPLAY_BUF_ID_ES_VID, bufLen, &StreamBuf, timeOut));
 
	
	while(pAvChnl -> open  == 1)
	{
		// 
		WV_UDP_Rx(&(pAvChnl->socketRx),pUdpBuf, &rxLen); 
		//
		WV_ESRX_Analyze(pUdpBuf,&esInfo,rxLen);
		if(esInfo.pid != 48 || esInfo.sync == 0)  continue;  
		memcpy(pRx,esInfo.pLoad,esInfo.loadLen);
		pRx += esInfo.loadLen;
		esLen += esInfo.loadLen;  
		if(esInfo.endFlag != 1) continue;  
		// proc es frame
		//HIS_CHNL_printf("recive  fame! len = %d",esLen);
		// enpty buf
		esPts = (WV_U32)((*((WV_U64 *)pEsBuf))/1000); 
		memcpy(StreamBuf.pu8Data,pEsBuf + WV_ESRX_FRAME_HEAD_LEN,esLen - WV_ESRX_FRAME_HEAD_LEN ); 
		if(full == 0)
		{
	      WV_ASSERT_RET (HI_UNF_AVPLAY_PutBuf(pAvChnl->avPlayHandl,HI_UNF_AVPLAY_BUF_ID_ES_VID, esLen - WV_ESRX_FRAME_HEAD_LEN ,esPts)); 
	     } 
	   // HIS_CHNL_printf("ES pts = %d len = %d",esPts,esLen - WV_ESRX_FRAME_HEAD_LEN);
	     
		pRx = pEsBuf;
		esLen = 0;
		full = 0;
		ret = HI_UNF_AVPLAY_GetBuf(pAvChnl->avPlayHandl, HI_UNF_AVPLAY_BUF_ID_ES_VID, bufLen, &StreamBuf, timeOut);
		 if(ret != WV_SOK)
		 {
		   WV_ERROR("chennl[%d] get ES buff eroo!",pAvChnl->id);
		   full = 1; 
		 }
           
 
	}
	pAvChnl -> open  = 0;
	pAvChnl -> close = 1; 
	
	return NULL;
}



/***************************************************************************************

WV_S32  HIS_CHNL_Creat(WV_AVPLAY_CHNL_E * avChnl);

***************************************************************************************/

WV_S32  HIS_CHNL_Creat(WV_AVPLAY_CHNL_E *avChnl)
{
  if(avChnl -> type  == 0)
   {
	  HIS_AVP_EsOpen(&(avChnl->avPlayHandl),avChnl->vEecType);
	  HIS_DIS_WinCreat(&(avChnl->winHandl),avChnl->winRect);
	  HIS_DIS_WinAttach(&(avChnl->winHandl),&(avChnl->avPlayHandl)); 
	  HIS_DIS_WinStart(&(avChnl->winHandl));
	  HI_UNF_AVPLAY_Start(avChnl->avPlayHandl, HI_UNF_AVPLAY_MEDIA_CHAN_VID, HI_NULL); 
	  WV_UDPRX_Creat(&(avChnl->socketRx));
	  WV_THR_Create(&(avChnl->hndlProc), HIS_CHNL_EsProc,WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT,avChnl); 
	    
  }
  else if(avChnl -> type  == 1)
  {
  
      HIS_AVP_EsOpen(&(avChnl->avPlayHandl),avChnl->vEecType);
	  HIS_DIS_WinCreat(&(avChnl->winHandl),avChnl->winRect);
	  HIS_DIS_WinAttach(&(avChnl->winHandl),&(avChnl->avPlayHandl)); 
	  HIS_DIS_WinStart(&(avChnl->winHandl));
	 // HI_UNF_AVPLAY_Start(avChnl->avPlayHandl, HI_UNF_AVPLAY_MEDIA_CHAN_VID, HI_NULL); 
	  HIS_PLAYER_Create(&(avChnl->playerHandl) ,&(avChnl->avPlayHandl));
	  HIS_PLAYER_Start(&(avChnl->playerHandl) ,avChnl->fileName); 
  }  
	  
  return WV_SOK;	
}



/***************************************************************************************

WV_S32  HIS_CHNL_Destroy(WV_AVPLAY_CHNL_E * avChnl);

***************************************************************************************/

WV_S32  HIS_CHNL_Destroy(WV_AVPLAY_CHNL_E *avChnl)
{
   if(avChnl -> type  == 0)
   {
	  if(avChnl -> open == 1)
	  {
	   avChnl -> open = 0;
	   while(avChnl -> close == 0);
	   WV_THR_Destroy(&(avChnl->hndlProc));
	  } 
	  //
	  WV_UDPRX_Destroy(&(avChnl->socketRx));
	  //
	  
	  HIS_DIS_WinStop(&(avChnl->winHandl));
	  HIS_DIS_WinDetach(&(avChnl->winHandl),&(avChnl->avPlayHandl));
	  HIS_DIS_WinDestroy(&(avChnl->winHandl));
	  HIS_AVP_Close(&(avChnl->avPlayHandl));
	}  
	else if(avChnl -> type  == 1)
	{
	  HIS_PLAYER_Destory(&(avChnl->playerHandl)); 
	  HIS_DIS_WinStop(&(avChnl->winHandl));
	  HIS_DIS_WinDetach(&(avChnl->winHandl),&(avChnl->avPlayHandl));
	  HIS_DIS_WinDestroy(&(avChnl->winHandl));
	  HIS_AVP_Close(&(avChnl->avPlayHandl)); 
	}  

  
  return WV_SOK;	
}
#endif

