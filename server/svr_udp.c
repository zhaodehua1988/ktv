#include "svr.h"
#include "hi_common.h"
#include "sys_ip.h"
#include "sys_env.h" 
#include "svr_udp.h"
#include "wv_string.h"
#include "wv_type.h"
#include "tsk_player.h"
#include "tsk_scene.h"
#include "net_uart.h"
#define SVR_UDP_RECV_PORT 6000
#define SVR_UDP_SEND_PORT 6001
#define SVR_UDP_SEND_PORT_PLAYER 6002
#define SVR_UDP_DATA_MAX_LEN 1024
#define SVR_UDP_MAX_SYNCDEV (TSK_SCENE_MAX_SYNCDEV)
//#define SVR_UDP_DEBUG_MODE 

#ifdef  SVR_UDP_DEBUG_MODE 
  #define SVR_UDP_printf(...) \
  				 do {\
  				  printf("\n\r[%s][%s]-%d:",__FILE__,__FUNCTION__, __LINE__);\
				  printf(__VA_ARGS__);\
  				  fflush(stdout); \
  				  } while(0)
#else 
  #define SVR_UDP_printf(...) 
#endif   
 
#define UDP_CMD_SYNC_WORD 0xa55a5a5a
#define SVR_UDP_CMD_GETIP         0x01
#define SVR_UDP_CMD_SEARCH_SLAVE  0x02
#define SVR_UDP_CMD_PLAY          0x03
#define SVR_UDP_CMD_SCENE_CHANGE  0x04

#define SVR_UDP_CMD_SCENE_PAUSE   0x05
#define SVR_UDP_CMD_SCENE_PLAY    0x06
#define SVR_UDP_CMD_SCENE_STOP    0x07
#define SVR_UDP_CMD_PROJECTOR    0x08   //投影开关命令


#define SVR_UDP_CMD_ASK_SEARCH_SLAVE 0x82
#define SVR_UDP_CMD_ASK_PLAY         0x83
#define SVR_UDP_CMD_ASK_SCENE_CHANGE  0x84

#define SVR_UDP_CMD_ASK_SCENE_PAUSE   0x85
#define SVR_UDP_CMD_ASK_SCENE_PLAY    0x86
#define SVR_UDP_CMD_ASK_SCENE_STOP    0x87
#define SVR_UDP_CMD_ASK_PROJECTOR    0x88



//static long long svr_udp_startTime[10];
//static long long svr_udp_endTime[10];

typedef struct SVR_UDP_DEV_E
{
	WV_THR_HNDL_T thrHndl;	
	WV_U32      open;
	WV_U32      close; 
	WV_S32      mSocket;
	WV_U32      port;
	SVR_UDP_HEAD_E  headBuf;
	WV_U16      recvCnt;
	WV_U16      sendCnt; 
	WV_U8      *pBuf;	
	struct sockaddr_in recvAddr;
	WV_U8 slaveDevEna[SVR_UDP_MAX_SYNCDEV];

}SVR_UDP_DEV_E;

static SVR_UDP_DEV_E *pSvrUdp;

WV_S32 SVR_UDP_SwitchChar(WV_S8 *pStr,WV_S32 *pValue)
{
	if(strncmp(pStr,"0",1) == 0){
		pValue[0] = 0;		
		return WV_SOK;
	}
	else if(strncmp(pStr,"1",1) == 0){
		pValue[0] = 1;
		return WV_SOK;
	}
	else if(strncmp(pStr,"2",1) == 0){
		pValue[0] = 2;
		return WV_SOK;
	}
	else if(strncmp(pStr,"3",1) == 0){
		pValue[0] = 3;
		return WV_SOK;
	}
	else if(strncmp(pStr,"4",1) == 0){
		pValue[0] = 4;
		return WV_SOK;
	}
	else if(strncmp(pStr,"5",1) == 0){
		pValue[0] = 5;
		return WV_SOK;
	}
	else if(strncmp(pStr,"6",1) == 0){
		pValue[0] = 6;
		return WV_SOK;
	}
	else if(strncmp(pStr,"7",1) == 0){
		pValue[0] = 7;
		return WV_SOK;
	}
	else if(strncmp(pStr,"8",1) == 0){
		pValue[0] = 8;
		return WV_SOK;
	}
	else if(strncmp(pStr,"9",1) == 0){
		pValue[0] = 9;
		return WV_SOK;
	}
	else if((strncmp(pStr,"a",1) == 0) || (strncmp(pStr,"A",1) == 0)){
		pValue[0] = 10;
		return WV_SOK;
	}
	else  if((strncmp(pStr,"b",1) == 0) || (strncmp(pStr,"B",1) == 0)){
		pValue[0] = 11;
		return WV_SOK;
	}
	else if((strncmp(pStr,"c",1) == 0) || (strncmp(pStr,"C",1) == 0)){
		pValue[0] = 12;
		return WV_SOK;
	}
	else  if((strncmp(pStr,"d",1) == 0) || (strncmp(pStr,"D",1) == 0)){
		pValue[0] = 13;
		return WV_SOK;
	}
	else if((strncmp(pStr,"e",1) == 0) || (strncmp(pStr,"E",1) == 0)){
		pValue[0] = 14;
		return WV_SOK;
	}		
	else if((strncmp(pStr,"f",1) == 0) || (strncmp(pStr,"F",1) == 0)){
		pValue[0] = 15;
		return WV_SOK;
	}else {
		return WV_EFAIL;
	}		
	
}

