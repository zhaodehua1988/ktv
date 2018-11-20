#include "his_dis.h"
#include "his_avplay.h"  
#include "his_player.h" 
#include "tsk_player.h"
#include "fpga_conf.h"
#include "hi_unf_avplay.h"
#include "sys/time.h"
#include "hi_unf_common.h"
#include "hi_unf_avplay.h"
#include "hi_unf_sound.h"
#include "hi_unf_disp.h"
#include "hi_unf_vo.h" 
#include "his_fb.h"

#include "hi_unf_common.h"
#include "hi_unf_ecs.h"
#include "hi_unf_avplay.h"
#include "hi_unf_sound.h"
#include "hi_unf_disp.h"
#include "hi_unf_vo.h"
#include "hi_unf_demux.h"
#include "hi_go.h"
#include "tsk_scene.h"


#define  TSK_PLAYER_CHNL_NUM    4

#define  TSK_PLAYER_FILENAME_LEN    128
#define  TSK_PLAYER_FILELIST_NUM    5
#define  TSK_PLAYER_VOL_FILE_PATH "./env/volume.txt"

static WV_U32 gPlayerVolume[3];

typedef struct TSK_PLAYER_CHNL_E 
{
    WV_U32      id;
    WV_U32      type;
    WV_THR_HNDL_T hndl;
    WV_U32      open;
    WV_U32      close;
    WV_U32      streamMode;
    HI_HANDLE   playerHandl;
    HI_HANDLE   avPlayHandl;
    HI_UNF_VCODEC_TYPE_E vEncType;
    HI_HANDLE   winHandl;
    HI_RECT_S   winRect;
    HI_RECT_S   winLastRect;
    WV_S8       fileName[128];
    WV_S8       fileList[5][128];
    WV_S32 playerEna;
    WV_S32 winEna;
    WV_U32 playerDelayTime;
    WV_U32 playNext;
    WV_U32 hTrack;
    //WV_U32 playerMode;
} TSK_PLAYER_CHNL_E; 

TSK_PLAYER_CHNL_E gTskPlayer[TSK_PLAYER_CHNL_NUM];

static unsigned int u32PlayerScreenMode;
/***********************************************************************
WV_S32  TSK_PLAYER_Init();
//初始化，先设置三个不同的播放器，播放三个不同的视频
***********************************************************************/
WV_S32  TSK_PLAYER_Init()
{
    //WV_S32 chl;
    //获取FPGA输出通道号
    //chl=FPGA_CONF_GetOutChl_Num();
    //set screenMode //0:3840*1080
    u32PlayerScreenMode = 2;

    gTskPlayer[0].type = 1;
    gTskPlayer[0].vEncType = HI_UNF_VCODEC_TYPE_HEVC;
    gTskPlayer[0].winRect.s32X = 0;
    gTskPlayer[0].winRect.s32Y = 4;
    gTskPlayer[0].winRect.s32Width = 3840;
    gTskPlayer[0].winRect.s32Height = 2160;
    sprintf(gTskPlayer[0].fileName, "%s", "mov0.mp4");
    gTskPlayer[0].playerEna = 0;
    gTskPlayer[0].winEna = 0;
    gTskPlayer[0].playNext = 0;


    gTskPlayer[1].type = 1;
    gTskPlayer[1].vEncType = HI_UNF_VCODEC_TYPE_HEVC;
    gTskPlayer[1].winRect.s32X =1920;
    gTskPlayer[1].winRect.s32Y = 4;
    gTskPlayer[1].winRect.s32Width = 1920;
    gTskPlayer[1].winRect.s32Height = 1080;
    sprintf(gTskPlayer[1].fileName, "%s", "./1526.mp4");
    gTskPlayer[1].playerEna =0;
    gTskPlayer[1].winEna = 0;
    gTskPlayer[1].playNext = 0;

    gTskPlayer[2].type = 1;
    gTskPlayer[2].vEncType = HI_UNF_VCODEC_TYPE_HEVC;
    gTskPlayer[2].winRect.s32X = 2560;
    gTskPlayer[2].winRect.s32Y = 4;
    gTskPlayer[2].winRect.s32Width = 1280;
    gTskPlayer[2].winRect.s32Height = 540;
    sprintf(gTskPlayer[2].fileName, "%s", "./sfy1.avi");
    gTskPlayer[2].playerEna = 0;
    gTskPlayer[2].winEna = 0;
    gTskPlayer[2].playNext = 0;

    gPlayerVolume[0]=0;
    gPlayerVolume[1]=0;
    gPlayerVolume[2]=0;

    return WV_SOK;
}

