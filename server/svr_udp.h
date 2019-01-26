#ifndef _SVR_UDP_UDP_H_
#define _SVR_UDP_UDP_H_

#include"wv_common.h"

typedef struct  SVR_UDP_HEAD_E 
{ 
 WV_U32      sync;
 WV_U8       cmdL0;
 WV_U8       cmdL1; 
 WV_U16      cnt;
 WV_U16      arg1;
 WV_U16      arg2;
 WV_U32      dataNum;   
} SVR_UDP_HEAD_E;

WV_S32 SVR_UDP_Open();
WV_S32 SVR_UDP_Close();
WV_S32 SVR_UDP_SyncScene(WV_U32 id);
WV_S32 SVR_UDP_SearchDev();
WV_S32 SVR_UDP_PlayerSync();
WV_S32 SVR_UDP_SyncScenePause();
WV_S32 SVR_UDP_SyncSceneStop();
WV_S32 SVR_UDP_SyncScenePlay();
WV_S32 SVR_UDP_PlayNotifySlave();
WV_S32 SVR_UDP_SyncProjectorStatus(WV_U16 status);
#endif
