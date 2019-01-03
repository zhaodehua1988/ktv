#include"svr.h"
#include "hi_common.h"
#include"svr_cmd.h"
#include"svr_frame.h"
#include "tsk_conf.h"
#include <sys/select.h>
#include "tsk_mobileShow.h"
typedef struct SVR_SERVER_DEV_E 
{

    WV_THR_HNDL_T thrHndl;
    WV_U32      open;
    WV_U32      close;
    WV_S32      mSocket;
    WV_S32      cSocket;
    WV_U32      port;
    SVR_FRAME_HEAD_E  headBuf;
    WV_U16      recvCnt;
    WV_U16      sendCnt;
    WV_U8      *pBuf;
    WV_U8      *pSendBuf;
    WV_U32      conectNum;
    WV_U32      startMs;
    WV_U32      cmdMs;
} SVR_SERVER_DEV_E; 

static SVR_SERVER_DEV_E  * pSvrDev;

/*******************************************************************
 WV_S32 SVR_PORT_RegisterConf();
*******************************************************************/

WV_S32 SVR_PORT_RegisterConf()
{ 
    WV_S8 value[20];
    sprintf(value, "%d",2323);
    SYS_ENV_Register(1 ,"SVR_PORT",value,"server port");

    return WV_SOK;
}

/*******************************************************************
 WV_S32 SVR_PORT_GetConf();
*******************************************************************/

WV_S32 SVR_PORT_GetConf()
{ 
    WV_S8 name[20];
    WV_U32 data;
    sprintf(name, "SVR_PORT");
    SYS_ENV_GetU32(name, & data);

    pSvrDev->port = data;
    return WV_SOK;
}

/*******************************************************************
 WV_S32 SVR_PORT_SetConf(,WV_U16 data);
*******************************************************************/

WV_S32 SVR_PORT_SetConf(WV_U16 data)
{ 
    pSvrDev-> port = data;
    return WV_SOK;
}

/******************************************************************
 WV_S32 SVR_PORT_SaveConf(SVR_SERVER_DEV_E  * pDev);
*******************************************************************/

WV_S32 SVR_PORT_SaveConf()
{ 

    WV_S8 name[20];
    WV_U32 data;
    sprintf(name, "SVR_PORT");
    data = pSvrDev-> port;
    SYS_ENV_SetU32(name,data);
    SYS_ENV_Save();

    sleep(1);
    return WV_SOK;
}


/********************************************************

WV_S32 SVR_GetConf(SVR_SERVER_DEV_E  * pDev)

********************************************************/
WV_S32  SVR_GetConf(SVR_SERVER_DEV_E  * pDev)
{
    pDev -> port = 2323;
    return WV_SOK;
}



/********************************************************

WV_S32 SVR_Init(SVR_SERVER_DEV_E  * pDev)

********************************************************/ 
WV_S32  SVR_Init(SVR_SERVER_DEV_E  * pDev)
{
    unsigned int reuse;
    socklen_t len;
    struct sockaddr_in addr;
    WV_S32 status;

    pDev -> mSocket = socket(AF_INET,SOCK_STREAM,0);
    if (pDev -> mSocket < 0)
    {
        WV_ERROR("SERVER  socket init error.\r\n");
        return  WV_EFAIL;
    }

    //fcntl(DpDev ->mSocket,F_SETFL,O_NONBLOCK);//no block
    reuse = 1;
    len = sizeof(reuse);
    setsockopt(pDev ->mSocket, SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,len);
    //get sen buff
    WV_S32   nSendBufLen;
    socklen_t optlen = sizeof(int);
    nSendBufLen = 0;
    getsockopt(pDev->mSocket, SOL_SOCKET, SO_SNDBUF,(char*)&nSendBufLen, &optlen);
    // WV_printf("SERVER socket SNDBUF  = %d.\r\n",nSendBufLen);


    addr.sin_family = AF_INET;
    addr.sin_port = htons(pDev ->port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(addr.sin_zero), 8);
    status  =  bind(pDev ->mSocket, (struct sockaddr *) &addr, sizeof(addr));
    if( status < 0 )
    {
        close(pDev ->mSocket);
        pDev ->mSocket = -1;
        WV_ERROR("SERVER socket bind  error = %d.\r\n",status);
        return WV_EFAIL;
    }

    return WV_SOK;
}

/********************************************************

WV_S32 SVR_DeInit(SVR_SERVER_DEV_E  * pDev)

********************************************************/ 

WV_S32 SVR_DeInit(SVR_SERVER_DEV_E *pDev)
{
    if(pDev -> cSocket >= 0)
    {
        close(pDev->cSocket);
        pDev->cSocket = -1;
    }

    if(pDev -> mSocket >= 0)
    {
        close(pDev->mSocket);
        pDev->mSocket = -1;
    }
    // WV_printf("SERVER socket DeInit OK\r\n");
    return  WV_SOK;
}

