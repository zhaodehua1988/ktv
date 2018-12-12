#ifndef  _SYS_ENV_ENV_H_H_
#define  _SYS_ENV_ENV_H_H_
#include "wv_common.h"  
WV_S32  SYS_ENV_Open();
WV_S32  SYS_ENV_Close();
WV_S32  SYS_ENV_Register(WV_S8 type ,WV_S8 * pName,WV_S8 *pDefault,WV_S8  *pNote);
WV_S32  SYS_ENV_Get(WV_S8 *pName ,WV_S8 *pValue);
WV_S32  SYS_ENV_GetU32(WV_S8 *pName ,WV_U32 * pValue);
WV_S32  SYS_ENV_Set(WV_S8 *pName ,WV_S8 *pValue);
WV_S32  SYS_ENV_SetU32(WV_S8 *pName ,WV_U32 data);
WV_S32  SYS_ENV_Save();

WV_S32  SYS_ENV_GetVersion(WV_U16 *pVer);

#define  SYS_ENV_VERSION_NO       "1.6.3.4"       //版本号

#endif
