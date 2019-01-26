#include "tsk_conf.h"
#include <sys/statfs.h>
#include <sys/stat.h>
#include <dirent.h>
#include "svr_control_cmd.h"
#include "tsk_player.h"
#define TSK_CONF_MOV_InfoFile         "./env/mov1.ini"
#define TSK_CONF_Scene_NameFile       "./env/scene.ini"

#define TSK_CONF_DISK_PATH            "/opt/app/"
#define TSK_CONF_MOV_COMPARE_FILE     "./mov/"
#define TSK_CONF_DISK_OBLIGATE        (256) //256 M
#define TSK_CONF_MOV_INFO_MAX_LEN     (1024*1024*2)
#define TSK_CONF_MOV_UPDATA_DATA_MAX_LEN 1024*1024*8
#define TSK_CONF_MOV_NAME_MAX_LEN      256        // 视频文件名称大小
#define TSK_CONF_MOV_MAX_NUM  (TSK_SCENE_MOV_FILE_MAX_NUM) //512
#define TSK_CONF_MOV_DEFAULT_NAME      "./mov/test.mp4"

#define TSK_CONF_GIF_PATH "./gif"
#define TSK_FADE_INOUT_FILE_PATH "./env/Fadeinout.txt"

/*
#define TSK_CONF_SCENE_COUNT  (TSK_SCENE_MAX_NUM )  //64             // 场景最大个数
#define TSK_CONF_SCENE_NAME_MAX_LEN     256                          // 场景名称大小               
#define TSK_CONF_SCENE_OUTLINE_MAX_NUM  (TSK_SCENE_WIN_MAX_NUM)//16  // OutLine最大个数
#define TSK_CONF_SCENE_BKPIC_MAX_NUM    (TSK_SCENE_GIF_MAX_NUM)//16  // 背景图片最大个数


#define TSK_CONF_MOV_TYPE_MAX_NUM  64
#define TSK_CONF_MOV_TYPE_NAME_LEN 8

#define TSK_CONF_SCENE_TYPE_MAX_NUM  64
#define TSK_CONF_SCENE_TYPE_NAME_LEN 8
*/

#pragma pack(push)
#pragma pack(1)

typedef struct TSK_CONF_SCENE_NAME
{
    WV_U8 name[TSK_CONF_SCENE_NAME_MAX_LEN];

}TSK_CONF_SCENE_NAME;

typedef struct TSK_CONF_SCENE_FILE
{
    TSK_CONF_SCENE_NAME sceneNameId[TSK_CONF_SCENE_COUNT];
}TSK_CONF_SCENE_FILE;

static TSK_CONF_SCENE_FILE *pSceneNameDev;
/**************scene *********************/
/*typedef struct TSK_SONF_SCENE_RECT_INFO
{// ID 位置
    WV_U16 usChannel;                                        // 通道 (0：A 1：B)
    WV_U16 usID;                                             // ID (1~16)
    WV_U16 usLeft;                                           // 左
    WV_U16 usTop;                                            // 上
    WV_U16 usRight;                                          // 右
    WV_U16 usBottom;                                         // 下

}TSK_SONF_SCENE_RECT_INFO;
typedef struct TSK_CONF_SCENE_MOV
{
    WV_U8 ucVideoFileName[TSK_CONF_SCENE_NAME_MAX_LEN];      //current scene mov name:视频文件名称 256
}TSK_CONF_SCENE_MOV;
typedef struct TSK_CONF_SCENE_CONF
{// 场景
    WV_U8 ucSceneId;                                               //scene id
    WV_U8 ucSceneName[TSK_CONF_SCENE_NAME_MAX_LEN];                        // 场景名称
    WV_U8  ucTypeName[TSK_CONF_SCENE_TYPE_NAME_LEN];                     // 类型名称
    TSK_CONF_SCENE_MOV   ucSceneMovName[4];            // 场景包含的视频列表
    WV_U16 u16SceneType;                                 //场景类型
    WV_U8 ucOutLineCount;                                    // OutLine个数
    WV_U8 ucBKFrameCount;                                    // 背景图片个数
    TSK_SONF_SCENE_RECT_INFO OutLine[TSK_CONF_SCENE_OUTLINE_MAX_NUM];                          // OutLine
    TSK_SONF_SCENE_RECT_INFO BKFrame[TSK_CONF_SCENE_BKPIC_MAX_NUM];                          // 背景图片

}TSK_CONF_SCENE_CONF;*/
////场景类型
typedef struct TSK_CONF_SCENE_TYPE
{
    WV_U8 u8TypeName[TSK_CONF_SCENE_TYPE_NAME_LEN];
}TSK_CONF_SCENE_TYPE;

typedef struct TSK_CONF_SCENE_INFO_E
{// 场景列表
    TSK_CONF_SCENE_CONF Scene[TSK_CONF_SCENE_COUNT];
    WV_U8 currentID; //当前场景ID
    TSK_CONF_SCENE_TYPE  SceneType[TSK_CONF_SCENE_TYPE_MAX_NUM];  //场景类型
}TSK_CONF_SCENE_INFO_E;

/**************************************************************************************/
typedef struct TSK_CONF_MOV_FILE
{
    WV_S8  name[TSK_CONF_MOV_NAME_MAX_LEN]; //视频名称
    WV_U8  typeName[TSK_CONF_MOV_TYPE_NAME_LEN]; // 类型名称
    WV_U16 dataLen;               //视频大小（M）
    WV_U16 u16Type;               //视频类型
    WV_U8  u8FirstUse;            //第一幕使用
    WV_U8  u8SecondUse;           //第二幕使用
}TSK_CONF_MOV_FILE;

typedef struct TSK_CONF_MOV_TYPE
{
    WV_U8 u8TypeName[TSK_CONF_MOV_TYPE_NAME_LEN];
}TSK_CONF_MOV_TYPE;

typedef struct TSK_CONF_MOV_INFO_E
{
    WV_U16 diskTotal;
    WV_U16 diskFree;
    TSK_CONF_MOV_FILE movFile[TSK_CONF_MOV_MAX_NUM];
    TSK_CONF_MOV_TYPE movType[TSK_CONF_MOV_TYPE_MAX_NUM];
}TSK_CONF_MOV_INFO_E;

typedef struct TSK_MOV_UPDATA_E
{
    WV_S32 movId;
    WV_S32 flag;  // 0:not update;1:update first package ;2:other package
    WV_U16 dataLen;
    WV_U16 nameLen;
    WV_S8 nameBuf[TSK_CONF_MOV_NAME_MAX_LEN];
    FILE *fp;
}TSK_MOV_UPDATA_E;


typedef struct TSK_MOV_TYPE_INFO
{
    WV_U8  typeEna;
    WV_S8  ucTypeName[TSK_CONF_MOV_TYPE_NAME_LEN];
    WV_S32 u32TotalNum;
    WV_S32 u32LocalNum;
}TSK_MOV_TYPE_INFO;


typedef struct TSK_CONF_PLAYERHANDLE{
    WV_U32 handle;
    WV_S32 movID;

}TSK_CONF_PLAYERHANDLE;

#pragma pack(pop)
static TSK_MOV_TYPE_INFO gMovTypeInfo[TSK_CONF_MOV_TYPE_MAX_NUM];

//static TSK_MOV_TYPE_INFO gSceneTypeInfo[TSK_CONF_SCENE_TYPE_MAX_NUM];

static TSK_CONF_SCENE_INFO_E *pSceneInfoDev;
static TSK_CONF_MOV_INFO_E *pMovInfoDev;

static TSK_MOV_UPDATA_E gMovUpdata;

static WV_S8 gCurrentTypeName[TSK_CONF_MOV_TYPE_NAME_LEN]={0};
static int gMovInfoInited=0;
static int gSceneInfoInited = 0;

