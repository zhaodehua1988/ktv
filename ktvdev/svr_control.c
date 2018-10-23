#include "svr_control.h"
#define SVR_CONTROL_BUF_MAXLEN 1024*4

typedef struct SVR_CONTROL_DEV_E
{
	WV_THR_HNDL_T thrHndl;	
	WV_U32      open;
	WV_U32      close; 
	WV_S32      mSocket;
	WV_U32      port;
	struct sockaddr_in svrAddr;
	struct sockaddr_in cliAddr;
	WV_U8       pBuf[SVR_CONTROL_BUF_MAXLEN];
	
}SVR_CONTROL_DEV_E;

static SVR_CONTROL_DEV_E gSvrControl;

/****************************************************************************

WV_S32 SVR_CONTROL_Init(SVR_CONTROL_DEV_E * pDev)

****************************************************************************/
WV_S32 SVR_CONTROL_Init(SVR_CONTROL_DEV_E * pDev)
{
	WV_U32 reuse;
	WV_S32 status;
	socklen_t len;
	
	struct sockaddr_in* pAddr;
	pAddr = &(pDev->svrAddr);
	if((pDev->mSocket = socket(AF_INET,SOCK_DGRAM,0)) == -1){
		SVR_CONTROL_printf("UDP socket init error.\r\n ");
		return WV_EFAIL;
	}

	reuse = 1;
	len = sizeof(reuse);
	setsockopt(pDev ->mSocket, SOL_SOCKET,SO_REUSEADDR,(WV_S8*)&reuse,len);

	pDev->port = SVR_CONTROL_GetKtvNetPort();
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
		SVR_CONTROL_printf("SERVER socket bind  error = %d.\r\n",status);
		return WV_EFAIL;
	} 

	return WV_SOK;
}

/****************************************************************************

WV_S32 SVR_CONTROL_DeInit(SVR_CONTROL_DEV_E * pDev)

****************************************************************************/
WV_S32 SVR_CONTROL_DeInit(SVR_CONTROL_DEV_E * pDev)
{

	if(pDev->mSocket >= 0)
	{
		close(pDev->mSocket);
		pDev->mSocket = -1; 
	}
	
	SVR_CONTROL_printf("UDP socket DeInit ok\r\n");
	return  WV_SOK;

}
/****************************************************************************

WV_S32 SVR_CONTROL_Recv(WV_S32 socket,WV_U8 * pBuf, WV_S32 len)

****************************************************************************/
WV_S32 SVR_CONTROL_Recv(WV_S32 socket,SVR_CONTROL_DEV_E * pDev, WV_S32 len)
{

  WV_S32 recvLen,recvToal,reqLen,ret =0;

  recvToal = 0; 
   WV_U8 pRevBuf[SVR_CONTROL_BUF_MAXLEN];
	socklen_t addrLen = sizeof(struct sockaddr_in);
	recvLen = recvfrom(socket,pRevBuf,len,0,(struct sockaddr*)&(gSvrControl.cliAddr),&addrLen );
	int i;
	if(recvLen >0)
	{ 	  

		memset(pDev->pBuf,0,SVR_CONTROL_BUF_MAXLEN);
		memcpy(pDev->pBuf,pRevBuf,recvLen);
		ret = recvLen;

	}
	else if(recvLen == 0)
	{
		SVR_CONTROL_printf("SERVER CNNECT loss !!!!!!!!!! %d",recvLen);
		ret = recvLen;

	}
	else
	{

		SVR_CONTROL_printf("SERVER recv EROOR !!!!!!!!!! %d",recvLen);
		ret = -1;

	}

     
 	return ret;
}

/****************************************************************************

WV_S32 SVR_CONTROL_Send(WV_U8 pData,WV_U32 dataLen)

****************************************************************************/
WV_S32 SVR_CONTROL_Send(WV_U8 *pData,WV_U32 dataLen)
{
	WV_S32 i;
	for(i=0;i<dataLen;i++)
	{
		printf("%02x ",pData[i]);
	}	
	printf("\r\n");
	socklen_t addrLen = sizeof(struct sockaddr_in);
	sendto(gSvrControl.mSocket, pData, dataLen, 0, &gSvrControl.cliAddr, addrLen);
	return WV_SOK;	
}

