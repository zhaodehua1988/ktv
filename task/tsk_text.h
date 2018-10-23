#ifndef _TSK_TEXT_H_H__
#define _TSK_TEXT_H_H__
#include "wv_common.h"
#include "tsk_go.h"
#include "tsk_scene.h"

WV_S32 TSK_TEXT_PutText(WV_U8 * pTxBuf,WV_U32 len);

WV_S32 TSK_TEXT_Open();
WV_S32 TSK_TEXT_Close();
WV_S32 TSK_TEXT_GetConf(WV_U8 saveMode,WV_U32 dataLen,WV_U8 *pData);
#endif

