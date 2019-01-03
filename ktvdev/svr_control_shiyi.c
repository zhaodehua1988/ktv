#if 1
#include "svr_control_shiyi.h"


#define SVR_CONTROL_SHIYI_MOVTYPE_NUM 64
#define SVR_CONTROL_SHIYI_CHANGECMD_MAX_LEN 16
#define SVR_CONTROL_SWITCH_CMD_MAX_NUN 64

//仅仅用于视易点歌机

typedef struct SVR_CONTROL_SHIYI_MOVETYPE
{
	WV_U8 typeName[8];
}SVR_CONTROL_SHIYI_MOVETYPE;

typedef struct SVR_CONTROL_SHIYI_SWITCH_CMD
{
	WV_U8 CmdLen;
	WV_U8 ScenceCmd[SVR_CONTROL_SHIYI_CHANGECMD_MAX_LEN];

}SVR_CONTROL_SHIYI_SWITCH_CMD;

typedef struct SVR_CONTROL_SHIYI_DEV
{
	SVR_CONTROL_SHIYI_MOVETYPE movType[SVR_CONTROL_SHIYI_MOVTYPE_NUM];	
	SVR_CONTROL_SHIYI_SWITCH_CMD swCmd[SVR_CONTROL_SWITCH_CMD_MAX_NUN];
}SVR_CONTROL_SHIYI_DEV;

static SVR_CONTROL_SHIYI_DEV gSvrShiyiDev;
/********************************************************

WV_S32 SVR_CONTROL_ShiYi_AnalyzeMovClassCmd(WV_S8 *buf);

*********************************************************/ 
WV_S32 SVR_CONTROL_ShiYi_AnalyzeMovClassCmd(WV_S8 *buf)
{
	
	
	int a,id,i,j=0;
	char temp[3];
	memset(temp,0,sizeof(temp));
	//************get  id****************
	for(i=0;i<strlen(buf);i++)
	{
		if(buf[i] != ' ')
		{
			if(buf[i] == '|') break;
	
			temp[j]=buf[i];
			j++;
			if(j>=3) return -1;
		}
	}
	sscanf(temp,"%x",&a);
	id = a;
	if(id >= SVR_CONTROL_SHIYI_MOVTYPE_NUM)
	{
		return -1;
	}
	
//*****************get scene cmd ********************
	memset(temp,0,sizeof(temp));

	j=0;
	for(i=i+1;i<strlen(buf);i++)
	{
		if(buf[i] == '|')
		{
			
			break;
		}
				
		if(buf[i] != ' ' && buf[i] != 0x0a )
		{
			gSvrShiyiDev.movType[id].typeName[j]=buf[i];
			j++;
			if(j==8){
				j=0;
				break;
			}
		}
	
	}

	//printf("get buf[%s],get name[%d][%s]\n",buf,id,gSvrShiyiDev.movType[id].typeName);
	return 0;

}
/********************************************************

WV_S32 SVR_CONTROL_ShiYi_AnalyzeSwitchCmd(WV_S8 *buf)
::仅仅用于视易点歌机

*********************************************************/ 
WV_S32 SVR_CONTROL_ShiYi_AnalyzeSwitchCmd(WV_S8 *buf)
{
	
	//printf("get AnalyzeSwitchCmd %s \n",buf);
	//char buf[128]="0f | F1 02 03 04 05 | 06 07 08 9f";
	int a,id,i,k=0,j=0;
	
	char temp[3];
	memset(temp,0,sizeof(temp));
	//************get scene id****************
	for(i=0;i<strlen(buf);i++)
	{
		if(buf[i] != ' ')
		{
			if(buf[i] == '|') break;
	
			temp[j]=buf[i];
			j++;
			if(j>=3) return -1;
		}
	}
	sscanf(temp,"%x",&a);
	id = a;
	if(id >= SVR_CONTROL_SWITCH_CMD_MAX_NUN)
	{
		//printf("get scene id error \n");
		return -1;
	}
	
//*****************get scene cmd ********************
	memset(temp,0,sizeof(temp));

	j=0;
	for(i=i+1;i<strlen(buf);i++)
	{
		if(buf[i] == '|')
		{
			gSvrShiyiDev.swCmd[id].CmdLen = k;
			break;
		}
				
		if(buf[i] != ' ' && buf[i] != 0x0a )
		{
			temp[j]=buf[i];
			j++;
			if(j==2){
				j=0;
				sscanf(temp,"%x",&a);
				gSvrShiyiDev.swCmd[id].ScenceCmd[k] = (WV_U8)a;//get swCmd
				k++;
				if(k > SVR_CONTROL_SHIYI_CHANGECMD_MAX_LEN) break;
				memset(temp,0,sizeof(temp));

		
			}
		}
	
	}
#if 0
	WV_printf("read shiyi sw cmd : ");
	for(i=0;i<gSvrShiyiDev.swCmd[id].CmdLen;i++)
	{
		printf("%02x ",gSvrShiyiDev.swCmd[id].ScenceCmd[i]);
	}
	printf("\n");
#endif
	return 0;

}	

/****************************************************************************

WV_S32 SVR_CONTROL_ShiYi(WV_S8 *pData,WV_S32 len)
//视易点歌机网络数据格式(16进制)：23 01 01 23

****************************************************************************/
WV_S32 SVR_CONTROL_ShiYi(WV_S8 *pData,WV_S32 len)
{

	WV_U8 movChangeMode;
	movChangeMode = SVR_CONTROL_GetKtvChangeMode();
	//if id exist ,use id
	if(movChangeMode == 0 )
	{
		 return WV_EFAIL;
	}
	if(len >64) return WV_EFAIL;
	WV_S32 i,j;
	for(i=0;i<SVR_CONTROL_SWITCH_CMD_MAX_NUN;i++)
	{
		if(gSvrShiyiDev.swCmd[i].CmdLen == len)
		{
			for(j=0;j<len;j++)
			{
				if(gSvrShiyiDev.swCmd[i].ScenceCmd[j] != pData[j])
				{
					//WV_printf("\nget scene cmd \n");
					break;
				}
			}

			if(j==len )
			{
				//WV_printf("get cmd scene id[%d]\n",i);	
				if(movChangeMode == 1 )      //1:背景视频随动 ;
				{
					TSK_CONF_changeMovByType((WV_S8 *)gSvrShiyiDev.movType[i].typeName);
				}else if(movChangeMode == 2 )//2:场景随动
				{
					TSK_CONF_changeSceneByType(gSvrShiyiDev.movType[i].typeName);
				}
				break;
			}
					
		}
	
	}

	return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_CONTROL_ShiYi(WV_S8 *pData,WV_S32 len)
//视易点歌机网络数据格式(16进制)：23 01 01 23

****************************************************************************/
WV_S32 SVR_CONTROL_ShiYi_Init()
{
	memset(&gSvrShiyiDev,0,sizeof(gSvrShiyiDev));
	return WV_SOK;
}

#endif
