#include "net_uart.h"
#include "svr_frame.h"
#include "tsk_go.h"
#include "tsk_scene.h"
#include "svr_udp.h"
#define NET_UART_BUF_MAXLEN 1024
#define NET_UART_CMDFILE  "./env/netContrl.dat"
#define NET_UART_MAX_LEN  256 
#define NET_UART_SCENE_NUM  64
#define NET_UART_PROJECTOR_NUM  6
#define NET_UART_SERVER_PORT  3798
#define NET_UART_CLIENT_PORT  6001
#define NET_UART_CLIENT_IP    "192.168.1.199"

//开关投影命令结构
typedef struct NET_UART_PROJECTOR_CMD_S
{
	
	WV_U8 CmdEna;               //开关闭影命令使能
	WV_U8 OpenCmdLen;           //开投影的命令长度
	WV_U8 OpenCmd[64];          //开投影的命令
	WV_U8 CloseCmdLen;          //关投影的命令长度
	WV_U8 CloseCmd[64];         //关投影的命令
}NET_UART_PROJECTOR_CMD_S;

//切换场景命令结构体
typedef struct NET_UART_SCENE_CMD_S
{
	WV_U8 CmdLen;
	WV_U8 ScenceCmd[64];
	WV_U8 CmdEna; //如果场景没有被切换过，该标志为0,切换完该场景以后置为1,相同命令所有场景都为1时，清除标志位
}NET_UART_SCENE_CMD_S;

typedef struct NET_UART_DEV_E
{
	WV_THR_HNDL_T thrHndl;	
	WV_U32      open;
	WV_U32      close; 
	WV_S32      mSocket; //接收的socket
	WV_S32      cSocket; //发送的socket
	WV_U32      port;
	WV_U8       clientIp[16];
	WV_U32      clientPort;
	struct sockaddr_in svrAddr;
	struct sockaddr_in cliAddr;
	WV_U8       pBuf[NET_UART_BUF_MAXLEN];
	SVR_FRAME_HEAD_E  *pHead;
	NET_UART_SCENE_CMD_S      sceneCmd[NET_UART_SCENE_NUM];          //切换场景命令
	NET_UART_PROJECTOR_CMD_S  projectorCmd[NET_UART_PROJECTOR_NUM];  //开关投影命令
}NET_UART_DEV_E;

static NET_UART_DEV_E* pgNetUartDev;


/****************************************************************************************

WV_S32 NET_UART_SetSceneCmd(WV_U8 *pData,WV_S32 len);

****************************************************************************************/
WV_S32 NET_UART_SetSceneCmd(WV_U8 *pData,WV_S32 len)
{
	FILE *fp;
	//open sceneCmd.dat
	fp = fopen(NET_UART_CMDFILE,"wb+");
	if(fp == NULL)
	{
		NET_UART_printf("fopen sceneCmd.dat error \n"); 
		return -1;
	}
	WV_S32 writeLen;
	writeLen =fwrite(pData,1,len,fp);
	if(writeLen != len)
	{
		NET_UART_printf("fwrite uart cmd file error \n");
		fclose(fp);	
		return -1;
	}
	
	fclose(fp);

	return 0;	
	
}


/********************************************************

WV_S32 NET_UART_GetNetConf(WV_U8 *buf)
配置外部网口转串口设备的ip、端口号和本地端口号
*********************************************************/ 
WV_S32 NET_UART_GetNetConf(WV_U8 *buf)
{
	WV_S8 name[64]={0};
	WV_U32 data=0;
	WV_S32 i,j=0,ret=0;
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
	
	if(strcmp(name,"ClientIp") == 0)
	{	j=0;
		WV_U8 ip[64]={0};
		for(;i<64;i++)
		{
			if(buf[i] == '#')
				break;
			if(buf[i] == ' ')
				continue;
			 ip[j] = buf[i];
			j++;
		}
		if(j == 0){
			 ret = -1;
			return ret;
		}

		sprintf((WV_S8 *)pgNetUartDev->clientIp,"%s",ip);
	
	}
	else if(strcmp(name,"ClientPort") == 0)
	{
		sscanf((WV_S8 *)&buf[i],"%d",&data);
		data = atoi((WV_S8 *)&buf[i]);
		pgNetUartDev->clientPort = data;
		
	}else if(strcmp(name,"ServerPort") == 0){
		sscanf((WV_S8 *)&buf[i],"%d",&data);
		data = atoi((WV_S8 *)&buf[i]);
		pgNetUartDev->port = data;
		
	}
	//printf("********SetSerialConf [%s]=%d *************\n ",name,data);
	return ret;	
}

