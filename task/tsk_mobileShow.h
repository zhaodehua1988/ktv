#ifndef _TSK_MOBILE_SHOW_H_H__
#define _TSK_MOBILE_SHOW_H_H__
#include "wv_common.h"

WV_S32 TSK_Mobile_GetShowFlag();
WV_S32 TSK_Mobile_getPicCleanFlag();
WV_S32 TSK_Mobile_ShowPic(WV_U8 *pData,WV_U32 dataLen,WV_U8 cmd);
WV_S32 TSK_Mobile_PlayMovUrl(WV_U8 cmd,WV_U8 *url,WV_S32 len);
#endif
