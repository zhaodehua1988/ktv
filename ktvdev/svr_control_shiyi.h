
#ifndef  _SVR_CONTROL_SHIYI_H_H_
#define  _SVR_CONTROL_SHIYI_H_H_
#include "wv_common.h"  
#include "svr_control_cmd.h"

WV_S32 SVR_CONTROL_ShiYi_AnalyzeMovClassCmd(WV_S8 *buf);
WV_S32 SVR_CONTROL_ShiYi_AnalyzeSwitchCmd(WV_S8 *buf);
WV_S32 SVR_CONTROL_ShiYi(WV_S8 *pData,WV_S32 len);
WV_S32 SVR_CONTROL_ShiYi_Init();
#endif

