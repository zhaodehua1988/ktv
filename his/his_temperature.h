#ifndef  _HIS_TEMPERATURE_H_
#define  _HIS_TEMPERATURE_H_
#include "wv_common.h" 
WV_S32  HIS_TEMP_Init();
WV_S32  HIS_TEMP_DeInit();
WV_S32 HIS_TEMP_ReadTemp(WV_S16 * pTemp);
#endif