/***********************************************************************
WV_S32  TSK_PLAYER_SetFile(WV_S32 id,WV_S8 * pFile);
//设置某个播放器播放某个视频
***********************************************************************/
WV_S32 TSK_PLAYER_SetFile(WV_S32 id,WV_S8 * pFile)
{
    sprintf(gTskPlayer[id].fileName, "%s",pFile);
    return WV_SOK;
}

/***********************************************************************
WV_S32  TSK_PLAYER_setWin(WV_S32 id,WV_U16 x,WV_U16 y,WV_U16 w,WV_U16 h);
设置播放器窗口位置可宽高
***********************************************************************/
WV_S32  TSK_PLAYER_setWin(WV_S32 id,WV_U16 x,WV_U16 y,WV_U16 w,WV_U16 h)
{
    gTskPlayer[id].winRect.s32X = 0+x;
    gTskPlayer[id].winRect.s32Y = 4+y;
    gTskPlayer[id].winRect.s32Width = w;
    gTskPlayer[id].winRect.s32Height = h;
    return WV_SOK;
}


/***********************************************************************
WV_S32  TSK_PLAYER_Start(WV_S32 id);
//开始播放
***********************************************************************/
WV_S32  TSK_PLAYER_Start(WV_S32 id)
{
    WV_CHECK_RET(HIS_PLAYER_Start(&(gTskPlayer[id].playerHandl) ,gTskPlayer[id].fileName));
    return WV_SOK;
}


/***********************************************************************
WV_S32  TSK_PLAYER_Play(WV_S32 id);
***********************************************************************/
WV_S32  TSK_PLAYER_Play(WV_S32 id)
{
    WV_CHECK_RET(HIS_PLAYER_Play(&(gTskPlayer[id].playerHandl)));
    return WV_SOK;
}

/***********************************************************************
WV_S32  TSK_PLAYER_Pause(WV_S32 id);
暂停
***********************************************************************/
WV_S32  TSK_PLAYER_Pause(WV_S32 id)
{
    WV_CHECK_RET(HIS_PLAYER_Pause(&(gTskPlayer[id].playerHandl)));
    return WV_SOK;
}



/***********************************************************************
WV_S32  TSK_PLAYER_Resume(WV_S32 id);
继续
***********************************************************************/
WV_S32  TSK_PLAYER_Resume(WV_S32 id)
{
    WV_CHECK_RET(HIS_PLAYER_Resume(&(gTskPlayer[id].playerHandl)));
    return WV_SOK;
}

/***********************************************************************
WV_S32  TSK_PLAYER_Seek(WV_S32 id,HI_U64 u64TimeInMs);
定位播放
***********************************************************************/
WV_S32  TSK_PLAYER_Seek(WV_S32 id,HI_U64 u64TimeInMs)
{

    WV_S32 ret;
    ret =HIS_PLAYER_Seek(&(gTskPlayer[id].playerHandl),u64TimeInMs);

    return ret;
}

/***********************************************************************
WV_S32  TSK_PLAYER_Stop(WV_S32 id);
停止
***********************************************************************/
WV_S32  TSK_PLAYER_Stop(WV_S32 id)
{
    WV_CHECK_RET(HIS_PLAYER_Stop(&(gTskPlayer[id].playerHandl)));
    return WV_SOK;
}


/***********************************************************************
WV_S32  TSK_PLAYER_DevCascade(WV_S32 id,WV_U32 DevCascading);
***********************************************************************/
WV_S32  TSK_PLAYER_DevCascade(WV_S32 id,WV_U32 DevCascading)
{
    switch(DevCascading)
    {
    case 0:
        WV_printf("player cascade mode is not sync \n");
        break;
    case 1:
        WV_printf("player cascade mode is master \n");
        TSK_PLAYER_Stop(id);
        break;
    case 2:
        TSK_PLAYER_Stop(id);
        WV_printf("player cascade mode is slave \n");
        break;
    default:
        break;
    }

}

/***********************************************************************
WV_S32  TSK_PLAYER_GetStatus(WV_S32 id);
***********************************************************************/
WV_S32  TSK_PLAYER_GetStatus(WV_S32 id)
{
    WV_S32 ret=0;
    WV_U32 status;
    ret = HIS_PLAYER_GetStatus(&(gTskPlayer[id].playerHandl),&status);

    return status;
}

/***********************************************************************
WV_S32  TSK_PLAYER_FreezeWindowBlack()
释放背景
***********************************************************************/
WV_S32  TSK_PLAYER_FreezeWindowBlack()
{
    WV_S32 i;
    for(i=0;i<3;i++)
    {
        if(gTskPlayer[i].winEna == 1)
        {
            HIS_DIS_WinFreeze(&(gTskPlayer[i].winHandl),HI_TRUE,1);
        }

    }

    return WV_SOK;
}


