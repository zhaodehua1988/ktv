/*************************************************************************


播放显示u盘图片和视频


*************************************************************************/
#include "tsk_usbShow.h"
#include "tsk_go.h"
#include "tsk_player.h"
#include "tsk_mobileShow.h"
#include "tsk_scene.h"
#define TSK_USBSHOW_PATH_PIC   "/usb/pic"
#define TSK_USBSHOW_PATH_MOV   "/usb/mov"

#define TSK_USBSHOW_NAME_MAXLEN 128
#define TSK_USBSHOW_NAME_MAXNUM 256
#define TSK_USBSHOW_PLAY_ID 2

typedef struct TSK_USB_SHOW_E
{

	WV_THR_HNDL_T 	thrHndl; 
	WV_U32      	open;
	WV_U32      	close;
	TSK_GO_MOV_PIC_POS_S showPos;	
	WV_U8           ena;
	WV_S8          *pName;
	WV_U32         num;
	WV_U8          plug;
	WV_U8          playEnd;
           	
}TSK_USB_SHOW_E;

static TSK_USB_SHOW_E  gShowPicDev;
static TSK_USB_SHOW_E  gShowMovDev;

static unsigned int gUsbShowFlag=0;
/******************************************************************************

WV_S32 TSK_USB_GetShowFlag();

******************************************************************************/
WV_S32 TSK_USB_GetShowFlag()
{

	return 	gUsbShowFlag;
}
/******************************************************************************

WV_S32 TSK_USB_SetShowEna();

******************************************************************************/
WV_S32 TSK_USB_SetShowEna()
{
	WV_S8 picPath[20];
	WV_S8 movPath[20];
	DIR *dir;
	struct    dirent    *ptr; 
	sprintf(picPath,"%s",TSK_USBSHOW_PATH_PIC);
	sprintf(movPath,"%s",TSK_USBSHOW_PATH_MOV);

	memset(gShowPicDev.pName,0,TSK_USBSHOW_NAME_MAXLEN*TSK_USBSHOW_NAME_MAXNUM+128);
	memset(gShowMovDev.pName,0,TSK_USBSHOW_NAME_MAXLEN*TSK_USBSHOW_NAME_MAXNUM+128);
 	
	gShowPicDev.num = 0;
	gShowMovDev.num = 0;
	
	if(access(picPath,F_OK) == 0 )
	{

		dir = opendir(picPath);
		if(dir == NULL)
	 	{
	   		printf(" %s not exist!!!\r\n",  picPath); 
	 	}else{	

			while((ptr = readdir(dir)) != NULL)
			 {
				if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0 )
				{
					continue;
				}
				
				if( NULL == strstr(ptr->d_name,".jpg") &&  NULL == strstr(ptr->d_name,".jpeg") && NULL == strstr(ptr->d_name,".bmp") && \
					NULL == strstr(ptr->d_name,".gif") && NULL == strstr(ptr->d_name,".png"))
				{
					continue;
				}				
					                           
				/*
				unsigned char d_type，文件或目录的类型。它有可能的取值如下：
				   DT_REG，普通文件
				*/		  
				if(ptr->d_type == DT_REG)
				{
					sprintf(&gShowPicDev.pName[gShowPicDev.num*TSK_USBSHOW_NAME_MAXLEN],"%s/%s",picPath,ptr->d_name);
					gShowPicDev.num++;
					if(gShowPicDev.num >=TSK_USBSHOW_NAME_MAXNUM)
					{
						break;
					}
	 			}


			}
			closedir(dir);

		}
			gUsbShowFlag |= 0x1;
			gShowPicDev.ena = 1;
	}else{


		//printf("get usb pic show is disable \n");
		gShowPicDev.ena = 0;
		gUsbShowFlag &= 0x2;
	}

	//get mov list

	if(access(movPath,F_OK) == 0 )
	{

		dir = opendir(movPath);
		if(dir == NULL)
	 	{
	   		printf(" %s not exist!!!\r\n",  movPath); 
 
	 	}else{	

			while((ptr = readdir(dir)) != NULL)
			 {
				if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0 )
				{
					continue;
				}	
			    if( NULL == strstr(ptr->d_name,".mov") && NULL == strstr(ptr->d_name,".mp4") &&   NULL == strstr(ptr->d_name,".avi") && \
					NULL == strstr(ptr->d_name,".rm") && NULL == strstr(ptr->d_name,".rmvb")  && NULL == strstr(ptr->d_name,".wmv") && NULL == strstr(ptr->d_name,".flv") )
				{
					continue;
				}                               
				/*
				unsigned char d_type，文件或目录的类型。它有可能的取值如下：
			       DT_REG，普通文件
		  		*/
				if(ptr->d_type == DT_REG)
				{
					sprintf(&gShowMovDev.pName[gShowMovDev.num*TSK_USBSHOW_NAME_MAXLEN],"%s/%s",movPath,ptr->d_name);
					gShowMovDev.num++;
					if(gShowMovDev.num >=TSK_USBSHOW_NAME_MAXNUM)
					{
						break;
					}
	 			}
			}
			closedir(dir);
		}
		// printf("set usb mov show is enable \n");
		gShowMovDev.ena = 1;
		gUsbShowFlag |= 0x2;

	}else{

		//printf("get usb mov show is disable \n");
		gUsbShowFlag &= 0x1;
		gShowMovDev.ena = 0;
	}

	if(TSK_Mobile_GetShowFlag() == 0 )
	{
	
		if(gShowMovDev.ena == 0 && gShowPicDev.ena == 0)
		{
			TSK_GO_SetColorKeyFlag(1);
			TSK_SCENE_OpenWinAndAni();
		}else{
			TSK_GO_SetColorKeyFlag(0);
			TSK_SCENE_CloseWinAndAni();
			usleep(500000);			
		}
	}	
	return WV_SOK;	
}