/*********************************************************

WV_S32 SVR_SERVER_SelectSocket( );

*********************************************************/
WV_S32 SVR_SERVER_SelectSocket(WV_S32 sock )
{
    WV_S32 ret,maxfdp;
    struct timeval timeout = {0,0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock,&fds);
    maxfdp = sock +1;
    ret = select(maxfdp,&fds,&fds,NULL,&timeout);
    return ret;
}

/********************************************************

WV_S32  SVR_SERVER_Recv(WV_S32 socket,WV_U8 * pBuf, WV_S32 len)
接受上位机通讯数据
********************************************************/ 

WV_S32 SVR_SERVER_Recv(WV_S32 socket,WV_U8 * pBuf, WV_S32 len)
{
    WV_S32 recvLen,recvToal,reqLen,ret =0;
    WV_U8 * pRevBuf;
    pRevBuf = pBuf;
    recvLen = len;
    recvToal = 0;
    reqLen = len;
    while(1)
    {
        ret = SVR_SERVER_SelectSocket(socket);
        if(ret <= 0){
            ret = -1;
            break;
        }
        recvLen = recv(socket,pRevBuf,reqLen,0);
        if(recvLen >0)
        {
            recvToal +=recvLen;

            if(recvToal == len)
            {
                ret = WV_SOK;
                break;
            }
            else
            {
                pRevBuf =  pBuf + recvToal;
                reqLen = len - recvToal;
            }

        }
        else if(recvLen == 0)
        {
            TSK_Mobile_getPicCleanFlag();
            WV_ERROR("SERVER CNNECT loss !!!!!!!!!! %d",recvLen);
            ret = 1;
            break;
        }
        else
        {
            TSK_Mobile_getPicCleanFlag();
            WV_ERROR("SERVER recv EROOR !!!!!!!!!! %d,errno:%d\n",recvLen,errno);
            perror("server recv error");
            //printf("error:%d\n",errno);
            ret = WV_EFAIL;
            break;
        }
    }


    return ret;
}


/*********************************************************

void * SVR_SERVER_GetCmd( );

*********************************************************/

WV_S32 SVR_SERVER_GetCmd(SVR_SERVER_DEV_E * pDev)
{

    WV_S32 len,ret;
    len = sizeof( SVR_FRAME_HEAD_E);
    //printf("recv 1\n");
    ret = SVR_SERVER_Recv (pDev ->cSocket,(WV_U8 *)&(pDev ->headBuf),len);
    //
    if(ret != 0){
        return -1;
    }

    WV_S32 i;

    pDev->headBuf.dataNum = pDev->headBuf.dataNum;
    pDev->headBuf.cnt = pDev->headBuf.cnt;



    WV_printf("recv cmd head:");

    for(i=0;i< sizeof(SVR_FRAME_HEAD_E);i++)
    {
        WV_printf("%02X",((WV_U8 *)&(pDev ->headBuf))[i]);
    }
    WV_printf("cnt[%d]dataNum = [%d]",pDev->headBuf.cnt,pDev->headBuf.dataNum);
    WV_printf("\r\n");

    
    //
    WV_CHECK_RET( SVR_FRAME_Analyze ( &(pDev ->headBuf), &(pDev ->recvCnt)) );

    if(pDev ->headBuf. dataNum  > SVR_FRAME_DATA_MAX_LEN )
    {
        return WV_EFAIL;
    }


    if(pDev -> headBuf.dataNum  > 0 )
    {
        memset(pDev -> pBuf,0,SVR_FRAME_DATA_MAX_LEN);
        WV_CHECK_RET( SVR_SERVER_Recv(pDev ->cSocket,pDev -> pBuf, pDev ->headBuf.dataNum) );
    }

    return WV_SOK;
}


/*********************************************************

void * SVR_SERVER_SendCmd( );

*********************************************************/

