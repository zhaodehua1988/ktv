#include "ktv_shiyi.h"
#include "tsk_scene.h"
#include "net_uart.h"
#include "svr_control_cmd.h"
#include "sys_ip.h"
   #include <sys/socket.h>
#define KTV_SHIYI_LOCALPORT 7320
#define KTV_SHIYI_LOADBUF_MAX_LEN 1024*4
#define KTV_SHIYI_LOCAL_PORT   3790
#define KTV_SHIYI_RE_REGIST_TIME  10
//消息类别
#define KTV_SHIYI_MSG_TYPE_REGISTER    "api/register/device"
#define KTV_SHIYI_MSG_TYPE_UNREGISTER  "api/unregister/device"
#define KTV_SHIYI_MSG_TYPE_SUBSCRIBE   "api/subscribe/events"

#define KTV_SHIYI_CMDFILE  "./env/netContrl.dat"

//九州
//#define KTV_SHIYI_REGISTMSG_DATA "{\"developerId\":\"JZ7807K4\",\"appId\":\"GD34B12D\",\"deviceId\":\"%02x%02x%02x%02x%02x%02x\"}"

//北京蕃秀
#define KTV_SHIYI_REGISTMSG_DATA "{\"developerId\":\"FXK5DZ3Z\",\"appId\":\"14BT4FAD\",\"deviceId\":\"%02x%02x%02x%02x%02x%02x\"}"

//subscribe 订阅开机、关机、歌曲播放三个事件

#define KTV_SHIYI_SUBSCRIBE_DATA "{events:[\"event/room/state/opened\",\"event/room/state/closed\",\"event/evmedia/program/start\"]}"
//#define KTV_SHIYI_SUBSCRIBE_DATA "{events:[\"event/room/state/opened\",\"event/room/state/closed\",\"event/evmedia/program/start\",\"event/evmedia/playctrl/next\",\"event/evmedia/playctrl/replay\",\"event/evmedia/track/ori\",\"event/evmedia/track/acc\",\"event/evmedia/playctrl/pause\"]}"

#define KTV_SHIYI_DB_FILE_PATH      "./env/shiyi.db"
#define KTV_SHIYI_DB_TABLE_NAME     "SongClass"
#define KTV_SHIYI_DB_SONGCLASS_NAME       "Class"

#define TIMEOUT 1

#define KTV_SHIYI_DEBUG_MODE 
#ifdef  KTV_SHIYI_DEBUG_MODE 
#define KTV_SHIYI_printf(...) \
  				 do {\
  				  printf("\n\r[%s]-%d:",__FUNCTION__, __LINE__);\
				  printf(__VA_ARGS__);\
  				  fflush(stdout); \
  				  } while(0)
#else 
  #define KTV_SHIYI_printf(...) 
#endif

#pragma pack(push)
#pragma pack(1)



#define TEST_DEBUG 1
typedef struct KTV_SHIYI_HEAD_E 
{
	WV_U8  identification[6];
	WV_U8  version[2];
	WV_U32 dialogID;
	WV_U32 msgLen;
}KTV_SHIYI_HEAD_E;
 

typedef struct KTV_SHIYI_DEV_E 
{

	WV_THR_HNDL_T thrHndl; 
	WV_U32      open;
	WV_U32      close; 
	WV_S32      mSocket;
	WV_U32      svrPort;
	//WV_U32      localPort;
	WV_S8       svrIp[24];
	KTV_SHIYI_HEAD_E  headBuf;
	WV_U8      *pBuf;
	WV_U32     dataLen;

} KTV_SHIYI_DEV_E; 
#pragma pack(pop)
static KTV_SHIYI_DEV_E  * pShiyiDev;