/********************************************************

WV_S32 NET_UART_GetProjectorConf(WV_S8 *buf)
//投影（projector）开关命令配置
*********************************************************/ 
WV_S32 NET_UART_GetProjectorConf(WV_S8 *buf)
{
	//projectorCmd
	//char buf[128]="0f | F1 02 03 04 05 | 06 07 08 9f";
	int a,id,i,k=0,j=0;
	char temp[3];
	memset(temp,0,sizeof(temp));
	//************get projector id****************
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
	if(id >= NET_UART_PROJECTOR_NUM )
	{
		//printf("get scene id error \n");
		return WV_EFAIL;
	}
	
//*****************get open projector cmd ********************
	memset(temp,0,sizeof(temp));

	j=0;
	for(i=i+1;i<strlen(buf);i++)
	{
		if(buf[i] == '|')
		{
			pgNetUartDev->projectorCmd[id].OpenCmdLen = k;
			pgNetUartDev->projectorCmd[id].CmdEna = 1;
			break;
		}
				
		if(buf[i] != ' ' && buf[i] != 0x0a )
		{
			temp[j]=buf[i];
			j++;
			if(j==2){
				j=0;
				sscanf(temp,"%x",&a);
				pgNetUartDev->projectorCmd[id].OpenCmd[k] = (WV_U8)a;//get open projector cmd
				k++;
				memset(temp,0,sizeof(temp));	
			}
		}	
	}
//******************get close projector cmd ************************
	k=0;
	j=0;
	for(i=i+1;i<strlen(buf);i++)
	{
		if(buf[i] == '|')
		{
			pgNetUartDev->projectorCmd[id].CloseCmdLen = k;
			//pgNetUartDev->projectorCmd[id].CmdEna = 1;
			break;
		}

		if(buf[i] != ' ')
		{
			temp[j]=buf[i];
			j++;
			if(j==2)
			{
				j=0;
				sscanf(temp,"%x",&a);
				pgNetUartDev->projectorCmd[id].CloseCmd[k] = (WV_U8)a;//get close projector cmd 
				k++;
				memset(temp,0,sizeof(temp));				
			}
		}
	}	


	return 0;

}

