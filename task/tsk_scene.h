#ifndef  _TASK_SCENE_SCENE_H_
#define  _TASK_SCENE_SCENE_H_
#include "wv_common.h"
#include "fpga_conf.h"
#include "his_go.h"
//#include "svr_frame.h"
#define TSK_SCENE_MAX_NUM        64
#define TSK_SCENE_WIN_MAX_NUM    16
#define TSK_SCENE_GIF_MAX_NUM   HIS_HIGO_GODEC_NUM
#define TSK_SCENE_MOV_MAX_NUM    4
#define TSK_SCENE_MOV_FILE_MAX_NUM    512
#define TSK_SCENE_MOV_USE_NUM    3



#define TSK_SCENE_SYNC_NONE    0
#define TSK_SCENE_SYNC_MASTER  1
#define TSK_SCENE_SYNC_SLAVE   2


#define TSK_SCENE_MAX_SYNCDEV 6 

/**************scene *********************/
#define TSK_CONF_SCENE_COUNT  (TSK_SCENE_MAX_NUM )  //64             // 场景最大个数
#define TSK_CONF_SCENE_NAME_MAX_LEN     256                          // 场景名称大小               
#define TSK_CONF_SCENE_OUTLINE_MAX_NUM  (TSK_SCENE_WIN_MAX_NUM)//16  // OutLine最大个数
#define TSK_CONF_SCENE_BKPIC_MAX_NUM    (TSK_SCENE_GIF_MAX_NUM)//16  // 背景图片最大个数


#define TSK_CONF_MOV_TYPE_MAX_NUM  64
#define TSK_CONF_MOV_TYPE_NAME_LEN 8

#define TSK_CONF_SCENE_TYPE_MAX_NUM  64
#define TSK_CONF_SCENE_TYPE_NAME_LEN 8

#define TSK_SCENE_TYPE_NETDATA 1
#define TSK_SCENE_TYPE_UARTDATA 2

#pragma pack(push)
#pragma pack(1)

typedef struct TSK_SONF_SCENE_RECT_INFO
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

}TSK_CONF_SCENE_CONF;

typedef struct TSK_SCENE_DEC_INFO_S 
{
    WV_U32    id;
	WV_U32    ena;
    WV_U32    x;
    WV_U32    y;
    WV_U32    w;
    WV_U32    h; 
} TSK_SCENE_DEC_INFO_S ; 


typedef struct TSK_SCENE_INFO_S
{
 WV_U32 sceneEna;
 TSK_SCENE_DEC_INFO_S    mov[TSK_SCENE_MOV_MAX_NUM];
 WV_U32  animationNum;
 TSK_SCENE_DEC_INFO_S    animation[TSK_SCENE_GIF_MAX_NUM];
 WV_U32  winNum;
 FPGA_WIN_INFO_S    win[TSK_SCENE_WIN_MAX_NUM];

} TSK_SCENE_INFO_S;

#pragma pack(pop)

WV_S32 TSK_SCENE_Init();
WV_S32 TSK_SCENE_DeInit();

WV_S32 TSK_SCENE_SceneClose();
WV_S32 TSK_SCENE_SceneOpen();

WV_S32 TSK_SCENE_Change(WV_U32 DataType, WV_U32  id);
WV_S32 TSK_SCENE_Restart();
WV_S32 TSK_SCENE_Save(WV_U32 id);

WV_S32 TSK_SCENE_SetMov(WV_U32 num,WV_U32 id);
WV_S32 TSK_SCENE_SetMovArea(WV_U32 num,WV_U32 ena,WV_U32 x,WV_U32 y,WV_U32 w,WV_U32 h);
WV_S32 TSK_SCENE_ConfMov(WV_U32 num);

WV_S32 TSK_SCENE_SetAniNum(WV_U32    num);
WV_S32 TSK_SCENE_SetAni(WV_U32    num,WV_U32    id,WV_U32    x,WV_U32    y,WV_U32    w,WV_U32 h);
WV_S32 TSK_SCENE_ConfAni();

WV_S32 TSK_SCENE_SetWinNum(WV_U32    num);
WV_S32 TSK_SCENE_SetWin(WV_U32    num,WV_U32    id,WV_U32    x,WV_U32    y,WV_U32    w,WV_U32 h,WV_U32 vid);
WV_S32 TSK_SCENE_ConfWin();

WV_S32 TSK_SCENE_GetMovArea(WV_U32 num,WV_U32 *pEna,WV_U32* pX,WV_U32* pY,WV_U32* pW,WV_U32 *pH);

WV_S32 TSK_SCENE_PlayMovUrl(WV_U8 cmd,WV_U8 *url,WV_S32 len);
WV_S32 TSK_SCENE_GetCurent(TSK_SCENE_INFO_S *pInfo);
WV_S32 TSK_SCENE_GetState();
WV_S32 TSK_SCENE_GetSceneCurId();
WV_S32 TSK_SCENE_GetSyncEna();
WV_S32 TSK_SCENE_GetCascading(WV_U8 *pCascadeInfo,WV_U16 *pCascadeNum,WV_U8 *pSlaveIp);
WV_S32 TSK_SCENE_SetCascading(WV_U8 cascadeInfo,WV_U16 cascadeNum,WV_U8 *pSlaveIp);
WV_S32 TSK_SCENE_SendSync();

WV_S32 TSK_SCENE_GetPlayMode();
WV_S32 TSK_SCENE_SetPlayMode(WV_U32 mode);
WV_S32 TSK_SCENE_ChangePlayMode(WV_U32 mode);

WV_S32 TSK_SCENE_OpenWinAndAni();
WV_S32 TSK_SCENE_CloseWinAndAni();
WV_S32 TSK_SCENE_SaveAngle(WV_U32 dataLen,WV_U8 *pData);
WV_S32 TSK_SCENE_GetAngle(WV_U32 *pDataLen,WV_U8 *pData);
WV_S32 TSK_SCENE_GetLightEna();
WV_S32 TSK_SCENE_SetLightEna(WV_U32 ena);

void TSK_SCENE_PlayerPlay();
void TSK_SCENE_PlayerPause();
void TSK_SCENE_PreScene();
void TSK_SCENE_NextScene();
void TSK_SCENE_PlayerVolUp();
void TSK_SCENE_PlayerVolDown();
void TSK_SCENE_PlayerStop();

WV_S32 TSK_SCENE_Standby(WV_S32 mode);
 WV_S32 TSK_SCENE_StartingUP(WV_S32 mode);

WV_S32 TSK_SCENE_AddLastWin();
WV_S32 TSK_SCENE_DeletLastWin();
WV_S32 TSK_SCENE_SetWinChange(WV_S32 ena);
#endif
