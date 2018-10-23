#ifndef  _SYS_DATE_ENV_H_H_
#define  _SYS_DATE_ENV_H_H_
#include "wv_common.h" 
 
WV_S32 SYS_DATE_Init();
WV_S32 SYS_DATE_DeInit();

WV_S32 SYS_DATE_SetYear(WV_S8 * pYear);
WV_S32 SYS_DATE_SetMon(WV_S8 * pMon);
WV_S32 SYS_DATE_SetDay(WV_S8 * pDay);
WV_S32 SYS_DATE_SetNum(WV_S8 * pNum);

WV_S32 SYS_DATE_GetYear(WV_S8 * pYear);
WV_S32 SYS_DATE_GetMon(WV_S8 * pMon);
WV_S32 SYS_DATE_GetDay(WV_S8 * pDay);
WV_S32 SYS_DATE_GetNum(WV_S8 * pNum);

WV_S32 SYS_DATE_SaveConf();

#endif
