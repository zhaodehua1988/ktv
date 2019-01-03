#include "svr_control_yinchuang.h"

#if 1


//#define SVR_CONTROL_YinChuang_DB_FILE_PATH      "./env/YinChuang.db"
#define SVR_CONTROL_YINCHUANG_CMD_MAX_NUM 16
//#define SVR_CONTROL_YinChuang_DB_TABLE_NAME     "SongClass"
//#define SVR_CONTROL_YinChuang_DB_SONGCLASS_NAME       "Class"
#define SVR_CONTROL_YINCHUANG_MOVTYPE_NUM 64
#define SVR_CONTROL_YINCHUANG_CMD_MAX_LEN  64
/********************control cmd ***************************/
#define 	SVR_CONTROL_YINCHUANG_STARTING_CMD           0x00        //设备开机 
#define 	SVR_CONTROL_YINCHUANG_STANDBY_CMD            0x01        //设备待机
#define 	SVR_CONTROL_YINCHUANG_OPEN_PROJECTOR_CMD     0x02        //投影仪开
#define 	SVR_CONTROL_YINCHUANG_CLOSE_PROJECTOR_CMD    0x03        //投影仪关
#define 	SVR_CONTROL_YINCHUANG_GET_MAC_CMD            0x06        //获取MAC地址
/***********************************************************/

typedef struct SVR_CONTROL_SHIYI_MOVETYPE
{
	WV_U8 typeName[8];
}SVR_CONTROL_YINCHUANG_MOVETYPE;


typedef struct SVR_CONTROL_YINCHUANG_DEVICE_CMD
{	
	WV_U8 CmdLen;
	WV_U8 cmd[SVR_CONTROL_YINCHUANG_CMD_MAX_LEN];
	WV_U8 AckLen;
	WV_U8 ack[SVR_CONTROL_YINCHUANG_CMD_MAX_LEN]; //可以是回复的命令，也可以是控制投影的命令，
}SVR_CONTROL_YINCHUANG_DEVICE_CMD;

typedef struct SVR_CONTROL_YINCHUANG_DEV
{
	WV_U8 movChangeByID;
	SVR_CONTROL_YINCHUANG_MOVETYPE movType[SVR_CONTROL_YINCHUANG_MOVTYPE_NUM];

	SVR_CONTROL_YINCHUANG_DEVICE_CMD controlDev[SVR_CONTROL_YINCHUANG_CMD_MAX_NUM];
}SVR_CONTROL_YINCHUANG_DEV;

SVR_CONTROL_YINCHUANG_DEV gSvrYinChuangDev;


/********************************************************

WV_S32 SVR_CONTROL_YinChuang_AnalyzeMovClassCmd(WV_S8 *buf);

*********************************************************/ 
WV_S32 SVR_CONTROL_YinChuang_AnalyzeMovClassCmd(WV_S8 *buf)
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
	if(id >= SVR_CONTROL_YINCHUANG_MOVTYPE_NUM)
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
			gSvrYinChuangDev.movType[id].typeName[j]=buf[i];
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
*
*WV_S32 SVR_CONTROL_YinChuang_ControlCmd(WV_S8 *buf)
*解析网络待机命令
*
*********************************************************/ 
WV_S32 SVR_CONTROL_YinChuang_ControlCmd(WV_S8 *buf)
{
	//命令格式如下：
	//cmd like :"00 |5a 5a 5a a5 00|"; #网络待机命令
	//cmd like :"01 |5a 5a 5a a5 01|"; #网络开机命令
	//cmd like :"02 |5a 5a 5a a5 02|7e 30 30 30 30 20 31 0d|"; #网络开投影
	//cmd like :"03 |5a 5a 5a a5 03|7e 30 30 30 30 20 30 0d|"; #网络关投影
	//cmd like :"06 |5a 5a 5a a5 04|"; #查询mac地址

	int a,id,i,k=0,j=0;
	char temp[3];
	memset(temp,0,sizeof(temp));
	//************get control device cmd id ****************
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
	if(id >= SVR_CONTROL_YINCHUANG_CMD_MAX_NUM)
	{
		//printf("get cmd error \n");
		return -1;
	}

//*****************get control device cmd  ********************


	memset(temp,0,sizeof(temp));

	j=0;
	for(i=i+1;i<strlen(buf);i++)
	{
		if(buf[i] == '#')
		{
			//gControlDev.controlDev[id].CmdLen = k;
			break;
		}
		
		if(buf[i] == '|')
		{
			gSvrYinChuangDev.controlDev[id].CmdLen = k;
			/*			
			k = 0;
			
			if(id == 2 || id == 3)//id为2，开投影，id为3关投影，需要后面跟开关命令
			{
				for(i=i+1;i<strlen(buf);i++)
				{
					if(buf[i] == '#')
					{
						//gControlDev.controlDev[id].AckLen = k;
						break;
					}			
					if(buf[i] == '|')
					{
						gSvrYinChuangDev.controlDev[id].AckLen = k;
						break;
					}
					
					if(buf[i] != ' ' && buf[i] != 0x0a )
					{
						temp[j]=buf[i];
						j++;
						if(j==2){
							j=0;
							sscanf(temp,"%x",&a);
							gSvrYinChuangDev.controlDev[id].ack[k] = (WV_U8)a;//get swCmd
							k++;
							memset(temp,0,sizeof(temp));	
						}
					}
				}
			}//end if(id == 2 || id == 3)
			*/			
			break;

		}
			
		if(buf[i] != ' ' && buf[i] != 0x0a )
		{
			temp[j]=buf[i];
			j++;
			if(j==2){
				j=0;
				sscanf(temp,"%x",&a);
				gSvrYinChuangDev.controlDev[id].cmd[k] = (WV_U8)a;//get swCmd
				k++;
				memset(temp,0,sizeof(temp));	
			}
		}
	
		if(i==strlen(buf)-1)
		{
			gSvrYinChuangDev.controlDev[id].CmdLen = k;
		}

	}

	return WV_SOK;	
}