/*******************************************************************
 WV_S32 SVR_UDP_getIp(WV_S8*pSrc,WV_U8* pIp);
*******************************************************************/
 WV_S32 SVR_UDP_getIp(WV_S8*pSrc,WV_U8* pIp)
{

	WV_S8* pData = pSrc;	
	WV_S32 len;
	WV_S32 i,j,k,data=0;
	WV_S32 des;
	len = strlen(pSrc);
	if(strncmp(pData,".",1) == 0){
		SVR_UDP_printf("get ip error\r\n");
		return WV_EFAIL;
	}
	j = 3;
	k = 1;
	for(i=len-1;i>=0;i--){
		 
		if(SVR_UDP_SwitchChar(&pData[i],&des)==0){
		
			data += des*k;
			pIp[j] = data;
			k*=10;
		
		}else{
			
			k=1;
			data = 0;
		
			j--;
			if(j<0){
				break;
			}
		}
	}

	return WV_SOK;	
}

/*******************************************************************
 WV_S32 SVR_UDP_getIp(WV_U8 str);
*******************************************************************/
 WV_S32 SVR_UDP_getMac(WV_S8*pSrc,WV_U8* pMac)
{
	
	WV_S8* pData = pSrc;	
	WV_S32 len;
	WV_S32 i,j,k,data=0;
	WV_S32 des;
	len = strlen(pSrc);
	if(strncmp(pData,".",1) == 0){
		SVR_UDP_printf("get mac error\r\n");
		return WV_EFAIL;
	}
	j = 5;
	k = 1;
	for(i=len-1;i>=0;i--){
		 
		if( SVR_UDP_SwitchChar(&pData[i],&des)==0){
			data += des*k;
			pMac[j] = data;
			k*=16;
		}else{
			k=1;
			data = 0;
			j--;
			if(j<0){
				break;
			}
		}
	}

	return WV_SOK;	
}

/****************************************************************************

WV_S32 SVR_UDP_Send(SVR_UDP_DEV_E * pDev)

****************************************************************************/
WV_S32 SVR_UDP_Send(SVR_UDP_DEV_E * pDev,WV_S32 port,WV_S8 *pIp)
{
	WV_S32 sendLen,reqLen;
	WV_U8  sendBuf[SVR_UDP_DATA_MAX_LEN ];
	WV_U32 headLen;
	WV_S32 brdcFd;
	SVR_UDP_printf("svr udp send cmd,port=%d \n",port);
	brdcFd = socket(PF_INET,SOCK_DGRAM,0);
	if(brdcFd < 0){
		SVR_UDP_printf("udp send socket error\r\n");
		return WV_EFAIL;
	}
	WV_S32 optval = 1;

	setsockopt(brdcFd,SOL_SOCKET,SO_BROADCAST|SO_REUSEADDR,&optval,sizeof(optval));
	
	struct sockaddr_in theirAddr;
	memset(&theirAddr,0,sizeof(struct sockaddr_in));

	theirAddr.sin_family = AF_INET;
	theirAddr.sin_addr.s_addr = inet_addr(pIp);  	
	theirAddr.sin_port = htons(port);
	bind(brdcFd, (struct sockaddr*)&theirAddr, sizeof(struct sockaddr));

	//send data
	headLen = sizeof(pDev->headBuf);
	memcpy(sendBuf,&pDev->headBuf,headLen);
	if(pDev->headBuf.dataNum > 0 )
	{
		memcpy(sendBuf+headLen,pDev->pBuf,pDev->headBuf.dataNum);
		sendLen = headLen+pDev->headBuf.dataNum;
	}else{
		sendLen = headLen;
	}	
	reqLen = sendto(brdcFd,sendBuf,sendLen,0,(struct sockaddr*)&(theirAddr),sizeof(struct sockaddr));
	
	if(reqLen != sendLen){
		SVR_UDP_printf("udp send head error !!!!!!!!!!!!!,reqLen[%d]sednLen[%d]\r\n",reqLen,sendLen);
		close(brdcFd);
		return WV_EFAIL;
	}
	close(brdcFd);
	return WV_SOK;
}