/********************************************************************

WV_S32 TSK_CONF_getMovTypeNum(); 
查询每个歌曲类型有多少个mov，然后进行循环切换
********************************************************************/
WV_S32 TSK_CONF_getMovTypeNum()
{

    memset(&gMovTypeInfo,0,sizeof(gMovTypeInfo));
    WV_S32 i,j;
    for(j=0;j<TSK_CONF_MOV_MAX_NUM;j++)
    {

        if(	pMovInfoDev->movFile[j].dataLen > 0)
        {

            for(i=0;i<	TSK_CONF_MOV_TYPE_MAX_NUM;i++)
            {

                if(gMovTypeInfo[i].typeEna == 1)
                {
                    if(strncmp((WV_S8 *)gMovTypeInfo[i].ucTypeName,(WV_S8 *)pMovInfoDev->movFile[j].typeName,TSK_CONF_MOV_TYPE_NAME_LEN) == 0 )
                    {
                        pMovInfoDev->movFile[j].u16Type = i;
                        gMovTypeInfo[i].u32TotalNum++;
                        break;
                    }


                }else{

                    memcpy(gMovTypeInfo[i].ucTypeName,pMovInfoDev->movFile[j].typeName,TSK_CONF_MOV_TYPE_NAME_LEN);
                    pMovInfoDev->movFile[j].u16Type = i;
                    gMovTypeInfo[i].u32TotalNum++;
                    gMovTypeInfo[i].typeEna = 1;
                    gMovTypeInfo[i].u32LocalNum = 1;
                    break;
                }
            }


        }
    }
#if 1
    WV_printf("--------------------------------------------------------\n");
    for(i=0;i<TSK_CONF_MOV_TYPE_MAX_NUM;i++)
    {
        if(gMovTypeInfo[i].typeEna == 1)
        {
            WV_printf("type[%d],totalNum[%d],typeName=[%s] \n",i,gMovTypeInfo[i].u32TotalNum,gMovTypeInfo[i].ucTypeName);
        }
    }
    WV_printf("--------------------------------------------------------\n");
#endif

    return WV_SOK;
}
/********************************************************************

WV_S32 TSK_CONF_saveMovType(WV_U8 *pType,WV_U32 dataLen); 

********************************************************************/
WV_S32 TSK_CONF_saveMovType(WV_U8 *pType,WV_U32 dataLen)
{
    //TSK_CONF_MOV_INFO_E *movType = (TSK_CONF_MOV_INFO_E *) pType;

    FILE *fp;
    WV_U32 writeLen;
    fp=fopen(TSK_CONF_MOV_InfoFile,"wb");
    if(fp == NULL)
    {

        return WV_EFAIL;
    }

    writeLen = fwrite(pType,1,dataLen,fp);

    if(writeLen != dataLen)
    {

        fclose(fp);
        return WV_EFAIL;
    }
    fclose(fp);

    memcpy(pMovInfoDev,pType,sizeof(TSK_CONF_MOV_INFO_E));

    TSK_CONF_getMovTypeNum();

    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_GetNextTypeMovID(WV_S32 movID); 
//根据当前场景的视频id，查询下一个相同类别的视频，返回视频查询到的视频id，
********************************************************************/
WV_S32 TSK_CONF_GetNextTypeMovID(WV_S32 movID)
{
    WV_S32 nextSameTypeMovID=-1;
    WV_S32 i,j=0;
    WV_U16 u16Type=0xffff;

    for(i=0;i<TSK_CONF_MOV_TYPE_MAX_NUM;i++)
    {

        if(gMovTypeInfo[i].typeEna == 0 ) continue;
        //printf("ucTypeName is %s\n",gMovTypeInfo[i].ucTypeName);
        if(strncmp((WV_S8 *)pMovInfoDev->movFile[movID].typeName,(WV_S8 *)gMovTypeInfo[i].ucTypeName,TSK_CONF_MOV_TYPE_NAME_LEN) == 0)
        {
            u16Type=i;
            //memcpy(gCurrentTypeName,pType,TSK_CONF_MOV_TYPE_NAME_LEN);
            break;
        }
    }

    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        if((pMovInfoDev->movFile[i].dataLen > 0) && (pMovInfoDev->movFile[i].u16Type == u16Type))
        {

            if(j<gMovTypeInfo[u16Type].u32LocalNum)
            {
                j++;
                continue;
            }

            //WV_printf("Get nextSameType mov ID %d \n",i);
            nextSameTypeMovID = i;
            gMovTypeInfo[u16Type].u32LocalNum ++;
            if(gMovTypeInfo[u16Type].u32LocalNum >=gMovTypeInfo[u16Type].u32TotalNum)
            {
                gMovTypeInfo[u16Type].u32LocalNum = 0 ;
            }
            break;
        }
    }

    return nextSameTypeMovID;
}
/********************************************************************

WV_S32 TSK_CONF_getMovType(WV_S32 movId,WV_S8 *pType); 

********************************************************************/
WV_S32 TSK_CONF_getMovType(WV_S32 movId,WV_S8 *pType)
{
    memcpy(pType, pMovInfoDev->movFile[movId].typeName,TSK_CONF_MOV_TYPE_NAME_LEN);

    return WV_SOK;
}
/********************************************************************

WV_S32 TSK_CONF_changeMovByType(WV_S8 *pType); 

********************************************************************/
WV_S32 TSK_CONF_changeMovByType(WV_S8 *pType)
{

    if(TSK_SCENE_GetSceneLockStatus() == 1){ //如果当前场景锁定，则只能播放相同类别的视频
        
        WV_S8 type[TSK_CONF_MOV_TYPE_NAME_LEN+1]={0};
        TSK_CONF_getMovType(TSK_SCENE_GetSceneCurId(),type);
        if(strncmp(pType,type,TSK_CONF_MOV_TYPE_NAME_LEN) != 0 ){
            WV_ERROR("当前场景锁定，播放视频类别错误\n");
            return WV_SOK;
        }
    }

    WV_S32 i,j=0;
    WV_U16 u16Type=0xffff;

    for(i=0;i<TSK_CONF_MOV_TYPE_MAX_NUM;i++)
    {

        if(gMovTypeInfo[i].typeEna == 0 ) continue;
        //printf("ucTypeName is %s\n",gMovTypeInfo[i].ucTypeName);
        if(strncmp(pType,gMovTypeInfo[i].ucTypeName,TSK_CONF_MOV_TYPE_NAME_LEN) == 0)
        {
            u16Type=i;
            memcpy(gCurrentTypeName,pType,TSK_CONF_MOV_TYPE_NAME_LEN);
            break;
        }
    }

    if(u16Type == 0xffff)
    {
        return WV_EFAIL;
    }

    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        if((pMovInfoDev->movFile[i].dataLen > 0) && (pMovInfoDev->movFile[i].u16Type == u16Type))
        {

            if(j<gMovTypeInfo[u16Type].u32LocalNum)
            {
                j++;
                continue;
            }

            TSK_SCENE_SetMov(0,i);
            TSK_SCENE_ConfMov(0);
            TSK_SCENE_SetMov(1,i);
            TSK_SCENE_ConfMov(1);

            gMovTypeInfo[u16Type].u32LocalNum ++;
            if(gMovTypeInfo[u16Type].u32LocalNum >=gMovTypeInfo[u16Type].u32TotalNum)
            {
                gMovTypeInfo[u16Type].u32LocalNum = 0 ;
            }
            break;
        }
    }

    TSK_SCENE_SendSync();
    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_changeMovRollType(WV_U32 handle); 

********************************************************************/
WV_S32 TSK_CONF_changeMovRollType(WV_U32 handle)
{
    if(gMovInfoInited == 0) return WV_EFAIL;
    WV_S32 ret=-1;
    WV_S32 movID=-1;
    movID=TSK_SCENE_GetMovIDByplayerHandle(handle);
    if(movID >512 || movID <0 ) return ret;
    WV_S8 pType[TSK_CONF_MOV_TYPE_NAME_LEN+1]={0};

    memcpy(pType,pMovInfoDev->movFile[movID].typeName,TSK_CONF_MOV_TYPE_NAME_LEN);

    WV_S32 i,j=0;
    WV_U16 u16Type=0xffff;

    for(i=0;i<TSK_CONF_MOV_TYPE_MAX_NUM;i++)
    {

        if(gMovTypeInfo[i].typeEna == 0 ) continue;
        if(strncmp(pType,gMovTypeInfo[i].ucTypeName,TSK_CONF_MOV_TYPE_NAME_LEN) == 0)
        {
            u16Type=i;
            memcpy(gCurrentTypeName,pType,TSK_CONF_MOV_TYPE_NAME_LEN);
            break;
        }
    }
    if(u16Type == 0xffff)
    {
        return WV_EFAIL;
    }

    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        if((pMovInfoDev->movFile[i].dataLen > 0) && (pMovInfoDev->movFile[i].u16Type == u16Type))
        {
            //WV_printf("get movFile[%d].type = %d \n",i,u16Type);

            if(j<gMovTypeInfo[u16Type].u32LocalNum)
            {
                j++;
                continue;
            }
            WV_S32 player2Ena=0;
            ret = TSK_SCENE_ChangeMovByPlayerHandle(handle,i);
            //printf("player[0] handle=%d\n",handle);
            if(TSK_UART_GetTypeRound() == 2)//如果==2 ，第二幕视频跟随第一幕视频播放
            {
                WV_S32 handle2;
                handle2=TSK_PLAYER_GetPlayerHandleByID(1);
                if(handle2 != -1 ){
                    //printf("player[1] handle=%d\n",handle2);
                    TSK_SCENE_ChangeMovByPlayerHandle(handle2,i);
                    player2Ena = 1;
                }
            }
            if(player2Ena == 1)//如果==2 ，第二幕视频跟随第一幕视频播放
            {
                TSK_PLAYER_Seek(0,0);
                TSK_PLAYER_Seek(1,60);
            }

            gMovTypeInfo[u16Type].u32LocalNum ++;
            if(gMovTypeInfo[u16Type].u32LocalNum >=gMovTypeInfo[u16Type].u32TotalNum)
            {
                gMovTypeInfo[u16Type].u32LocalNum = 0 ;
            }
            break;
        }
    }

    return ret;

}

/********************************************************************

WV_S32 TSK_CONF_GetFileSize(const WV_S8 *path); 

********************************************************************/
WV_U16 TSK_CONF_GetFileSize(const WV_S8 *path)
{

    WV_U16 fileSize = -1;
    struct stat statbuff;
    if(stat(path,&statbuff) < 0)
    {
        return fileSize;
    }else{
        fileSize = statbuff.st_size/(1024*1024);
    }
    return fileSize;
}
/********************************************************************

WV_S32 TSK_CONF_GetResolution(WV_U8 *pData,WV_U32 *pDataLen); 

********************************************************************/
WV_S32 TSK_CONF_GetResolution(WV_U8 *pData,WV_U32 *pDataLen)
{
    WV_U32 data=0;
    WV_U16 *pBuf=(WV_U16 *)pData;
    SYS_ENV_GetU32("Output_fRate",&data);
    pBuf[0] = data;

    data = 0;
    SYS_ENV_GetU32("Output_totalPix",&data);
    pBuf[1] = data;

    data = 0;
    SYS_ENV_GetU32("Output_totalLine",&data);
    pBuf[2] = data;

    data = 0;
    SYS_ENV_GetU32("Output_hsyncWidth",&data);
    pBuf[3] = data;

    data = 0;
    SYS_ENV_GetU32("Output_vsyncWidth",&data);
    pBuf[4] = data;

    data = 0;
    SYS_ENV_GetU32("Output_actHstart",&data);
    pBuf[5] = data;

    data = 0;
    SYS_ENV_GetU32("Output_actHsize",&data);
    pBuf[6] = data;

    data = 0;
    SYS_ENV_GetU32("Output_actVstart",&data);
    pBuf[7] = data;

    data = 0;
    SYS_ENV_GetU32("Output_actVsize",&data);
    pBuf[8] = data;

    *pDataLen = 18;

    return 0;
}
/********************************************************************

WV_S32 TSK_CONF_GetSplit(WV_U8 *pData,WV_U32 *pDataLen); 

********************************************************************/
WV_S32 TSK_CONF_GetSplit(WV_U8 *pData,WV_U32 *pDataLen)
{

    WV_U32 data;
    WV_U16 *pBuf=(WV_U16 *)pData;
    // get output ena
    WV_S8 name[20];
    data = 0;
    SYS_ENV_GetU32("Output_ena",&data);

    pBuf[0] = data<<8 | data>>8 ;

    //get mov x y w h
    WV_U32 i,ena,x,y,w,h;
    for(i=0;i<4;i++)
    {
        TSK_SCENE_GetMovArea(i,&ena,&x,&y ,&w,&h);

        if(ena == 0)
        {
            pBuf[i*4+1] = 0;
            pBuf[i*4+2] = 0;
            pBuf[i*4+3] = 0;
            pBuf[i*4+4] = 0;

            continue;
        }

        pBuf[i*4+1] = x;
        pBuf[i*4+2] = y;
        pBuf[i*4+3] = w;
        pBuf[i*4+4] = h;

    }
    for(i=0;i<8;i++)
    {
        data = 0;
        sprintf(name,"Split_startX%d",i);
        SYS_ENV_GetU32(name,&data);
        pBuf[17+i*4] = data;

        data = 0;
        sprintf(name,"Split_endX%d",i);
        SYS_ENV_GetU32(name,&data);
        pBuf[18+i*4] = data;

        data = 0;
        sprintf(name,"Split_startY%d",i);
        SYS_ENV_GetU32(name,&data);
        pBuf[19+i*4] = data;

        data = 0;
        sprintf(name,"Split_endY%d",i);
        SYS_ENV_GetU32(name,&data);
        pBuf[20+i*4] = data;

    }

    *pDataLen = 98;
    return 0;
}