WV_S32 SVR_SERVER_SendCmd(SVR_SERVER_DEV_E * pDev)
{

    WV_S32 len,sendLen,dataLen;
    //send frame head
    WV_S32 pack = 26672;
    WV_S32 i;
    WV_printf("ack cmd head:");

    for(i=0;i< sizeof(SVR_FRAME_HEAD_E);i++)
    {
        WV_printf("%02X",((WV_U8 *)&(pDev ->headBuf))[i]);
    }
    WV_printf("\r\n");
    
    len = sizeof( SVR_FRAME_HEAD_E);
    pDev->headBuf.dataNum = pDev->headBuf.dataNum;
    pDev->headBuf.cnt = pDev->headBuf.cnt;

    sendLen = send(pDev->cSocket,(WV_U8 *)&(pDev ->headBuf),len,0);
    if(sendLen != len)
    {

        WV_ERROR("SERVER send  Head EROOR ! %d[%d]\r\n",sendLen,len);
        return WV_EFAIL;
    }
    //send load
    dataLen = pDev -> headBuf.dataNum;
    i=0;
    if(pDev -> headBuf.dataNum  > 0 )
    {

        while(dataLen > pack)
        {

            sendLen = send(pDev->cSocket,&pDev->pBuf[i],pack,0);
            //printf("+++++++send load[%d] **********",pack);
            if(sendLen !=  pack)
            {
                WV_ERROR("SERVER send  load EROOR ! %d[%d]\r\n",sendLen,pDev -> headBuf.dataNum);
                return WV_EFAIL;
            }
            dataLen -= pack;
            i+=pack;
        }
        if(dataLen > 0 )
        {
            //printf("******send load[%d] **********\n",dataLen);
            sendLen = send(pDev->cSocket,&pDev -> pBuf[i],dataLen,0);
            if(sendLen !=  dataLen)
            {
                WV_ERROR("SERVER send  load EROOR ! %d[%d]\r\n",sendLen,pDev -> headBuf.dataNum);
                return WV_EFAIL;
            }
        }
    }

    return WV_SOK;
}

/*********************************************************

void * SVR_SERVER_SendCmd2( );

*********************************************************/

WV_S32 SVR_SERVER_SendCmd2(SVR_SERVER_DEV_E * pDev)
{

    WV_S32 len,sendLen;
    //send frame head
    
    WV_S32 i;
    WV_printf("ack cmd head:");

    for(i=0;i< sizeof(SVR_FRAME_HEAD_E);i++)
    {
        WV_printf("%02X",((WV_U8 *)&(pDev ->headBuf))[i]);
    }
    WV_printf("\r\n");
    
    len = sizeof( SVR_FRAME_HEAD_E);
    pDev->headBuf.dataNum = pDev->headBuf.dataNum;
    pDev->headBuf.cnt = pDev->headBuf.cnt;
    //clear buf
    memset(pSvrDev->pSendBuf,0,sizeof(SVR_FRAME_HEAD_E) + SVR_FRAME_DATA_MAX_LEN);

    memcpy(pSvrDev->pSendBuf,(WV_U8 *)&pDev->headBuf,len);

    if(pDev->headBuf.dataNum >0){
        //len = pDev->headBuf.dataNum;
        memcpy(&pSvrDev->pSendBuf[len],(WV_U8 *)pDev->pBuf,pDev->headBuf.dataNum);
    }

    len = len + pDev->headBuf.dataNum;

    sendLen = send(pDev->cSocket,pSvrDev->pSendBuf,len,0);
    //printf("sendLen = %d [%d]\n",sendLen,len);
    //sendLen = send(pDev->cSocket,(WV_U8 *)&(pDev ->headBuf),len,0);
    if(sendLen != len)
    {

        WV_ERROR("SERVER send  Head EROOR ! %d[%d]\r\n",sendLen,len);
        return WV_EFAIL;
    }

    return WV_SOK;
}


/*********************************************************

void * SVR_SERVER_Proc(void * prm);
线程处理函数
*********************************************************/
void * SVR_SERVER_Proc(void * prm)
{
    SVR_SERVER_DEV_E  * pDev;
    WV_S32 ret;
    unsigned int uiBuf = 10485760 + 100;
    pDev = (SVR_SERVER_DEV_E *) prm;
    //
    if( SVR_Init(pDev) == WV_EFAIL)
    {
        return NULL;
    }
    ret  =  listen(pDev -> mSocket,1);
    if ( ret  < 0 )
    {
        close(pDev ->mSocket);
        pDev ->mSocket = -1;
        WV_ERROR("SERVER socket listen  error = %d.\r\n",ret);
        return NULL;
    }
    //  WV_printf("SERVER socket listen OK\r\n");

    pDev -> open   = 1;
    pDev -> close  = 0;
    while(pDev -> open  == 1)
    {


        //setsockopt(pDev->cSocket, SOL_SOCKET, SO_SNDBUF,(char*)&nSendBufLen, &optlen);
        pDev ->cSocket = accept(  pDev ->mSocket,NULL, NULL);
        if( pDev -> cSocket < 0)
        {
            WV_ERROR("SERVER  SOCKET  ACCEPT   error =.\r\n");
            continue;
        }
        pDev -> recvCnt = 0;
        pDev -> sendCnt = 0;
        //get sen buff
        WV_S32   nSendBufLen;
        //socklen_t optlen = sizeof(int);
        nSendBufLen = 0;

        //getsockopt(pDev->cSocket, SOL_SOCKET, SO_SNDBUF,(char*)&nSendBufLen, &optlen);
        setsockopt(pDev->cSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&uiBuf, sizeof(int));
        setsockopt(pDev->cSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&uiBuf, sizeof(int));

        //WV_printf("SERVER cSocket SNDBUF  = %d.\r\n",nSendBufLen);
        //WV_printf("SERVER  SOCKET  ACCEPT   OK\r\n");

        pDev -> recvCnt = 0;
        pDev -> sendCnt = 0;
        pDev -> conectNum++;
        HI_SYS_GetTimeStampMs  ( &(pDev -> startMs));
        while( pDev ->cSocket >=  0 )
        {
            //get cmd frame
            ret = SVR_SERVER_GetCmd(pDev);

            if(ret != WV_SOK)
            {
                WV_printf("SERVER  SOCKET  DISconet  GetCmd \r\n");
                //shutdown(pDev->cSocket, SHUT_RDWR);
                close(pDev->cSocket);
                pDev->cSocket = -1;
                break;
            }

            // proc
            ret = SVR_CMD_Proc(&(pDev ->headBuf),pDev -> pBuf);
            //ack  cmd

            ret = SVR_SERVER_SendCmd2(pDev);
            //ret = SVR_SERVER_SendCmd(pDev);
            if(ret != WV_SOK)
            {
                WV_printf("SERVER  SOCKET  DISconet  SendCmd \r\n");
                //shutdown(pDev->cSocket, SHUT_RDWR);
                close(pDev->cSocket);
                pDev->cSocket = -1;
                break;
            }

            HI_SYS_GetTimeStampMs  ( &(pDev ->cmdMs));
        }

    }

    SVR_DeInit(pDev);
    pDev -> close  = 1;

    return NULL;
}