/********************************************************

WV_S32 NET_UART_GetSceneCmd(WV_S8 *buf)
//控制场景切换命令
*********************************************************/ 
WV_S32 NET_UART_GetSceneCmd(WV_S8 *buf)
{
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
	if(id >= NET_UART_SCENE_NUM)
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
			pgNetUartDev->sceneCmd[id].CmdLen = k;
			break;
		}
				
		if(buf[i] != ' ' && buf[i] != 0x0a )
		{
			temp[j]=buf[i];
			j++;
			if(j==2){
				j=0;
				sscanf(temp,"%x",&a);
				pgNetUartDev->sceneCmd[id].ScenceCmd[k] = (WV_U8)a;//get sceneCmd
				k++;
				memset(temp,0,sizeof(temp));	
			}
		}
		
		if(i==strlen(buf)-1)
		{
			pgNetUartDev->sceneCmd[id].CmdLen = k;
			break;
		}
	
	}

	return 0;


}
/********************************************************

WV_S32 NET_UART_PrintConfInfo()

*********************************************************/ 
WV_S32 NET_UART_PrintConfInfo()
{
	
	printf("\n");
	printf("ClientIp = [%s] clientPort[%d] localport[%d]\n",pgNetUartDev->clientIp,pgNetUartDev->clientPort,pgNetUartDev->port);

	int c,b;
	for(c=0;c<6;c++){
		if( pgNetUartDev->projectorCmd[c].CmdEna == 1){
			
			printf("open cmd[%d]:",c);
			for(b=0;b<pgNetUartDev->projectorCmd[c].OpenCmdLen;b++){

				printf("%02x ",pgNetUartDev->projectorCmd[c].OpenCmd[b]);			
			}
			printf("close cmd:");
			for(b=0;b<pgNetUartDev->projectorCmd[c].CloseCmdLen;b++){

				printf("%02x ",pgNetUartDev->projectorCmd[c].CloseCmd[b]);			
			}
			printf("\n");			
		}		
					
	}


	for(c = 0;c<NET_UART_SCENE_NUM;c++)
	{
		if(pgNetUartDev->sceneCmd[c].CmdLen != 0 )
		{
			printf("scene[%d]=",c);
			for(b=0;b<pgNetUartDev->sceneCmd[c].CmdLen;b++)
			{
				printf("%02x ",pgNetUartDev->sceneCmd[c].ScenceCmd[b]);
			}
			printf("\r\n");		
		}
	}
	
	return WV_SOK;
}
/********************************************************

WV_S32 NET_UART_GetNetUartConf(NET_UART_DEV_E  * pDev)

*********************************************************/ 
WV_S32 NET_UART_GetNetUartConf(NET_UART_DEV_E  * pDev)
{

	WV_S8 buf[1024];
	WV_S32 ret;
	ret = access(NET_UART_CMDFILE,W_OK);

	if(ret != 0){
		NET_UART_printf("access file [%s] error\n",NET_UART_CMDFILE);
		return -1;
	}

	FILE *fp;
	//open sceneCmd.dat
	fp = fopen(NET_UART_CMDFILE,"rb+");
	if(fp == NULL)
	{
		NET_UART_printf("fopen file [%s] error\n",NET_UART_CMDFILE);
		return -1;
	}

	while(fgets(buf,sizeof(buf),fp))
	{
		if(buf[0] == '#')
			continue;
		if(buf[0] == '<')
		{
			if(strncmp(buf,"<UartDev>",9) == 0)
			{
				while(fgets(buf,sizeof(buf),fp))
				{
					if(buf[0] == '#')
						continue;
					if(buf[0] == '<')
						break;
					NET_UART_GetNetConf((WV_U8 *)buf);
				}
				break;
			} 
			
		}

	}
	fclose(fp);
	//获取投影开关命令
	fp = fopen(NET_UART_CMDFILE,"rb+");
	if(fp == NULL)
	{
		NET_UART_printf("fopen file [%s] error\n",NET_UART_CMDFILE);
		return -1;
	}

	//get projector open/close cmd 
	while(fgets(buf,sizeof(buf),fp))
	{
		if(buf[0] == '#')
			continue;
		if(buf[0] == '<')
		{
			if(strncmp(buf,"<ProjectorDev>",14) == 0)
			{
				while(fgets(buf,sizeof(buf),fp))
				{
					if(buf[0] == '#')
						continue;
					if(buf[0] == '<')
						break;
						NET_UART_GetProjectorConf(buf);
				}
				break;
			} 
			
		}
	}
	
	fclose(fp);
	//获取切换场景命令
	fp = fopen(NET_UART_CMDFILE,"rb+");
	if(fp == NULL)
	{
		NET_UART_printf("fopen file [%s] error\n",NET_UART_CMDFILE);
		return -1;
	}
	while(fgets(buf,sizeof(buf),fp))
	{
		if(buf[0] == '#')
			continue;
		if(buf[0] == '<')
		{
			if(strncmp(buf,"<SceneConf>",11) == 0)
			{
				while(fgets(buf,sizeof(buf),fp))
				{
					if(buf[0] == '#')
						continue;
					if(buf[0] == '<')
						break;
						NET_UART_GetSceneCmd(buf);
				}
				break;
			} 
			
		}
	}	
	//get SceneConf
	

	fclose(fp);

	//NET_UART_PrintConfInfo();
	return 0;
}