/********************************************************

WV_S32 KTV_SHIYI_GetIpAndPort(WV_U8 *buf)
获取视易服务器新版本ip和端口号配置
*********************************************************/ 
WV_S32 KTV_SHIYI_GetIpAndPort(WV_U8 *buf)
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
	
	if(strcmp(name,"SYServerIP") == 0)
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

		sprintf(pShiyiDev->svrIp,"%s",ip);
	
	}
	else if(strcmp(name,"SYServerPort") == 0)
	{
		sscanf((WV_S8 *)&buf[i],"%d",&data);
		data = atoi((WV_S8 *)&buf[i]);
		pShiyiDev->svrPort = data;
		
	}
	//KTV_SHIYI_printf("********get shiyi [%s]=%d *************\n ",name,data);
	return ret;	
}
/********************************************************

WV_S32 KTV_SHIYI_GetServerConf()

*********************************************************/ 
WV_S32 KTV_SHIYI_GetServerConf()
{

	WV_S8 buf[1024];
	WV_S32 ret;
	ret = access(KTV_SHIYI_CMDFILE,W_OK);

	if(ret != 0){
		NET_UART_printf("access file [%s] error\n",KTV_SHIYI_CMDFILE);
		return -1;
	}

	FILE *fp;
	//open sceneCmd.dat
	fp = fopen(KTV_SHIYI_CMDFILE,"rb+");
	if(fp == NULL)
	{
		NET_UART_printf("fopen file [%s] error\n",KTV_SHIYI_CMDFILE);
		return -1;
	}

	while(fgets(buf,sizeof(buf),fp))
	{
		if(buf[0] == '#')
			continue;
		if(buf[0] == '<')
		{
			if(strncmp(buf,"<SYDevConf>",11) == 0)
			{
				while(fgets(buf,sizeof(buf),fp))
				{
					if(buf[0] == '#')
						continue;
					if(buf[0] == '<')
						break;
						KTV_SHIYI_GetIpAndPort((WV_U8 *)buf);
				}
				break;
			} 
			
		}

	}
	fclose(fp);

	return 0;
}


/******************************************************************************

WV_S32 KTV_SHIYI_callback_DB(void* data, int ncols, char** values, char** headers) ;

******************************************************************************/  
WV_S32 KTV_SHIYI_callback_DB(void* data, int ncols, char** values, char** headers)  
{  
    WV_S32 i;  
    WV_S32 len =0;  
    WV_S32 ll=0;
	WV_U8 destbuf[16];
	WV_U8 srcbuf[16];
	memset(destbuf,0,sizeof(destbuf));
	memset(srcbuf,0,sizeof(srcbuf));
    for(i=0; i < ncols; i++)  
    {  
        if(strlen(headers[i])>len)  
            len = strlen(headers[i]);  
    }  
    for(i=0; i < ncols; i++)   
    {  
        ll = len-strlen(headers[i]);  
        while(ll)  
        {  
            //KTV_SHIYI_printf(" \n");  
            --ll;  
        }  
	 	//WV_printf("[%d]%s: %s\n",i, headers[i], values[i]);
		memcpy(srcbuf,values[i],strlen(values[i]));

		if(utf8togb2312((WV_S8 *)srcbuf,strlen((WV_S8 *)srcbuf),(WV_S8 *)destbuf,sizeof(destbuf)) != 0)
		{
			return WV_EFAIL;
		}
         //SVR_CONTROL_printf("[%d]%s: %s\n",i, headers[i], values[i]);
			 	//根据歌曲类别查找视频
		
		if(SVR_CONTROL_GetKtvChangeMode() == 1 )     //1:背景视频随动 
		{
			TSK_CONF_changeMovByType((WV_S8 *)destbuf);
		}else if(SVR_CONTROL_GetKtvChangeMode() == 2 )//2:场景随动
		{
			TSK_CONF_changeSceneByType(destbuf);
		}
		return WV_SOK;  
    }    
     //SVR_CONTROL_printf("\n");  
    return WV_EFAIL;  
} 