/****************************************************************************

WV_S32 SVR_UDP_SyncScene(WV_U32 id);

****************************************************************************/
WV_S32 SVR_UDP_SyncScene(WV_U32 id)
{
	
	WV_S32 ret=0;

	WV_S32 i;
	WV_U8 cascadeInfo;
	WV_U16 cascadeNum;
	WV_U8 ipInt[30];
	WV_S8 ipString[20];
	TSK_SCENE_GetCascading(&cascadeInfo,&cascadeNum,ipInt);
	for(i=0;i<cascadeNum;i++)
	{

		if(pSvrUdp->slaveDevEna[i] == 1)
		{
			sprintf(ipString,"%d.%d.%d.%d",ipInt[i*4],ipInt[i*4+1],ipInt[i*4+2],ipInt[i*4+3]);
			memset(&pSvrUdp->headBuf,0,sizeof(pSvrUdp->headBuf));
			pSvrUdp->headBuf.sync = UDP_CMD_SYNC_WORD;
			pSvrUdp->headBuf.dataNum = 0;
			pSvrUdp->headBuf.cmdL0 = SVR_UDP_CMD_SCENE_CHANGE;
			pSvrUdp->headBuf.arg1 = id;
			ret|=SVR_UDP_Send(pSvrUdp,SVR_UDP_RECV_PORT,ipString);
			
		}
			
	}

	return ret;
	
}

/****************************************************************************

WV_S32 SVR_UDP_SyncScenePause();
主机发送暂停命令
****************************************************************************/
WV_S32 SVR_UDP_SyncScenePause()
{
	
	WV_S32 ret=0;

	WV_S32 i;
	WV_U8 cascadeInfo;
	WV_U16 cascadeNum;
	WV_U8 ipInt[30];
	WV_S8 ipString[20];
	TSK_SCENE_GetCascading(&cascadeInfo,&cascadeNum,ipInt);
	for(i=0;i<cascadeNum;i++)
	{

		if(pSvrUdp->slaveDevEna[i] == 1)
		{
			sprintf(ipString,"%d.%d.%d.%d",ipInt[i*4],ipInt[i*4+1],ipInt[i*4+2],ipInt[i*4+3]);
			memset(&pSvrUdp->headBuf,0,sizeof(pSvrUdp->headBuf));
			pSvrUdp->headBuf.sync = UDP_CMD_SYNC_WORD;
			pSvrUdp->headBuf.dataNum = 0;
			pSvrUdp->headBuf.cmdL0 = SVR_UDP_CMD_SCENE_PAUSE;
			pSvrUdp->headBuf.arg1 = 0;
			ret|=SVR_UDP_Send(pSvrUdp,SVR_UDP_RECV_PORT,ipString);
			
		}
			
	}

	return ret;
	
}
/****************************************************************************

WV_S32 SVR_UDP_SyncSceneStop();
主机发送停止
****************************************************************************/
WV_S32 SVR_UDP_SyncSceneStop()
{
	
	WV_S32 ret=0;

	WV_S32 i;
	WV_U8 cascadeInfo;
	WV_U16 cascadeNum;
	WV_U8 ipInt[30];
	WV_S8 ipString[20];
	TSK_SCENE_GetCascading(&cascadeInfo,&cascadeNum,ipInt);
	for(i=0;i<cascadeNum;i++)
	{

		if(pSvrUdp->slaveDevEna[i] == 1)
		{
			sprintf(ipString,"%d.%d.%d.%d",ipInt[i*4],ipInt[i*4+1],ipInt[i*4+2],ipInt[i*4+3]);
			memset(&pSvrUdp->headBuf,0,sizeof(pSvrUdp->headBuf));
			pSvrUdp->headBuf.sync = UDP_CMD_SYNC_WORD;
			pSvrUdp->headBuf.dataNum = 0;
			pSvrUdp->headBuf.cmdL0 = SVR_UDP_CMD_SCENE_STOP;
			pSvrUdp->headBuf.arg1 = 0;
			ret|=SVR_UDP_Send(pSvrUdp,SVR_UDP_RECV_PORT,ipString);
			
		}
			
	}

	return ret;
	
}
/****************************************************************************

WV_S32 SVR_UDP_SyncProjectorStatus(WV_U16 status);
主机发送开/关投影命令
****************************************************************************/
WV_S32 SVR_UDP_SyncProjectorStatus(WV_U16 status)
{
	
	WV_S32 ret=0;

	WV_S32 i;
	WV_U8 cascadeInfo;
	WV_U16 cascadeNum;
	WV_U8 ipInt[30];
	WV_S8 ipString[20];
	TSK_SCENE_GetCascading(&cascadeInfo,&cascadeNum,ipInt);
	for(i=0;i<cascadeNum;i++)
	{

		if(pSvrUdp->slaveDevEna[i] == 1)
		{
			sprintf(ipString,"%d.%d.%d.%d",ipInt[i*4],ipInt[i*4+1],ipInt[i*4+2],ipInt[i*4+3]);
			memset(&pSvrUdp->headBuf,0,sizeof(pSvrUdp->headBuf));
			pSvrUdp->headBuf.sync = UDP_CMD_SYNC_WORD;
			pSvrUdp->headBuf.dataNum = 0;
			pSvrUdp->headBuf.cmdL0 = SVR_UDP_CMD_PROJECTOR;
			pSvrUdp->headBuf.arg1 = status;
			ret|=SVR_UDP_Send(pSvrUdp,SVR_UDP_RECV_PORT,ipString);
			
		}
			
	}

	return ret;	

}
/****************************************************************************

WV_S32 SVR_UDP_SyncScenePlay();
主机发送 场景开始命令
****************************************************************************/
WV_S32 SVR_UDP_SyncScenePlay()
{
	
	WV_S32 ret=0;

	WV_S32 i;
	WV_U8 cascadeInfo;
	WV_U16 cascadeNum;
	WV_U8 ipInt[30];
	WV_S8 ipString[20];
	TSK_SCENE_GetCascading(&cascadeInfo,&cascadeNum,ipInt);
	for(i=0;i<cascadeNum;i++)
	{

		if(pSvrUdp->slaveDevEna[i] == 1)
		{
			sprintf(ipString,"%d.%d.%d.%d",ipInt[i*4],ipInt[i*4+1],ipInt[i*4+2],ipInt[i*4+3]);
			memset(&pSvrUdp->headBuf,0,sizeof(pSvrUdp->headBuf));
			pSvrUdp->headBuf.sync = UDP_CMD_SYNC_WORD;
			pSvrUdp->headBuf.dataNum = 0;
			pSvrUdp->headBuf.cmdL0 = SVR_UDP_CMD_SCENE_PLAY;
			pSvrUdp->headBuf.arg1 = 0;
			ret|=SVR_UDP_Send(pSvrUdp,SVR_UDP_RECV_PORT,ipString);
			
		}
			
	}

	return ret;
	
}