/***********************************************************************

WV_S32  TSK_PLAYER_SetHiPlayNext(WV_U32 ena,HI_HANDLE playerHandl);

只试用与player id=2,播放u盘视频

***********************************************************************/
WV_S32  TSK_PLAYER_SetHiPlayNext(WV_U32 ena,HI_HANDLE playerHandl)
{


    if(gTskPlayer[2].playerHandl == playerHandl)
    {
        //printf("set hiplaye next ret=0\n");
        gTskPlayer[2].playNext = ena;
        return WV_SOK;
    }
    //printf("set hiplaye next ret=-1\n");
    return WV_EFAIL;
}

/***********************************************************************
WV_S32  TSK_PLAYER_GetUsbPlayNext();
获取优盘播放的下一个内容
***********************************************************************/
WV_S32  TSK_PLAYER_GetUsbPlayNext()
{

    return gTskPlayer[2].playNext;
}

/***********************************************************************
WV_S32  TSK_PLAYER_SetUsbPlayNext(WV_U32 ena,WV_U32 playerID);
设置优盘播放的下一个文件
***********************************************************************/
WV_S32  TSK_PLAYER_SetUsbPlayNext(WV_U32 ena,WV_U32 playerID)
{
    if(ena != 0 && ena != 1 )
        return WV_EFAIL;
    gTskPlayer[playerID].playNext = ena;
    return WV_SOK;
}

/***********************************************************************

WV_S32  TSK_PLAYER_SetVolume(WV_S32 volume,WV_U32 playerID);
设置声音音量
***********************************************************************/
WV_S32  TSK_PLAYER_SetVolume(WV_S32 volume,WV_U32 playerID)
{

    HIS_AVP_SetVolume(playerID,volume);
    gPlayerVolume[playerID]=volume;
    TSK_PLAYER_SaveVolume();
    return WV_SOK;
}
/***********************************************************************

WV_S32  TSK_PLAYER_GetVolume(WV_S32 *pVolume,WV_U32 playerID);
获取音量
***********************************************************************/
WV_S32  TSK_PLAYER_GetVolume(WV_S32 *pVolume,WV_U32 playerID)
{
    WV_S32 ret=0;
    //ret = HIS_AVP_GetVolume(playerID,pVolume);
    *pVolume = 	gPlayerVolume[playerID];
    return ret;
}
/***********************************************************************

WV_S32  TSK_PLAYER_SaveVolume();
保存设置的音量
***********************************************************************/
WV_S32  TSK_PLAYER_SaveVolume()
{

    FILE *fp;
    WV_U32 writeLen;
    fp = fopen(TSK_PLAYER_VOL_FILE_PATH,"wb+");
    if(fp == NULL)
    {
        WV_printf("fopen %s error \n",TSK_PLAYER_VOL_FILE_PATH);
        return WV_EFAIL;
    }

    writeLen=fwrite((WV_U8 *)gPlayerVolume,1,sizeof(gPlayerVolume),fp);
    if(writeLen != sizeof(gPlayerVolume))
    {
        WV_printf("fwrite %s error \n",TSK_PLAYER_VOL_FILE_PATH);
        fclose(fp);
        return WV_EFAIL;
    }

    fclose(fp);

    return WV_SOK;
}
/***********************************************************************

WV_S32  TSK_PLAYER_ReadVolume();
读取音量配置文件
***********************************************************************/
WV_S32  TSK_PLAYER_ReadVolume()
{

    FILE *fp=NULL;
    WV_U32 readLen,i;
    fp = fopen(TSK_PLAYER_VOL_FILE_PATH,"rb");
    if(fp == NULL)
    {
//        WV_printf("TSK_PLAYER_ReadVolume():fopen %s error \n",TSK_PLAYER_VOL_FILE_PATH);

        for(i=0;i<3;i++)
        {
            gPlayerVolume[i] = 50;
        }
        //return WV_EFAIL;
    }else{
        readLen=fread((WV_U8 *)gPlayerVolume,1,sizeof(gPlayerVolume),fp);
        if(readLen != sizeof(gPlayerVolume))
        {
            WV_printf("TSK_PLAYER_ReadVolume():fread %s error \n",TSK_PLAYER_VOL_FILE_PATH);
            fclose(fp);
            for(i=0;i<3;i++)
            {
                gPlayerVolume[i] = 0;
            }
            //return WV_EFAIL;
        }else{

            fclose(fp);
        }
    }

    for(i=0;i<3;i++)
    {
        TSK_PLAYER_SetVolume(gPlayerVolume[i],i);
    }

    return WV_SOK;
}


