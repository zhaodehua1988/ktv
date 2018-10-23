/**
 \file
 \brief common module
 \author Shenzhen Hisilicon Co., Ltd.
 \date 2008-2018
 \version 1.0
 \author
 \date 2011-10-9
 */

#ifndef __HI_SVR_CHARSET_H__
#define __HI_SVR_CHARSET_H__

#include "hi_type.h"
#include "hi_svr_charset_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/******************************* API declaration *****************************/
/** \addtogroup      Hiplayer */
/** @{ */  /** <!-- [Hiplayer] */

extern HI_CHARSET_FUN_S g_stCharsetMgr_s;

/**
\brief  init CHARSET module. CNcomment:��ʼ��CHARSETģ��CNend
\attention \n
None. CNcomment:��CNend
\param None. CNcomment:��CNend

\retval ::HI_SUCCESS success. CNcomment:��ʼ���ɹ�ʱ����HI_SUCCESS CNend
\retval ::HI_FAILURE fail. CNcomment:��ʼ��ʧ��ʱ����HI_FAILURE CNend

\see \n
None. CNcomment:��CNend
*/
HI_S32 HI_CHARSET_Init(HI_VOID);

/**
\brief deinit CHARSET module. CNcomment:ȥ��ʼ��CHARSETģ��CNend
\attention \n
None. CNcomment:��CNend
\param None. CNcomment:��CNend

\retval ::HI_SUCCESS success. CNcomment:ȥ��ʼ���ɹ�ʱ����HI_SUCCESS CNend
\retval ::HI_FAILURE fail. CNcomment:ȥ��ʼ��ʧ��ʱ����HI_FAILURE CNend

\see \n
None.  CNcomment:��CNend
*/
HI_S32 HI_CHARSET_Deinit(HI_VOID);

/**
\brief regist charset lib. CNcomment:ע��charset��̬��CNend
\attention \n
None.  CNcomment:��CNend
\param[in] dllName charset lib name. CNcomment:charset��̬������CNend

\retval ::HI_SUCCESS success. CNcomment:ע��ɹ�ʱ����HI_SUCCESS CNend
\retval ::HI_FAILURE fail, this lib regist already. CNcomment:ע��ʧ��ʱ����HI_FAILURE ,�ö�̬���Ѿ�ע�����CNend

\see \n
None. CNcomment:��CNend
*/
HI_S32 HI_CHARSET_RegisterDynamic(const HI_CHAR *dllName);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_SVR_CHARSET_H__ */