/****************************************************************************

WV_S32 SVR_UDP_PlayNotifySlave();

****************************************************************************/
WV_S32 SVR_UDP_PlayNotifySlave()
{
	
	//printf("-------------SVR_UDP_PlayNotifySlave() --------------\n");

	if(TSK_SCENE_GetSyncEna() != 1)
	{
		//SVR_UDP_PlayerPlay(pSvrUdp->headBuf.arg1);
		return WV_SOK;	
	}
	WV_S32 i;
	WV_U8 cascadeInfo;
	WV_U16 cascadeNum;
	WV_U8 ipInt[30];
	WV_S8 ipString[20];
	TSK_SCENE_GetCascading(&cascadeInfo,&cascadeNum,ipInt);
	for(i=0;i<cascadeNum;i++)
	{

		if(pSvrUdp->slaveDevEna[i] == 1)
		{
			sprintf(ipString,"%d.%d.%d.%d",ipInt[i*4],ipInt[i*4+1],ipInt[i*4+2],ipInt[i*4+3]);
			memset(&pSvrUdp->headBuf,0,sizeof(pSvrUdp->headBuf));
			pSvrUdp->headBuf.sync = UDP_CMD_SYNC_WORD;
			pSvrUdp->headBuf.dataNum = 0;
			pSvrUdp->headBuf.cmdL0 = SVR_UDP_CMD_PLAY;
			pSvrUdp->headBuf.arg1 = 0;
			//printf("udp send to %s cmd[%d]\n",ipString,SVR_UDP_CMD_PLAY);
			SVR_UDP_Send(pSvrUdp,SVR_UDP_RECV_PORT,ipString);
			
		}
			
	}

	for(i=0;i<10;i++)
	{
		if(TSK_PLAYER_GetStatus(0) == 2)//play
		{
			return WV_SOK;
		}
		usleep(10000);
	}

	//SVR_UDP_PlayerPlay(pSvrUdp->headBuf.arg1);	

	return WV_EFAIL;
}

/****************************************************************************

WV_S32 SVR_UDP_SlaveAskOK(WV_U16 devID);

****************************************************************************/
WV_S32 SVR_UDP_SlaveAskOK(WV_U16 devID)
{
	if(devID >= SVR_UDP_MAX_SYNCDEV)
		return WV_EFAIL;
	pSvrUdp->slaveDevEna[devID] = 1;
	printf("**** SVR_UDP_SlaveAskOK devID[%d]******\n",devID);
	return WV_SOK;
}

/****************************************************************************

WV_S32 SVR_UDP_CleanDev();

****************************************************************************/
WV_S32 SVR_UDP_CleanDev()
{

	WV_S32 i;
	for(i=0;i<SVR_UDP_MAX_SYNCDEV;i++)
	{
		pSvrUdp->slaveDevEna[i]=0;
	}
	return WV_SOK;
}