/***********************************************************************
WV_S32  TSK_PLAYER_Creat(WV_S32 id);
***********************************************************************/
WV_S32  TSK_PLAYER_Creat(WV_S32 id)
{
#if 0
    WV_U8  i;
    for(i=25;i>1;i--){

        HIS_FB_SetAlpha(i*10);
        printf("create set Alpha=%d \n",i*10);
        //usleep(100000);
    }
#endif
    WV_S32 ret = 0;
    //usleep(100);
    if(gTskPlayer[id].playerEna == 0)
    {
//        printf("\n*****TSK_PLAYER_Creat(%d)*****\n",id);
        if(gTskPlayer[id].winEna == 0 )
        {

            WV_CHECK_RET(  HIS_AVP_EsOpen(&(gTskPlayer[id].avPlayHandl),gTskPlayer[id].vEncType,id));
            WV_CHECK_RET(  HIS_DIS_WinCreat(&(gTskPlayer[id].winHandl),gTskPlayer[id].winRect));
            WV_CHECK_RET(  HIS_DIS_WinAttach(&(gTskPlayer[id].winHandl),&(gTskPlayer[id].avPlayHandl)));
            WV_CHECK_RET(  HIS_DIS_WinStart(&(gTskPlayer[id].winHandl)));
           // WV_CHECK_RET(  HIS_PLAYER_Create(&(gTskPlayer[id].playerHandl) ,&(gTskPlayer[id].avPlayHandl)));
            gTskPlayer[id].winEna = 1;
            if( (gTskPlayer[id].winLastRect.s32X != gTskPlayer[id].winRect.s32X) || (gTskPlayer[id].winLastRect.s32Y != gTskPlayer[id].winRect.s32Y) \
                    || (gTskPlayer[id].winLastRect.s32Width != gTskPlayer[id].winRect.s32Width) || (gTskPlayer[id].winLastRect.s32Height != gTskPlayer[id].winRect.s32Height))
            {
                gTskPlayer[id].winLastRect.s32X = gTskPlayer[id].winRect.s32X;
                gTskPlayer[id].winLastRect.s32Y = gTskPlayer[id].winRect.s32Y;
                gTskPlayer[id].winLastRect.s32Width = gTskPlayer[id].winRect.s32Width;
                gTskPlayer[id].winLastRect.s32Height = gTskPlayer[id].winRect.s32Height;
            }


        }
        //开始播放
        WV_CHECK_RET(  HIS_PLAYER_Create(&(gTskPlayer[id].playerHandl) ,&(gTskPlayer[id].avPlayHandl)));
        TSK_PLAYER_Start(id);
        HI_BOOL bEnable;
        HIS_DIS_GetWinFreezeStatus(&(gTskPlayer[id].winHandl),&bEnable);
        printf("start..win freez is [%d]\n",bEnable);
        if(	bEnable != HI_FALSE)
        {
            HIS_DIS_WinFreeze(&(gTskPlayer[id].winHandl),HI_FALSE,0);
        }


    }

    gTskPlayer[id].playerEna = 1;

    return WV_SOK;
}


/***********************************************************************
WV_S32  TSK_PLAYER_Destory(WV_S32 id);
***********************************************************************/
WV_S32  TSK_PLAYER_Destory(WV_S32 id)
{    
#if 0
    WV_U8  i;
    for(i=0;i<25;i++){

        printf("destory set Alpha=%d \n",i*10);
        HIS_FB_SetAlpha(i*10);
        //usleep(100000);
    }
#endif	


    if(gTskPlayer[id].playerEna == 1)
    {
        system("sync");
        system("echo 3 > /proc/sys/vm/drop_caches");//  echo 3 > /proc/sys/vm/drop_caches");

        //WV_printf(" TSK_PLAYER_Destory  %d\n",id);
#if 1
        HI_BOOL bEnable;
        HIS_DIS_GetWinFreezeStatus(&(gTskPlayer[id].winHandl),&bEnable);
        printf("stop .win freez is [%d]\n",bEnable);
        if(	bEnable != HI_TRUE)
        {
            //设置输出定格在最后一帧画面，防止切换视频黑场
            HIS_DIS_WinFreeze(&(gTskPlayer[id].winHandl),HI_TRUE,0);
        }
#endif
        TSK_PLAYER_Stop(id);
        HIS_PLAYER_Destory(&(gTskPlayer[id].playerHandl));
        gTskPlayer[id].playerEna = 0;

    }
    if(gTskPlayer[id].winEna == 1)
    {

        if( (gTskPlayer[id].winLastRect.s32X != gTskPlayer[id].winRect.s32X) || (gTskPlayer[id].winLastRect.s32Y != gTskPlayer[id].winRect.s32Y) \
                || (gTskPlayer[id].winLastRect.s32Width != gTskPlayer[id].winRect.s32Width) || (gTskPlayer[id].winLastRect.s32Height != gTskPlayer[id].winRect.s32Height))
        {
            //HIS_PLAYER_Destory(&(gTskPlayer[id].playerHandl));
            WV_printf("****destroy window ****\n");
            gTskPlayer[id].winLastRect.s32X 	 = gTskPlayer[id].winRect.s32X;
            gTskPlayer[id].winLastRect.s32Y 	 = gTskPlayer[id].winRect.s32Y;
            gTskPlayer[id].winLastRect.s32Width  = gTskPlayer[id].winRect.s32Width;
            gTskPlayer[id].winLastRect.s32Height = gTskPlayer[id].winRect.s32Height;
            HIS_DIS_WinFreeze(&(gTskPlayer[id].winHandl),HI_TRUE,0);
            HIS_DIS_WinStop(&(gTskPlayer[id].winHandl));
            HIS_DIS_WinDetach(&(gTskPlayer[id].winHandl),&(gTskPlayer[id].avPlayHandl));
            HIS_DIS_WinDestroy(&(gTskPlayer[id].winHandl));
            HIS_AVP_Close(&(gTskPlayer[id].avPlayHandl),id);
            gTskPlayer[id].winEna = 0;
        }


    }

    return WV_SOK;
}

