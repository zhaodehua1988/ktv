#include "tsk_text.h"
#include "wv_que.h"

#define TSK_TEXT_QUE_DEEP          10
#define TSK_TEXT_QUE_BUF_LEN       1000
#define TSK_TEXT_SHOW_WORD_ROW_NUM     3
#define TSK_TEXT_SHOW_WORD_COL_NUM     9
#define TSK_TEXT_SHOW_POSFILE  "./env/textPos.txt"

typedef struct TSK_TEXT_DEV_E
{
	WV_THR_HNDL_T thrHndl;	
	WV_U32      open;
	WV_U32      close;
	WV_U32 	    showPosEna; 

}TSK_TEXT_DEV_E;
//

#pragma pack(push)
#pragma pack(1)

typedef struct TSK_TEXT_CONF
{// 窗口类型
    WV_U16 x;                                           // x
    WV_U16 y;                                           // y
    WV_U16 w;                                          	// w
    WV_U16 h;                                         	// h
    WV_U32 u32Color;                                            // 颜色
    WV_U32 u32FontSize;                                         // 字号

}TSK_TEXT_CONF;

#pragma pack(pop)
typedef struct TSK_TEXT_NODE  
{  
	WV_U32 len;
	WV_U32 lastLen;
    WV_U8 text[1024];//内容 
	WV_U8 *pText;
	WV_U8 num;  
    //WV_U32 color;//字符串颜色   
    WV_U32 x;
	WV_U32 y; //每个字符串的初始位置 
	WV_U32 w;
	WV_U32 h;
	WV_U32 lostX;	  

}TSK_TEXT_NODE;
  
//struct node gTextData[3][3]={0};
TSK_TEXT_NODE gTextData[TSK_TEXT_SHOW_WORD_COL_NUM];
TSK_TEXT_CONF gTextConf;

TSK_TEXT_DEV_E 		gTskQueDev;
 WV_QUE_DEV_E     	gSrxQueRx; 
 WV_QUE_DEV_E     	gSrxQueFree; 
 WV_U8            * pSrxQueBuf;

/*****************************************************************************

WV_S32 TSK_TEXT_ReadTextPosConf(WV_U32 dataLen,WV_U8 *pData);

*****************************************************************************/
WV_S32 TSK_TEXT_ReadTextPosConf(WV_U32 dataLen,WV_U8 *pData)
{
	WV_U32 readLen;
	FILE *fp;
	fp = fopen(TSK_TEXT_SHOW_POSFILE,"rb");
	if(fp == NULL)
	{
		WV_printf("fopen %s error",TSK_TEXT_SHOW_POSFILE);
		return WV_EFAIL;
	}	
	
	readLen = fread(pData,1,dataLen,fp);
	if(readLen != dataLen)
	{
		WV_printf("read PicAndMov pos file error \n");
		fclose(fp);
		return WV_EFAIL;
	}

	fclose(fp);
	return WV_SOK;
}

/*****************************************************************************

WV_S32 TSK_TEXT_SaveText_Pos(WV_U32 dataLen,WV_U8 *pData);

*****************************************************************************/
WV_S32 TSK_TEXT_SaveText_Pos(WV_U32 dataLen,WV_U8 *pData)
{
	WV_U32 writeLen;
	FILE *fp;
	fp = fopen(TSK_TEXT_SHOW_POSFILE,"wb+");
	if(fp == NULL)
	{
		WV_printf("fopen %s error",TSK_TEXT_SHOW_POSFILE);
		return WV_EFAIL;
	}

	writeLen = fwrite(pData,1,dataLen,fp);
	if(writeLen != dataLen)
	{
		WV_printf("write PicAndMov pos file error \n");
		fclose(fp);
		return WV_EFAIL;
	}

	fclose(fp);
	return WV_SOK;
}