/******************************************************************************

WV_S32 NET_UART_SceneCheck(NET_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);

******************************************************************************/
WV_S32 NET_UART_SceneCheck(NET_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len)
{

	WV_S32 i,j;
	for(i=0;i<NET_UART_SCENE_NUM;i++)
	{
	
		if(pDev->sceneCmd[i].CmdLen == len)
		{
			
			for(j=0;j<len;j++)
			{
				if(pDev->sceneCmd[i].ScenceCmd[j] != pBuf[j])
				{
					break;
				}
			}

			if((j==len) && (pDev->sceneCmd[i].CmdEna == 0))
			{
				return WV_SOK;
			}
					
		}
	
	}

	if(i== 	NET_UART_SCENE_NUM)
	{
		//printf("clean cmd ena flag \n");	
		for(i=0;i<NET_UART_SCENE_NUM;i++)
		{
			if(pDev->sceneCmd[i].CmdLen == len)
			{
				for(j=0;j<len;j++)
				{
					if(pDev->sceneCmd[i].ScenceCmd[j] != pBuf[j])
						break;
				}

				if(j==len && pDev->sceneCmd[i].CmdEna == 1)
				{
						NET_UART_printf("clean cmd ena flga [%d]\n",i);
						pDev->sceneCmd[i].CmdEna = 0;
					
				}
					
			}
	
		}	
		
	}
	
	return WV_SOK;	

}

/****************************************************************************

WV_S32 NET_UART_Send(WV_U8 pData,WV_U32 dataLen)

****************************************************************************/
WV_S32 NET_UART_Send(WV_U8 *pData,WV_U32 dataLen)
{
	NET_UART_printf(" net to uart send cmd\n");
	WV_S32 i;
	for(i=0;i<dataLen;i++)
	{
		printf("%02x ",pData[i]);
	}	
	printf("\r\n");
	socklen_t addrLen = sizeof(struct sockaddr_in);
	sendto(pgNetUartDev->mSocket, pData, dataLen, 0, (struct sockaddr *)&pgNetUartDev->cliAddr, addrLen);
	return WV_SOK;	
}
/******************************************************************************

WV_S32 NET_UART_SceneChange(NET_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);

******************************************************************************/
WV_S32 NET_UART_SceneChange(NET_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len)
{

	if(NET_UART_SceneCheck(pDev,pBuf,len) != 0 )
	{
		NET_UART_printf("\nNET_UART_SceneCheck error \n");
		return WV_EFAIL;
	}
	WV_S32 i,j;
	for(i=0;i<NET_UART_SCENE_NUM;i++)
	{
		if(pDev->sceneCmd[i].CmdLen == len)
		{
			for(j=0;j<len;j++)
			{
				if(pDev->sceneCmd[i].ScenceCmd[j] != pBuf[j])
				{
					NET_UART_printf("scene cmd error \n");
					break;
				}
			}
			if(j==len && pDev->sceneCmd[i].CmdEna == 0)
			{	
				TSK_SCENE_Change(TSK_SCENE_TYPE_NETDATA,i);				
				pDev->sceneCmd[i].CmdEna = 1;
				break;
			}
					
		}
	
	}

	return WV_SOK;
}