/****************************************************************************

WV_S32 SVR_CONTROL_Get_Cmd(SVR_CONTROL_DEV_E * pDev)

****************************************************************************/
WV_S32 SVR_CONTROL_Get_Cmd(SVR_CONTROL_DEV_E * pDev)
{
	//get 
	WV_S32 len,i;
	
	len = SVR_CONTROL_Recv(pDev ->mSocket,pDev,SVR_CONTROL_BUF_MAXLEN);

	return len;
}


/****************************************************************************

WV_S32 SVR_CONTROL_CMD_Porc(SVR_CONTROL_DEV_E *pDev,WV_S32 cmdLen)

****************************************************************************/
WV_S32 SVR_CONTROL_CMD_Porc(SVR_CONTROL_DEV_E *pDev,WV_S32 cmdLen)
{

	WV_S32 ret=0;
	WV_U32 port;
	WV_S8 ip[20];
	WV_S32 i;
#if 0
	printf("get cmd:");
	for(i=0;i<cmdLen;i++)
	{
		printf("%02x",pDev->pBuf[i]);
	}	
	printf("\n");
#endif
	//SVR_CONTROL_printf("get cmd =%s\n",pDev->pBuf);
	switch(SVR_CONTROL_GetKtvDev())
	{
		case 0:
			ret = SVR_CONTROL_LeiShi(pDev->pBuf,cmdLen);
			break;
		case 1:
			ret = SVR_CONTROL_ShiYi(pDev->pBuf,cmdLen);
			break;
		case 2:
			ret = SVR_CONTROL_YinChuang(pDev->pBuf,cmdLen);
			break;
		default:
			break;
		
	}

	return ret;
}

/****************************************************************************

void *SVR_CONTROL_Proc(void *prm)

****************************************************************************/
void *SVR_CONTROL_Proc(void *prm)
{

	SVR_CONTROL_DEV_E *pDev;
	WV_S32 ret,recvLen;
	pDev = (SVR_CONTROL_DEV_E *)prm;
	if(SVR_CONTROL_Init(pDev) == WV_EFAIL){
		return NULL;
	}

	pDev->open = 1;
	pDev->close = 0;
	while(pDev->open == 1){
		
		recvLen = SVR_CONTROL_Get_Cmd(pDev);
		if(recvLen <=0 ){
			SVR_CONTROL_printf("udp get cmd error!!!!!!!!!!\n");
			continue;
		}
		
		ret = SVR_CONTROL_CMD_Porc(pDev,recvLen);
		if(ret != WV_SOK){
			SVR_CONTROL_printf("udp get info error!!!!!!!\r\n");
			continue;				
		}
			
	}
	return WV_SOK;
}

/****************************************************************************

WV_S32 SVR_CONTROL_Open()

****************************************************************************/
WV_S32 SVR_CONTROL_Open()
{

	SVR_CONTROL_printf("svr_control open\n");
	memset(&gSvrControl,0,sizeof(gSvrControl));
	//SVR_CONTROL_ReadConfFileCmd();

	SVR_CONTROL_Cmd_Init();
	WV_CHECK_RET( WV_THR_Create(&(gSvrControl.thrHndl),SVR_CONTROL_Proc, WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, (void *)&gSvrControl));

	return WV_SOK;		

}

/****************************************************************************

WV_S32 SVR_CONTROL_Close()

****************************************************************************/
WV_S32 SVR_CONTROL_Close()
{
  
   if(gSvrControl.open == 1)
   {
   	  gSvrControl.open = 0;
   	  while(gSvrControl.close == 1) ;
      WV_THR_Destroy(&(gSvrControl.thrHndl));
   }

   SVR_CONTROL_DeInit(&gSvrControl);

	return WV_SOK;
}

