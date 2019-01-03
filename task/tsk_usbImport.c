/*************************************************************************


u盘导入删除加密视频


*************************************************************************/
#include "wv_common.h" 
#include "tsk_conf.h"
#define TSK_USBIMPORT_SONGCLASS_SRC_FILE   "/usb/update/leishiConf.txt"
#define TSK_USBIMPORT_SONGCLASS_DES_FILE   "/opt/app/env/netContrl.txt"
 
#define TSK_USBIMPORT_MOVCONF_SRC_FILE     "/usb/update/VideoConf.DAT"
#define TSK_USBIMPORT_MOVCONF_DES_FILE     "/opt/app/env/mov1.ini"

#define TSK_USBIMPORT_MOV_SRC_DIR         "/usb/update/video/"
#define TSK_USBIMPORT_MOV_DES_DIR         "/opt/app/mov/"

#define TSK_USBIMPORT_UPDATE_FILE     "/usb/update/upVideo.txt"
#define TSK_USBIMPORT_UPDATE_LOCAL_FILE     "/opt/app/upVideo.txt"
#define TSK_USBIMPORT_LEISHI_DB_SRC  "/usb/update/leishi.db"
#define TSK_USBIMPORT_LEISHI_DB_DES  "/opt/app/env/leishi.db"
#define TSK_USBIMPORT_SHIYI_DB_SRC  "/usb/update/shiyi.db"
#define TSK_USBIMPORT_SHIYI_DB_DES  "/opt/app/env/shiyi.db"

#define TSK_USBIMPORT_CP_FILE_MAXLEN 1024*500
/******************************************************************************

WV_S32  TSK_FILE_CpyMovDecrypt(WV_S8 *filenameScr,WV_S8 *filenameDst);
//导入背景视频 暂时不用，使用TSK_USBIMPROT_MovFIle（）导入加密视频
******************************************************************************/
WV_S32  TSK_FILE_CpyMovDecrypt(WV_S8 *filenameScr,WV_S8 *filenameDst)
{  

	WV_S32 ret;
	WV_S32 readlen,writeLen,fileLen,rLen,wLen,lenAvailM;
	FILE    *fpScr = NULL ,*fpDst = NULL; 
	WV_S8  buf[TSK_USBIMPORT_CP_FILE_MAXLEN]; 
	ret = WV_FILE_Access(filenameScr);
	if(ret != 0)
		{ 
		WV_ERROR("  %s not exist!! \r\n",filenameScr); 
		return WV_EFAIL;
		}
	fileLen = 0; 
	ret = WV_FILE_GetLen(filenameScr,&fileLen);
	if(ret != WV_SOK)
		{  
		WV_ERROR("  %s not _IFREG!! \r\n",filenameScr);  
		return WV_EFAIL;
		}
	if(fileLen <= 0)
		{
		WV_ERROR("  file %s len is erro [%d]!!\r\n",filenameScr,fileLen); 
		return WV_EFAIL;  
		}	

	ret = WV_FILE_GetAvail(filenameScr,&lenAvailM);		
	if(ret != 0)
	{  
		return WV_EFAIL;
	} 
	if(fileLen/1000000 >= lenAvailM)
	{
		WV_ERROR(" there is not enough disk space  %d[%d] \r\n",fileLen/1000000,lenAvailM); 
		return WV_EFAIL; 
	}
	// 
	fpScr = fopen(filenameScr, "rb");
	if(fpScr == NULL)
		{ 
		WV_ERROR(" open %s rb ERROR!! [%d]\r\n",filenameScr,errno); 
		return WV_EFAIL; 
		} 

	ret = WV_FILE_Access(filenameDst);
	if(ret == 0)
		{  
		WV_FILE_Remove(filenameDst);
		}       

	fpDst = fopen(filenameDst, "wb");
	if(fpDst == NULL)
	{ 
		WV_ERROR(" open  %s wb ERROR!! [%d]\r\n",filenameDst,errno); 
		fclose(fpScr);
		return WV_EFAIL; 
	} 

	readlen =0;
	writeLen = 0;

	readlen = fread(buf, sizeof(WV_S8),1024,fpScr);
	if(readlen != 1024)
	{
		WV_ERROR("cp mov file %s error ,len < 1024\n ",filenameScr);
		fclose(fpDst);
		fclose(fpScr);
	
		remove(filenameDst);
		return WV_EFAIL;		
	}
	while(writeLen < (fileLen-1024))

	{     
		rLen = fread(buf, sizeof(WV_S8),TSK_USBIMPORT_CP_FILE_MAXLEN ,fpScr);
		if(rLen == 0)
			{
			WV_ERROR(" fread  %s  ERROR!! [%d]\r\n",filenameScr,errno);  
			break;  
			}
		readlen +=rLen;
		wLen = fwrite(buf, sizeof(WV_S8),rLen,fpDst);
		if(wLen != rLen)
			{
			WV_ERROR(" fwrite  %s  ERROR!! [%d]\r\n",filenameDst,errno);  
			break;  
			} 
		
		writeLen += wLen;   
	}
	fclose(fpDst);
	fclose(fpScr);
	sync();
	//
	struct stat statBuf; 
	ret = stat(filenameScr,&statBuf);
	if(ret != 0)
		{ 
		WV_ERROR(" %s GET mode !![ %d] \r\n",filenameScr,errno); 
		return WV_EFAIL; 
		}   
	ret = chmod(filenameDst,statBuf.st_mode);
	if(ret != 0)
		{ 
		WV_ERROR(" %s SET mode  !![ %d] \r\n",filenameDst,errno); 
		return WV_EFAIL; 
		}
	WV_printf("<<<<%s read %d[%d]  %s write %d>>>\r\n",filenameScr,readlen,fileLen-1024,filenameDst,writeLen); 
	if(writeLen == (fileLen-1024))  return WV_SOK;
	return WV_EFAIL;
}