/****************************************************************************

WV_S32 TSK_PLAYER_CMDStart(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_PLAYER_CMDStart(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{


    WV_U16 data,addr;
    WV_U32  temp;
    WV_S32 id,ret = 0;

    if(argc < 2)
    {
        prfBuff += sprintf(prfBuff,"player start <u16Id>  <charFilename>\r\n");
        return WV_SOK;
    }
    //

    ret =WV_STR_S2v(argv[0],&id);

    if(ret != WV_SOK  || id >= TSK_PLAYER_CHNL_NUM )
    {
        prfBuff += sprintf(prfBuff,"input Chennl  erro !\r\n");
        return WV_SOK;
    }

    sprintf(gTskPlayer[id].fileName, "%s",argv[1]);

       TSK_PLAYER_Start(id);
        //sleep(2);
        //printf("----------sleep 2--------------\n");
        //清除窗口冻结
        HI_BOOL bEnable;
        HIS_DIS_GetWinFreezeStatus(&(gTskPlayer[id].winHandl),&bEnable);
        printf("start..win freez is [%d]\n",bEnable);
        if(	bEnable != HI_FALSE)
        {
            HIS_DIS_WinFreeze(&(gTskPlayer[id].winHandl),HI_FALSE,0);
        }


    //TSK_PLAYER_Stop(id);
    //TSK_PLAYER_Start(id);

    prfBuff += sprintf(prfBuff,"Player[%d] start %s \r\n",id,gTskPlayer[id].fileName);
    return WV_SOK;
}



/****************************************************************************

WV_S32 TSK_PLAYER_CMDPlay(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_PLAYER_CMDPlay(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{

    WV_U16 data,addr;
    WV_U32  temp;
    WV_S32 id,ret = 0;

    if(argc <1)
    {
        prfBuff += sprintf(prfBuff,"player play <u32Id> \r\n");
        return WV_SOK;
    }
    //

    ret =WV_STR_S2v(argv[0],&id);

    if(ret != WV_SOK  || id >= TSK_PLAYER_CHNL_NUM )
    {
        prfBuff += sprintf(prfBuff,"input Chennl  erro !\r\n");
        return WV_SOK;
    }

    TSK_PLAYER_Play(id);
    prfBuff += sprintf(prfBuff,"Player[%d] play %s \r\n",id,gTskPlayer[id].fileName);
    return WV_SOK;
}

/****************************************************************************

WV_S32 TSK_PLAYER_CMDPause(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_PLAYER_CMDPause(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{

    WV_U16 data,addr;
    WV_U32  temp;
    WV_S32 id,ret = 0;

    if(argc < 1)
    {
        prfBuff += sprintf(prfBuff,"player pause <u32Id> \r\n");
        return WV_SOK;
    }
    //

    ret =WV_STR_S2v(argv[0],&id);

    if(ret != WV_SOK  || id >= TSK_PLAYER_CHNL_NUM )
    {
        prfBuff += sprintf(prfBuff,"input Chennl  erro !\r\n");
        return WV_SOK;
    }

    TSK_PLAYER_Pause(id);
    prfBuff += sprintf(prfBuff,"Player[%d] pause %s \r\n",id,gTskPlayer[id].fileName);
    return WV_SOK;
}

/****************************************************************************

WV_S32 TSK_PLAYER_CMDResume(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_PLAYER_CMDResume(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{

    WV_U16 data,addr;
    WV_U32  temp;
    WV_S32 id,ret = 0;

    if(argc < 1)
    {
        prfBuff += sprintf(prfBuff,"player resume <u32Id> \r\n");
        return WV_SOK;
    }
    //

    ret =WV_STR_S2v(argv[0],&id);

    if(ret != WV_SOK  || id >= TSK_PLAYER_CHNL_NUM )
    {
        prfBuff += sprintf(prfBuff,"input Chennl  erro !\r\n");
        return WV_SOK;
    }

    TSK_PLAYER_Resume(id);
    prfBuff += sprintf(prfBuff,"Player[%d] resume  %s \r\n",id,gTskPlayer[id].fileName);
    return WV_SOK;
}
/****************************************************************************

WV_S32 TSK_PLAYER_CMDStop(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_PLAYER_CMDStop(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{


    WV_S32 id,ret = 0;

    if(argc <1 )
    {
        prfBuff += sprintf(prfBuff,"player stop  <u32Id> \r\n");
        return WV_SOK;
    }
    //

    ret = WV_STR_S2v(argv[0],&id);

    if(ret != WV_SOK  || id >= TSK_PLAYER_CHNL_NUM )
    {
        prfBuff += sprintf(prfBuff,"input Chennl  erro !\r\n");
        return WV_SOK;
    }


    //prfBuff += sprintf(prfBuff,"Player[%d] Stop  %s \r\n",id,gTskPlayer[id].fileName);

            HI_BOOL bEnable;
        HIS_DIS_GetWinFreezeStatus(&(gTskPlayer[id].winHandl),&bEnable);
        printf("stop .win freez is [%d]\n",bEnable);
        if(	bEnable != HI_TRUE)
        {
            //设置输出定格在最后一帧画面，防止切换视频黑场
            HIS_DIS_WinFreeze(&(gTskPlayer[id].winHandl),HI_TRUE,0);
        }
        TSK_PLAYER_Stop(id);
    return WV_SOK;

}

/****************************************************************************

WV_S32 TSK_PLAYER_CMDChangeMode(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_PLAYER_CMDChangeMode(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{


    WV_S32 mode,ret = 0;

    if(argc <1 )
    {
        prfBuff += sprintf(prfBuff,"player mode  <mode> \r\n");
        return WV_SOK;
    }
    //

    ret = WV_STR_S2v(argv[0],&mode);

    //TSK_PLAYER_Stop(id);
    TSK_PLAYER_ChangeMode(mode);
    prfBuff += sprintf(prfBuff,"play win change mode  %d \r\n",mode);
    return WV_SOK;

}

/***********************************************************************
WV_S32 TSK_PLAYER_GetMode()
***********************************************************************/
WV_S32 TSK_PLAYER_GetMode()
{

    return u32PlayerScreenMode;
}