/****************************************************************************

WV_S32 NET_UART_ProjectorCmd(WV_U32 openProjector);
//发送控制投影开关命令
****************************************************************************/
WV_S32 NET_UART_ProjectorCmd(WV_U32 openProjector)
{
	
	//开关投影命令不生效，直接返回
    if(SVR_CONTROL_GetOpenProjector() != 1 && TSK_UART_GetOpenprojectorMode() != 1)
	{
		WV_printf("开关投影命令不生效");
        return WV_SOK;
    }
	if(TSK_SCENE_GetSyncEna() == 1){
		SVR_UDP_SyncProjectorStatus(openProjector);
	}
	//接收ktv设备发送的开关机，开关投影命令，通过网络发送给网转串设备。
	SVR_FRAME_HEAD_E head={SVR_SYNC_WORD,0x2e,0x3,0,0,0,0};
	WV_U8 buf[64]={0};	
	WV_S32 i;
	WV_U32 len=sizeof(SVR_FRAME_HEAD_E);

	if(openProjector == 1){      //开

		for(i=0;i<NET_UART_PROJECTOR_NUM;i++){

			if(pgNetUartDev->projectorCmd[i].CmdEna != 1 || pgNetUartDev->projectorCmd[i].OpenCmdLen >40 || pgNetUartDev->projectorCmd[i].OpenCmdLen == 0) continue;
			head.arg1 = i+1;
			head.dataNum = pgNetUartDev->projectorCmd[i].OpenCmdLen;
			memcpy(buf,(WV_U8 *)&head,len);
			memcpy(&buf[len],pgNetUartDev->projectorCmd[i].OpenCmd,pgNetUartDev->projectorCmd[i].OpenCmdLen);

			NET_UART_Send(buf,len+head.dataNum);
	
		}
	
	}else if(openProjector == 0){ //关

		for(i=0;i<NET_UART_PROJECTOR_NUM;i++){

			if(pgNetUartDev->projectorCmd[i].CmdEna != 1 || pgNetUartDev->projectorCmd[i].CloseCmdLen >40 || pgNetUartDev->projectorCmd[i].CloseCmdLen == 0) continue;
			head.arg1 = i+1;
			head.dataNum = pgNetUartDev->projectorCmd[i].CloseCmdLen;
			memcpy(buf,(WV_U8 *)&head,len);
			memcpy(&buf[len],pgNetUartDev->projectorCmd[i].CloseCmd,pgNetUartDev->projectorCmd[i].CloseCmdLen);

			NET_UART_Send(buf,len+head.dataNum);
	
		}
		
	}
	return WV_SOK;
}

/****************************************************************************

WV_S32 NET_UART_InfraredCmd(WV_U8 disable);
//接收到红外人体感应
****************************************************************************/
WV_S32 NET_UART_InfraredCmd(WV_U8 disable)
{

	if(disable == 0 ){
		//开窗口
		TSK_SCENE_AddLastWin();	
	}else if(disable == 1){
		//关窗口		
		TSK_SCENE_DeletLastWin();
	}	
	
	return WV_SOK;
}
/****************************************************************************

WV_S32 NET_UART_Uart485Cmd(WV_U8* pBuf,WV_S32 dataLen);
接收到墙板485控制命令
****************************************************************************/
WV_S32 NET_UART_Uart485Cmd(WV_U8* pBuf,WV_U32 dataLen)
{
	NET_UART_printf("get 485 cmd\n");

	if(dataLen == 0 ){
		return WV_EFAIL;
	}
/*
	int i;
	for(i=0;i<dataLen;i++){
		
		printf("%02x ",pBuf[i]);
	}
	printf("\n");
*/
	if(NET_UART_SceneChange(pgNetUartDev,pBuf,dataLen) != 0 )
	{
		NET_UART_printf("change scene cmd error  \n");
		return WV_EFAIL;		
	}

	//NET_UART_ProjectorCmd(pgNetUartDev,1);
	//NET_UART_ProjectorCmd(pgNetUartDev,0);
	return WV_SOK;
}

/****************************************************************************

WV_S32 NET_UART_CliInit(NET_UART_DEV_E * pDev)

****************************************************************************/
WV_S32 NET_UART_CliInit(NET_UART_DEV_E * pDev)
{
	
	struct sockaddr_in* pAddr;
	pAddr = &(pDev->cliAddr);

	memset(pAddr,0,sizeof(struct sockaddr_in));
	pAddr->sin_family = AF_INET;
	pAddr->sin_port = htons(pDev->clientPort);
	//pAddr->sin_addr.s_addr = htonl(INADDR_ANY);  
	pAddr->sin_addr.s_addr = inet_addr((WV_S8 *)pDev->clientIp);
	bzero(&(pAddr->sin_zero), 8);

	printf("-----------------------client ip=%s,port=%d\n",pDev->clientIp,pDev->clientPort);

	return WV_SOK;	
}