/****************************************************************************

WV_S32 KTV_SHIYI_ChangeMovByCmd(WV_U8 *pID)

功能：根据id或者name切换对应歌曲类型的视频
参数：pID是歌曲id(固定7个字节)，pName(最多8个字节)
返回值：成功返回0，不成功返回-1
****************************************************************************/ 
WV_S32 KTV_SHIYI__ChangeMovByCmd(WV_U8 *pID)
{

	
	//if id exist ,use id
	if(SVR_CONTROL_GetKtvChangeMode()==0 )
	{
		 KTV_SHIYI_printf("shiyi get change mode=0\n");
		 return WV_EFAIL;
	}

	//根据歌曲ID切换对应类别的视频,查询数据库
	WV_S8 sql[128];
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select %s from %s where ID = '%s' ",KTV_SHIYI_DB_SONGCLASS_NAME,KTV_SHIYI_DB_TABLE_NAME,pID);
#if TEST_DEBUG
	printf("sql:%s\n",sql);	
#endif
	WV_SQLITE_SearchVal(KTV_SHIYI_DB_FILE_PATH ,sql,KTV_SHIYI_callback_DB);

	return WV_SOK;
}

/********************************************************

WV_S32 KTV_SHIYI_Recv(WV_S32 mSockfd,WV_U8 *pData,WV_U32 *pLen);

*********************************************************/ 
WV_S32 KTV_SHIYI_Recv(WV_S32 mSockfd,WV_U8 *pData,WV_U32 *pLen)
{
	KTV_SHIYI_HEAD_E  headBuf;
	WV_U32 dataLen;
	int i=0;
	//

	memset(&headBuf,0,sizeof(KTV_SHIYI_HEAD_E));
	//printf("recv start..\n");
	if(recv(mSockfd,(WV_S8*)&headBuf,sizeof(KTV_SHIYI_HEAD_E),0) != sizeof(KTV_SHIYI_HEAD_E))
    {
		if(errno==EAGAIN)
		{	
			//KTV_SHIYI_printf("recv time out\n");
			return TIMEOUT;
		}   
        KTV_SHIYI_printf("recv head() error[%d]\n",errno);
  
	   return WV_EFAIL;
    }

	memcpy(pData,&headBuf,sizeof(KTV_SHIYI_HEAD_E));
	if(headBuf.msgLen == 0){
		*pLen = sizeof(KTV_SHIYI_HEAD_E);	

#if TEST_DEBUG
		printf("\n---------------recv cmd -------------------\n");
		for(i=0;i<*pLen;i++)
		{
			if(i%10 == 0) printf("\n");
			printf("%02x ",pData[i]);
		}
		printf("\n-----------------------------------------\n");
#endif
		return WV_SOK;
	}	

	KTV_SHIYI_printf("recv palode() start...\n");
	dataLen=recv(pShiyiDev->mSocket,(WV_S8*)&pData[sizeof(KTV_SHIYI_HEAD_E)],KTV_SHIYI_LOADBUF_MAX_LEN,0);
	//if(dataLen != ntohl(headBuf.msgLen))
	if(dataLen != headBuf.msgLen)    
	{
        KTV_SHIYI_printf("recv palode() error\n");
		*pLen=0;
        return WV_EFAIL;
    }

	*pLen = dataLen + sizeof(KTV_SHIYI_HEAD_E);

#if TEST_DEBUG
		printf("\n---------------recv cmd -------------------\n");
		for(i=0;i<*pLen;i++)
		{
			if(i%10 == 0) printf("\n");
			printf("%02x ",pData[i]);
		}
		printf("\n-----------------------------------------\n");
#endif

	return WV_SOK;
		
}

/********************************************************

WV_S32 KTV_SHIYI_Send(WV_S32 mSockfd,WV_U8 *pData,WV_U32 len);

*********************************************************/ 
WV_S32 KTV_SHIYI_Send(WV_S32 mSockfd,WV_U8 *pData,WV_U32 len)
{
	WV_S32 num;
	if((num=send(mSockfd,pData,len,0))==-1){
        KTV_SHIYI_printf("send() error\n");
        return  WV_EFAIL;
    }
#if TEST_DEBUG
	int i;
	printf("--------------send cmd---------------------------");
	for(i=0;i<len;i++)
	{
		if(i%10 == 0) printf("\n");
		printf("%02x ",pData[i]);
	}
	printf("\n--------------------------------------------\n");
#endif
	if(num != len)
	{
		KTV_SHIYI_printf("send %d data,success %d data,error\n",len,num);
		return WV_EFAIL;
	}
	return  WV_SOK;
}

