#include "tsk_mobileShow.h"
#include "tsk_go.h"
#include "tsk_player.h"
#include "tsk_scene.h"
#include "tsk_usbShow.h"
#define TSK_MOBILE_PHONE_PIC_PATH  "./env/phonePic.jpg"
//typedef struct TSK_MOBILE_SHOW_E
//{
//	WV_U8  gRecvPicFlag;
//	WV_U32 gMobileShowPic;
//}
static unsigned char gRecvPicFlag=2;
static unsigned int  gMobileShowFlag=0; // if (gMobileShowFlag == 0) there is no mobile show ena; bit 0 for pic ,bit 1 for mov

/***************************************************************

WV_S32 TSK_Mobile_SetshowFlag(WV_U32 flag);

***************************************************************/
WV_S32 TSK_Mobile_SetShowFlag(WV_U32 flag)
{
	gMobileShowFlag = flag;
	return WV_SOK;
}
/***************************************************************

WV_S32 TSK_Mobile_GetShowFlag();

***************************************************************/
WV_S32 TSK_Mobile_GetShowFlag()
{
	return gMobileShowFlag;
}
/***************************************************************

WV_S32 TSK_Mobile_getPicCleanFlag();

#if server cenect is break ,clean the flag gRecvPicFlag
***************************************************************/
WV_S32 TSK_Mobile_getPicCleanFlag()
{
	gRecvPicFlag = 2;
	return WV_SOK;
}

/***************************************************************
WV_S32 TSK_Mobile_ShowPic(WV_U8 *pData,WV_U32 dataLen,WV_U8 cmd);

pData:    pic data
dataLen:  pic file len
cmd :     if(cmd == 1)get  pic file is not end ,get next package
	      if(cmd == 2)get  pic file is end ,show the pic
		  if(cmd == 3)close the pic
***************************************************************/
WV_S32 TSK_Mobile_ShowPic(WV_U8 *pData,WV_U32 dataLen,WV_U8 cmd)
{
	
	WV_U32 writeLen;
	FILE *fp;
	WV_S8 filePath[30];
	TSK_GO_MOV_PIC_POS_S movPos,picPos;
	TSK_GO_GetUsb_PicAndMov_Pos(&movPos,&picPos);
	if(picPos.startX == 0 && picPos.startY == 0 && picPos.endX == 0 && picPos.endY == 0)
	{
		return WV_EFAIL;
	}
	if(cmd == 1)
	{
		if(gRecvPicFlag == 1)
		{
			fp = fopen(TSK_MOBILE_PHONE_PIC_PATH,"ab+");
			if(fp == NULL)
			{
				WV_printf("get phone pic:fopen %s error \n",TSK_MOBILE_PHONE_PIC_PATH);
				return WV_EFAIL;
			}
		}else if(gRecvPicFlag == 2)
		{
			fp = fopen(TSK_MOBILE_PHONE_PIC_PATH,"wb+");
			if(fp == NULL)
			{
				WV_printf("get phone pic:fopen %s error \n",TSK_MOBILE_PHONE_PIC_PATH);
				return WV_EFAIL;
			}
		}
			writeLen = fwrite(pData,1,dataLen,fp);
			if(writeLen != dataLen)
			{		
				fclose(fp);
				WV_printf("fwrite phone pic to local path[%s] error \n",TSK_MOBILE_PHONE_PIC_PATH);
				return WV_EFAIL;	
			}
			fclose(fp);

	}else if(cmd == 2){
	
		if(gRecvPicFlag == 1)
		{
			fp = fopen(TSK_MOBILE_PHONE_PIC_PATH,"ab+");
			if(fp == NULL)
			{
				WV_printf("get phone pic:fopen %s error \n",TSK_MOBILE_PHONE_PIC_PATH);
				return WV_EFAIL;
			}
		}else if(gRecvPicFlag == 2)
		{
			fp = fopen(TSK_MOBILE_PHONE_PIC_PATH,"wb+");
			if(fp == NULL)
			{
				WV_printf("get phone pic:fopen %s error \n",TSK_MOBILE_PHONE_PIC_PATH);
				return WV_EFAIL;
			}
		}
		writeLen = fwrite(pData,1,dataLen,fp);
		if(writeLen != dataLen)
		{		
			fclose(fp);
			WV_printf("fwrite phone pic to local path[%s] error \n",TSK_MOBILE_PHONE_PIC_PATH);
			return WV_EFAIL;	
		}
		fclose(fp);
		
		TSK_SCENE_CloseWinAndAni(); //close outline and pic
		TSK_GO_SetColorKeyFlag(0);
		sprintf(filePath,TSK_MOBILE_PHONE_PIC_PATH);
		//TSK_GO_DecClose(0);
		TSK_GO_DecOpen(0,filePath, picPos.startX,  picPos.startY,  picPos.endX - picPos.startX, picPos.endY - picPos.startY);
		gMobileShowFlag |= 0x1; // set bit 1 ena(show pic)
	}else if(cmd ==3 )
	{
		if((gMobileShowFlag & 0x1) == 0x1)
		{
			TSK_GO_SetColorKeyFlag(1);
			TSK_GO_DecClose(0);
			gMobileShowFlag &= 0x2; // set bit 1 disable(close pic)
		}
		if(gMobileShowFlag == 0 )
		{
			if(TSK_USB_GetShowFlag() == 0 )
			{
				TSK_SCENE_OpenWinAndAni();
			}else{
				TSK_GO_SetColorKeyFlag(0);
			}
		}
	}

	return 0;

}

/*******************************************************************

 WV_S32 TSK_Mobile_PlayMovUrl(WV_U8 cmd,WV_U8 *url,WV_S32 len);

cmd: cmd == 3 ,close player 2
*******************************************************************/
WV_S32 TSK_Mobile_PlayMovUrl(WV_U8 cmd,WV_U8 *url,WV_S32 len)
{
	WV_printf("url:%s\n",url);
	
	//WV_U8 name[64];
	//memset(name,0,sizeof(name));
	//sprintf(name,"%s",url);
	TSK_GO_MOV_PIC_POS_S movPos,picPos;
	TSK_GO_GetUsb_PicAndMov_Pos(&movPos,&picPos);

	if(movPos.startX == 0 && movPos.startY == 0 && movPos.endX == 0 && movPos.endY == 0)
	{
		return WV_EFAIL;
	}

	if(cmd == 3)//stop the player
	{
		if((gMobileShowFlag & 0x2) == 0x2)
		{
			TSK_PLAYER_setWin(2,0,0,0,0);
			TSK_PLAYER_Destory(2);
			gMobileShowFlag &= 0x1; //set bit 2 disable (close player 2)
		}
		if(gMobileShowFlag ==0)
		{
			if(TSK_USB_GetShowFlag() == 0 )
			{
				TSK_SCENE_OpenWinAndAni();
			}
		}
		return WV_SOK;		
	}

	if(movPos.startX == 0 && movPos.startY == 0 && (movPos.endX-movPos.startX)<200 && (movPos.endY-movPos.startY)<200)
	{
		return WV_EFAIL;
	}
	if(TSK_USB_GetShowFlag()  == 0 && gMobileShowFlag==0)
	{
		TSK_SCENE_CloseWinAndAni();
	}	
	TSK_PLAYER_Destory(2);
	TSK_PLAYER_SetFile(2,(WV_S8 *)url);
	TSK_PLAYER_setWin(2,movPos.startX,movPos.startY,movPos.endX-movPos.startX,movPos.endY-movPos.startY);
	TSK_PLAYER_Creat(2);

	gMobileShowFlag |= 0x2;
	
    return WV_SOK;
} 