//优盘导入视频以及配置
/******************************************************************************

WV_S32 TSK_USBIMPORT_SongClassFile();
//导入歌曲类别文件
******************************************************************************/
WV_S32 TSK_USBIMPORT_SongClassFile()
{
	
	WV_S32 ret=-1;
	ret=WV_FILE_Cpy(TSK_USBIMPORT_SONGCLASS_SRC_FILE,TSK_USBIMPORT_SONGCLASS_DES_FILE);

	if(ret != WV_SOK)
	{
		WV_printf("cp %s to %s error \n",TSK_USBIMPORT_SONGCLASS_SRC_FILE,TSK_USBIMPORT_SONGCLASS_DES_FILE);
	}else{
		WV_printf("cp %s to %s ok \n",TSK_USBIMPORT_SONGCLASS_SRC_FILE,TSK_USBIMPORT_SONGCLASS_DES_FILE);
	}
	return ret;
}
/******************************************************************************

WV_S32 TSK_USBIMPROT_MovFIle();
//导入背景视频
******************************************************************************/
WV_S32 TSK_USBIMPROT_MovFIle()
{
	WV_S32 ret=-1;
	ret = TSK_CONF_cpVideoToLocal(TSK_USBIMPORT_MOV_SRC_DIR ,TSK_USBIMPORT_MOV_DES_DIR);

	return ret;
}

/******************************************************************************

WV_S32 TSK_USBIMPROT_MovConf();
//导入视频配置文件（关联歌曲类别）
******************************************************************************/
#if 0
WV_S32 TSK_USBIMPROT_MovConf()
{
	WV_S32 ret=-1;
	ret=WV_FILE_Cpy(TSK_USBIMPORT_MOVCONF_SRC_FILE,TSK_USBIMPORT_MOVCONF_DES_FILE);
	if(ret != WV_SOK)
	{
		WV_printf("cp %s to %s error \n",TSK_USBIMPORT_MOVCONF_SRC_FILE,TSK_USBIMPORT_MOVCONF_DES_FILE);
	}else{
		WV_printf("cp %s to %s ok \n",TSK_USBIMPORT_MOVCONF_SRC_FILE,TSK_USBIMPORT_MOVCONF_DES_FILE);
	}	
	TSK_CONF_MovGetConf();
	return ret;
}
#endif

/********************************************************
解析命令 
WV_S32 TSK_USBIMPROT_ParseConfFile(const WV_S8 *pCmd,WV_S32 cmdLen,WV_S32 (*pCmdFun)(WV_S8 * pBuf))

*********************************************************/ 
WV_S32 TSK_USBIMPROT_ParseConfFile(const WV_S8 *pCmd,WV_S32 cmdLen,WV_S32 (*pCmdFun)(WV_S8 * pBuf))
{
	
	WV_S8 buf[1024];
	WV_S32 i,ret;
	ret = access(TSK_USBIMPORT_UPDATE_LOCAL_FILE,W_OK);
	if(ret != 0){
		WV_printf("access file [%s] error\n",TSK_USBIMPORT_UPDATE_LOCAL_FILE);
		return -1;
	}

	FILE *fp;
	//open netControl.dat
	fp = fopen(TSK_USBIMPORT_UPDATE_LOCAL_FILE,"rb+");
	if(fp == NULL)
	{
		WV_printf("fopen file [%s] error\n",TSK_USBIMPORT_UPDATE_LOCAL_FILE);
		return -1;
	}


	while(fgets(buf,sizeof(buf),fp))
	{
		if(buf[0] == '#')
			continue;
		if(buf[0] == '<')
		{
			if(strncmp(buf,pCmd,cmdLen) == 0)
			{
				//printf("buf=%s \n",buf);
				while(fgets(buf,sizeof(buf),fp))
				{
					if(buf[0] == '#')
						continue;
					if(buf[0] == '<')
						goto end;
					for(i=0;i<1024;i++)
					{
						if(buf[i] == 0x0d || buf[i] == 0x0a)
						{
							buf[i] = 0x0a;
							break;				
						}
					}
					pCmdFun(buf);
				}
				break;
			} 
			
		}

	}

end:
	fclose(fp);
	return WV_SOK;	
}