/***********************************************************************
WV_S32 TSK_PLAYER_ChangeMode(WV_S32 mode)
***********************************************************************/
WV_S32 TSK_PLAYER_ChangeMode(WV_S32 mode)
{

    if(u32PlayerScreenMode != mode)
    {
        HIS_DIS_SetVirtualScreen(mode);
        //HIS_DIS_SetCustomTiming(mode);
        u32PlayerScreenMode = mode;
        //FPGA_CONF_ResetA();
    }
    return WV_SOK;
}

/*******************************************************************
WV_S32 TSK_Player_playRoll(WV_U32 id);
*******************************************************************/
WV_S32 TSK_Player_playRoll(WV_S32 id)
{
    WV_S32 ret;
    ret = HIS_PLAYER_playRoll(&(gTskPlayer[id].playerHandl));
    return ret ;
}

/*******************************************************************
WV_S32 TSK_Player_Replay(WV_U32 id);
*******************************************************************/
WV_S32 TSK_Player_Replay(WV_S32 id)
{
    
    if (gTskPlayer[id].playerEna == 1){
        printf("***********player replay[%d] ********* \n",id);
      HIS_PLAYER_Replay(&(gTskPlayer[id].playerHandl));      
    }
    return WV_SOK;
} 

/*******************************************************************

WV_S32 TSK_Player_SetPlayMode(WV_U32 Mode);

*******************************************************************/
WV_S32 TSK_Player_SetPlayMode(WV_U32 Mode)
{

    //HIS_PLAYER_Replay(&(gTskPlayer[id].playerHandl));
    return WV_SOK;
}