/********************************************************

WV_S32 KTV_SHIYI_GetCmd(WV_U8 *pBuf);

*********************************************************/ 
WV_S32 KTV_SHIYI_GetCmd(WV_U8 *pBuf)
{
	WV_U32 dataLen=0;
	WV_S8 *p;
	WV_S32 ret;
	ret=KTV_SHIYI_Recv(pShiyiDev->mSocket,pBuf,&dataLen);

	if(ret !=0 )
	{
		if(ret == TIMEOUT)
		{
			KTV_SHIYI_printf(" get cmd time out ret=[%d] !\n",ret);
		}else{
			KTV_SHIYI_printf(" get cmd error ret=[%d]!\n",ret);
		}
		return ret;
	}

	if(dataLen < sizeof(KTV_SHIYI_HEAD_E)) return WV_EFAIL;

	p=(WV_S8 *)&pBuf[sizeof(KTV_SHIYI_HEAD_E)];
#if TEST_DEBUG
	printf("get cmd :%s \n",p);
#endif	
	
	if(strstr(p,"opened") != NULL){
		//opened dev
		TSK_SCENE_StartingUP(TSK_SCENE_TYPE_NETDATA);
		NET_UART_ProjectorCmd(1);
		//
	}else if(strstr(p,"closed") != NULL){
		TSK_SCENE_Standby(TSK_SCENE_TYPE_NETDATA);
		NET_UART_ProjectorCmd(0);	
		//closed dev	
	}else if((p=strstr(p,"start")) != NULL){
		//change mov
		p=&p[strlen("start")+1];
#if TEST_DEBUG
		printf("video info:%s\n",p);
#endif
		
		if((p=strstr(p,"mediaId")) == NULL) return WV_EFAIL;
		if((p=strstr(p,":")) == NULL) return WV_EFAIL;
		if((p=strstr(p,"\"")) == NULL) return WV_EFAIL;
//#if TEST_DEBUG
		printf("id info:%s\n",p);
//#endif
		WV_U8 id[8+1]={0};
		WV_S32 i=0;
		for(i=0;i<9;i++)
		{
			if(p[i+1] < 0x30 || p[i+1] > 0x39) break;
			id[i]=p[i+1];
		}
#if TEST_DEBUG
		printf("get mediaID = [%s]\n",id);
#endif		
		KTV_SHIYI__ChangeMovByCmd(id);
	}	

	return WV_SOK;
}

/********************************************************

WV_S32 KTV_SHIYI_CheckErr();

*********************************************************/ 
WV_S32 KTV_SHIYI_CheckErr()
{

	WV_S32 ret=-1;
	WV_U32 totalLen;
	memset(pShiyiDev->pBuf,0,sizeof(KTV_SHIYI_HEAD_E) + KTV_SHIYI_LOADBUF_MAX_LEN);
	ret=KTV_SHIYI_Recv(pShiyiDev->mSocket,pShiyiDev->pBuf,&totalLen);
	if(ret != 0 )	
	{
		KTV_SHIYI_printf(" check()recv ask cmd error[%d] !\n",errno);
		return WV_EFAIL;
	}
	//"errorCode":0,
	//"errorDescription":"xxxxxxxx"
	WV_S8 *p=NULL;
	int i;
	for(i=0;i<totalLen;i++)
	{
		if(pShiyiDev->pBuf[i]=='{'){
			p=(WV_S8 *)&pShiyiDev->pBuf[i];
			break;
		}
	}
	if(p==NULL)
	{
		KTV_SHIYI_printf(" recv cmd error p=NULL!\n");		
		return WV_EFAIL;
	}
#if TEST_DEBUG		
	printf(" recv error msg:%s\n",p);
#endif
	p=strstr(p,"errorCode");
	if(p == NULL){
		KTV_SHIYI_printf(" check(errorCode) recv cmd error !\n");
		return WV_EFAIL;
	}
	if((p=strstr(p,":")) == NULL){
		KTV_SHIYI_printf(" check (errorCode :) recv cmd error!\n");
		return WV_EFAIL;
	}

	WV_S32 errorCode;
	
	sscanf(&p[1],"%d",&errorCode);
	if(errorCode != 0 ){

		if((p=strstr((WV_S8 *)pShiyiDev->pBuf,"errorDescription")) == NULL){

			KTV_SHIYI_printf(" check(errorDescription) recv cmd error !\n");
			return WV_EFAIL;				
		}
	
		if((p=strstr(p,":")) == NULL){
			KTV_SHIYI_printf(" check (errorDescription :) recv cmd error!\n");
			return WV_EFAIL;
		}

		KTV_SHIYI_printf(" errorCode=%d,Description:%s",errorCode,&p[1]);
		return WV_EFAIL;
	}

	return WV_SOK;
}