/****************************************************************************

WV_S32 SVR_SERVER_CmdGetInfo(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 SVR_SERVER_CmdGetInfo(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    WV_U32  timeMs;
    WV_U32  i;
    HI_SYS_GetTimeStampMs  ( &timeMs );
    prfBuff += sprintf(prfBuff,"Server status[%u]:\r\n",timeMs);
    prfBuff += sprintf(prfBuff,"Connect  socket =   %d\r\n",pSvrDev -> cSocket);
    prfBuff += sprintf(prfBuff,"Connect  number =   %d\r\n",pSvrDev -> conectNum);
    if(pSvrDev -> cSocket >= 0)
    {
        prfBuff += sprintf(prfBuff,"Connect  run    =   %dMS\r\n",timeMs - pSvrDev -> startMs);
        prfBuff += sprintf(prfBuff,"command  idle   =   %dMS\r\n",timeMs - pSvrDev -> cmdMs);
        prfBuff += sprintf(prfBuff,"recv     number =   %d\r\n",pSvrDev -> recvCnt);
        prfBuff += sprintf(prfBuff,"send     number =   %d\r\n",pSvrDev -> sendCnt);
    }
    prfBuff += sprintf(prfBuff,"last cmd head:" );
    for(i=0;i< sizeof(SVR_FRAME_HEAD_E);i++)
    {
        prfBuff += sprintf(prfBuff,"%02X",((WV_U8 *)(&(pSvrDev ->headBuf)))[i] );
    }
    prfBuff += sprintf(prfBuff,"\r\n" );
    
    return WV_SOK;

}




/********************************************************************

WV_S32  SVR_Open();

********************************************************************/

WV_S32  SVR_Open()
{

    pSvrDev = (SVR_SERVER_DEV_E *)malloc(sizeof(SVR_SERVER_DEV_E));
    memset(pSvrDev,0,sizeof(SVR_SERVER_DEV_E));
    SVR_PORT_RegisterConf(pSvrDev);
    SVR_PORT_GetConf(pSvrDev);
    //SVR_GetConf(pSvrDev);
    pSvrDev -> pBuf = (WV_U8 *)malloc( SVR_FRAME_DATA_MAX_LEN);
    pSvrDev->pSendBuf = (WV_U8 *)malloc(sizeof(SVR_FRAME_HEAD_E) + SVR_FRAME_DATA_MAX_LEN);
    SVR_CMD_Init();

    WV_CMD_Register("get","svr","get protocl server info",SVR_SERVER_CmdGetInfo);

    WV_CHECK_RET( WV_THR_Create(&(pSvrDev->thrHndl),SVR_SERVER_Proc, WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, (void *)pSvrDev));
    return WV_SOK;
}


/********************************************************************

WV_S32  SVR_Close();

********************************************************************/

WV_S32  SVR_Close()
{

    if(pSvrDev->open == 1)
    {
        pSvrDev->open = 0;
        while(pSvrDev->close == 1) ;
        WV_THR_Destroy(&(pSvrDev->thrHndl));
    }

    free (pSvrDev -> pSendBuf);
    free (pSvrDev -> pBuf);
    free (pSvrDev);
    SVR_CMD_DeInit();
    return WV_SOK;
}