/***********************************************************************
WV_S32  TSK_PLAYER_CMDSetVol(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff);
***********************************************************************/
WV_S32 TSK_PLAYER_CMDSetVol(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{
    WV_S32 vol,id,ret = 0;

    if(argc <2 )
    {
        prfBuff += sprintf(prfBuff,"player setVol <playerID> <val> (0~100)\r\n");
        return WV_SOK;
    }
    //
    ret = WV_STR_S2v(argv[0],&id);
    if(id > 2)
    {
        prfBuff += sprintf(prfBuff,"set vol error ,playerID=%d \r\n",id);
    }
    ret = WV_STR_S2v(argv[1],&vol);
    //TSK_PLAYER_SetVolume(vol);
    TSK_PLAYER_SetVolume(vol,id);
    prfBuff += sprintf(prfBuff,"play set player[%d]vol  %d \r\n",id,vol);

    return WV_SOK;
}
/***********************************************************************
WV_S32  TSK_PLAYER_CMDGetVol(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff);
***********************************************************************/
WV_S32 TSK_PLAYER_CMDGetVol(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{
    WV_S32 vol0=-1,vol1=-1,vol2=-1,id;

    TSK_PLAYER_GetVolume(&vol0,0);
    TSK_PLAYER_GetVolume(&vol1,1);
    TSK_PLAYER_GetVolume(&vol2,2);

    prfBuff += sprintf(prfBuff,"player_0 vol[%d],player_1 vol[%d] ,player_2 vol[%d]\r\n",vol0,vol1,vol2);

    return WV_SOK;
}
/***********************************************************************
WV_S32  TSK_PLAYER_CMDCreate(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff);
***********************************************************************/
WV_S32 TSK_PLAYER_CMDCreate(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{
    WV_S32 ret = -1;
    WV_S32 id = 0 ;
   if(argc <2 )
    {
        prfBuff += sprintf(prfBuff,"player create <playerID> \r\n");
        return WV_SOK;
    }
    //prfBuff += sprintf(prfBuff,"player_0 vol[%d],player_1 vol[%d] ,player_2 vol[%d]\r\n",vol0,vol1,vol2);
    ret = WV_STR_S2v(argv[0],&id);
    if(ret != 0 ) return WV_EFAIL;

    TSK_PLAYER_Creat(id);
    
    return WV_SOK;
}

/***********************************************************************
WV_S32  TSK_PLAYER_CMDDesctory(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff);
***********************************************************************/
WV_S32 TSK_PLAYER_CMDDesctory(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{
    WV_S32 ret = -1;
    WV_S32 id = 0 ;
   if(argc <2 )
    {
        prfBuff += sprintf(prfBuff,"player create <playerID> \r\n");
        return WV_SOK;
    }
    //prfBuff += sprintf(prfBuff,"player_0 vol[%d],player_1 vol[%d] ,player_2 vol[%d]\r\n",vol0,vol1,vol2);
    ret = WV_STR_S2v(argv[0],&id);
    if(ret != 0 ) return WV_EFAIL;

    TSK_PLAYER_Destory(id);
    return WV_SOK;
}

/***********************************************************************
WV_S32  TSK_PLAYER_Open();
***********************************************************************/

WV_S32  TSK_PLAYER_Open()
{ 
    WV_S32 id;
    TSK_PLAYER_Init();      //player初始化
    //注册一些跟player相关的终端命令
    WV_CMD_Register("player",NULL,"palyer comand ",NULL);
    WV_CMD_Register("player","start","palyer comand start media file",TSK_PLAYER_CMDStart);
    WV_CMD_Register("player","play","palyer comand  Play",TSK_PLAYER_CMDPlay);
    WV_CMD_Register("player","pause","palyer comand pause",TSK_PLAYER_CMDPause);
    WV_CMD_Register("player","resume","palyer comand resume ",TSK_PLAYER_CMDResume);
    WV_CMD_Register("player","stop","palyer comand stop",TSK_PLAYER_CMDStop);
    WV_CMD_Register("player","mode","palyer comand mode",TSK_PLAYER_CMDChangeMode);
    WV_CMD_Register("player","setvol","palyer comand set vol",TSK_PLAYER_CMDSetVol);
    WV_CMD_Register("player","getvol","palyer comand get vol",TSK_PLAYER_CMDGetVol);
    WV_CMD_Register("player","create","palyer comand create",TSK_PLAYER_CMDCreate);
    WV_CMD_Register("player","destory","palyer comand destory",TSK_PLAYER_CMDDesctory);
    
    return WV_SOK;
}


/***********************************************************************
WV_S32  TSK_PLAYER_Close();
***********************************************************************/

WV_S32  TSK_PLAYER_Close()
{ 
    return WV_SOK;
}

/***********************************************************************
WV_S32 TSK_PLAYER_VidFrameToPicture(HI_CHAR *FileName);
***********************************************************************/

WV_S32 TSK_PLAYER_VidFrameToPicture(WV_S32 player)
{
    HI_S32 Ret;
    HI_UNF_VIDEO_FRAME_INFO_S VidFrame;
    HI_HANDLE MemSurface;
    HIGO_SURINFO_S MemSurfaceInfo = { 0 };
    HIGO_BLTOPT_S BltOpt = { 0 };
    BltOpt.EnableScale = HI_TRUE;
    HI_HANDLE screenSurf[2];
    HI_RECT Rect;

    if (player >= TSK_SCENE_MOV_USE_NUM)
    {
        return WV_EFAIL;
    }
    Ret = HI_UNF_VO_CapturePicture(gTskPlayer[player].winHandl, &VidFrame);
    if (HI_SUCCESS != Ret)
    {
        printf("[%s] HI_UNF_VO_CapturePicture failed 0x%x\n", __FUNCTION__, Ret);
        return WV_EFAIL;
    }
    MemSurfaceInfo.Width = VidFrame.u32Width;
    MemSurfaceInfo.Height = VidFrame.u32Height;
    MemSurfaceInfo.PixelFormat = HIGO_PF_BUTT;
    Rect.x = 0;
    Rect.y = 0;
    Rect.w = MemSurfaceInfo.Width;
    Rect.h = MemSurfaceInfo.Height;
    switch (VidFrame.enVideoFormat)
    {
        case HI_UNF_FORMAT_YUV_SEMIPLANAR_422:
            MemSurfaceInfo.PixelFormat = HIGO_PF_YUV422;
            break;
        case HI_UNF_FORMAT_YUV_SEMIPLANAR_420:
            MemSurfaceInfo.PixelFormat = HIGO_PF_YUV420;
            break;
        case HI_UNF_FORMAT_YUV_SEMIPLANAR_400:
            MemSurfaceInfo.PixelFormat = HIGO_PF_YUV400;
            break;
        default:
            printf("[%s] not support format %d\n", __FUNCTION__, VidFrame.enVideoFormat);
    }
    if (HIGO_PF_BUTT != MemSurfaceInfo.PixelFormat)
    {
        MemSurfaceInfo.Pitch[0] = VidFrame.stVideoFrameAddr[0].u32YStride;
        MemSurfaceInfo.Pitch[1] = VidFrame.stVideoFrameAddr[0].u32CStride;
        MemSurfaceInfo.pPhyAddr[0] = (HI_CHAR*)VidFrame.stVideoFrameAddr[0].u32YAddr;
        MemSurfaceInfo.pPhyAddr[1] = (HI_CHAR*)VidFrame.stVideoFrameAddr[0].u32CAddr;
        MemSurfaceInfo.pVirAddr[0] = HI_NULL;
        MemSurfaceInfo.pVirAddr[1] = HI_NULL;
        MemSurfaceInfo.MemType = HIGO_MEMTYPE_MMZ;
        MemSurfaceInfo.bPubPalette = HI_FALSE;
        Ret = HI_GO_CreateSurfaceFromMem(&MemSurfaceInfo, &MemSurface);
        if (HI_SUCCESS != Ret)
        {
            HI_GO_FreeSurface(MemSurface);
            HI_UNF_VO_CapturePictureRelease(gTskPlayer[player].winHandl, &VidFrame);
            printf("[%s] HI_GO_CreateSurface failed 0x%x\n", __FUNCTION__, Ret);
            return WV_EFAIL;
        }
        Ret = HIS_FB_SurfGetForPic(screenSurf);
        if (Ret == WV_EFAIL)
        {
            HI_GO_FreeSurface(MemSurface);
            HI_UNF_VO_CapturePictureRelease(gTskPlayer[player].winHandl, &VidFrame);
            printf("[%s] HIS_FB_SurfGetForPic failed 0x%x\n", __FUNCTION__, Ret);
            return WV_EFAIL;
        }
        if (player == 0x00)
        {
            Rect.x = 0;
            Rect.y = 0;
            Rect.w = Rect.w / 2;
            Rect.h = 539;
        }
        else if (player == 0x01)
        {
            Rect.x = 0;
            Rect.y = 539;
            Rect.w = Rect.w / 2;
            Rect.h = 539;
        }
        BltOpt.EnableScale = HI_TRUE;
        WV_CHECK_RET(HI_GO_Blit(MemSurface, NULL, screenSurf[0], &Rect, &BltOpt));
        Ret = HI_GO_FreeSurface(MemSurface);
        if (HI_SUCCESS != Ret)
        {
            HI_UNF_VO_CapturePictureRelease(gTskPlayer[player].winHandl, &VidFrame);
            printf("[%s] HI_GO_FreeSurface failed 0x%x\n", __FUNCTION__, Ret);
            return WV_EFAIL;
        }
    }
    Ret = HI_UNF_VO_CapturePictureRelease(gTskPlayer[player].winHandl, &VidFrame);
    if (HI_SUCCESS != Ret)
    {
        printf("[%s] HI_UNF_VO_CapturePictureRelease failed 0x%x\n", __FUNCTION__, Ret);
        return WV_EFAIL;
    }
    return WV_SOK;
}


