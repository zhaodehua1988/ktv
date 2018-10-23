#ifndef  _SYS_INFO_H_H_
#define  _SYS_INFO_H_H_
#include "wv_common.h" 


WV_S32 SYS_INFO_Open();
WV_S32 SYS_INFO_Close();
WV_S32 SYS_INFO_DevReset();
WV_S32 SYS_INFO_GetInfo(WV_U32 *pDataLen,WV_S8 *pData );
#endif