/********************************************************************

WV_S32 TSK_CONF_GetMem(WV_U16 *pTotalDisk,WV_U16 *pFreeDisk); 

********************************************************************/
WV_S32 TSK_CONF_GetMem(WV_U16 *pTotalDisk,WV_U16 *pFreeDisk)
{
    struct statfs diskInfo;
    WV_U32 blocksize;
    statfs(TSK_CONF_DISK_PATH,&diskInfo);

    blocksize = diskInfo.f_bsize/1024;

    *pTotalDisk = ((blocksize * (diskInfo.f_blocks)))/1024;
    if(((blocksize * (diskInfo.f_bavail)))/1024 >=  TSK_CONF_DISK_OBLIGATE)
    {
        *pFreeDisk = ((blocksize * (diskInfo.f_bavail)))/1024 - TSK_CONF_DISK_OBLIGATE;
    }else{

        *pFreeDisk = ((blocksize * (diskInfo.f_bavail)))/1024;
    }
    return 0;
}

/********************************************************************

WV_S32 TSK_CONF_MovSaveConf(); 

********************************************************************/
WV_S32 TSK_CONF_MovSaveConf()
{

    WV_S32 ret;
    WV_S8 *pBuf = (WV_S8 *)pMovInfoDev;
    WV_U16 totalDisk=0;
    WV_U16 freeDisk=0;


    //access
    ret = access(TSK_CONF_MOV_InfoFile,W_OK);

    if(ret != 0){
        WV_printf("access file [%s] error\n",TSK_CONF_MOV_InfoFile);
        return -1;
    }

    FILE *fp;
    //open mov.ini
    fp = fopen(TSK_CONF_MOV_InfoFile,"wb+");
    if(fp == NULL)
    {
        WV_printf("fopen file [%s] error\n",TSK_CONF_MOV_InfoFile);
        return -1;
    }

    TSK_CONF_GetMem(&totalDisk,&freeDisk);
    pMovInfoDev->diskTotal = totalDisk;
    pMovInfoDev->diskFree = freeDisk;

    WV_S32 writeLen=0;
    writeLen = fwrite(pBuf,1,sizeof(TSK_CONF_MOV_INFO_E),fp);

    fclose(fp);

    TSK_CONF_getMovTypeNum();

    return WV_SOK;
}



/********************************************************************

WV_S32 TSK_CONF_MovGetConf(); 

********************************************************************/
WV_S32 TSK_CONF_MovGetConf()
{

    WV_S32 ret;
    //WV_S8 *pBuf = (WV_S8 *)pMovInfoDev;
    TSK_CONF_MOV_INFO_E movInfo;
    WV_U16 totalDisk=0;
    WV_U16 freeDisk=0;
    FILE *fp;
    //access
    ret = access(TSK_CONF_MOV_InfoFile,R_OK);

    if(ret != 0){
        WV_printf("access file [%s] error\n",TSK_CONF_MOV_InfoFile);
        return -1;
    }

    fp = fopen(TSK_CONF_MOV_InfoFile,"rb+");
    if(fp == NULL)
    {
        WV_printf("fopen file [%s] error\n",TSK_CONF_MOV_InfoFile);
        return -1;
    }
    //open mov.ini

    WV_S32 readLen=0;

    TSK_CONF_GetMem(&totalDisk,&freeDisk);
    pMovInfoDev->diskTotal = totalDisk;
    pMovInfoDev->diskFree = freeDisk;

    readLen = fread(&movInfo,1,sizeof(TSK_CONF_MOV_INFO_E),fp);

    if(readLen != sizeof(TSK_CONF_MOV_INFO_E))
    {	//如果配置文件长度跟结构体长度不一致，则使用默认配置
        WV_printf("error read mov.ini len[%d],struct TSK_CONF_MOV_INFO_E len[%d]\n",readLen,sizeof(TSK_CONF_MOV_INFO_E));
        ret = -1;
    }else {
        memcpy(pMovInfoDev,&movInfo,sizeof(TSK_CONF_MOV_INFO_E));
        ret = 0;
    }
    fclose(fp);


    return ret;
}


/********************************************************************

WV_S32 TSK_CONF_SetUpdateMovType(WV_U8 dataLen,WV_U8 *pType) ; 
设置正在升级的mov视频的类型
dataLen:类型长度(最多8个字节=四个汉字)
pType:类型名称
********************************************************************/
WV_S32 TSK_CONF_SetUpdateMovType(WV_U32 movID,WV_U8 dataLen,WV_U8 *pType) 
{

    if(dataLen > 8 ) return WV_EFAIL;
    memset(pMovInfoDev->movFile[movID].typeName,0,8);
    memcpy(pMovInfoDev->movFile[movID].typeName,pType,dataLen);
    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_UpdateMov(WV_U8 *pData ,WV_U32 dataLen); 

********************************************************************/
#if 1
WV_S32 TSK_CONF_UpdateMov(WV_U8 *pData ,WV_U32 dataLen)
{
    WV_S32 ret = -1;
    WV_U8 *pTemp=pData;
    WV_U32 len = dataLen;
    if(gMovUpdata.flag == 0){
        WV_printf("updata flag is error[%d]\n",gMovUpdata.flag);
        return -1;
    }else if(gMovUpdata.flag == 1)
    {
        WV_printf("update mov first pack \n");
        if(pTemp[0] == 'H' && \
                pTemp[6] == 'I' && \
                pTemp[12] == 'S' && \
                pTemp[18] == '3' && \
                pTemp[24] == '7' && \
                pTemp[30] == '9' && \
                pTemp[36] == '8' && \
                pTemp[42] == 'M' ) ////视频加密会在第0,6 、12 、18、24,30,36,42 个字节处分别填写HIS3798M
        {

            ret = TSK_CONF_SetUpdateMovType(gMovUpdata.movId,pTemp[43],&pTemp[44]);//设置视频类型，第43个字节为视频类型长度，44个字节开始至对应长度后面为视频类型
            if(ret != 0 )
            {
                fclose(gMovUpdata.fp);
                remove(TSK_CONF_MOV_DEFAULT_NAME);
                gMovUpdata.flag = 0;
                return WV_EFAIL;
            }

            if(len > 1024)//视频加密包头长度为1024
            {
                WV_printf("mov is encrypt \n");
                pTemp=pTemp+1024;
                len=len-1024;
            }else{
                WV_printf("get mov buf len [%d] < 1024 \n",len);

                fclose(gMovUpdata.fp);
                remove(TSK_CONF_MOV_DEFAULT_NAME);
                gMovUpdata.flag = 0;

                return WV_EFAIL;
            }
        }

        gMovUpdata.flag = 2;
    }
    WV_S32 writeLen;
    writeLen = fwrite(pTemp,1,len,gMovUpdata.fp);
    if(writeLen != len){
        WV_printf("error: writeLen [%d],dataLen[%d]\n",writeLen,len);
        fclose(gMovUpdata.fp);
        remove(TSK_CONF_MOV_DEFAULT_NAME);
        gMovUpdata.flag = 0;
        return WV_EFAIL;

    }


    return WV_SOK;
}
#endif


/********************************************************************

WV_S32 TSK_CONF_UpdateMovName(); 

********************************************************************/
#if 1
WV_S32 TSK_CONF_UpdateMovName(WV_U8 *pData ,WV_U32 dataLen)
{
    WV_S32 i,ret;

    gMovUpdata.flag = 0;
    gMovUpdata.dataLen = 0;
    memset(gMovUpdata.nameBuf,0,TSK_CONF_MOV_NAME_MAX_LEN);
    memcpy(gMovUpdata.nameBuf,pData,dataLen);
    gMovUpdata.nameLen = dataLen;
    gMovUpdata.fp = fopen(TSK_CONF_MOV_DEFAULT_NAME,"wb");
    if(gMovUpdata.fp == NULL)
    {
        return -1;
    }

    //replace local mov
    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        ret = strncmp((WV_S8 *)pMovInfoDev->movFile[i].name,(WV_S8 *)pData,dataLen);
        if(ret == 0 ){
            gMovUpdata.movId = i;
            gMovUpdata.flag = 1;
            return ret;
        }
    }
    //add mov
    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        if(	pMovInfoDev->movFile[i].dataLen == 0)
        {
            gMovUpdata.movId = i;
            gMovUpdata.flag = 1;
            return 0;
        }

    }

    return  -1;
}
#endif

