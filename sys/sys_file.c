/******************************************************************************
  File Name     : sys_file.c
  Version       : 
  Author        : zhaodehua
  Created       : 
  Description   : 保存配置文件等
  History       :
  1.Date        : 2018-11-15
    Author      :
    Modification: Created file

*******************************************************************************/
#include "sys_file.h"

#define FILENAME_UART_CONF "./env/sceneCmd.dat"
#define FILENAME_NET_CONF ""
#define FILENAME_LEISHI_DB "./env/leishi.db"
#define FILENAME_SHIYI_DB "./env/shiyi.db"
/****************************************************************************************

WV_S32 SYS_FILE_GetConfFileWV_U8 *pData,WV_S32 len,WV_U8 cmd,WV_U16 arg)
//导入配置文件
cmd 0：串口配置文件，
cmd 1：网络配置文件(点歌机) 
cmd 2:导入点歌机数据库  arg==0 leishi.db ;arg == 1 shiyi.db;
****************************************************************************************/
WV_S32 SYS_FILE_GetConfFile(WV_U8 *pData,WV_S32 len,WV_U8 cmd,WV_U16 arg)
{
    WV_printf("import conf file ...\n");
    WV_S32 ret = -1;
    switch (cmd){
        case 0:
            ret = SYS_FILE_GetConfFileToDest(pData,len,FILENAME_UART_CONF);
            break;
        case 1:
            ret = SYS_FILE_GetConfFileToDest(pData,len,FILENAME_NET_CONF);
            break;
        case 2:
            if(arg == 0){
                ret = SYS_FILE_GetConfFileToDest(pData,len,FILENAME_LEISHI_DB);
            }else if(arg == 1){
                ret = SYS_FILE_GetConfFileToDest(pData,len,FILENAME_SHIYI_DB);
            }
            break;
        default:
            break;
    }

    return ret;
	
}

/****************************************************************************************

WV_S32 SYS_FILE_GetConfFileToDest(WV_U8 *pData,WV_S32 len,WV_S8 *pDestFileName)

****************************************************************************************/
WV_S32 SYS_FILE_GetConfFileToDest(WV_U8 *pData,WV_S32 len,WV_S8 *pDestFileName)
{
	FILE *fp;
	fp = fopen(pDestFileName,"wb+");
	if(fp == NULL)
	{
		printf("fopen %s error \n",pDestFileName); 
		return -1;
	}
	WV_S32 writeLen;
	writeLen =fwrite(pData,1,len,fp);
	if(writeLen != len)
	{
		printf("fwrite %s error \n",pDestFileName);
		fclose(fp);	
		return -1;
	}
	
	fclose(fp);
	return 0;	
	
}