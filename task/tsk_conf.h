#ifndef  _TSK_CONF_H_H_
#define  _TSK_CONF_H_H_
#include "wv_common.h"  
#include "fpga_conf.h" 
#include "sys_env.h"
#include "tsk_scene.h"
WV_S32 TSK_CONF_GetResolution(WV_U8 *pData,WV_U32 *pDataLen);
WV_S32 TSK_CONF_GetSplit(WV_U8 *pData,WV_U32 *pDataLen); 
WV_S32 TSK_CONF_GetMovInfo(WV_U8 * pBuf,WV_U32 *pDataLen);
WV_S32 TSK_CONF_MovInfo_Init();
WV_S32 TSK_CONF_MovInfo_DeInit();

WV_S32 TSK_CONF_ChangeMov(WV_U32 movNum,WV_S8 *pData ,WV_U32 dataLen);

WV_S32 TSK_CONF_UpdateMov(WV_U8 *pData ,WV_U32 dataLen);
WV_S32 TSK_CONF_UpdateMovName(WV_U8 *pData ,WV_U32 dataLen);
WV_S32 TSK_CONF_UpdataMovEnd(WV_U8 *pData ,WV_U32 dataLen);
WV_S32 TSK_CONF_DeleteMov(WV_U8 *pData ,WV_U32 dataLen);

WV_S32 TSK_CONF_SceneInfo_Init();
WV_S32 TSK_CONF_SceneInfo_DeInit();
WV_S32 TSK_CONF_ChangeScene(WV_U8 *pData ,WV_U32 dataLen);
WV_S32 TSK_CONF_GetSceneInfo(WV_U8 * pBuf,WV_U32 *pDataLen);
WV_S32 TSK_CONF_SaveCurrentScene(WV_U8 *pData ,WV_U32 dataLen);
WV_S32 TSK_CONF_SceneDelet(WV_U8 * pBuf,WV_U32 dataLen);
WV_S32 TSK_CONF_SaveNewScene(WV_U8 *pData ,WV_U32 dataLen);

WV_S32 TSK_CONF_changeMovByType(WV_S8 *pType);
WV_S32 TSK_CONF_saveMovType(WV_U8 *pType,WV_U32 dataLen);
WV_S32 TSK_CONF_cpVideoToLocal(WV_S8 *srcFilePath,WV_S8 *desFilePath); 
WV_S32 TSK_CONF_MovGetConf();

WV_S32 TSK_CONF_SaveSceneType(WV_U8 id,WV_U8 type);
WV_S32 TSK_CONF_changeSceneByType(WV_U8 *pType);

WV_S32 TSK_CONF_changeMovRollType();

WV_S32 TSK_CONF_SaveCustomScene(WV_U8 *pData ,WV_U32 dataLen);
WV_S32 TSK_CONF_SaveFadeinout(WV_U8 Fade);
WV_S32 TSK_CONF_LoadFadeinout(WV_U8 *Fade);
WV_S32 TSK_CONF_Mov_GetIDByName(WV_U32 *pid,WV_S8 *pFileName); 
WV_S32 TSK_CONF_ReNameMov(WV_U8 *pData ,WV_U32 dataLen);
WV_S32 TSK_CONF_SetGif(WV_U8 * pBuf,WV_U32 dataLen,WV_U8 chn);
#endif
