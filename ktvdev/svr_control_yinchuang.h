#ifndef  _SVR_CONTROL_YINCHUANG_H_H_
#define  _SVR_CONTROL_YINCHUANG_H_H_
#include "wv_common.h"  
#include "svr_control_cmd.h"
WV_S32 SVR_CONTROL_YinChuang_Init();
WV_S32 SVR_CONTROL_YinChuang_ControlCmd(WV_S8 *buf);
WV_S32 SVR_CONTROL_YinChuang_AnalyzeMovClassCmd(WV_S8 *buf);
WV_S32 SVR_CONTROL_YinChuang(WV_S8 *pData,WV_S32 len);
#endif