/********************************************************

WV_S32 KTV_SHIYI_SubscribeEvents(KTV_SHIYI_DEV_E  * pDev);
订阅事件
*********************************************************/ 
WV_S32 KTV_SHIYI_SubscribeEvents(KTV_SHIYI_DEV_E  * pDev)
{
	KTV_SHIYI_printf("....SubscribeEvents start ... !\n");
	WV_U32 headLen,typeLen,dataLen,totalLen;
	WV_S32 ret=-1;
	WV_S8 msg[1024]={0};
	sprintf(msg,KTV_SHIYI_SUBSCRIBE_DATA);

	headLen = sizeof(KTV_SHIYI_HEAD_E);
	typeLen = strlen(KTV_SHIYI_MSG_TYPE_SUBSCRIBE);
	dataLen = strlen(msg);
	totalLen = headLen+typeLen+dataLen+1;

	memset(pDev->pBuf,0,sizeof(KTV_SHIYI_HEAD_E) + KTV_SHIYI_LOADBUF_MAX_LEN);
	//set msg
	pDev->headBuf.msgLen = dataLen+typeLen+1;	
	memcpy(pDev->pBuf,(WV_U8 *)&pDev->headBuf,headLen);//cp head
	memcpy(&pDev->pBuf[headLen],KTV_SHIYI_MSG_TYPE_SUBSCRIBE,typeLen); //cp msg type
	memcpy(&pDev->pBuf[headLen+typeLen+1],msg,dataLen);//cp msg data
#if TEST_DEBUG
	printf("msgtype:%s\n",&pDev->pBuf[headLen]);
	printf("msg:%s\n",&pDev->pBuf[headLen+typeLen+1]);
#endif 
	ret=KTV_SHIYI_Send(pDev->mSocket,pDev->pBuf,totalLen);
	if(ret != 0 ){

		KTV_SHIYI_printf("SubscribeEvents cmd send error !\n");
		return WV_SOK;	
	}

	if(KTV_SHIYI_CheckErr() != 0 )
	{
		KTV_SHIYI_printf("SubscribeEvents cmd check error !\n");
		return WV_EFAIL;		
	}
	KTV_SHIYI_printf(".....SubscribeEvents ok ... !\n");
	return WV_SOK;

}
/********************************************************

WV_S32 KTV_SHIYI_Register(KTV_SHIYI_DEV_E  * pDev);
注册
*********************************************************/ 
WV_S32 KTV_SHIYI_Register(KTV_SHIYI_DEV_E  * pDev)
{
	KTV_SHIYI_printf("Register start ... !\n");
	WV_U32 headLen,typeLen,dataLen,totalLen;
	WV_S32 ret=-1;
	WV_S8 msg[1024]={0};
	WV_U8 mac[6]={0};
	SYS_IP_getMacInt(mac);
	sprintf(msg,KTV_SHIYI_REGISTMSG_DATA,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	//printf("register msg=%s\n",msg);
	headLen = sizeof(KTV_SHIYI_HEAD_E);
	typeLen = strlen(KTV_SHIYI_MSG_TYPE_REGISTER);
	dataLen = strlen(msg);
	totalLen = headLen+typeLen+dataLen+1;

	memset(pDev->pBuf,0,(sizeof(KTV_SHIYI_HEAD_E) + KTV_SHIYI_LOADBUF_MAX_LEN));
	//set msg
	pDev->headBuf.msgLen = dataLen+typeLen+1;
	memcpy(pDev->pBuf,(WV_U8 *)&pDev->headBuf,headLen);//cp head
	memcpy(&pDev->pBuf[headLen],KTV_SHIYI_MSG_TYPE_REGISTER,typeLen); //cp msg type
	memcpy(&pDev->pBuf[headLen+typeLen+1],msg,dataLen);//cp msg data

	ret=KTV_SHIYI_Send(pDev->mSocket,pDev->pBuf,totalLen);
	
	if(ret != 0 ){

		KTV_SHIYI_printf("Register cmd send error !\n");
		return WV_EFAIL;	
	}

	if(KTV_SHIYI_CheckErr() != 0 )
	{
		KTV_SHIYI_printf("Register cmd check error !\n");
		return WV_EFAIL;		
	}

	KTV_SHIYI_printf(".....KTV_SHIYI_Register ok....\n");

	return WV_SOK;
			
}
/********************************************************

WV_S32 KTV_SHIYI_UnRegister(KTV_SHIYI_DEV_E  * pDev);
反注册
*********************************************************/ 
WV_S32 KTV_SHIYI_UnRegister(KTV_SHIYI_DEV_E  * pDev)
{
	KTV_SHIYI_printf("....UnRegister start ... !\n");
	WV_U32 headLen,typeLen,dataLen,totalLen;
	WV_S32 ret=-1;
	headLen = sizeof(KTV_SHIYI_HEAD_E);
	typeLen = strlen(KTV_SHIYI_MSG_TYPE_UNREGISTER);
	dataLen = 0;
	totalLen = headLen+typeLen+dataLen+1;

	memset(pDev->pBuf,0,sizeof(KTV_SHIYI_HEAD_E) + KTV_SHIYI_LOADBUF_MAX_LEN);
	pDev->headBuf.msgLen = dataLen+typeLen+1;

	memcpy(pDev->pBuf,&pDev->headBuf,headLen);//cp head
	memcpy(&pDev->pBuf[headLen],KTV_SHIYI_MSG_TYPE_UNREGISTER,typeLen); //cp msg type
#if TEST_DEBUG
	printf("msgtype:%s\n",(WV_S8 *)&pDev->pBuf[headLen]);
	//printf("msg:%s\n",&pDev->pBuf[headLen+typeLen+1]);
#endif 
	//send msg
	ret=KTV_SHIYI_Send(pDev->mSocket,pDev->pBuf,totalLen);
	if(ret != 0 ){

		KTV_SHIYI_printf("UnRegister cmd send error !\n");
		return WV_SOK;	
	}
	memset(pDev->pBuf,0,sizeof(KTV_SHIYI_HEAD_E) + KTV_SHIYI_LOADBUF_MAX_LEN);
	ret=KTV_SHIYI_Recv(pDev->mSocket,pDev->pBuf,&totalLen);
	if(ret != 0 )	
	{
		KTV_SHIYI_printf("UnRegister recv ask cmd error !\n");
		return WV_EFAIL;
	}

	if(KTV_SHIYI_CheckErr() != 0 )
	{
		KTV_SHIYI_printf("UnRegister cmd check error !\n");
		return WV_EFAIL;		
	}

	KTV_SHIYI_printf(".....UnRegister ok ..... !\n");

	return WV_SOK;
}


/********************************************************

WV_S32 KTV_SHIYI_GetConf();

*********************************************************/ 
WV_S32 KTV_SHIYI_GetConf()
{
	KTV_SHIYI_GetServerConf();
	memcpy(pShiyiDev->headBuf.identification,"ECDNIP",6);
	pShiyiDev->headBuf.version[0] = 1;
	pShiyiDev->headBuf.version[1] = 0;
	pShiyiDev->headBuf.dialogID=3798;
	pShiyiDev->headBuf.msgLen = 0;
#if TEST_DEBUG
	printf("<---------shiyi conf---------->\n");
	printf("shiyi server ip:[%s]\n",pShiyiDev->svrIp);
	printf("shiyi server port:[%d]\n",pShiyiDev->svrPort);
	printf("shiyi identification :[%c%c%c%c%c%c]\n",pShiyiDev->headBuf.identification[0],pShiyiDev->headBuf.identification[1],\
													pShiyiDev->headBuf.identification[2],pShiyiDev->headBuf.identification[3],\
													pShiyiDev->headBuf.identification[4],pShiyiDev->headBuf.identification[5]);
	printf("shiyi version :[%d:%d]\n",pShiyiDev->headBuf.version[0],pShiyiDev->headBuf.version[1]);
	printf("shiyi dialogID :[%d]\n",pShiyiDev->headBuf.dialogID);
	printf("shiyi msgLen :[%d]\n",pShiyiDev->headBuf.msgLen);
	printf("<----------------------------->\n");
#endif
	return WV_SOK;	
}
/********************************************************

WV_S32 KTV_SHIYI_Init(KTV_SHIYI_DEV_E  * pDev);

*********************************************************/ 
WV_S32 KTV_SHIYI_Init(KTV_SHIYI_DEV_E  * pDev)
{

	KTV_SHIYI_printf("KTV_SHIYI_Init start\n");

	if((pDev->mSocket=socket(AF_INET, SOCK_STREAM, 0))==-1){  
	    KTV_SHIYI_printf("socket()error\n");
		return WV_EFAIL;   
	}

	//设置本地端口号
	/*
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(KTV_SHIYI_LOCALPORT);
	if(bind(pDev->mSocket,(struct sockaddr *)&client,sizeof(client)) != 0 ){
		KTV_SHIYI_printf("socket bind err\n");
		return WV_EFAIL;
	}*/

	struct sockaddr_in server;
   
	bzero(&server,sizeof(server));  
	server.sin_family= AF_INET;  
	server.sin_port = htons(pDev->svrPort);  
	server.sin_addr.s_addr =inet_addr(pDev->svrIp);  


	if(connect(pDev->mSocket,(struct sockaddr *)&server,sizeof(server))==-1){  
	   KTV_SHIYI_printf("connect()error,errno=%d\n",errno);
	   return WV_EFAIL;
	}
	//set time out
	struct timeval timeout={10,0};//5s
	setsockopt(pDev->mSocket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));	
	setsockopt(pDev->mSocket,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));	
	//KTV_SHIYI_printf("KTV_SHIYI_Connect ok\n");
	KTV_SHIYI_printf("KTV_SHIYI_Init ok\n");

	return WV_SOK;
}
/********************************************************

WV_S32 KTV_SHIYI_Connect(KTV_SHIYI_DEV_E  * pDev);

*********************************************************/ 
WV_S32 KTV_SHIYI_Connect(KTV_SHIYI_DEV_E  * pDev)
{

	struct sockaddr_in server;
   
	bzero(&server,sizeof(server));  
	server.sin_family= AF_INET;  
	server.sin_port = htons(pDev->svrPort);  
	server.sin_addr.s_addr =inet_addr(pDev->svrIp);  


	if(connect(pDev->mSocket,(struct sockaddr *)&server,sizeof(server))==-1){  
	   KTV_SHIYI_printf("connect()error,errno=%d\n",errno);
	   return WV_EFAIL;
	}
	//set time out
	struct timeval timeout={10,0};//5s
	setsockopt(pDev->mSocket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));	
	setsockopt(pDev->mSocket,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));	
	KTV_SHIYI_printf("KTV_SHIYI_Connect ok\n");
	return WV_SOK;

}
/********************************************************

WV_S32 KTV_SHIYI_DeInit();

*********************************************************/ 
WV_S32 KTV_SHIYI_DeInit()
{
	if(pShiyiDev -> mSocket >= 0)
	{
		close(pShiyiDev->mSocket);
		pShiyiDev->mSocket = -1;
	}
	return WV_SOK;
}

