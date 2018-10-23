/**
 \file
 \brief Error codes of the server (SVR) player module. CNcomment:svr playerģ�������CNend
 \author Shenzhen Hisilicon Co., Ltd.
 \date 2008-2018
 \version 1.0
 \author
 \date 2010-02-10
 */

#ifndef __HI_SVR_PLAYER_ERRNO_H__
#define __HI_SVR_PLAYER_ERRNO_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
/***************************** Macro Definition ******************************/
/** \addtogroup      Hiplayer */
/** @{ */  /** <!--[Hiplayer] */

/** Unsupported file formats, HI_SUCCESS = 0 */
/** CNcomment:��֧�ֵ��ļ���ʽ��HI_SUCCESS = 0 */
#define HI_ERRNO_NOT_SUPPORT_FORMAT    (HI_SUCCESS + 0x1)

/** Unsupported protocols, HI_SUCCESS = 0 */
/** CNcomment:��֧�ֵ�Э�飬HI_SUCCESS = 0 */
#define HI_ERRNO_NOT_SUPPORT_PROTOCOL  (HI_SUCCESS + 0x2)
 /** @} */  /*! <!-- Macro Definition end */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_SVR_PLAYER_ERRNO_H__ */
