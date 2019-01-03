#include"tsk_cast.h"
#include"his_dis.h"
#include"his_venc.h"

typedef struct TSK_CAST_DEV_E 
{
 WV_THR_HNDL_T thrHndl; 
 WV_U32      open;
 WV_U32      close; 
 WV_U32      encHandl;
 WV_U32      castHandl;
 WV_U8       *pEsBuf;
 WV_U8       *pSockBuf;
 WV_U32      esLen; 
 WV_U32      esPtsMs;
 WV_U32      CurPtsMs;
 WV_ETH_UDP_TX_E  txSock; 
 WV_U16      vPid;
 WV_U8       pkgCnt;
 WV_U32      erroCnt;
}TSK_CAST_DEV_E;

#define  TSK_CASK_BUF_LENMAX     1920*1200*4

#define ESTX_PAG_LOAD_LEN                1316
#define ESTX_PAG_HEAD_LEN                12
#define ESTX_PAG_LEN               ESTX_PAG_LOAD_LEN + ESTX_PAG_HEAD_LEN 


static TSK_CAST_DEV_E gCastDev;

/******************************************************************************

void *TSK_CAST_Proc(Void * prm);
******************************************************************************/
void * TSK_CAST_Proc ( void * prm)
{
 TSK_CAST_DEV_E  * pDev; 
 WV_U8  *sockBuf, * esBuf;
 WV_U32  esLen,fillLen;
 WV_S32 ret =0; 
 pDev = (TSK_CAST_DEV_E  *) prm;
 
 pDev-> open  = 1;
 pDev-> close  = 0;  
  while(pDev -> open == 1)
    {  
     	ret = HIS_VENC_GetStream(&(pDev->encHandl), pDev-> pEsBuf, &(pDev->esLen), &(pDev->esPtsMs));
		if(ret == WV_SOK)
		{ 
			//WV_printf("recev pack len [%d]  pts[ %d]   ptsUS[ %lld]  ptsUs[%lld]\r\n", pDev->esLen,pDev->esPtsMs,*((WV_U64*)(pDev-> pEsBuf)),((WV_U64)(pDev->esPtsMs))*1000);  
			esLen = pDev->esLen;
			esBuf = pDev-> pEsBuf;
			while(esLen >0)
			{
				sockBuf = pDev->pSockBuf; 
				sockBuf[0] = 0x47;  
				if(esLen > ESTX_PAG_LOAD_LEN)
				{
					sockBuf[1] = 0x40;
					fillLen = ESTX_PAG_LOAD_LEN;
				}
				else  //end of ES
				{
					sockBuf[1] = 0x80; 
					fillLen = esLen;
				}
				sockBuf[1] = sockBuf[1] | ((pDev->vPid>>8) & 0xf); //payload_unit_start_indicator and pid_h
				sockBuf[2] = pDev->vPid & 0xFF; //pid_l 
				pDev->pkgCnt ++;
				sockBuf[3]  = pDev->pkgCnt&0xff;
				HI_SYS_GetTimeStampMs(&(pDev->CurPtsMs)); 
				 
				*((WV_U64 *)(sockBuf+4)) = ((WV_U64)(pDev->CurPtsMs))*1000; 
				sockBuf = pDev->pSockBuf + 12;
				memcpy(sockBuf,esBuf, fillLen);
				esLen    = esLen -fillLen;
				esBuf    = esBuf +fillLen;

				ret = WV_UDP_Tx(&(pDev->txSock),pDev->pSockBuf, fillLen + ESTX_PAG_HEAD_LEN);

				if(ret != WV_SOK )			
				{ 
				  pDev->erroCnt++; 
				}     	

			} 
		}
		else
		{
			usleep(20000);
		} 
    } 
 pDev-> open  = 0;
 pDev-> close = 1;
return NULL;  
} 
/****************************************************************************************

WV_S32 TASK_CAST_ConfInit();

****************************************************************************************/

WV_S32 TASK_CAST_ConfInit()
{   
  WV_S8  ipLocal[20] = "192.168.15.98";
  WV_S8  ipTx[20] = "231.0.30.100"; 
	gCastDev.txSock.addrLocal.sin_addr.s_addr = inet_addr(ipLocal); 
	bzero(&( gCastDev.txSock.addrLocal.sin_zero),8);

	gCastDev.txSock.addrLocal.sin_family = AF_INET;
	gCastDev.txSock.addrLocal.sin_port = htons(7001);
	
    gCastDev.txSock.addrTx.sin_addr.s_addr = inet_addr(ipTx); 
    bzero(&( gCastDev.txSock.addrTx.sin_zero),8);
   
    gCastDev.txSock.addrTx.sin_family = AF_INET;
    gCastDev.txSock.addrTx.sin_port = htons(7001);
    
    gCastDev.vPid = 48; 
    
     
  return WV_SOK;
}

/****************************************************************************************

WV_S32 TASK_CAST_Open();

****************************************************************************************/

WV_S32 TASK_CAST_Open()
{ 
	WV_S32 ret = 0; 
	memset(&gCastDev,0,sizeof(gCastDev)); 
	TASK_CAST_ConfInit(); 
	WV_RET_ADD(HIS_DIS_InitCast(&(gCastDev.castHandl)),ret);
	WV_RET_ADD(HIS_VENC_Create(&(gCastDev.encHandl) ,&(gCastDev.castHandl)),ret); 
	gCastDev.pEsBuf = (WV_U8 *)malloc(TSK_CASK_BUF_LENMAX +  2048); 
	gCastDev.pSockBuf = gCastDev.pEsBuf + TSK_CASK_BUF_LENMAX;

	WV_RET_ADD(WV_UDPTX_Creat(&(gCastDev.txSock)),ret);

	WV_RET_ADD( WV_THR_Create(&gCastDev.thrHndl, TSK_CAST_Proc , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gCastDev),ret);

	return ret;  
}

/****************************************************************************************

WV_S32 TASK_CAST_Close();

****************************************************************************************/

WV_S32 TASK_CAST_Close()
{ 

	WV_S32 ret = 0;
   if(gCastDev.open == 1)
   {
   	  gCastDev.open = 0;
   	  while(gCastDev.close == 1) ;
     WV_CHECK_RET( WV_THR_Destroy(&gCastDev.thrHndl));	
   } 
	
	WV_RET_ADD(HIS_VENC_Destroy(&(gCastDev.encHandl)),ret);
	WV_RET_ADD(HIS_DIS_DeInitCast(&(gCastDev.castHandl)),ret); 
	free (gCastDev.pEsBuf);
	WV_RET_ADD(WV_UDPTX_Destroy(&(gCastDev.txSock)),ret);
	return ret;  
}