/*****************************************************************************

WV_S32 TSK_TEXT_GetConf(WV_U8 saveMode,WV_U32 dataLen,WV_U8 *pData);

*****************************************************************************/
WV_S32 TSK_TEXT_GetConf(WV_U8 saveMode,WV_U32 dataLen,WV_U8 *pData)
{
	
	WV_U16 *pTemp16;
	WV_U32 *pTemp32;
	//clean last 
	TSK_GO_CleanRect( gTextConf.x,gTextConf.y,gTextConf.w,gTextConf.h);

	if(dataLen != sizeof(TSK_TEXT_CONF))
	{
		return WV_EFAIL;	
	}	

	if(gTskQueDev.showPosEna == 0 )
	{
		gTskQueDev.showPosEna = 1;
	}

	TSK_GO_CleanLayer();
	//show pos
	{
		pTemp16 = (WV_U16*)pData;

		gTextConf.x = pTemp16[0]/2;
		gTextConf.y = pTemp16[1]/2;
		gTextConf.w = (pTemp16[2] - pTemp16[0])/2;
		gTextConf.h = (pTemp16[3] - pTemp16[1])/2;

		pTemp32 = (WV_U32*)&pData[8];
		gTextConf.u32Color = ((0xff & pTemp32[0] ) <<  16)|(0xff00 & pTemp32[0])| ((0xff0000 & pTemp32[0]) >> 16) ;
		gTextConf.u32FontSize = pTemp32[1];

		TSK_GO_CleanRect( gTextConf.x,gTextConf.y,gTextConf.w,gTextConf.h);
		//WV_printf("x %d,y %d,w%d,h%d,color:0x%x,uiFontSize:%d",gTextConf.x*2,gTextConf.y*2,gTextConf.w*2,gTextConf.h*2,gTextConf.u32Color,gTextConf.u32FontSize); 

		TSK_GO_ShowText( gTextConf.x,gTextConf.y,gTextConf.w,gTextConf.h,gTextConf.u32Color,gTextConf.u32FontSize,"弹幕调试",1,1);
		TSK_GO_RefreshLayerForPic();	
	}

	if(saveMode == 1)
	{
		TSK_TEXT_SaveText_Pos(sizeof(TSK_TEXT_CONF),(WV_U8 *)&gTextConf);
		TSK_GO_CleanLayer();
		TSK_GO_RefreshLayerForPic();
		TSK_SCENE_OpenWinAndAni();
		gTskQueDev.showPosEna = 0;
	}
	return WV_SOK;
}
/******************************************************************************

WV_S32 TSK_TEXT_GetText(WV_U8 * pRxBuf,WV_U32 *len)

******************************************************************************/
WV_S32 TSK_TEXT_GetText(WV_U8 * pRxBuf,WV_U32 *len)
{


    WV_S32 ret; 
	WV_U8  *pQue; 
	//WV_printf("TSK_SRX_TestRx strat \r\n"); 
	ret = WV_QUE_Get(&gSrxQueRx,(WV_U32 *)(&pQue),1); 
	
	if(ret != WV_SOK)
		{
		return WV_EFAIL;
		}  
	*len = *((WV_U32 *)pQue);
	 memcpy(pRxBuf,pQue + 4,*len);
	 WV_CHECK_RET( WV_QUE_Put(&gSrxQueFree,(WV_U32)pQue,1) );

	return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_TEXT_PutText(WV_U8 * pTxBuf,WV_U32 len)

******************************************************************************/
WV_S32 TSK_TEXT_PutText(WV_U8 * pTxBuf,WV_U32 len)
{
	//printf("TSK_TEXT_PutText len = %d \n",len);
	WV_S32 ret=0;
	WV_U8  *pQue; 
	ret = WV_QUE_Get(&gSrxQueFree,(WV_U32 *)(&pQue),1);
	if( ret != 0 ) return ret; 
	*((WV_U32 *)pQue) = len;
	memcpy(pQue + 4,pTxBuf,len);
	ret = WV_QUE_Put(&gSrxQueRx,(WV_U32)pQue,1);
	return ret; 
}

/******************************************************************************

static WV_S32 TSK_TEXT_PutText(WV_U8 *pText)

******************************************************************************/


/******************************************************************************

WV_S32 TSK_TEXT_Proc()

******************************************************************************/ 
void *TSK_TEXT_Proc(void *prm)
{

	TSK_TEXT_DEV_E *pDev;
	WV_S32 ret;
	pDev = (TSK_TEXT_DEV_E *)prm;

	int i;
	pDev->open = 1;
	pDev->close = 0;

	WV_U32 speed=5;
	WV_U32 textShow=0;
	
#if 1 	
	while(pDev->open == 1){
		
		textShow = 0;
		
		//printf("TSK_TEXT_Proc(void *prm)===============\n");
		for(i=0;i<TSK_TEXT_SHOW_WORD_COL_NUM;i++)
		{
			
			if(gTextData[i].len == 0)
			{
				memset(gTextData[i].text,0,sizeof(gTextData[i].text));
				ret = TSK_TEXT_GetText(gTextData[i].text,&gTextData[i].len);

				if(ret == WV_SOK)
				{
					gTextData[i].x = (gTextConf.x+gTextConf.w);
					gTextData[i].y = gTextConf.y;
					gTextData[i].w = (gTextData[i].len/2) * (gTextConf.w/4);//字的个数*每个字的大小
					gTextData[i].h = gTextConf.h;

					gTextData[i].lostX = (gTextConf.w/4);
					gTextData[i].pText = &gTextData[i].text[1];

				}								
			}else{
				//如果x坐标大于移动速度，则x坐标减去移动速度（像素）
				if(gTextData[i].x > speed) 
				{
					TSK_GO_CleanRect( gTextData[i].x,gTextData[i].y,gTextData[i].w,gTextData[i].h);

					gTextData[i].x-=speed;
					
					ret = TSK_GO_ShowText( gTextData[i].x,gTextData[i].y,gTextData[i].w,gTextData[i].h,gTextConf.u32Color,gTextConf.u32FontSize,(WV_S8 *)gTextData[i].pText,gTextData[i].text[0],0);
					textShow = 1;
					if(ret != 0 ){
						gTextData[i].pText = NULL;
						gTextData[i].len = 0;
						gTextData[i].x = 0;
						gTextData[i].w = 0;
						continue;
												
					}
					//TSK_GO_CleanRect( 0,gTextConf.y,(gTextConf.w/2),gTextConf.h);//左侧空余两个汉字大小
					
				}else{
					//如果x坐标小于移动速度，减少一个汉字，x=0，使用丢弃汉字后新的x坐标即lostX
					if(gTextData[i].x != 0)
					{
						TSK_GO_CleanRect( gTextData[i].x,gTextData[i].y,gTextData[i].w,gTextData[i].h);

						gTextData[i].pText+=2;//移动一个字
						gTextData[i].w -=(gTextConf.w/4);//减少一个字的宽度

						gTextData[i].x = 0;
					}else{
						TSK_GO_CleanRect( gTextData[i].lostX,gTextData[i].y,gTextData[i].w,gTextData[i].h);
					}					
					//

					if(gTextData[i].lostX > speed)
					{
						gTextData[i].lostX = gTextData[i].lostX-speed;
					}else{
						gTextData[i].pText+=2;//移动一个字

						gTextData[i].w -=(gTextConf.w/4);//减少一个字的宽度
						gTextData[i].lostX = (gTextConf.w/4)- speed;						
					
					}
					
					if(gTextData[i].w >= (gTextConf.w/4))
					{
						ret = TSK_GO_ShowText( gTextData[i].lostX,gTextData[i].y,gTextData[i].w,gTextData[i].h,gTextConf.u32Color,gTextConf.u32FontSize,(WV_S8 *)gTextData[i].pText,gTextData[i].text[0],0);
						textShow = 1;
						if(ret != 0 ){
							gTextData[i].pText = NULL;
							gTextData[i].len = 0;
							gTextData[i].x = 0;
							gTextData[i].w = 0;
							continue;						
						}
						
					}else{

						gTextData[i].pText = NULL;
						gTextData[i].len = 0;
						gTextData[i].x = 0;
						gTextData[i].w = 0;
					}
				}
	
			} 
		}
		if(textShow == 1)
		{
			TSK_GO_RefreshLayerForPic();
			//printf("******refresh layer***** \n");
			usleep(500);
		}else{
			sleep(1);
		}

	}
#endif

	pDev-> open  = 0;
	pDev-> close = 1;
	return WV_SOK;
}
/******************************************************************************

WV_S32 TSK_TEXT_Open()

******************************************************************************/ 
WV_S32 TSK_TEXT_Open()
{
	WV_printf("----------TSK_TEXT_Open() ---------------\n");
	memset(gTextData,0,sizeof(TSK_TEXT_NODE)*9);
	pSrxQueBuf = (WV_U8*) malloc(TSK_TEXT_QUE_BUF_LEN *TSK_TEXT_QUE_DEEP);
	WV_CHECK_RET(   WV_QUE_Create(  &gSrxQueRx,TSK_TEXT_QUE_DEEP)  );  
	WV_CHECK_RET(   WV_QUE_Create(  &gSrxQueFree,TSK_TEXT_QUE_DEEP)  );   
	WV_S32 i;
	for(i =0;i<TSK_TEXT_QUE_DEEP;i++){
		WV_CHECK_RET(  WV_QUE_Put(&gSrxQueFree, (WV_U32)(pSrxQueBuf+ TSK_TEXT_QUE_BUF_LEN*i),0) );
	}  

	memset(&gTextData,0,sizeof(TSK_TEXT_NODE)*TSK_TEXT_SHOW_WORD_COL_NUM);
	memset(&gTextConf,0,sizeof(TSK_TEXT_CONF));

	TSK_TEXT_ReadTextPosConf(sizeof(TSK_TEXT_CONF),(WV_U8 *)&gTextConf);

	WV_CHECK_RET( WV_THR_Create(&gTskQueDev.thrHndl,TSK_TEXT_Proc, WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, (void *)(&gTskQueDev)));

	return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_TEXT_Close()

******************************************************************************/ 
WV_S32 TSK_TEXT_Close()
{

   if(gTskQueDev.open == 1)
   {
   	  gTskQueDev.open = 0;
   	  while(gTskQueDev.close == 1);
      WV_THR_Destroy(&(gTskQueDev.thrHndl));
   }

  WV_CHECK_RET(   WV_QUE_Destroy( &gSrxQueRx  )  );  
  WV_CHECK_RET(   WV_QUE_Destroy( &gSrxQueFree)  );  
  free(pSrxQueBuf);

	return WV_SOK;
}