/****************************************************************************

WV_S32 SVR_UDP_SearchDev();

****************************************************************************/
WV_S32 SVR_UDP_SearchDev()//如何是主机则循环查询从机设备
{
	
	//printf("----------SVR_UDP_SearchDev()--------------\n");
	//WV_S32 ret = 0;
	if(TSK_SCENE_GetSyncEna() != 1)
	{
		usleep(30000);
		//printf("---sync [%d] \n",TSK_SCENE_GetSyncEna());
		return  WV_SOK; 
	}

	SVR_UDP_CleanDev();

	WV_U8 cascadeInfo;
	WV_U16 cascadeNum;
	WV_U8 ipInt[30];
	WV_S8 ipString[20];
	WV_S32 searchOk,i;
	TSK_SCENE_GetCascading(&cascadeInfo,&cascadeNum,ipInt);
	while(TSK_SCENE_GetSyncEna() == 1)
	{
		searchOk = 0;

		for(i=0;i<cascadeNum;i++)
		{

			if(pSvrUdp->slaveDevEna[i] != 1)
			{
				sprintf(ipString,"%d.%d.%d.%d",ipInt[i*4],ipInt[i*4+1],ipInt[i*4+2],ipInt[i*4+3]);
				memset(&pSvrUdp->headBuf,0,sizeof(pSvrUdp->headBuf));
				pSvrUdp->headBuf.sync = UDP_CMD_SYNC_WORD;
				pSvrUdp->headBuf.dataNum = 0;
				pSvrUdp->headBuf.cmdL0 = SVR_UDP_CMD_SEARCH_SLAVE;
				pSvrUdp->headBuf.arg1 = i;
				//printf(" send to [%s]cmd [%d]\n",ipString,SVR_UDP_CMD_SEARCH_SLAVE);
				SVR_UDP_Send(pSvrUdp,SVR_UDP_RECV_PORT,ipString);
				searchOk++;
				//usleep(33000);			
			}
						
		}
		if(searchOk == 0 )
		{
			printf("all slave devs are ready ");
			//usleep(300000);		
			break;
		}		
		usleep(30000);

	}

	//SVR_UDP_PlaySync();

	return WV_SOK;
	
}


/****************************************************************************

WV_S32 SVR_UDP_PlayerSync();

****************************************************************************/
WV_S32 SVR_UDP_PlayerSync()//发送同步命令
{
	WV_S32 ret;
	SVR_UDP_SearchDev();
	ret = SVR_UDP_PlayNotifySlave();

	while((ret != 0) )
	{
		printf("SVR_UDP_PlayerSync() error  test once\n ");
		SVR_UDP_SearchDev();
		ret = SVR_UDP_PlayNotifySlave();
	}
	printf("****SVR_UDP_PlayerSync() end***** \n");
	return ret;
}

/****************************************************************************

WV_S32 SVR_UDP_Init(SVR_UDP_DEV_E * pDev)

****************************************************************************/
WV_S32 SVR_UDP_Init(SVR_UDP_DEV_E * pDev)
{
	WV_U32 reuse;
	WV_S32 status;
	socklen_t len;
	
	struct sockaddr_in* pAddr;

	pAddr = &(pDev->recvAddr);
	
	if((pDev->mSocket = socket(AF_INET,SOCK_DGRAM,0)) == -1){
		SVR_UDP_printf("UDP socket init error.\r\n ");
		return WV_EFAIL;
	}

	reuse = 1;
	len = sizeof(reuse);
	setsockopt(pDev ->mSocket, SOL_SOCKET,SO_REUSEADDR,(WV_S8*)&reuse,len);

	memset(pAddr,0,sizeof(struct sockaddr_in));
	pAddr->sin_family = AF_INET;
	pAddr->sin_port = htons(SVR_UDP_RECV_PORT);
	pAddr->sin_addr.s_addr = htonl(INADDR_ANY);  
	bzero(&(pAddr->sin_zero), 8);
	status  =  bind(pDev ->mSocket, (struct sockaddr*)pAddr, sizeof(struct sockaddr));
	if( status < 0 )
	{
		close(pDev ->mSocket); 
		pDev ->mSocket = -1; 
		SVR_UDP_printf("SERVER socket bind  error = %d.\r\n",status);
		return WV_EFAIL;
	} 

	return WV_SOK;
}