/****************************************************************************

WV_S32 NET_UART_SvrInit(NET_UART_DEV_E * pDev)

****************************************************************************/
WV_S32 NET_UART_SvrInit(NET_UART_DEV_E * pDev)
{
	WV_U32 reuse;
	WV_S32 status;
	socklen_t len;
	
	struct sockaddr_in* pAddr;
	pAddr = &(pDev->svrAddr);
	if((pDev->mSocket = socket(AF_INET,SOCK_DGRAM,0)) == -1){
		NET_UART_printf("UDP socket init error.\r\n ");
		return WV_EFAIL;
	}

	reuse = 1;
	len = sizeof(reuse);
	setsockopt(pDev ->mSocket, SOL_SOCKET,SO_REUSEADDR,(WV_S8*)&reuse,len);

	//pDev->port = NET_UART_SERVER_PORT;
	memset(pAddr,0,sizeof(struct sockaddr_in));
	pAddr->sin_family = AF_INET;
	pAddr->sin_port = htons(pDev->port);
	pAddr->sin_addr.s_addr = htonl(INADDR_ANY);  
	bzero(&(pAddr->sin_zero), 8);
	status  =  bind(pDev ->mSocket, (struct sockaddr*)pAddr, sizeof(struct sockaddr));
	if( status < 0 )
	{
		close(pDev ->mSocket); 
		pDev ->mSocket = -1; 
		NET_UART_printf("SERVER socket bind  error = %d.\r\n",status);
		return WV_EFAIL;
	} 

	return WV_SOK;
}

/****************************************************************************

WV_S32 NET_UART_SvrDeInit(NET_UART_DEV_E * pDev)

****************************************************************************/
WV_S32 NET_UART_SvrDeInit(NET_UART_DEV_E * pDev)
{

	if(pDev->mSocket >= 0)
	{
		close(pDev->mSocket);
		pDev->mSocket = -1; 
	}
	
	NET_UART_printf("UDP socket DeInit ok\r\n");
	return  WV_SOK;

}
/****************************************************************************

WV_S32 NET_UART_Recv(WV_S32 socket,WV_U8 * pBuf, WV_S32 len)

****************************************************************************/
WV_S32 NET_UART_Recv(WV_S32 socket,NET_UART_DEV_E * pDev, WV_S32 len)
{

	WV_S32 recvLen,recvToal,ret =0;
	recvToal = 0; 
	WV_U8 pRevBuf[NET_UART_BUF_MAXLEN];
	recvLen = recvfrom(socket,pRevBuf,len,0,NULL,NULL);
	if(recvLen >0)
	{ 	  

		//printf("-----------net uart get udp data ----------------\n");
		memset(pDev->pBuf,0,NET_UART_BUF_MAXLEN);
		memcpy(pDev->pBuf,pRevBuf,recvLen);
		ret = recvLen;

	}
	else if(recvLen == 0)
	{
		NET_UART_printf("SERVER CNNECT loss !!!!!!!!!! %d",recvLen);
		ret = recvLen;

	}
	else
	{

		NET_UART_printf("SERVER recv EROOR !!!!!!!!!! %d",recvLen);
		ret = -1;

	}    
 	return ret;
}


/****************************************************************************

WV_S32 NET_UART_Get_Cmd(NET_UART_DEV_E * pDev)

****************************************************************************/
WV_S32 NET_UART_Get_Cmd(NET_UART_DEV_E * pDev)
{
	//get 
	WV_S32 len;
	
	len = NET_UART_Recv(pDev ->mSocket,pDev,NET_UART_BUF_MAXLEN);

	return len;
}