/********************************************************************

WV_S32 TSK_CONF_UpdataMovEnd(); 

********************************************************************/
#if 1
WV_S32 TSK_CONF_UpdataMovEnd(WV_U8 *pData ,WV_U32 dataLen)
{


    WV_U8 *pTemp=pData;
    WV_U32 len = dataLen;

    WV_S32 ret, writeLen=0;
    struct stat movStat;
    if(gMovUpdata.flag == 0){
        WV_printf("updata flag is error[%d]\n",gMovUpdata.flag);
        return -1;
    }else if(gMovUpdata.flag == 1)
    {
        WV_printf("update end mov first pack \n");
        if(pTemp[0] == 'H' && \
                pTemp[6] == 'I' && \
                pTemp[12] == 'S' && \
                pTemp[18] == '3' && \
                pTemp[24] == '7' && \
                pTemp[30] == '9' && \
                pTemp[36] == '8' && \
                pTemp[42] == 'M' ) ////视频加密会在第6 、12 、18、24 个字节处分别填写3798
        {

            ret = TSK_CONF_SetUpdateMovType(gMovUpdata.movId,pTemp[43],&pTemp[44]);//设置视频类型，第43个字节为视频类型长度，44个字节开始至对应长度后面为视频类型
            if(ret != 0 )
            {
                fclose(gMovUpdata.fp);
                remove(TSK_CONF_MOV_DEFAULT_NAME);
                gMovUpdata.flag = 0;
                return WV_EFAIL;
            }

            if(len > 1024)   //视频加密包头长度为1024
            {
                WV_printf("mov is encrypt \n");
                pTemp=pTemp+1024;
                len=len-1024;
            }else{
                WV_printf("get mov buf len [%d] < 1024 \n",len);
                fclose(gMovUpdata.fp);
                remove(TSK_CONF_MOV_DEFAULT_NAME);
                gMovUpdata.flag = 0;
                return WV_EFAIL;
            }
//        }else if(SVR_CONTROL_GetKtvDev() == 0 ){ //ktvDev == 0 是雷石点歌机，雷石点歌机不支持非加密视频，返回-1
        }else if((SVR_CONTROL_GetKtvDev() == 0) || (SVR_CONTROL_GetKtvDev() == 4)){ //ktvDev == 0 是雷石点歌机，雷石点歌机不支持非加密视频，返回-1
            fclose(gMovUpdata.fp);
            remove(TSK_CONF_MOV_DEFAULT_NAME);
            gMovUpdata.flag = 0;
            return WV_EFAIL;
        }
        gMovUpdata.flag = 2;
    }


    if(len != 0)
    {
        writeLen = fwrite(pTemp,1,len,gMovUpdata.fp);
        if(writeLen != len)
        {
            fclose(gMovUpdata.fp);
            //rm file
            ret = remove(TSK_CONF_MOV_DEFAULT_NAME);
            gMovUpdata.flag = 0;
            WV_printf("error updata mov end,error \n ");
            return -1;
        }
    }
    fclose(gMovUpdata.fp);
    WV_S8 buf[300];
    sprintf(buf,"%smov%d.mp4",TSK_CONF_MOV_COMPARE_FILE,gMovUpdata.movId);
    //
    memcpy(pMovInfoDev->movFile[gMovUpdata.movId].name, gMovUpdata.nameBuf,gMovUpdata.nameLen);
    //save mov*.mp4
    rename(TSK_CONF_MOV_DEFAULT_NAME,buf);
    //set mov size
    stat(buf,&movStat);
    if(movStat.st_size <= 1024*1024)
    {
        pMovInfoDev->movFile[gMovUpdata.movId].dataLen = 1;
    }else{
        pMovInfoDev->movFile[gMovUpdata.movId].dataLen = movStat.st_size /(1024*1024);
    }
    //save conf
    pMovInfoDev->movFile[gMovUpdata.movId].u8FirstUse = 1;
    TSK_CONF_MovSaveConf();
    gMovUpdata.flag = 0;
    return 0;
}
#endif

/********************************************************************

WV_S32 TSK_CONF_DeleteMov(WV_U8 *pData ,WV_U32 dataLen); 

********************************************************************/
WV_S32 TSK_CONF_DeleteMov(WV_U8 *pData ,WV_U32 dataLen)
{
    WV_S32 i,j,ret,len;
    WV_S8 name[300];
    //printf("delet name [%s],len[%d]\n",pData,dataLen);

    WV_U8 *pRecv = pData;
    len = 0;
    for(i=0;i<dataLen;i++)
    {

        if(pData[i] == '?')
        {
            memset(name,0,300);
            memcpy(name,pRecv,len);//get mov name
            for(j=0;j<TSK_CONF_MOV_MAX_NUM;j++)
            {

                if(strlen(pMovInfoDev->movFile[j].name) == len)
                {
                    ret = strncmp(name,pMovInfoDev->movFile[j].name,len);
                    if(ret ==0)
                    {
                        sprintf(name,"%smov%d.mp4",TSK_CONF_MOV_COMPARE_FILE,j);
                        WV_printf("remove mov [%d]...\n",j);
                        ret = remove(name);
                        //clear mov info
                        memset(&pMovInfoDev->movFile[j],0,sizeof(TSK_CONF_MOV_FILE));
                        break;
                    }
                }
            }
            len = 0;
            pRecv= &pData[i]+1;

        }else{ //else if
            len ++;
        }
    }
    //reset mov conf file
    TSK_CONF_MovSaveConf();

    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_ChangeMov(WV_U32 movNum,WV_U8 *pData ,WV_U32 dataLen); 

函数功能：切换背景视频
movNum：切换第几幕视频
pData:视频名称
dataLen：视频名字长度
********************************************************************/
WV_S32 TSK_CONF_ChangeMov(WV_U32 movNum,WV_S8 *pData ,WV_U32 dataLen)
{
    WV_S32 i,ret;

    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {

        if(strlen(pMovInfoDev->movFile[i].name) == dataLen)
        {
            ret = strncmp(pData,pMovInfoDev->movFile[i].name,dataLen);
            if(ret == 0)
            {
                TSK_SCENE_SetMov(movNum,i);
                TSK_SCENE_ConfMov(movNum);
                break;
            }
        }
    }

    return ret;
}

/********************************************************************

WV_S32 TSK_CONF_ReNameMov(WV_U8 *pData ,WV_U32 dataLen); 

********************************************************************/

WV_S32 TSK_CONF_ReNameMov(WV_U8 *pData ,WV_U32 dataLen)
{
    WV_S8 nameSrc[TSK_CONF_MOV_NAME_MAX_LEN];
    WV_S8 nameDes[TSK_CONF_MOV_NAME_MAX_LEN];

    memset(nameSrc,0,TSK_CONF_MOV_NAME_MAX_LEN);
    memset(nameDes,0,TSK_CONF_MOV_NAME_MAX_LEN);

    WV_S32 i=0,j=0,ret=-1,nameLen=0;

    for(i=0;i<dataLen;i++)
    {
        if(pData[i] != '?')
        {
            nameSrc[i] = pData[i];

        }else{

            nameLen = i;
            //printf("nameLen = %d\n",nameLen);
            break;
        }
    }

    for(i=nameLen+1;i<dataLen;i++)
    {
        nameDes[j] = pData[i];
        j++;
    }
    //printf("data:%s\n",pData);
    //printf("change name %s to %s\n",nameSrc,nameDes);

    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        if(0 == strncmp(nameSrc,pMovInfoDev->movFile[i].name,nameLen))
        {
            memset(pMovInfoDev->movFile[i].name,0,TSK_CONF_MOV_NAME_MAX_LEN);
            memcpy(pMovInfoDev->movFile[i].name,nameDes,dataLen-nameLen-1);
            ret = 0;
            TSK_CONF_MovSaveConf();
            return ret;
        }
    }

    return ret;
}

/********************************************************************

WV_S32 TSK_CONF_GetMovInfo(WV_U8 * pBuf,WV_U32 *pDataLen); 

********************************************************************/
WV_S32 TSK_CONF_GetMovInfo(WV_U8 * pBuf,WV_U32 *pDataLen)
{
    WV_S32 ret=0;

    ret = TSK_CONF_MovGetConf();
    if(ret == 0){

        *pDataLen = sizeof(TSK_CONF_MOV_INFO_E);
        memcpy(pBuf,(WV_U8*)pMovInfoDev,sizeof(TSK_CONF_MOV_INFO_E));
    }else{
        *pDataLen = 0;

    }
    TSK_CONF_getMovTypeNum();
    //TSK_CONF_MovSaveConf();
    return 0;

}

/********************************************************************

WV_S32 TSK_CONF_atoi(); 

********************************************************************/
WV_S32 TSK_CONF_atoi(WV_S8 *pSrc,WV_S32 len)
{

    WV_S32 i,val=0;
    WV_S32 temp;

    for(i=0; i<len; i++)
    {

        if(pSrc[i] == '.')
            break;
        temp = pSrc[i] - '0';

        if((temp >=0) && (temp <=9))
        {
            val=val*10 +temp;
        }
    }

    return val;

}