/******************************************************************************

WV_S32 TSK_USB_ShowPic();

******************************************************************************/
WV_S32 TSK_USB_ShowPic()
{

			
	return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_USB_ShowMov();

******************************************************************************/
WV_S32 TSK_USB_ShowMov()
{
	return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_USB_ShowMov();

******************************************************************************/
WV_S32 TSK_USB_ShowMovEnd()
{	
	return gShowMovDev.playEnd;
}

/******************************************************************************

WV_S32 TSK_USB_ShowPicProc(void *prm);

******************************************************************************/
void* TSK_USB_ShowPicProc(void *prm)
{

    //DIR    *dir;
    WV_S8 filePath[TSK_USBSHOW_NAME_MAXLEN+20];
	WV_S32 i;
	TSK_USB_SHOW_E    * pDev; 
	pDev = (TSK_USB_SHOW_E *)prm;
	pDev-> open  = 1;
	pDev-> close  = 0;  
	
	while(pDev-> open== 1)
	{
		if((TSK_Mobile_GetShowFlag() & 0x1) != 0 )
		{
			sleep(1);
			continue;
		}
		if(	pDev->ena == 1)
		{
			
			TSK_GO_GetUsb_PicAndMov_Pos(&gShowMovDev.showPos,&gShowPicDev.showPos);
			if(pDev->showPos.startX == 0 && pDev->showPos.startY == 0 && pDev->showPos.endX == 0 && pDev->showPos.endY == 0)
			{
				sleep(1);
				continue;
			}

			if(pDev->num != 1)
			{
				if((TSK_Mobile_GetShowFlag() & 0x1) == 0)
				{
					TSK_GO_DecClose(0);
				}
			}	
			for(i=0;i<pDev->num;i++)
			{
				if((TSK_Mobile_GetShowFlag() & 0x1) == 0)
				{
					memcpy(filePath,&pDev->pName[i*TSK_USBSHOW_NAME_MAXLEN],TSK_USBSHOW_NAME_MAXLEN);
					TSK_GO_SetColorKeyFlag(0);
					TSK_GO_DecOpen(0,filePath, pDev->showPos.startX,pDev->showPos.startY,pDev->showPos.endX-pDev->showPos.startX,pDev->showPos.endY-pDev->showPos.startY);				
					sleep(8);// 8秒播放一张图片						
				}
				if(pDev->num != 1)
				{
					if((TSK_Mobile_GetShowFlag() & 0x1) == 0)
					{	if(pDev->ena == 1 )
						{
							TSK_GO_DecClose(0);
						}
					}
				}		
			}
		}
		sleep(1);
	}
	pDev-> open  = 0;
	pDev-> close = 1;
	return WV_SOK;	
}

/******************************************************************************

void* TSK_USB_ShowMovProc(void *prm)

******************************************************************************/
void* TSK_USB_ShowMovProc(void *prm)
{

    WV_S8 filePath[TSK_USBSHOW_NAME_MAXLEN+20];
	WV_S32 i=0,volume;
	TSK_USB_SHOW_E    * pDev; 
	pDev = (TSK_USB_SHOW_E *)prm;
	pDev-> open  = 1;
	pDev-> close  = 0;  
	
	while(pDev-> open== 1)
	{
		if((TSK_Mobile_GetShowFlag() & 0x2) != 0  )
		{
			sleep(1);
			continue;
		}
		if(	pDev->ena == 1)
		{
				TSK_GO_GetUsb_PicAndMov_Pos(&gShowMovDev.showPos,&gShowPicDev.showPos);
				if(pDev->showPos.startX == 0 && pDev->showPos.startY == 0 && pDev->showPos.endX == 0 && pDev->showPos.endY == 0)
				{
					sleep(1);
					continue;
				}
				//printf(" mov num[%d],player 2 mov name[%d][%s]\n",pDev->num,i,&pDev->pName[i*TSK_USBSHOW_NAME_MAXLEN]);
				memcpy(filePath,&pDev->pName[i*TSK_USBSHOW_NAME_MAXLEN],TSK_USBSHOW_NAME_MAXLEN);
				//if the player is creat ,destoryed it.			
				TSK_PLAYER_Destory(TSK_USBSHOW_PLAY_ID);
			
				TSK_PLAYER_SetFile(TSK_USBSHOW_PLAY_ID,filePath);
				TSK_PLAYER_setWin(TSK_USBSHOW_PLAY_ID,pDev->showPos.startX,pDev->showPos.startY,pDev->showPos.endX-pDev->showPos.startX,pDev->showPos.endY-pDev->showPos.startY);
				TSK_PLAYER_Creat(TSK_USBSHOW_PLAY_ID);
				
				TSK_PLAYER_GetVolume(&volume,TSK_USBSHOW_PLAY_ID);
				TSK_PLAYER_SetVolume(volume,TSK_USBSHOW_PLAY_ID);
				while(TSK_PLAYER_GetUsbPlayNext() == 0)
				{
					if(pDev->ena == 0)
					{	
						TSK_PLAYER_setWin(TSK_USBSHOW_PLAY_ID,0,0,0,0);			
						i=0;
						break;				
					}
				
					if((TSK_Mobile_GetShowFlag() & 0x2) != 0  )
					{
						break;
					}
					sleep(1);
				}
				if((TSK_Mobile_GetShowFlag() & 0x2) == 0)
				{
					TSK_PLAYER_Destory(TSK_USBSHOW_PLAY_ID);

					if(i < (pDev->num-1) ){

						i++;
					}else{
						i=0;
					}
					TSK_PLAYER_SetUsbPlayNext(0,TSK_USBSHOW_PLAY_ID);
				}
		}else{
			i=0;
		}

		pDev->playEnd = 1;
		sleep(1);
	}
	pDev-> open  = 0;
	pDev-> close = 1;
	return WV_SOK;	
}
/******************************************************************************

WV_S32 TSK_USB_ShowOpen();

******************************************************************************/
WV_S32 TSK_USB_ShowOpen()
{

	memset(&gShowPicDev,0,sizeof(gShowPicDev));
	gShowPicDev.pName = (WV_S8 *)malloc(TSK_USBSHOW_NAME_MAXLEN*TSK_USBSHOW_NAME_MAXNUM+128);
	memset(gShowPicDev.pName,0,TSK_USBSHOW_NAME_MAXLEN*TSK_USBSHOW_NAME_MAXNUM+128);

	//malloc mov 
	memset(&gShowMovDev,0,sizeof(gShowMovDev));
	gShowMovDev.pName =(WV_S8 *)malloc(TSK_USBSHOW_NAME_MAXLEN*TSK_USBSHOW_NAME_MAXNUM+128);
	memset(gShowMovDev.pName,0,TSK_USBSHOW_NAME_MAXLEN*TSK_USBSHOW_NAME_MAXNUM+128);
 
	TSK_GO_GetUsb_PicAndMov_Pos(&gShowMovDev.showPos,&gShowPicDev.showPos);
	WV_CHECK_RET( WV_THR_Create(&gShowPicDev.thrHndl, TSK_USB_ShowPicProc , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gShowPicDev));
	WV_CHECK_RET( WV_THR_Create(&gShowMovDev.thrHndl, TSK_USB_ShowMovProc , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gShowMovDev));
 
	return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_USB_ShowClose();

******************************************************************************/
WV_S32 TSK_USB_ShowClose()
{

	if(gShowPicDev.open == 1)
	{
		gShowPicDev.open = 0;
		WV_CHECK_RET( WV_THR_Destroy(&gShowPicDev.thrHndl));	
	} 
	free(gShowPicDev.pName);

	if(gShowMovDev.open == 1)
	{
		gShowMovDev.open = 0;
		WV_CHECK_RET( WV_THR_Destroy(&gShowMovDev.thrHndl));	
	} 
	free(gShowMovDev.pName);

	return WV_SOK;
}