/****************************************************************************

WV_S32 NET_UART_CMD_Porc(NET_UART_DEV_E *pDev,WV_S32 cmdLen)

****************************************************************************/
WV_S32 NET_UART_CMD_Porc(NET_UART_DEV_E *pDev,WV_S32 cmdLen)
{

	WV_S32 ret=0;
#if 0
	printf("get cmd:");
	for(i=0;i<cmdLen;i++)
	{
		printf("%02x",pDev->pBuf[i]);
	}	
	printf("\n");
#endif
	
	if(cmdLen < sizeof(SVR_FRAME_HEAD_E)){

		WV_ERROR("get uart dev to server cmd error !");
		return WV_EFAIL;
	}

	pDev->pHead = (SVR_FRAME_HEAD_E *)pDev->pBuf;
	if(pDev->pHead->sync != SVR_SYNC_WORD || pDev->pHead->cmdL0 !=0x2e){
		WV_ERROR("get uart dev to server cmd error !");
		return WV_EFAIL;		
	}

	if(pDev->pHead->dataNum != (cmdLen - sizeof(SVR_FRAME_HEAD_E))){
		WV_ERROR("get uart dev to server cmd error !");
		return WV_EFAIL;		
	}
	
	switch(pDev->pHead->cmdL1){

		case 1:
			//接收红外感应命令，发送给融合主机
			NET_UART_InfraredCmd((WV_U8)pDev->pBuf[sizeof(SVR_FRAME_HEAD_E)]);	
			//printf("get cmd [hongwai] %d \n",pDev->pBuf[sizeof(SVR_FRAME_HEAD_E)]);
			break;
		case 2://接收485命令（切换场景），发送给融合主机
			NET_UART_Uart485Cmd(&pDev->pBuf[sizeof(SVR_FRAME_HEAD_E)],pDev->pHead->dataNum);
			//printf("get cmd [hongwai]\n");
			break;
		default:
			break;
	}

	return ret;
}

/****************************************************************************

void *NET_UART_Proc(void *prm)

****************************************************************************/
void *NET_UART_Proc(void *prm)
{

	NET_UART_DEV_E *pDev;
	WV_S32 ret,recvLen;
	pDev = (NET_UART_DEV_E *)prm;
//	if(NET_UART_Init(pDev) == WV_EFAIL){
//		return NULL;
//	}

	pDev->open = 1;
	pDev->close = 0;
	while(pDev->open == 1){
		
		recvLen = NET_UART_Get_Cmd(pDev);
		if(recvLen <=0 ){
			NET_UART_printf("udp get cmd error!!!!!!!!!!\n");
			continue;
		}
		
		ret = NET_UART_CMD_Porc(pDev,recvLen);
		if(ret != WV_SOK){
			NET_UART_printf("udp get info error!!!!!!!\r\n");
			continue;				
		}
		

	
		
	}
	return WV_SOK;
}

/****************************************************************************

WV_S32 NET_UART_Open()

****************************************************************************/
WV_S32 NET_UART_Open()
{

	
	NET_UART_printf("NET_UART open\n");
	pgNetUartDev = ( NET_UART_DEV_E*)malloc( sizeof(NET_UART_DEV_E));
	memset(pgNetUartDev,0,sizeof(NET_UART_DEV_E));

	if (NET_UART_GetNetUartConf(pgNetUartDev) != 0 ){
		return WV_EFAIL;
	}
	
	if(NET_UART_SvrInit(pgNetUartDev) == WV_EFAIL){
		return WV_EFAIL;
	}
	if(NET_UART_CliInit(pgNetUartDev) == WV_EFAIL){
		return WV_EFAIL;	
	}
	WV_CHECK_RET( WV_THR_Create(&(pgNetUartDev->thrHndl),NET_UART_Proc, WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, (void *)pgNetUartDev));
NET_UART_printf("NET_UART open ok\n");
	return WV_SOK;		
	
}

/****************************************************************************

WV_S32 NET_UART_Close()

****************************************************************************/
WV_S32 NET_UART_Close()
{
	 
   if(pgNetUartDev->open == 1)
   {
   	  pgNetUartDev->open = 0;
   	  while(pgNetUartDev->close == 1) ;
      WV_THR_Destroy(&(pgNetUartDev->thrHndl));
   }

    NET_UART_SvrDeInit(pgNetUartDev);

	free(pgNetUartDev);
	return WV_SOK;
}

