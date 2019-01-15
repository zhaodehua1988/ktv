#include "svr_control_cmd.h"
#include "tsk_scene.h"

#define SVR_CONTROL_DEV_LEISHI       0
#define SVR_CONTROL_DEV_SHIYI        1
#define SVR_CONTROL_DEV_YINCHUANG    2

#define SVR_CONTROL_CMDFILE      "./env/netContrl.dat"

typedef struct SVR_CONTROL_CONF_E
{
	WV_U8 KtvDev;  // 0：雷石 1：视易 2:音创 3:视易新版本 4:雷石新版本
	SVR_CONTROL_KTV_CHANGEMODE_E movChangeMode; //0:关闭 ;1:背景视频随动 ;2:场景随动
	WV_S32 port; //udp端口号
	WV_S32 openDev; //0:ktv控制开关机不生效，1:ktv控制开关机生效
	WV_S32 openProjector; //0：ktv控制开关投影不生效，1:ktv控制开关投影生效
	WV_S32 typeRound;// 根据歌曲类别进行循环播放，1：ena，0disable

}SVR_CONTROL_CONF_E;

static SVR_CONTROL_CONF_E gControlDev;

/********************************************************

WV_S32 SVR_CONTROL_GetKtvChangeMode();

*********************************************************/ 
WV_S32 SVR_CONTROL_GetKtvChangeMode()
{
	SVR_CONTROL_printf("get movChangeMode dev=%d \n",gControlDev.movChangeMode);
	return gControlDev.movChangeMode;
}

/********************************************************

WV_S32 SVR_CONTROL_GetKtvDev();

*********************************************************/ 
WV_S32 SVR_CONTROL_GetKtvDev()
{
	SVR_CONTROL_printf("get ktv dev=%d \n",gControlDev.KtvDev);
	return gControlDev.KtvDev;
}

/********************************************************

WV_S32 SVR_CONTROL_GetKtvNetPort();

*********************************************************/ 
WV_S32 SVR_CONTROL_GetKtvNetPort()
{
	SVR_CONTROL_printf("get Ktv Dev port[%d]\n",gControlDev.port);
	return gControlDev.port;
}
/********************************************************

WV_S32 SVR_CONTROL_GetTypeRound();

*********************************************************/ 
WV_S32 SVR_CONTROL_GetTypeRound()
{
	SVR_CONTROL_printf("get Ktv Dev TypeRound[%d]\n",gControlDev.typeRound);
	return gControlDev.typeRound;
}
/********************************************************

WV_S32 SVR_CONTROL_GetOpenDev();

*********************************************************/ 
WV_S32 SVR_CONTROL_GetOpenDev()
{
	SVR_CONTROL_printf("get Ktv Dev openDev [%d]\n",gControlDev.openDev);
	return gControlDev.openDev;
}

/********************************************************

WV_S32 SVR_CONTROL_GetOpenProjector();

*********************************************************/ 
WV_S32 SVR_CONTROL_GetOpenProjector()
{
	SVR_CONTROL_printf("get Ktv Dev openProjector[%d]\n",gControlDev.openProjector);
	return gControlDev.openProjector;
}

/********************************************************

WV_S32 SVR_CONTROL_GetKTVConf(WV_S8 * buf);

*********************************************************/ 
WV_S32 SVR_CONTROL_GetKTVConf(WV_S8 * buf)
{
	WV_S8 name[64];
	WV_U32 data=0;
	WV_S32 i,j=0;
	memset(name,0,sizeof(name));
	for(i=0;i<64;i++)
	{
		if(buf[i] == '#')
			return 0;
		if(buf[i] == ' ')
			continue;
		if(buf[i] == '=')
			break;
		 name[j] = buf[i];
		j++;
	}
	
	i++;	
	sscanf(&buf[i],"%d",&data);
	data = atoi(&buf[i]);
	if(strcmp(name,"KTVDev") == 0)
	{	
		gControlDev.KtvDev = data;
	}
	else if(strcmp(name,"MovChange") == 0)
	{
		if(data != 1 && data != 2) data = 0;
		gControlDev.movChangeMode = data;
	}else if(strcmp(name,"port") == 0)
	{
		gControlDev.port = data;
	}else if(strcmp(name,"TypeRound") == 0)
	{
		gControlDev.typeRound = data;
	}else if(strcmp(name,"OpenDev") == 0)
	{
		gControlDev.openDev = data;
	}else if(strcmp(name,"OpenProjector") == 0)
	{
		gControlDev.openProjector = data;
	}
	
	SVR_CONTROL_printf("KTVDev = %d ,MovChangeMode = %d \n",gControlDev.KtvDev,gControlDev.movChangeMode);
	return 0;
	
}