/****************************************************************************

WV_S32 SVR_UDP_DeInit(SVR_UDP_DEV_E * pDev)

****************************************************************************/
WV_S32 SVR_UDP_DeInit(SVR_UDP_DEV_E * pDev)
{

	if(pDev->mSocket >= 0)
	{
		close(pDev->mSocket);
		pDev->mSocket = -1; 
	}
	
	SVR_UDP_printf("UDP socket DeInit ok\r\n");
	return  WV_SOK;

}
/****************************************************************************

WV_S32 SVR_UDP_Recv(WV_S32 socket,WV_U8 * pBuf, WV_S32 len)

****************************************************************************/
WV_S32 SVR_UDP_Recv(WV_S32 socket,SVR_UDP_DEV_E * pDev, WV_S32 len)
{

  WV_S32 recvLen,recvToal,reqLen,ret =0;
  WV_U8 pRevBuf[1024+18];
  WV_U32 headLen=sizeof(pDev ->headBuf);
  reqLen = sizeof(pRevBuf);
  recvToal = 0; 

	socklen_t addrLen = sizeof(struct sockaddr_in);
	recvLen = recvfrom(socket,pRevBuf,reqLen,0,(struct sockaddr*)&(pSvrUdp->recvAddr),&addrLen );
	
	if(recvLen >= headLen)
	{ 	  

		memcpy(&pDev->headBuf,pRevBuf,headLen);

		if(pDev->headBuf.dataNum > 0 && (recvLen-headLen) == pDev->headBuf.dataNum)
		{		
			memcpy(pDev->pBuf,&pRevBuf[headLen],pDev->headBuf.dataNum);

		}
			ret = WV_SOK;
	}
	else if(recvLen == 0)
	{
		SVR_UDP_printf("SERVER CNNECT loss !!!!!!!!!! %d",recvLen);
		ret = 1;

	}
	else
	{

		SVR_UDP_printf("SERVER recv EROOR !!!!!!!!!! %d",recvLen);
		ret = WV_EFAIL;

	}
     
 	return ret;
}

