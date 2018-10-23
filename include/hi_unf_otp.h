#ifndef __HI_UNF_OTP_H__
#define __HI_UNF_OTP_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define HI_UNF_OTP_Open(HI_VOID) HI_UNF_OTP_Init(HI_VOID)
#define HI_UNF_OTP_Close(HI_VOID) HI_UNF_OTP_DeInit(HI_VOID)

/******************************* API declaration *****************************/
/** \addtogroup      OTP */
/** @{ */  /** <!-- [OTP] */


/** 
\brief Init an otp device.
\brief CNcomment:��ʼ��OTP�豸�� CNend
\attention \n
N/A
\param N/A                                    				CNcomment:�ޡ� CNend
\retval ::HI_SUCCESS  Call this API successful.           CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 				CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_Init(HI_VOID);

/** 
\brief Deinit an otp device.
\brief CNcomment:ȥ��ʼ��OTP�豸�� CNend
\attention \n
N/A
\param N/A                                    				CNcomment:�ޡ� CNend
\retval ::HI_SUCCESS  Call this API successful.          CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 				CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_DeInit(HI_VOID);

/** 
\brief Set customer key, the customer key is used by the customer to encrypt some private data.
\brief CNcomment:����customer key��customer key�ɿͻ��Լ����壬���Զ�����;�� CNend
\attention \n
N/A
\param[in] pKey:  Customer key to be written to otp.    				CNcomment:Customer key��ֵ����д��OTP�� CNend
\param[in] u32KeyLen:  The length of customer key, must be 16bytes.		CNcomment:Customer key�ĳ��ȣ�����Ϊ16�ֽڡ�           CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_SetCustomerKey(HI_U8 *pKey, HI_U32 u32KeyLen);

/** 
\brief Get customer key, the customer key is used by the customer to encrypt some private data.
\brief CNcomment:��ȡcustomer key�� CNend
\attention \n
N/A
\param[in] pKey:Buffer to store the customer key read from otp.		CNcomment:�洢customer key��buffer�� CNend
\param[in] u32KeyLen:The length of buffer, must be 16bytes. 			CNcomment:customer key �ĳ��ȣ�����Ϊ16�ֽڡ� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_GetCustomerKey(HI_U8 *pKey, HI_U32 u32KeyLen);


/** 
\brief Set stb private data, the stb private data is used by the customer to set some private data.
\brief CNcomment:���ÿͻ�˽�����ݡ� CNend
\attention \n
N/A
\param[in] u32Offset:  The offset to set the private data, should be between 0 and 15.	CNcomment:����stbprivData��ƫ�ƣ�����Ϊ0~15֮�䡣 CNend
\param[in] u8Data:  The private data to be set.							CNcomment:��Ҫ���õ�ֵ�� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_SetStbPrivData(HI_U32 u32Offset, HI_U8 u8Data);

/** 
\brief Get stb private data, the stb private data is used by the customer to set some private data.
\brief CNcomment:��ȡ�ͻ�˽�����ݡ� CNend
\attention \n
N/A
\param[in] u32Offset:  The offset to get the private data, should be between 0 and 15.  CNcomment:��ȡstbprivData��ƫ�ƣ�����Ϊ0~15֮�䡣 CNend
\param[out] pu8Data:  The data read from otp.  							CNcomment:��ȡ��ֵ�� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_GetStbPrivData(HI_U32 u32Offset, HI_U8 *pu8Data);


/** 
\brief  Set hdcp root key to otp, 16bytes length.
\brief CNcomment:��оƬ�ڲ�д��hdcp root key������Ϊ16�ֽڳ��� CNend
\attention \n
N/A
\param[in] pu8RootKey:  Set hdcp root key to otp.					CNcomment����hdcp root key��OTP�� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_WriteHdcpRootKey(HI_U8 *pu8HdcpRootKey, HI_U32 u32Keylen);

/** 
\brief  Get hdcp root key from otp, 16bytes length.
\brief CNcomment:��ȡоƬ�ڲ����õ�hdcp root key������Ϊ16�ֽڳ��� CNend
\attention \n
N/A
\param[out] pu8RootKey:  Point to hdcp root key from otp.				CNcomment:��OTP�л�ȡhdcp root key�� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_ReadHdcpRootKey(HI_U8 *pu8HdcpRootKey, HI_U32 u32Keylen);
    
/** 
\brief  Lock hdcp root key in otp.
\brief CNcomment:����оƬ�ڲ����õ�hdcp root key��������hdcp root key�����ɱ���ȡ�� CNend
\attention \n
N/A
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_LockHdcpRootKey(HI_VOID);

/** 
\brief  Get hdcp root Key lock flag.
\brief CNcomment:��ȡоƬ�ڲ����õ�hdcp root key��������־λ�� CNend
\attention \n
N/A
\param[out] pbLockFlag:  Point to hdcp root key lock flag from otp.		CNcomment:ָ���ȡ���ı�־λ�� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_GetHdcpRootKeyLockFlag(HI_BOOL *pbLockFlag);

/** 
\brief Set stb root key to otp.
\brief CNcomment:��оƬ�ڲ�����stb root key�� CNend
\attention \n
N/A
\param[in] u8StbRootKey:  Point to stb root key value.					CNcomment:Stb root key��ֵ�� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_WriteStbRootKey(HI_U8 *pu8StbRootKey, HI_U32 u32Keylen);

/** 
\brief Get stb root key from otp.
\brief CNcomment:��ȡоƬ�ڲ����õ�stb root key�� CNend
\attention \n
N/A
\param[out] pu8RootKey:  Point to stb root key from otp.				CNcomment:ֻ���ȡ����Stb root key�� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_ReadStbRootKey(HI_U8 *pu8StbRootKey, HI_U32 u32Keylen);

/** 
\brief Lock stb root key in otp.
\brief CNcomment:��סоƬ�ڲ����õ�stb root key��������stb root key�����ɱ���ȡ�� CNend
\attention \n
N/A
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_LockStbRootKey(HI_VOID);

/** 
\brief Get stb root Key lock flag
\brief CNcomment:��ȡоƬ�ڲ�stb root key��������־λ�� CNend
\attention \n
N/A
\param[out] pbLockFlag:  Point to stb root key lock flag from otp.		CNcomment:ָ���ȡ���ı�־λ�� CNend
\retval ::HI_SUCCESS  Call this API successful.          			CNcomment:APIϵͳ���óɹ��� CNend
\retval ::HI_FAILURE  Call this API fails. 							CNcomment:APIϵͳ����ʧ�ܡ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_OTP_GetStbRootKeyLockFlag(HI_BOOL *pbLockFlag);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_UNF_OTP_H__ */