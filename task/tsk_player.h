#ifndef  _TSK_PLAYER_LOCAL_H_
#define  _TSK_PLAYER_LOCAL_H_
#include "wv_common.h" 
#include "hi_type.h"
#include "hi_go_comm.h"

WV_S32  TSK_PLAYER_Open();
WV_S32  TSK_PLAYER_Close(); 

WV_S32  TSK_PLAYER_SetFile(WV_S32 id,WV_S8 * pFile);
WV_S32  TSK_PLAYER_setWin(WV_S32 id,WV_U16 x,WV_U16 y,WV_U16 w,WV_U16 h);
WV_S32  TSK_PLAYER_Creat(WV_S32 id);
WV_S32  TSK_PLAYER_Destory(WV_S32 id);
WV_S32  TSK_PLAYER_Start(WV_S32 id);
WV_S32  TSK_PLAYER_Play(WV_S32 id);
WV_S32  TSK_PLAYER_Pause(WV_S32 id);
WV_S32  TSK_PLAYER_Resume(WV_S32 id);
WV_S32  TSK_PLAYER_Stop(WV_S32 id);
WV_S32  TSK_PLAYER_ChangeMode(WV_S32 mode);
WV_S32  TSK_PLAYER_Seek(WV_S32 id,HI_U64 u64TimeInMs);
WV_S32  TSK_PLAYER_GetMode();
WV_S32  TSK_Player_Replay(WV_S32 id);
WV_S32  TSK_PLAYER_GetStatus(WV_S32 id);
WV_S32  TSK_PLAYER_DevCascade(WV_S32 id,WV_U32 DevCascading);
WV_S32  TSK_PLAYER_FreezeWindowBlack();
WV_S32  TSK_PLAYER_SetHiPlayNext(WV_U32 ena,HI_HANDLE playerHandl);
WV_S32  TSK_PLAYER_GetUsbPlayNext();
WV_S32  TSK_PLAYER_SetUsbPlayNext(WV_U32 ena,WV_U32 playerID);
WV_S32  TSK_PLAYER_SetVolume(WV_S32 volume,WV_U32 playerID);
WV_S32  TSK_PLAYER_GetVolume(WV_S32 *pVolume,WV_U32 playerID);
WV_S32  TSK_PLAYER_ReadVolume();
WV_S32 TSK_PLAYER_VidFrameToPicture(WV_S32 player);

#define TSK_PLAYER_MODE_3840_1080  0
#define TSK_PLAYER_MODE_3840_2160  1
#define TSK_PLAYER_MODE_1920_1080  2


#endif