/******************************************************************************

WV_S32 TSK_USBIMPROT_AnalyzeDeletMovCmd(WV_S8 *buf);

******************************************************************************/
WV_S32 TSK_USBIMPROT_AnalyzeDeletMovCmd(WV_S8 *buf)
{
	WV_S32 nameMaxLen = 256+8;
	WV_U8 temp[256]={0};
	WV_S32 nameLen=0;
	WV_S8 *p=NULL;
	printf(" get TSK_USBIMPROT_AnalyzeDeletMovCmd(WV_S8 *buf) cmd \n");
	
	WV_S32 i,j;
	if(strlen(buf) > nameMaxLen)
	{
		nameMaxLen = strlen(buf);	
	}
	if(buf[0] != 'r' && buf[1] != 'm') return WV_EFAIL;

	for(i=0;i<nameMaxLen-2;i++)
	{
		
		if(buf[i] == '|')
		{
			p=&buf[i +1];
			break;
		}
	
	}
	if(p == NULL) return WV_EFAIL;
	for(j=0;j<nameMaxLen-i;j++)
	{
		if(p[j] == '|')
		{
			nameLen = j;
			break;
		}
		
		temp[j] = p[j];
	}
	if(nameLen == 0 || nameLen >=256) return WV_EFAIL;

	WV_S8 name[256+8]={0};
	sprintf(name,"%s?",temp);

	TSK_CONF_DeleteMov((WV_U8*)name,nameLen+1);
	return WV_SOK;	
}

/******************************************************************************

WV_S32 TSK_USBIMPROT_UpdateConf();
拷贝歌曲配置文件到本地：配置文件包括要删除的视频名称
******************************************************************************/
WV_S32 TSK_USBIMPROT_UpdateConf()
{
	WV_S32 ret= -1;
	ret = WV_FILE_Cpy(TSK_USBIMPORT_UPDATE_FILE,TSK_USBIMPORT_UPDATE_LOCAL_FILE);
	
	if(ret != 0 ) return WV_EFAIL;

	TSK_USBIMPROT_ParseConfFile("<RemoveMov>",11,TSK_USBIMPROT_AnalyzeDeletMovCmd);
	remove(TSK_USBIMPORT_UPDATE_FILE);
	return ret;
}
/******************************************************************************

WV_S32 TSK_USBIMPROT_UpdateSongDB();
//升级歌曲类别的数据库
******************************************************************************/
WV_S32 TSK_USBIMPROT_UpdateSongDB()
{
	WV_S32 ret = -1;
	WV_printf("cp %s to %s \n",TSK_USBIMPORT_LEISHI_DB_SRC,TSK_USBIMPORT_LEISHI_DB_DES);
	ret = WV_FILE_Cpy(TSK_USBIMPORT_LEISHI_DB_SRC,TSK_USBIMPORT_LEISHI_DB_DES);
	WV_printf("cp %s to %s \n",TSK_USBIMPORT_LEISHI_DB_SRC,TSK_USBIMPORT_LEISHI_DB_DES);
	ret |= WV_FILE_Cpy(TSK_USBIMPORT_SHIYI_DB_SRC,TSK_USBIMPORT_SHIYI_DB_DES);

	return ret;

}

/******************************************************************************

WV_S32 TSK_USBIMPROT_Proc();

******************************************************************************/
WV_S32 TSK_USBIMPROT_Proc()
{
	
	if(access(TSK_USBIMPORT_UPDATE_FILE,F_OK) != 0 )
	{
		WV_printf("there is not need to update video info\n");
		return WV_EFAIL;
	}
	WV_printf("start to  update video info\n");


	TSK_USBIMPORT_SongClassFile();
	//ret |= TSK_USBIMPROT_MovConf();
	TSK_USBIMPROT_UpdateConf();	
	TSK_USBIMPROT_UpdateSongDB();
	TSK_USBIMPROT_MovFIle();
	remove(TSK_USBIMPORT_UPDATE_FILE);
	return WV_SOK;
}