/********************************************************

void * KTV_SHIYI_Proc(void * prm);

*********************************************************/ 
void * KTV_SHIYI_Proc(void * prm)
{
	KTV_SHIYI_DEV_E  * pDev;
	pDev = (KTV_SHIYI_DEV_E *) prm; 
	pDev -> open   = 1;
	pDev -> close  = 0; 
	WV_S32 ret;
	while(pDev -> open  == 1)
	{
		
		if(KTV_SHIYI_Init(pDev) != 0 ){
			sleep(KTV_SHIYI_RE_REGIST_TIME); //延时10秒重新注册
			KTV_SHIYI_DeInit();
			continue;
		}
		
		if(KTV_SHIYI_Register(pDev) != 0 ){
			sleep(KTV_SHIYI_RE_REGIST_TIME);//延时10秒重新注册
			KTV_SHIYI_DeInit();
			sleep(1);			
			continue;
		}
		if(KTV_SHIYI_SubscribeEvents(pDev) != 0 ){
			sleep(KTV_SHIYI_RE_REGIST_TIME);//延时10秒重新订阅事件
			KTV_SHIYI_UnRegister(pDev);
			KTV_SHIYI_DeInit();
			sleep(1);
			continue;
		}

		while(pDev ->mSocket >=  0 && pDev -> open  == 1){
			
			ret=KTV_SHIYI_GetCmd( pDev->pBuf);
			
			if(ret == TIMEOUT ){  //time out
				continue;
			}else if(ret == -1){
				KTV_SHIYI_UnRegister(pDev);
				KTV_SHIYI_DeInit();
				sleep(1);
				break;				
			}
		
		}
		//链接断开 重新注册
		sleep(1);
		
	}

	pDev -> close  = 1; 
	KTV_SHIYI_UnRegister(pDev);
	KTV_SHIYI_DeInit();	 
	return NULL;
}