/********************************************************************

WV_S32 TSK_CONF_MovCompare(); 

********************************************************************/
WV_S32 TSK_CONF_MovCompare()
{
    WV_S32 val,ret;
    DIR *dirptr;
    struct stat movStat;
    struct dirent *entry;
    WV_S8 buf[300];
    if((dirptr = opendir(TSK_CONF_MOV_COMPARE_FILE)) == NULL)
    {
        WV_printf("opendir failed !\n");
        return -1;
    }else{

        while(entry=readdir(dirptr))
        {
            if((strcmp(entry->d_name,".")==0) || (strcmp(entry->d_name,"..")==0) )
            {
                continue;
            }else{

                val = TSK_CONF_atoi((WV_S8 *)&(entry->d_name),strlen(entry->d_name));

                if(pMovInfoDev->movFile[val].dataLen == 0)
                {
                    if(NULL == strstr(entry->d_name,".mp4") || NULL == strstr(entry->d_name,"mov") )	continue; //函数用于判断字符串str2是否是str1的子串


                    memset(pMovInfoDev->movFile[val].name,0,TSK_CONF_MOV_NAME_MAX_LEN);
                    sprintf(pMovInfoDev->movFile[val].name,"mov%d.mp4",val);
                    memset(buf,0,300);
                    sprintf(buf,"%s%s",TSK_CONF_MOV_COMPARE_FILE,pMovInfoDev->movFile[val].name);
                    ret = stat(buf,&movStat);
                    pMovInfoDev->movFile[val].dataLen  = movStat.st_size /(1024*1024);

                    //TSK_CONF_MovSaveConf();
                }

            }

        }
        closedir(dirptr);
    }

    TSK_CONF_MovSaveConf();
    return 0;
}
/********************************************************************

WV_S32 TSK_CONF_MovInfo_movEnaInit(TSK_CONF_MOV_INFO_E *pDev); 
::初始化视频默认为第一幕播放

********************************************************************/
WV_S32 TSK_CONF_MovInfo_movEnaInit(TSK_CONF_MOV_INFO_E *pDev)
{

    //WV_U8  u8FirstUse;
    //WV_U8  u8SecondUse;
    WV_S32 i;
    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        pDev->movFile[i].u8FirstUse = 1;
        pDev->movFile[i].u8SecondUse = 0;
    }
    return WV_SOK;
}
/********************************************************************

WV_S32 TSK_CONF_MovInfo_Init(); 

********************************************************************/
WV_S32 TSK_CONF_MovInfo_Init()
{
    WV_S32 ret;
    pMovInfoDev= (TSK_CONF_MOV_INFO_E*)malloc(sizeof(TSK_CONF_MOV_INFO_E));
    memset(pMovInfoDev,0,sizeof(TSK_CONF_MOV_INFO_E));
    TSK_CONF_MovInfo_movEnaInit(pMovInfoDev);

    ret = access(TSK_CONF_MOV_InfoFile,R_OK);

    if(ret != 0){
        FILE *fp;
        WV_printf("access file [%s] error\n",TSK_CONF_MOV_InfoFile);
        if((fp = fopen(TSK_CONF_MOV_InfoFile,"a+")) == NULL){
            WV_printf("creat file [%s] failed\n",TSK_CONF_MOV_InfoFile);
            return -1;
        }
        WV_printf("creat file [%s] ok\n",TSK_CONF_MOV_InfoFile);
        fclose(fp);
        //Init conf
        TSK_CONF_MovSaveConf();

    }
    if(TSK_CONF_MovGetConf() != 0)
    {
        TSK_CONF_MovSaveConf();
    }
    TSK_CONF_MovCompare();
    //SVR_CONTROL_ReadConfFileCmd();
    TSK_CONF_getMovTypeNum();
    gMovInfoInited = 1;
    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_MovInfo_DeInit(); 

********************************************************************/
WV_S32 TSK_CONF_MovInfo_DeInit()
{
    gMovInfoInited = 0;
    free(pMovInfoDev);
    return 0;
}

/********************************************************************

WV_S32 TSK_CONF_Mov_GetNameByID(WV_U32 id,WV_S8 *pFileName); 

********************************************************************/
WV_S32 TSK_CONF_Mov_GetNameByID(WV_U32 id,WV_S8 *pFileName)
{

    memcpy(pFileName,pMovInfoDev->movFile[id].name,TSK_CONF_MOV_NAME_MAX_LEN);
    return 0;
}

/***************************** 场  景  ******************************/
/********************************************************************

WV_S32 TSK_CONF_getSceneType(); 

查询每个类型有多少个scene，然后进行循环切换
********************************************************************/

WV_S32 TSK_CONF_getSceneType()
{
    WV_U32 i=0,j=0,data=0;

    WV_S8 name[WV_CONF_NAME_MAX_LEN];

    for(i=0;i<TSK_CONF_SCENE_COUNT;i++)
    {

        sprintf(name,"Scene%dEnable",i);
        SYS_ENV_GetU32(name,&data);
        if(data == 0) continue;// get scene enable status ,if sceneEnable ==0,continue;

        //get current scene mov name
        sprintf(name,"Scene%dMov%dEna",i,j);
        SYS_ENV_GetU32(name,&data);
        if(data == 0) continue;

        sprintf(name, "Scene%dMov%dId",i,j);
        SYS_ENV_GetU32(name,&data);
        memcpy(pSceneInfoDev->Scene[i].ucTypeName,pMovInfoDev->movFile[data].typeName,TSK_CONF_SCENE_TYPE_NAME_LEN);

    }

    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_changeSceneByType(WV_U8 *pType);

********************************************************************/
WV_S32 TSK_CONF_changeSceneByType(WV_U8 *pType)
{

    WV_S32 i=0;
    i = pSceneInfoDev->currentID;
    do{
        i++;
        i = i%TSK_CONF_SCENE_TYPE_MAX_NUM;
        if(strncmp((WV_S8 *)pSceneInfoDev->Scene[i].ucTypeName ,(WV_S8 *)pType, TSK_CONF_SCENE_TYPE_NAME_LEN) == 0)
        {
            TSK_SCENE_Change(TSK_SCENE_TYPE_NETDATA, i);
            pSceneInfoDev->currentID = i;
            printf("pSceneInfoDev->currentID = %d\n",i);
            break;
        }
        if(i == pSceneInfoDev->currentID)
        {
            break;
        }
    }while (1);

//    if((SVR_CONTROL_GetKtvDev() == 0) || (SVR_CONTROL_GetKtvDev() == 4)) //雷石点歌机，初始化时默认歌曲随机播放
//    {
        //printf("in TSK_CONF_changeSceneByType I used the TSK_SCENE_ChangePlayMode -----\n");
//        TSK_SCENE_ChangePlayMode(0);//
//    }
    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_GetSceneConfByID(WV_U32 sceneID ,TSK_CONF_SCENE_CONF *pScene); 

********************************************************************/
WV_S32 TSK_CONF_GetSceneConfByID(WV_U32 sceneID ,TSK_CONF_SCENE_CONF *pScene)
{

    WV_U32 j=0,data;
    WV_U32 winNum,aniNum;

    WV_S8 name[WV_CONF_NAME_MAX_LEN];
    sprintf(name,"Scene%dEnable",sceneID);
    SYS_ENV_GetU32(name,&data);
    if(data == 0)return WV_SOK;// get scene enable status ,if sceneEnable ==0,continue;
    //get scene name
    if(strlen((WV_S8 *)pSceneNameDev->sceneNameId[sceneID].name) == 0)
    {
        sprintf(name,"Scene%dEnable",sceneID);
        SYS_ENV_SetU32(name,0);
        return WV_SOK;
    }
    pScene->ucSceneId = sceneID;

    sprintf(name,"Scene%dType",sceneID);
    SYS_ENV_GetU32(name,&data);
    pScene->u16SceneType = data;

    memcpy(pScene->ucSceneName,pSceneNameDev->sceneNameId[sceneID].name,TSK_CONF_SCENE_NAME_MAX_LEN);
    //get current scene mov name
    for(j=0;j<4;j++)
    {
        sprintf(name,"Scene%dMov%dEna",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        if(data == 0) continue;
        sprintf(name, "Scene%dMov%dId",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        memcpy(pScene->ucSceneMovName[j].ucVideoFileName,pMovInfoDev->movFile[data].name,TSK_CONF_SCENE_NAME_MAX_LEN);
       // printf("id:%d,movName:%s\n",sceneID,pScene->ucSceneMovName[j].ucVideoFileName);
    }
    //get scene win info
    winNum = 0;
    sprintf(name,"Scene%dwinNum",sceneID);
    SYS_ENV_GetU32(name,&winNum);
    pScene->ucOutLineCount =winNum;

    for(j=0;j<winNum;j++)
    {
        sprintf(name,"Scene%dWin%dVid",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->OutLine[j].usChannel = data;

        sprintf(name,"Scene%dWin%dOutId",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->OutLine[j].usID =data;

        sprintf(name,"Scene%dWin%dX",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->OutLine[j].usLeft =data;

        sprintf(name,"Scene%dWin%dY",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->OutLine[j].usTop =data;

        sprintf(name,"Scene%dWin%dW",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->OutLine[j].usRight = pScene->OutLine[j].usLeft+data;

        sprintf(name,"Scene%dWin%dH",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->OutLine[j].usBottom = pScene->OutLine[j].usTop+data;

    }
    //get scene ani info
    aniNum = 0;
    sprintf(name,"Scene%dAniNum",sceneID);
    SYS_ENV_GetU32(name,&aniNum);
    pScene->ucBKFrameCount = aniNum;
    for(j=0;j<aniNum;j++)
    {

        sprintf(name,"Scene%dAni%dId",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->BKFrame[j].usID =(WV_U16) data;

        sprintf(name,"Scene%dAni%dX",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->BKFrame[j].usLeft =(WV_U16) data;

        sprintf(name,"Scene%dAni%dY",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->BKFrame[j].usTop =(WV_U16) data;

        sprintf(name,"Scene%dAni%dW",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->BKFrame[j].usRight = pScene->BKFrame[j].usLeft+(WV_U16) data;

        sprintf(name,"Scene%dAni%dH",sceneID,j);
        SYS_ENV_GetU32(name,&data);
        pScene->BKFrame[j].usBottom =pScene->BKFrame[j].usTop +(WV_U16) data;

    }
    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_ChangeScene(WV_U8 *pData ,WV_U32 dataLen); 

********************************************************************/
WV_S32 TSK_CONF_ChangeScene(WV_U8 *pData ,WV_U32 dataLen)
{
    WV_S32 i,ret;

    for(i=0;i<TSK_CONF_SCENE_COUNT;i++)
    {
        
        if(strlen((WV_S8 *)pSceneNameDev->sceneNameId[i].name) == dataLen)
        {
            ret = strncmp((WV_S8 *)pData,(WV_S8 *)pSceneNameDev->sceneNameId[i].name,dataLen);
            if(ret == 0)
            {

                TSK_CONF_GetSceneConfByID(pSceneInfoDev->currentID,&pSceneInfoDev->Scene[pSceneInfoDev->currentID]);
                TSK_SCENE_Change(TSK_SCENE_TYPE_NETDATA, i);
                break;
            }
        }
    }

    return ret;
}
/********************************************************************

WV_S32 TSK_CONF_SaveSceneName(); 

********************************************************************/
WV_S32 TSK_CONF_SaveSceneName()
{
    WV_S32 ret;
    WV_S8 *pBuf = (WV_S8 *)pSceneNameDev;
    FILE *fp;
    ret = access(TSK_CONF_Scene_NameFile,R_OK);
    WV_printf("save scene name start \n");
    if(ret != 0){
        WV_printf("access file [%s] error\n",TSK_CONF_Scene_NameFile);
        return -1;
    }

    fp = fopen(TSK_CONF_Scene_NameFile,"wb+");
    if(fp == NULL)
    {
        WV_printf("fopen file [%s] error\n",TSK_CONF_Scene_NameFile);
        return -1;
    }
    //open mov.ini

    WV_S32 writeLen=0;

    writeLen = fwrite(pBuf,1,sizeof(TSK_CONF_SCENE_FILE),fp);

    if(writeLen != sizeof(TSK_CONF_SCENE_FILE))
    {
        WV_printf("[TSK_CONF_SaveSceneName()]write scene.ini len[%d],struct TSK_CONF_SCENE_INFO_E len[%d]\n",writeLen,sizeof(TSK_CONF_SCENE_FILE));
        fclose(fp);
        return -1;
    }

    WV_printf("save scene name ok\n");
    fclose(fp);
    //set scene name to dev
    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_GetSceneName(TSK_CONF_SCENE_INFO_E *pDev); 

********************************************************************/
WV_S32 TSK_CONF_GetSceneName()
{

    WV_S32 ret,i;
    WV_S8 *pBuf = (WV_S8 *)pSceneNameDev;
    FILE *fp;

    //access scene name file
    ret = access(TSK_CONF_Scene_NameFile,R_OK);
    if(ret != 0)
    {
        WV_S8 name[20];
        WV_U32 data;
        WV_printf("access file [%s] error\n",TSK_CONF_Scene_NameFile);
        if((fp = fopen(TSK_CONF_Scene_NameFile,"a+")) == NULL){
            WV_printf("creat file [%s] failed\n",TSK_CONF_Scene_NameFile);
            return -1;
        }
        WV_printf("creat file [%s] ok\n",TSK_CONF_Scene_NameFile);
        fclose(fp);
        //Init conf
        for(i=0;i<TSK_CONF_SCENE_COUNT;i++)
        {

            sprintf(name, "Scene%dEnable",i);
            SYS_ENV_GetU32(name,&data);
            if(data == 1 )
            {
                sprintf((WV_S8 *)pSceneNameDev->sceneNameId[i].name,"scene%d",i);
            }
        }

        TSK_CONF_SaveSceneName();
        return WV_SOK;

    }

    fp = fopen(TSK_CONF_Scene_NameFile,"rb+");
    if(fp == NULL)
    {
        WV_printf("fopen file [%s] error\n",TSK_CONF_Scene_NameFile);
        return -1;
    }
    //open mov.ini

    WV_S32 readLen=0;

    readLen = fread(pBuf,1,sizeof(TSK_CONF_SCENE_FILE),fp);

    if(readLen != sizeof(TSK_CONF_SCENE_FILE))
    {
        printf("read scene.ini len[%d],struct TSK_CONF_SCENE_FILE len[%d]\n",readLen,sizeof(TSK_CONF_SCENE_FILE));
        fclose(fp);
        return -1;
    }
    fclose(fp);
    //set scene name to dev

    return ret;
}


/********************************************************************

WV_S32 TSK_CONF_SaveSceneType(WV_U8 id,WV_U8 u8Type); 

********************************************************************/
WV_S32 TSK_CONF_SaveSceneType(WV_U8 id,WV_U8 type)
{
    /*
        WV_S8 name[WV_CONF_NAME_MAX_LEN];
        WV_U32 data = 0;
        data = type;
        pSceneInfoDev->Scene[id].u16SceneType = type;
        sprintf(name,"Scene%dType",id);
        SYS_ENV_SetU32(name,data);

        SYS_ENV_Save();
        TSK_CONF_getSceneTypeNum();
*/

    return WV_SOK;
}
/********************************************************************

WV_S32 TSK_CONF_SceneGetLive(TSK_CONF_SCENE_INFO_E *pScene); 

********************************************************************/
WV_S32 TSK_CONF_SceneGetLive(TSK_CONF_SCENE_CONF *pScene)  //get real time sceneInfo
{

    WV_S32 j;
    TSK_SCENE_INFO_S sceneInfo;
    TSK_SCENE_GetCurent(&sceneInfo);
    //get current scene mov name
    for(j=0;j<4;j++)
    {
        if(sceneInfo.mov[j].ena == 0) continue;
        memcpy(pScene->ucSceneMovName[j].ucVideoFileName,pMovInfoDev->movFile[sceneInfo.mov[j].id].name,TSK_CONF_SCENE_NAME_MAX_LEN);
    }
    pScene->ucOutLineCount = sceneInfo.winNum;
    for(j=0;j<sceneInfo.winNum;j++)
    {


        pScene->OutLine[j].usChannel = sceneInfo.win[j].videoId;
        pScene->OutLine[j].usID =sceneInfo.win[j].outId;
        pScene->OutLine[j].usLeft =sceneInfo.win[j].x;
        pScene->OutLine[j].usTop =sceneInfo.win[j].y;
        pScene->OutLine[j].usRight = pScene->OutLine[j].usLeft +sceneInfo.win[j].w;
        pScene->OutLine[j].usBottom =pScene->OutLine[j].usTop + sceneInfo.win[j].h;
    }
    //get scene ani info

    pScene->ucBKFrameCount = sceneInfo.animationNum;
    for(j=0;j<sceneInfo.animationNum;j++)
    {
        pScene->BKFrame[j].usID = sceneInfo.animation[j].id;
        pScene->BKFrame[j].usLeft = sceneInfo.animation[j].x;
        pScene->BKFrame[j].usTop = sceneInfo.animation[j].y;
        pScene->BKFrame[j].usRight = pScene->BKFrame[j].usLeft + sceneInfo.animation[j].w;
        pScene->BKFrame[j].usBottom =pScene->BKFrame[j].usTop + sceneInfo.animation[j].h;

    }

    return 0;

}

/********************************************************************

WV_S32  TSK_CONF_SaveNewScene(WV_U8 *pData ,WV_U32 dataLen); 

********************************************************************/
WV_S32 TSK_CONF_SaveNewScene(WV_U8 *pData ,WV_U32 dataLen)
{
    WV_S32 i,ret=0;
    //WV_U32 sceneEna;
    //WV_S8 name[WV_CONF_NAME_MAX_LEN];
    WV_S8 srcName[TSK_CONF_SCENE_NAME_MAX_LEN];
    WV_S8 desName[TSK_CONF_SCENE_NAME_MAX_LEN];
    WV_U8 sceneId;
    memset(srcName,0,sizeof(srcName));
    memset(desName,0,sizeof(desName));

    WV_S32 reName = 0,srcNameLen,desNameLen;
    for(i=1;i<dataLen;i++)
    {
        if(pData[i] == '?' )
        {
            srcNameLen = i;
            desNameLen = dataLen-i-1;
            memcpy(srcName,pData,srcNameLen);
            memcpy(desName,&pData[i+1],desNameLen);
            reName = 1;
            break;
        }

        if(i==(dataLen -1))
        {
            srcNameLen = dataLen;
            memcpy(desName,&pData[1],dataLen-1);
            sceneId = pData[0];


            if(sceneId >= TSK_CONF_SCENE_COUNT)
            {
                WV_printf("input scene id error,id[%d] >= %d \n",sceneId,TSK_CONF_SCENE_COUNT);
                return -1;
            }
            break;
        }
    }


    if(reName == 0)//add scene
    {
        //WV_printf("add new scene \n");
        memset(pSceneNameDev->sceneNameId[sceneId].name,0,TSK_CONF_SCENE_NAME_MAX_LEN);
        memcpy(pSceneNameDev->sceneNameId[sceneId].name,desName,dataLen-1);
        //WV_printf("new scene [%d]name %s\n",sceneId,desName);
        TSK_CONF_SceneGetLive(&pSceneInfoDev->Scene[sceneId]);
        memcpy(pSceneInfoDev->Scene[sceneId].ucSceneName , pSceneNameDev->sceneNameId[sceneId].name ,TSK_CONF_SCENE_NAME_MAX_LEN);
        pSceneInfoDev->Scene[sceneId].ucSceneId = sceneId;

        ret |=TSK_SCENE_Save(sceneId);
        ret |=TSK_CONF_SaveSceneName();

    }else{//rename scene

        for(i=0;i<TSK_CONF_SCENE_COUNT;i++)
        {

            if(strncmp(srcName,(WV_S8 *)pSceneNameDev->sceneNameId[i].name,TSK_CONF_SCENE_NAME_MAX_LEN) == 0)
            {
                memcpy(pSceneNameDev->sceneNameId[i].name,desName,TSK_CONF_SCENE_NAME_MAX_LEN);
                memcpy(pSceneInfoDev->Scene[i].ucSceneName , pSceneNameDev->sceneNameId[i].name ,TSK_CONF_SCENE_NAME_MAX_LEN);
                pSceneInfoDev->Scene[sceneId].ucSceneId = sceneId;
                ret |=TSK_CONF_SaveSceneName();
                //memcpy(pSceneInfoDev->Scene[i].ucSceneName , pSceneNameDev->sceneNameId[i].name ,TSK_CONF_SCENE_NAME_MAX_LEN);
            }

        }
    }
    TSK_CONF_getSceneType();
    return ret;

}

/********************************************************************

WV_S32 TSK_CONF_SceneGetConf(TSK_CONF_SCENE_INFO_E *pDev); 

********************************************************************/
WV_S32 TSK_CONF_SceneGetConf(TSK_CONF_SCENE_INFO_E *pDev)
{


    memset(pDev,0,sizeof(TSK_CONF_SCENE_INFO_E));

    WV_U32 i=0,data;
    //WV_U32 winNum,aniNum;

    //WV_S8 name[WV_CONF_NAME_MAX_LEN];
    //get scene name
    TSK_CONF_GetSceneName();
    //get scene conf
    for(i=0;i<TSK_CONF_SCENE_COUNT;i++)
    {

        TSK_CONF_GetSceneConfByID(i ,&pDev->Scene[i]);
        /*
                sprintf(name,"Scene%dEnable",i);
                SYS_ENV_GetU32(name,&data);
                if(data == 0) continue;// get scene enable status ,if sceneEnable ==0,continue;
                //get scene name
                pDev->Scene[i].ucSceneId = i;

                sprintf(name,"Scene%dType",i);
                SYS_ENV_GetU32(name,&data);
            pDev->Scene[i].u16SceneType = data;

                memcpy(pDev->Scene[i].ucSceneName,pSceneNameDev->sceneNameId[i].name,TSK_CONF_SCENE_NAME_MAX_LEN);
                //get current scene mov name
                for(j=0;j<4;j++)
                {
                        sprintf(name,"Scene%dMov%dEna",i,j);
                        SYS_ENV_GetU32(name,&data);
                        if(data == 0) continue;
                        sprintf(name, "Scene%dMov%dId",i,j);
                        SYS_ENV_GetU32(name,&data);
                        memcpy(pDev->Scene[i].ucSceneMovName[j].ucVideoFileName,pMovInfoDev->movFile[data].name,TSK_CONF_SCENE_NAME_MAX_LEN);
                        //memcpy(pDev->Scene[i].ucVideoFileName,pMovInfoDev->movFile[data].name,TSK_CONF_SCENE_NAME_MAX_LEN);
                }
                //get scene win info
                winNum = 0;
                sprintf(name,"Scene%dwinNum",i);
                SYS_ENV_GetU32(name,&winNum);
                pDev->Scene[i].ucOutLineCount =winNum;

                for(j=0;j<winNum;j++)
                {
                        sprintf(name,"Scene%dWin%dVid",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].OutLine[j].usChannel = data;

                        sprintf(name,"Scene%dWin%dOutId",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].OutLine[j].usID =data;

                        sprintf(name,"Scene%dWin%dX",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].OutLine[j].usLeft =data;

                        sprintf(name,"Scene%dWin%dY",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].OutLine[j].usTop =data;

                        sprintf(name,"Scene%dWin%dW",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].OutLine[j].usRight =pDev->Scene[i].OutLine[j].usLeft+data;

                        sprintf(name,"Scene%dWin%dH",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].OutLine[j].usBottom =pDev->Scene[i].OutLine[j].usTop+data;

                }
                //get scene ani info
                aniNum = 0;
                sprintf(name,"Scene%dAniNum",i);
                SYS_ENV_GetU32(name,&aniNum);
                pDev->Scene[i].ucBKFrameCount = aniNum;
                for(j=0;j<aniNum;j++)
                {

                        sprintf(name,"Scene%dAni%dId",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].BKFrame[j].usID =(WV_U16) data;

                        sprintf(name,"Scene%dAni%dX",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].BKFrame[j].usLeft =(WV_U16) data;

                        sprintf(name,"Scene%dAni%dY",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].BKFrame[j].usTop =(WV_U16) data;

                        sprintf(name,"Scene%dAni%dW",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].BKFrame[j].usRight = pDev->Scene[i].BKFrame[j].usLeft+(WV_U16) data;

                        sprintf(name,"Scene%dAni%dH",i,j);
                        SYS_ENV_GetU32(name,&data);
                        pDev->Scene[i].BKFrame[j].usBottom =pDev->Scene[i].BKFrame[j].usTop +(WV_U16) data;

                }
*/
    }
    SYS_ENV_GetU32("SceneCurId",&data);
    pDev->currentID = data;

    //SVR_CONTROL_GetSongType((WV_S8 *)pDev->SceneType);

    TSK_CONF_SceneGetLive(&pDev->Scene[pDev->currentID]);

    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_SceneDelet(WV_U8 * pBuf,WV_U32 dataLen); 

********************************************************************/
WV_S32 TSK_CONF_SceneDelet(WV_U8 * pBuf,WV_U32 dataLen)
{


    WV_S32 ret=0;
    WV_S32 i;
    WV_S8 name[WV_CONF_NAME_MAX_LEN];
    WV_U32 data = 0;
    for(i=0;i<TSK_CONF_SCENE_COUNT;i++)
    {
        if(strlen((WV_S8 *)pSceneNameDev->sceneNameId[i].name) == dataLen)
        {

            if(strncmp((WV_S8 *)pBuf,(WV_S8 *)pSceneNameDev->sceneNameId[i].name,dataLen) == 0)
            {
                //set scene enable to 0
                memset(pSceneNameDev->sceneNameId[i].name,0,TSK_CONF_SCENE_NAME_MAX_LEN);
                sprintf(name,"Scene%dEnable",i);
                SYS_ENV_SetU32(name,data);

                //set scene type to default 0
                sprintf(name,"Scene%dType",i);
                SYS_ENV_SetU32(name,data);
                pSceneInfoDev->Scene[i].u16SceneType = 0;

                ret |= TSK_CONF_SaveSceneName();
                ret |= SYS_ENV_Save();
                memset(pSceneInfoDev->Scene[i].ucSceneName , 0 ,TSK_CONF_SCENE_NAME_MAX_LEN);
                break;
            }
        }
    }

    return ret;
}

/********************************************************************

WV_S32 TSK_CONF_SaveCurrentScene(WV_S8 * pBuf,WV_U32 dataLen); 

********************************************************************/
WV_S32 TSK_CONF_SaveCurrentScene(WV_U8 * pBuf,WV_U32 dataLen)
{

    WV_S32 ret=0;
    WV_S32 i;
    WV_S8 name[WV_CONF_NAME_MAX_LEN];
    WV_U32 data = 0;

    for(i=0;i<TSK_CONF_SCENE_COUNT;i++)
    {
        if(strlen((WV_S8 *)pSceneNameDev->sceneNameId[i].name) == dataLen)
        {
            ret=strncmp((WV_S8 *)pBuf,(WV_S8 *)pSceneNameDev->sceneNameId[i].name,dataLen) ;
            if(ret==0)
            {


                TSK_CONF_SceneGetLive(&pSceneInfoDev->Scene[i]);

                data = pSceneInfoDev->Scene[i].u16SceneType;
                sprintf(name,"Scene%dType",i);
                SYS_ENV_SetU32(name,data);
                TSK_SCENE_Save(i);
                //memcpy(pSceneInfoDev->Scene[i].ucSceneName , pSceneNameDev->sceneNameId[i].name ,TSK_CONF_SCENE_NAME_MAX_LEN);

                break;
            }
        }
    }
    return ret;
}


/********************************************************************

WV_S32 TSK_CONF_GetSceneInfo(WV_U8 * pBuf,WV_U32 *pDataLen); 

********************************************************************/
WV_S32 TSK_CONF_GetSceneInfo(WV_U8 * pBuf,WV_U32 *pDataLen)
{


    /*
        ret = TSK_CONF_SceneGetConf(pSceneInfoDev);
        if(ret == 0){
                *pDataLen = sizeof(TSK_CONF_SCENE_INFO_E);
                memcpy(pBuf,(WV_U8*)pSceneInfoDev,sizeof(TSK_CONF_SCENE_INFO_E));
        }else{
                *pDataLen = 0;
        }
*/

    WV_U32 data;
    SYS_ENV_GetU32("SceneCurId",&data);
    pSceneInfoDev->currentID = (WV_U8)data;

    TSK_CONF_SceneGetLive(&pSceneInfoDev->Scene[data]);
    TSK_CONF_getSceneType();
    *pDataLen = sizeof(TSK_CONF_SCENE_INFO_E);
    memcpy(pBuf,(WV_U8*)pSceneInfoDev,sizeof(TSK_CONF_SCENE_INFO_E));

    return WV_SOK;
}

/********************************************************************

WV_S32 TSK_CONF_SceneInfo_Init(); 

********************************************************************/
WV_S32 TSK_CONF_SceneInfo_Init()
{

    pSceneNameDev=(TSK_CONF_SCENE_FILE*)malloc(sizeof(TSK_CONF_SCENE_FILE));
    pSceneInfoDev= (TSK_CONF_SCENE_INFO_E*)malloc(sizeof(TSK_CONF_SCENE_INFO_E));
    memset(pSceneNameDev,0,sizeof(TSK_CONF_SCENE_FILE));
    memset(pSceneInfoDev,0,sizeof(TSK_CONF_SCENE_INFO_E));

    TSK_CONF_SceneGetConf(pSceneInfoDev);
    TSK_CONF_getSceneType();
    gSceneInfoInited = 1;
    return 0;
}

/********************************************************************

WV_S32 TSK_CONF_SceneInfo_DeInit(); 

********************************************************************/
WV_S32 TSK_CONF_SceneInfo_DeInit()
{
    gMovInfoInited = 0;
    free(pSceneNameDev);
    free(pSceneInfoDev);

    return 0;
}


// get dev info 
/********************************************************************

WV_S32 TSK_CONF_GetDevInfo(WV_U8 * pBuf,WV_U32 *pDataLen); 

********************************************************************/
WV_S32 TSK_CONF_GetDevInfo(WV_U8 * pBuf,WV_U32 *pDataLen)
{

    return 0;
}
/********************************************************************

WV_S32 TSK_CONF_SetGif(WV_U8 * pBuf,WV_U32 dataLen); 

********************************************************************/
WV_S32 TSK_CONF_SetGif(WV_U8 * pBuf,WV_U32 dataLen,WV_U8 chn)
{

    WV_S32 ret;
    FILE *fp;
    WV_S32 writeLen;
    ret = access(TSK_CONF_GIF_PATH, F_OK);
    if(ret != 0 ){
        ret = mkdir(TSK_CONF_GIF_PATH,0755);
        if(ret != 0 )
        {
            WV_printf("*********mkdir gif error **************\n");
            return -1;
        }
    }
    WV_S8 name[20];
    sprintf(name,"%s/%d.gif",TSK_CONF_GIF_PATH,chn);
    fp = fopen(name,"wb+");
    if(fp == NULL)
    {
        WV_printf("*********fopen %s  error **************\n",name);
        return -1;
    }

    writeLen = fwrite(pBuf,1,dataLen,fp);
    if(writeLen != dataLen )
    {
        WV_printf("*********fopen %s  error **************\n",name);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}



/********************************************************************

WV_S32 TSK_CONF_GetFreeMovID(const WV_U8 *pName); 
函数说明：查询当前未被占用的视频ID，如有同名的视频，则替返回同名的视频ID。
参数：视频名称
返回值：成功返回id，失败返回-1
********************************************************************/
WV_S32 TSK_CONF_GetFreeMovID(const WV_U8 *pName)
{
    WV_S32 movID,i;
    movID = -1;
    for(i=0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        if(pMovInfoDev->movFile[i].dataLen == 0 )
        {
            if(movID == -1) movID = i;
            continue;
        }

        if(strcmp((WV_S8 *)pMovInfoDev->movFile[i].name,(WV_S8 *)pName) == 0 )
        {
            movID = i;

            break;//视频名字相同，则查找到对应的视频movID=i

        }else{

            continue;	//视频名字不同，继续查找
        }

    }

    WV_printf("get mov name ,id=%d \n",movID);

    if(movID == -1 || movID >= TSK_CONF_MOV_MAX_NUM)
    {
        return WV_EFAIL;
    }

    return movID;
}

/********************************************************************

WV_S32 TSK_CONF_CheckMovEncrypt(WV_U8 *pFile,WV_U8 *pType); 
函数说明：检测u盘导入视频是否加密
参数：文件目录,视频类型
返回值：0加密，-1不加密
********************************************************************/
WV_S32 TSK_CONF_CheckMovEncrypt(WV_U8 *pFile,WV_U8 *pType)
{

    FILE *fp=NULL;
    WV_U8 buf[1024+1];
    WV_S32 readLen=0;
    if(access((WV_S8 *)pFile,F_OK) != 0 )
    {
        return WV_EFAIL;
    }

    fp = fopen((WV_S8 *)pFile,"rb");
    if(fp == NULL)
    {
        return WV_EFAIL;
    }

    readLen = fread(buf,1,1024,fp);
    if(readLen != 1024)
    {
        fclose(fp);
        return WV_EFAIL;
    }


    if(buf[0] == 'H' && \
            buf[6] == 'I' && \
            buf[12] == 'S' && \
            buf[18] == '3' && \
            buf[24] == '7' && \
            buf[30] == '9' && \
            buf[36] == '8' && \
            buf[42] == 'M' && \
            buf[43] <= TSK_CONF_MOV_TYPE_NAME_LEN ) ////视频加密会在第0,6 、12 、18、24,30,36,42 个字节处分别填写HIS3798M,第48个字节为歌曲类别（最多支持64个） 0x00~0x3f
    {

        WV_printf("mov is encrypt \n");
        memcpy(pType,&buf[44],buf[43]);
        fclose(fp);
        return WV_SOK;

    }

    fclose(fp);
    return WV_EFAIL;
}

/********************************************************************

WV_S32 TSK_CONF_cpVideoToLocal(WV_S8 *srcFilePath,WV_S8 *desFilePath); 

********************************************************************/
WV_S32 TSK_CONF_cpVideoToLocal(WV_S8 *srcFilePath,WV_S8 *desFilePath)
{

    WV_S8 movSrc[300];
    WV_S8 movDes[300];
    WV_U8 movType[TSK_CONF_MOV_TYPE_NAME_LEN];
    struct stat movStat;

    DIR *pDir;
    struct dirent *pDirent;
    pDir = opendir(srcFilePath);
    if(pDir == NULL)
    {
        WV_printf("can not open dir %s \n",srcFilePath);
        return WV_EFAIL;
    }

    pDirent = readdir(pDir);
    WV_S32 movID;
    while((pDirent = readdir(pDir)))
    {
        if(pDirent->d_type != 8)
        {
            continue;
        }


        movID = TSK_CONF_GetFreeMovID((WV_U8 *)pDirent->d_name);
        if(movID == -1) continue;

        memset(movSrc,0,sizeof(movSrc));
        memset(movDes,0,sizeof(movDes));

        sprintf(movSrc,"%s%s",srcFilePath,pDirent->d_name);
        memset(movType,0,sizeof(movType));
        if(TSK_CONF_CheckMovEncrypt((WV_U8 *)movSrc,movType)  !=  0) continue; //检测视频是否加密

        if(WV_FILE_CpyWithOutHead(1024,movSrc,TSK_CONF_MOV_DEFAULT_NAME) != 0 )
        {
            WV_ERROR("cp with out head error \n");
            remove(TSK_CONF_MOV_DEFAULT_NAME);
            continue;
        }

        sprintf(movDes,"%smov%d.mp4",desFilePath,movID);
        if(rename(TSK_CONF_MOV_DEFAULT_NAME,movDes) != 0) continue;

        WV_printf("rename mov file ok \n");
        //set mov size
        stat(movDes,&movStat);
        if(movStat.st_size <= 1024*1024)
        {
            pMovInfoDev->movFile[movID].dataLen = 1;
        }else{
            pMovInfoDev->movFile[movID].dataLen = movStat.st_size /(1024*1024);
        }
        memset(pMovInfoDev->movFile[movID].name,0,TSK_CONF_MOV_NAME_MAX_LEN);
        sprintf(pMovInfoDev->movFile[movID].name,"%s",pDirent->d_name);
        memcpy(pMovInfoDev->movFile[movID].typeName,movType,sizeof(movType));

        pMovInfoDev->movFile[movID].u8FirstUse = 1;            //第一幕使用
        pMovInfoDev->movFile[movID].u8SecondUse = 0;           //第二幕使用

        //save conf
        WV_printf("-----------save movconf -----------------\n");
        TSK_CONF_MovSaveConf();

    }
    closedir(pDir);
    TSK_CONF_getMovTypeNum();
    return WV_SOK;

}

/********************************************************************

WV_S32 TSK_CONF_Mov_GetIDByName(WV_U32 *pid,WV_S8 *pFileName); 

********************************************************************/
WV_S32 TSK_CONF_Mov_GetIDByName(WV_U32 *pid,WV_S8 *pFileName)
{
    WV_U32 i;

    if(pFileName == NULL)
    {
        return WV_EFAIL;
    }
    for(i = 0;i<TSK_CONF_MOV_MAX_NUM;i++)
    {
        if(pMovInfoDev->movFile[i].name[0] == 0)
        {
            continue;
        }
        if(strcmp(pFileName,pMovInfoDev->movFile[i].name) == 0)
        {
            *pid = i;
            return WV_SOK;
        }
    }
    return WV_EFAIL;
}


/********************************************************************

WV_S32  TSK_CONF_SaveCustomOneScene(TSK_CONF_SCENE_CONF *pScene); 
//
********************************************************************/
WV_S32 TSK_CONF_SaveCustomOneScene(TSK_CONF_SCENE_CONF *pScene)
{
    WV_S32 ret = 0;

    memset(pSceneNameDev->sceneNameId[pScene->ucSceneId].name, 0, TSK_CONF_SCENE_NAME_MAX_LEN);
    memcpy(pSceneNameDev->sceneNameId[pScene->ucSceneId].name, pScene->ucSceneName, strlen((WV_S8 *)pScene->ucSceneName));
    memcpy(&pSceneInfoDev->Scene[pScene->ucSceneId], pScene, sizeof(TSK_CONF_SCENE_CONF));
    ret |= TSK_SCENE_SaveCustom(pScene->ucSceneId, pScene);
    ret |= TSK_CONF_SaveSceneName();
    TSK_CONF_getSceneType();
    return ret;
}

/********************************************************************

WV_S32  TSK_CONF_SaveCustomScene(WV_U8 *pData ,WV_U32 dataLen); 
//
********************************************************************/
WV_S32 TSK_CONF_SaveCustomScene(WV_U8 *pData ,WV_U32 dataLen)
{
printf("CustomScene dataLen = %d\n",dataLen);
printf("TSK_CONF_SCENE_INFO_E dataLen = %d\n",sizeof(TSK_CONF_SCENE_INFO_E));
    if (dataLen == sizeof(TSK_CONF_SCENE_INFO_E))
    {
        TSK_CONF_SCENE_INFO_E *pScene;
        pScene = (TSK_CONF_SCENE_INFO_E*)malloc(sizeof(TSK_CONF_SCENE_INFO_E));
        memset(pScene, 0, sizeof(TSK_CONF_SCENE_INFO_E));
        memcpy(pScene, pData, dataLen);
        int i;
        for (i = 0; i < TSK_CONF_SCENE_COUNT; i++)
        {
            if (pSceneInfoDev->Scene[i].ucSceneName[0] != 0x00)
            {
                TSK_CONF_SceneDelet(pSceneInfoDev->Scene[i].ucSceneName, strlen((WV_S8 *)pSceneInfoDev->Scene[i].ucSceneName));
            }
        }
        for (i = 0; i < TSK_CONF_SCENE_COUNT; i++)
        {
            if (pScene->Scene[i].ucSceneName[0] != 0x00)
            {
                TSK_CONF_SaveCustomOneScene(&pScene->Scene[i]);
            }
        }
        free(pScene);
        pScene = NULL;
printf("TSK_CONF_SCENE_INFO_E OK\n");
    }
    return WV_SOK;
}

/********************************************************************

WV_S32  TSK_CONF_SaveFadeinout(WV_U8 Fade); 
//
********************************************************************/
WV_S32 TSK_CONF_SaveFadeinout(WV_U8 Fade)
{
    FILE *fp;
    WV_U32 writeLen;
    fp = fopen(TSK_FADE_INOUT_FILE_PATH,"wb+");
    if(fp == NULL)
    {
        WV_printf("fopen %s error \n",TSK_FADE_INOUT_FILE_PATH);
        return WV_EFAIL;
    }
    writeLen=fwrite(&Fade,1,sizeof(Fade),fp);
    if(writeLen != sizeof(Fade))
    {
        WV_printf("fwrite %s error \n",TSK_FADE_INOUT_FILE_PATH);
        fclose(fp);
        return WV_EFAIL;
    }
    fclose(fp);
    return WV_SOK;
}

/********************************************************************

WV_S32  TSK_CONF_LoadFadeinout(WV_U8 *Fade); 
//
********************************************************************/
WV_S32 TSK_CONF_LoadFadeinout(WV_U8 *Fade)
{
    FILE *fp;
    WV_U32 writeLen;
    WV_U8 Fadein;

    Fadein = 0;
    fp = fopen(TSK_FADE_INOUT_FILE_PATH,"rb");
    if(fp == NULL)
    {
        Fade = 0;
        WV_printf("fopen %s error \n",TSK_FADE_INOUT_FILE_PATH);
        return WV_EFAIL;
    }

    writeLen=fread(&Fadein,1,sizeof(Fadein),fp);
    if(writeLen != sizeof(Fadein))
    {
        Fade = 0;
        WV_printf("fread %s error \n",TSK_FADE_INOUT_FILE_PATH);
        fclose(fp);
        return WV_EFAIL;
    }
    fclose(fp);
    *Fade = Fadein;
    return WV_SOK;
}