/****************************************************************************

WV_S32 SVR_UDP_PlayerPlay(WV_U32 id);

****************************************************************************/
WV_S32 SVR_UDP_PlayerPlay(WV_U32 id)
{

	//printf("SVR_UDP_PlayerPlay [%d]\n",id);
	TSK_Player_playRoll(id);
	return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_UDP_CmdCheck(SVR_UDP_HEAD_E head)

****************************************************************************/
WV_S32 SVR_UDP_CmdCheck(SVR_UDP_HEAD_E head)
{
	//check sync
	if(head.sync != UDP_CMD_SYNC_WORD){
		SVR_UDP_printf(" udp sync error !!!!!!!!\r\n");
		return WV_EFAIL;
	}
	
	return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_UDP_GetCmd()

****************************************************************************/
WV_S32 SVR_UDP_Get_Cmd(SVR_UDP_DEV_E * pDev)
{
	//get 
	WV_S32 len,ret;
	
	len = sizeof(SVR_UDP_HEAD_E);
	//ret = SVR_UDP_Recv(pDev ->mSocket,(WV_U8 *)&(pDev ->headBuf),len);
	ret = SVR_UDP_Recv(pDev ->mSocket,pDev,len);
			  
	ret = SVR_UDP_CmdCheck(pDev->headBuf);

	return ret;
}

/****************************************************************************

WV_S32 SVR_UDP_GetInfo(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)

****************************************************************************/
WV_S32 SVR_UDP_GetInfo(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
{

	//printf("SVR_UDP_GetInfo\n");
	//pHead->cmdL0 |= 0x80;
	WV_S8  ip[20];
	WV_S8  mac[20];

	WV_U32 data;
	WV_U8 *pData;
	WV_U16 *pTemp;
	pData = pBuf;
	//get ip
	SYS_IP_GetIp(ip);
	SVR_UDP_getIp(ip,pData);
	SVR_UDP_printf("ip:%d.%d.%d.%d\n",pData[0],pData[1],pData[2],pData[3]);
	pData+=4;
	
	//get mac
	SYS_IP_GetMac(mac);
	SVR_UDP_getMac(mac,pData);
	SVR_UDP_printf("mac:%02x:%02x:%02x:%02x:%02x:%02x\n",pData[0],pData[1],pData[2],pData[3],pData[4],pData[5]);
	pData+=6;
	//get port
	SYS_ENV_GetU32("SVR_PORT",&data);
	pData[0] = data & 0xff;
	pData[1] = (data & 0xff00) >> 8;
	pTemp =(WV_U16 *) pData;
	SVR_UDP_printf("port:%d\n",pTemp[0]);
	pData+=2;
	//get year
	SYS_ENV_GetU32("DateYear",&data);
	pData[0] = data & 0xff;
	pData[1] = (data & 0xff00) >> 8;
	pTemp =(WV_U16 *) pData;
	SVR_UDP_printf("year:%d\n",pTemp[0]);
	pData+=2;
	//get mon
	SYS_ENV_GetU32("DateMon",&data);
	pData[0] = data;
	SVR_UDP_printf("mon:%d\n",*pData);
	pData+=1;
	//get day
	SYS_ENV_GetU32("DateDay",&data);
	pData[0] = data;
	SVR_UDP_printf("day:%d\n",*pData);
	pData+=1;

	//get id
	SYS_ENV_GetU32("DateNum",&data);
	pData[0] = data &0xff;
	pData[1] = (data & 0xff00)>>8;
	pTemp =(WV_U16 *) pData;
	SVR_UDP_printf("id:%d\n",pTemp[0]);
	pData+=2;
	//argv1 hello version  
	WV_U16 arg1,arg2,chn;
	SYS_ENV_GetVersion(&arg1);
	pData[0]=arg1 & 0x00ff;
	pData[1]=(arg1 >> 8) & 0x00ff;
	//printf("arg1 = [%04x]\n",pHead->arg1);
	pData+=2;
	//argv2 fpga version;	
	arg2= FPGA_CONF_GerVer();
	pData[0]=arg2 & 0x00ff;
	pData[1]=(arg2 >> 8) & 0x00ff;
	pData+=2;
	
	//get chn num
	WV_U32 num,id;
 	num = FPGA_CONF_GetOutChl_Num();
	id = FPGA_CONF_GetVersion_ID();

	if(id == 0)
	{
		chn= num;
	}
	if(id == 1)
	{
		chn = num*10 + id;
	}

	pData[0] = chn & 0x00ff;
	pData[1] = (chn>>8) & 0x00ff;
	pHead->dataNum = 24;
	return WV_SOK;
	
}
/****************************************************************************

WV_S32 SVR_UDP_SetPlayer(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)

****************************************************************************/
WV_S32 SVR_UDP_SetPlayer(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
{	
	WV_S32 ret=0;
	//printf("***********player replay[%d] ********* \n",pHead->arg1);
	ret = 	TSK_Player_Replay(0);
	if(ret != 0 )
	{
		printf("player[%d] rePlay error\n",pHead->arg1);	
	}
	ret = 	TSK_Player_Replay(1);
	if(ret != 0 )
	{
		printf("player[%d] rePlay error\n",pHead->arg1);	
	}
	pHead->dataNum = 0;
	return ret;

}

/****************************************************************************

WV_S32 SVR_UDP_ChangeScene(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)

****************************************************************************/
WV_S32 SVR_UDP_ChangeScene(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
{	
	WV_U32 id;
	id = pHead->arg1;
	return TSK_SCENE_Change(TSK_SCENE_TYPE_NETDATA, id);
}

/****************************************************************************

WV_S32 SVR_UDP_ChangeScenePause(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
//切换场景暂停
****************************************************************************/
WV_S32 SVR_UDP_ChangeScenePause(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
{	
	TSK_SCENE_PlayerPause();
	return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_UDP_ChangeSceneStop(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
//切换场景停止
****************************************************************************/
WV_S32 SVR_UDP_ChangeSceneStop(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
{	
	TSK_SCENE_PlayerStop();
	return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_UDP_ChangeScenePlay(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
//切换场景播放
****************************************************************************/
WV_S32 SVR_UDP_ChangeScenePlay(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
{	
	TSK_SCENE_PlayerPlay();
	return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_UDP_ChangeScenePlay(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
//开关投影命令
****************************************************************************/
WV_S32 SVR_UDP_ChangeProjectorStatus(SVR_UDP_HEAD_E *pHead,WV_U8 *pBuf)
{	
	//TSK_SCENE_PlayerPlay();
	NET_UART_ProjectorCmd( (WV_U32)pHead->arg1);
	return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_UDP_CMD_Porc(SVR_UDP_DEV_E *pDev)

****************************************************************************/
WV_S32 SVR_UDP_CMD_Porc(SVR_UDP_DEV_E *pDev)
{

	WV_S32 ret=0;
	WV_U32 port;
	WV_S8 ip[20];
	//printf("get cmd0 = 0x%02x \n",pHead->cmdL0);
	switch(pDev->headBuf.cmdL0){
		case SVR_UDP_CMD_GETIP:
			ret = SVR_UDP_GetInfo(&pDev->headBuf,pDev->pBuf);
			port = SVR_UDP_SEND_PORT;
			strcpy(ip,"255.255.255.255");
			pDev->headBuf.cmdL0 |= 0x80;
			ret |= SVR_UDP_Send(pDev,port,ip);

			break;
		case SVR_UDP_CMD_SEARCH_SLAVE:
			if(TSK_SCENE_GetSyncEna() == 2)
			{
				port = SVR_UDP_RECV_PORT;
				pDev->headBuf.cmdL0 |= 0x80;
				sprintf(ip,"%s",inet_ntoa(pDev->recvAddr.sin_addr));
				ret = SVR_UDP_Send(pDev,port,ip);
			}
			break;
		case SVR_UDP_CMD_PLAY:
			if(TSK_SCENE_GetSyncEna() == 2)
			{
				port = SVR_UDP_RECV_PORT;
				pDev->headBuf.cmdL0 = SVR_UDP_CMD_ASK_PLAY;
				sprintf(ip,"%s",inet_ntoa(pDev->recvAddr.sin_addr));
				//printf("play ask ip[%s]\n",ip);
				ret = SVR_UDP_Send(pDev,port,ip);
				ret = SVR_UDP_SetPlayer(&pDev->headBuf,pDev->pBuf);
			}		
			break;
		case SVR_UDP_CMD_SCENE_CHANGE:
			if(TSK_SCENE_GetSyncEna() == 2)
			{
				ret = SVR_UDP_ChangeScene(&pDev->headBuf,pDev->pBuf);
			}		
			break;
		case SVR_UDP_CMD_SCENE_PAUSE:
			if(TSK_SCENE_GetSyncEna() == 2)
			{
				ret = SVR_UDP_ChangeScenePause(&pDev->headBuf,pDev->pBuf);
				//ret = SVR_UDP_ChangeScene(&pDev->headBuf,pDev->pBuf);
			}
			break;
		case SVR_UDP_CMD_SCENE_STOP:
			if(TSK_SCENE_GetSyncEna() == 2)
			{
				ret = SVR_UDP_ChangeSceneStop(&pDev->headBuf,pDev->pBuf);
			}
			break;
		case SVR_UDP_CMD_SCENE_PLAY:
			if(TSK_SCENE_GetSyncEna() == 2)
			{
				ret = SVR_UDP_ChangeScenePlay(&pDev->headBuf,pDev->pBuf);
			}						
			break;
		case SVR_UDP_CMD_PROJECTOR:
			//开投影
			if(TSK_SCENE_GetSyncEna() == 2)
			{
				ret = SVR_UDP_ChangeProjectorStatus(&pDev->headBuf,pDev->pBuf);
			}						
			break;
					
		case SVR_UDP_CMD_ASK_SEARCH_SLAVE:
			ret = SVR_UDP_SlaveAskOK(pDev->headBuf.arg1);
			//SVR_UDP_CMD_ASK_TO_MASTER(pHead->arg1);
			break;
		case SVR_UDP_CMD_ASK_PLAY:
			ret=SVR_UDP_PlayerPlay(pDev->headBuf.arg1);
			break;
		case SVR_UDP_CMD_ASK_SCENE_CHANGE:
			break;
		default:
			ret = -1;
			SVR_UDP_printf("no this cmd\r\n");
	}
	
	return ret;
}

/****************************************************************************

void *SVR_UDP_Proc(void *prm)

****************************************************************************/
void *SVR_UDP_Proc(void *prm)
{

	SVR_UDP_DEV_E *pDev;
	WV_S32 ret;
	pDev = (SVR_UDP_DEV_E *)prm;
	if(SVR_UDP_Init(pDev) == WV_EFAIL){
		return NULL;
	}

	pDev->open = 1;
	pDev->close = 0;
	while(pDev->open == 1){
		
		ret = SVR_UDP_Get_Cmd(pDev);
		if(ret != WV_SOK){
			SVR_UDP_printf("udp get cmd error!!!!!!!!!!\n");
			continue;
		}
		
		ret = SVR_UDP_CMD_Porc(pDev);
		if(ret != WV_SOK){
			SVR_UDP_printf("udp get info error!!!!!!!\r\n");
			continue;				
		}
		
		//ret = SVR_UDP_Set_Cmd(pDev);
	
		
	}
	return WV_SOK;
}

/****************************************************************************

WV_S32 SVR_UDP_Open()

****************************************************************************/
WV_S32 SVR_UDP_Open()
{
	pSvrUdp = (SVR_UDP_DEV_E *)malloc(sizeof(SVR_UDP_DEV_E)); 
	memset(pSvrUdp,0,sizeof(SVR_UDP_DEV_E));

	//SVR_GetConf(pSvrDev);
	pSvrUdp -> pBuf = (WV_U8 *)malloc(SVR_UDP_DATA_MAX_LEN); 
	memset(pSvrUdp->pBuf,0,sizeof(SVR_UDP_DATA_MAX_LEN));

	WV_CHECK_RET( WV_THR_Create(&(pSvrUdp->thrHndl),SVR_UDP_Proc, WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, (void *)pSvrUdp));
	return WV_SOK;		

}

/****************************************************************************

WV_S32 SVR_UDP_Close()

****************************************************************************/
WV_S32 SVR_UDP_Close()
{
  
   if(pSvrUdp->open == 1)
   {
   	  pSvrUdp->open = 0;
   	  while(pSvrUdp->close == 1) ;
      WV_THR_Destroy(&(pSvrUdp->thrHndl));
   }
   free (pSvrUdp -> pBuf);
   free (pSvrUdp);
   SVR_UDP_DeInit(pSvrUdp);

	return WV_SOK;
}


