#ifndef  _SYS_LICENCES_H_H_
#define  _SYS_LICENCES_H_H_
#include "wv_common.h" 
WV_S32 SYS_LICENCES_Open();
WV_S32 SYS_LICENCES_Close();

WV_S32 SYS_LICENCES_Check_ID(WV_U8 *pId);
WV_S32 SYS_LICENCES_GetID(WV_S32 *pLen,WV_U8 *pBuf);
WV_S32 SYS_LICENCES_WriteLic(WV_U8 *pLic,WV_S32 len);
WV_S32 SYS_LIC_Encrypt(WV_U8 *pIn,WV_S8 *pwd,WV_U8 *pOut,WV_U32 len);
WV_S32 SYS_LIC_GetTimeOut(WV_U8 *pData);
#endif