/****************************************************************************

WV_S32 SVR_CONTROL_YinChuang_controlDev(WV_S8 *pData,WV_S32 len)
//控制本机和外部设备(投影仪)命令格式(16进制)：5a 5a 5a a5 00

****************************************************************************/
WV_S32 SVR_CONTROL_YinChuang_controlDev(WV_S8 *pData,WV_S32 len)
{

	WV_S32 i,j;
	WV_U8 mac[6];
	WV_S32 ret = -1;
	for(i=SVR_CONTROL_YINCHUANG_CMD_MAX_NUM;i>=0;i--) 
	{
	//printf("*************i = %d *************\n",i);
		if(gSvrYinChuangDev.controlDev[i].CmdLen == len)
		{
			for(j=0;j<len;j++)
			{
				if(gSvrYinChuangDev.controlDev[i].cmd[j] != pData[j])
				{
					//WV_printf("\nget scene cmd \n");
					break;
				}
				//WV_printf("i=%d,j=%d[%02x] \n",i,j,pData[j]);
			}

			if(j==len )
			{			
				//printf("*************i = %d *************\n",i);
				switch(i)
				{
					case SVR_CONTROL_YINCHUANG_STARTING_CMD: 
						SVR_CONTROL_printf("**********get cmd: starting up dev ***************\n");
						TSK_SCENE_StartingUP(TSK_SCENE_TYPE_NETDATA);
						ret = 0;
						break;
					case SVR_CONTROL_YINCHUANG_STANDBY_CMD:
						SVR_CONTROL_printf("**********get cmd: standby dev***************\n");
						TSK_SCENE_Standby(TSK_SCENE_TYPE_NETDATA);
						ret = 0;
						break;
					case SVR_CONTROL_YINCHUANG_OPEN_PROJECTOR_CMD:
						NET_UART_ProjectorCmd(1);	
						SVR_CONTROL_printf("**********get cmd: open external  projector 1***************\n");
						ret = 0;
						break;
					case SVR_CONTROL_YINCHUANG_CLOSE_PROJECTOR_CMD:
						NET_UART_ProjectorCmd(0);
						SVR_CONTROL_printf("**********get cmd: close external  projector 1 ***************\n");
						ret = 0;
						break;
					case SVR_CONTROL_YINCHUANG_GET_MAC_CMD:
						SYS_IP_getMacInt(mac);
						SVR_CONTROL_Send(mac,sizeof(mac));						
						SVR_CONTROL_printf("**********get cmd: get mac addr ***************\n");
						ret = 0;						
					default:
						SVR_CONTROL_printf("**********get cmd: error ***************\n");
						break;	

				}
			}//endif


		}
	
	}

	return ret;
}