/****************************************************************************

WV_S32 SVR_CONTROL_GetConfFileCmd(WV_U8 *pData,WV_S32 len)

****************************************************************************/
WV_S32 SVR_CONTROL_GetConfFileCmd(WV_U8 *pData,WV_S32 len)
{
	FILE *fp;
	//open s_ceneCmd.dat
	fp = fopen(SVR_CONTROL_CMDFILE,"wb+");
	if(fp == NULL)
	{
		SVR_CONTROL_printf("fopen s_ceneCmd.dat error \n"); 
		return -1;
	}
	WV_S32 writeLen;
	writeLen =fwrite(pData,1,len,fp);
	if(writeLen != len)
	{
		SVR_CONTROL_printf("fwrite uart cmd file error \n");
		fclose(fp);	
		return -1;
	}
	
	fclose(fp);

	SVR_CONTROL_ReadConfFileCmd();
	return 0;	
	
}

/********************************************************
解析命令 
WV_S32 SVR_CONTROL_ParseConfFile(const WV_S8 *pCmd,WV_S32 cmdLen,WV_S32 (*pCmdFun)(WV_S8 * pBuf))

*********************************************************/ 
WV_S32 SVR_CONTROL_ParseConfFile(const WV_S8 *pCmd,WV_S32 cmdLen,WV_S32 (*pCmdFun)(WV_S8 * pBuf))
{
	
	WV_S8 buf[1024];
	WV_S32 i,ret;
	ret = access(SVR_CONTROL_CMDFILE,W_OK);
	if(ret != 0){
		SVR_CONTROL_printf("access file [%s] error\n",SVR_CONTROL_CMDFILE);
		return -1;
	}

	FILE *fp;
	//open netControl.dat
	fp = fopen(SVR_CONTROL_CMDFILE,"rb+");
	if(fp == NULL)
	{
		SVR_CONTROL_printf("fopen file [%s] error\n",SVR_CONTROL_CMDFILE);
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

/********************************************************

WV_S32 SVR_CONTROL_ReadConfFileCmd();
//读取配置文件,
*********************************************************/ 
WV_S32 SVR_CONTROL_ReadConfFileCmd()
{

	memset(&gControlDev,0,sizeof(SVR_CONTROL_CONF_E));
	gControlDev.KtvDev = -1;
	SVR_CONTROL_ParseConfFile("<KtvConf>",9,SVR_CONTROL_GetKTVConf);//点歌机配置

	switch(gControlDev.KtvDev)
	{
		case SVR_CONTROL_DEV_LEISHI:
			SVR_CONTROL_ParseConfFile("<ControlDev>",12,SVR_CONTROL_Leishi_ControlCmd);//控制本机和外部设备(投影仪)命令
			break;
		case SVR_CONTROL_DEV_SHIYI:
			SVR_CONTROL_ParseConfFile("<MovClass>",10,SVR_CONTROL_ShiYi_AnalyzeMovClassCmd);//歌曲视频类型
			SVR_CONTROL_ParseConfFile("<SwitchCmd>",11,SVR_CONTROL_ShiYi_AnalyzeSwitchCmd);//解析视易 的切换场景命令
			break;
		case SVR_CONTROL_DEV_YINCHUANG:
		 	SVR_CONTROL_ParseConfFile("<ControlDev>",12,SVR_CONTROL_YinChuang_ControlCmd);
			SVR_CONTROL_ParseConfFile("<MovClass>",10, SVR_CONTROL_YinChuang_AnalyzeMovClassCmd);//歌曲视频类型
			break;
		default:
			break;
	}

	return 0;
}
/********************************************************

WV_S32 SVR_CONTROL_Cmd_Init();

*********************************************************/ 
WV_S32 SVR_CONTROL_Cmd_Init()
{
	WV_S32 ret = -1;

	SVR_CONTROL_LeiShi_Init();	
	SVR_CONTROL_ShiYi_Init();

	ret =  SVR_CONTROL_ReadConfFileCmd();
	if(gControlDev.KtvDev == SVR_CONTROL_DEV_LEISHI) //雷石点歌机，初始化时默认歌曲随机播放
	{
		TSK_SCENE_ChangePlayMode(1);//
	}
	return ret;
}