/********************************************************

WV_S32 KTV_SHIYI_Open();

*********************************************************/ 
WV_S32 KTV_SHIYI_Open()
{

	printf("KTV shiyi(3) Open\n");
	if(SVR_CONTROL_GetKtvDev() != 3)
	{
		printf("this is not new ktv_shiyi dev\n");
		return WV_EFAIL;
	}
	pShiyiDev = (KTV_SHIYI_DEV_E *)malloc(sizeof(KTV_SHIYI_DEV_E)); 
	memset(pShiyiDev,0,sizeof(KTV_SHIYI_DEV_E));
  	pShiyiDev->pBuf = (WV_U8 *)malloc(sizeof(KTV_SHIYI_HEAD_E) + KTV_SHIYI_LOADBUF_MAX_LEN);
	KTV_SHIYI_GetConf();
	//pShiyiDev->headBuf
	WV_CHECK_RET( WV_THR_Create(&(pShiyiDev->thrHndl),KTV_SHIYI_Proc, WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, (void *)pShiyiDev));
		
	return WV_SOK;
}
/********************************************************

WV_S32 KTV_SHIYI_Close();

*********************************************************/ 
WV_S32 KTV_SHIYI_Close()
{
	printf("KTV shiyi(3) Close\n");
	if(SVR_CONTROL_GetKtvDev() != 3)
	{
		printf("this is not new ktv_shiyi dev\n");
		return WV_EFAIL;
	}
	if(pShiyiDev->open == 1)
	{
		pShiyiDev->open = 0;
		while(pShiyiDev->close == 1);
		WV_THR_Destroy(&(pShiyiDev->thrHndl));
	}

	free(pShiyiDev->pBuf);
	free(pShiyiDev);

	return WV_SOK;
}