/******************************************************************************

WV_S32 SVR_CONTROL_YinChuang_MapSongType() ;
3=好声音
4=我是歌手
5=全能星战
6=好歌曲
7=最美和声
8=合唱
11=的士高
12=革命
13=戏曲
16=流行
18=儿歌
19=民歌
20=试音
22=喜庆
23=舞曲
25=军歌
27=DJ
28=演示短片
29=高清歌曲
33=动画卡通
37=相声小品
40=影视歌曲
******************************************************************************/  
WV_S32 SVR_CONTROL_YinChuang_MapSongType(WV_S32 id,WV_U8 *pOut)  
{ 
	WV_S8 nameMap[16]={0};
	switch(id){
		case 3:
			sprintf(nameMap,"好声音");
			break;		
		case 4:
			sprintf(nameMap,"我是歌手");
			break;		
		case 5:
			sprintf(nameMap,"全能星战");
			break;		
		case 6:
			sprintf(nameMap,"好歌曲");
			break;		
		case 7:
			sprintf(nameMap,"最美和声");
			break;		
		case 8:
			sprintf(nameMap,"合唱");
			break;		
		case 11:
			sprintf(nameMap,"的士高");
			break;		
		case 12:
			sprintf(nameMap,"革命");
			break;		
		case 13:
			sprintf(nameMap,"戏曲");
			break;		
		case 16:
			sprintf(nameMap,"流行");
			break;		
		case 18:
			sprintf(nameMap,"儿歌");
			break;		
		case 19:
			sprintf(nameMap,"民歌");
			break;		
		case 20:
			sprintf(nameMap,"试音");
			break;		
		case 22:
			sprintf(nameMap,"喜庆");
			break;		
		case 23:
			sprintf(nameMap,"舞曲");
			break;		
		case 25:
			sprintf(nameMap,"军歌");
			break;		
		case 27:
			sprintf(nameMap,"DJ");
			break;		
		case 28:
			sprintf(nameMap,"演示短片");
			break;		
		case 29:
			sprintf(nameMap,"高清歌曲");
			break;		
		case 33:
			sprintf(nameMap,"动画卡通");
			break;		
		case 37:
			sprintf(nameMap,"相声小品");
			break;		
		case 40:
			sprintf(nameMap,"影视歌曲");
			break;		
		default:
			break;

	}

	if(strlen(nameMap) == 0 ) return WV_EFAIL;

	if(utf8togb2312(nameMap,strlen(nameMap),(WV_S8 *)pOut,8) != 0)
	{
		WV_ERROR(" Map song type is error \n");	
		return WV_EFAIL;
	}

	return WV_SOK;
}

/****************************************************************************

WV_S32 SVR_CONTROL_YinChuang_ChangeMovByCmd(WV_S32 classID)

功能：根据id或者name切换对应歌曲类型的视频
参数：pID是歌曲id(固定7个字节)，pName(最多8个字节)
返回值：成功返回0，不成功返回-1
****************************************************************************/ 
WV_S32 SVR_CONTROL_YinChuang_ChangeMovByCmd(WV_S32 classID)
{

	SVR_CONTROL_KTV_CHANGEMODE_E movChangeMode = SVR_CONTROL_CHANGE_NULL;
	movChangeMode = SVR_CONTROL_GetKtvChangeMode();
	//if id exist ,use id
	if(movChangeMode == SVR_CONTROL_CHANGE_NULL )
	{
		 return WV_EFAIL;
	}
	WV_U8 mapName[9]={0};
	if(SVR_CONTROL_YinChuang_MapSongType(classID,mapName) != 0 ) return WV_EFAIL;
	//根据歌曲类别查找视频
	if(movChangeMode == SVR_CONTROL_CHANGE_MOV )     //1:背景视频随动 ;
	{
		TSK_CONF_changeMovByType((WV_S8 *)mapName);
	}else if(movChangeMode == SVR_CONTROL_CHANGE_SCENE )//2:场景随动
	{
		TSK_CONF_changeSceneByType(mapName);
	}

	return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_CONTROL_YinChuang(WV_S8 *pData,WV_S32 len)
//点播命令码值:YC_SONG|歌曲编号|歌曲名称|歌曲分类|灯光分类
//轮播命令码值:YC_RANDOM|歌曲编号|歌曲名称|歌曲分类|灯光分类
****************************************************************************/
WV_S32 SVR_CONTROL_YinChuang(WV_S8 *pData,WV_S32 len)
{

	//
	if(SVR_CONTROL_YinChuang_controlDev(pData,len) == 0 )
	{
		return WV_SOK;
	}

	if(len < 20)
	{
		return WV_SOK; 
	}

	WV_S32 ret=0;
	WV_S32 i=0;
	WV_S8 *p=pData;
	WV_S32 classID=-1;
	if(strncmp(pData,"YC_SONG",7) != 0 && strncmp(pData,"YC_RANDOM",9) != 0){
		return WV_EFAIL;
	}


    //char *s = "0123456789012345678901234567890";  
    //unsigned char tmp;
	//YC_RANDOM|歌曲编号|歌曲名称|歌曲分类|灯光分类
	for(i=0;i<3;i++)
	{
		p = strchr(p, '|');
		p++;
	    printf("%s\n",p);  		
	}

	  
	sscanf(p,"%d",&classID);
	printf("class ID = %d\n",classID);
	if(classID >50 || classID < 0) return WV_EFAIL;

	ret = SVR_CONTROL_YinChuang_ChangeMovByCmd(classID);
	return ret;
		
}

/****************************************************************************

WV_S32 SVR_CONTROL_YinChuang_Init()

****************************************************************************/
WV_S32 SVR_CONTROL_YinChuang_Init()
{
	memset(&gSvrYinChuangDev,0,sizeof(gSvrYinChuangDev));
	return WV_SOK;
}

#endif
