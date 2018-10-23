/**
 \file
 \brief common module
 \author Shenzhen Hisilicon Co., Ltd.
 \date 2008-2018
 \version 1.0
 \author
 \date 2011-10-9
 */

#ifndef __HI_SVR_CHARSET_NORM_H__
#define __HI_SVR_CHARSET_NORM_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


/***************************** Structs Definition ******************************/
/** \addtogroup     Hiplayer */
/** @{ */  /** <!--[Hiplayer] */

/** Codec type of charset */
/**
CNcomment:�ַ�����������,
����ַ�������ѭ����Լ��:
1������ַ�������Ҫ���ַ����������ͺ�����ϱ�������
2����ӵı������ͱ��������
3����ӵı������ͻ���ǰ��ı������ͱ��+1
*/
typedef enum hiCHARSET_CODETYPE_E {
    HI_CHARSET_CODETYPE_NONE = 0,        /**< Default value *//**< CNcomment:Ĭ��ֵ�������� */
    HI_CHARSET_CODETYPE_BIG5 = 1,        /**< big5 *//**< CNcomment:�������� */
    HI_CHARSET_CODETYPE_UTF8 = 2,        /**< uft8 *//**< UTF8 */
    HI_CHARSET_CODETYPE_ISO8859_1 = 3,   /**< iso8850_1 *//**< CNcomment:��ŷ��ϵ ��������1 */
    HI_CHARSET_CODETYPE_ISO8859_2 = 4,   /**< iso8850_2 *//**< CNcomment:��ŷ�Ͷ�ŷ��ϵ��������2 */
    HI_CHARSET_CODETYPE_ISO8859_3 = 5,   /**< iso8850_3 *//**< CNcomment:ŷ�޶��ϲ���ϵ��������3 */
    HI_CHARSET_CODETYPE_ISO8859_4 = 6,   /**< iso8850_4 *//**< CNcomment:˹������ά�ǺͲ��޵ĺ���ϵ��������4 */
    HI_CHARSET_CODETYPE_ISO8859_5 = 7,   /**< iso8850_5 *//**< CNcomment:�����ĺ�˹������ */
    HI_CHARSET_CODETYPE_ISO8859_6 = 8,   /**< iso8850_6 *//**< CNcomment:�����ĺͰ������� */
    HI_CHARSET_CODETYPE_ISO8859_7 = 9,   /**< iso8850_7 *//**< CNcomment:�����ĺ�ϣ���� */
    HI_CHARSET_CODETYPE_ISO8859_8 =10,   /**< iso8850_8 *//**< CNcomment:�����ĺ�ϣ������ */
    HI_CHARSET_CODETYPE_ISO8859_9 =11,   /**< iso8850_9 *//**< CNcomment:Ϊ�������������������ģ�������5 */
    HI_CHARSET_CODETYPE_ISO8859_10=12,   /**< iso8850_10 *//**< CNcomment:�����ˡ���ŷ�Ͱ�˹��Ħ�˵����֣�������6 */
    HI_CHARSET_CODETYPE_ISO8859_11=13,   /**< iso8850_11 *//**< CNcomment:��������̩�� */
    HI_CHARSET_CODETYPE_ISO8859_13=14,   /**< iso8850_13 *//**< CNcomment:���޵ĺ��ܱ���ϵ��������7 */
    HI_CHARSET_CODETYPE_ISO8859_14=15,   /**< iso8850_14 *//**< CNcomment:�������ģ�������8 */
    HI_CHARSET_CODETYPE_ISO8859_15=16,   /**< iso8850_15 *//**< CNcomment:�Ľ���������1��������9 */
    HI_CHARSET_CODETYPE_ISO8859_16=17,   /**< iso8850_16 *//**< CNcomment:���������ģ�������10 */
    HI_CHARSET_CODETYPE_UNICODE_16LE=18, /**< unicode_16 little-endian *//**< CNcomment:16λС�˵�UNICODE */
    HI_CHARSET_CODETYPE_UNICODE_16BE=19, /**< unicode_16 big-endian *//**< CNcomment:16λ��˵�UNICODE */
    HI_CHARSET_CODETYPE_GBK=20,          /**< gbk *//**< CNcomment:��������GBK */
    HI_CHARSET_CODETYPE_CP1250 = 21,     /**< cp1250 *//**< CNcomment:��ŷ */
    HI_CHARSET_CODETYPE_CP1251 = 22,     /**< cp1251 *//**< CNcomment:������� */
    HI_CHARSET_CODETYPE_CP1252 = 23,     /**< cp1252 *//**< CNcomment:������I */
    HI_CHARSET_CODETYPE_CP1253 = 24,     /**< cp1253 *//**< CNcomment:ϣ���� */
    HI_CHARSET_CODETYPE_CP1254 = 25,     /**< cp1254 *//**< CNcomment:�������� */
    HI_CHARSET_CODETYPE_CP1255 = 26,     /**< cp1255 *//**< CNcomment:ϣ������ */
    HI_CHARSET_CODETYPE_CP1256 = 27,     /**< cp1256 *//**< CNcomment:�������� */
    HI_CHARSET_CODETYPE_CP1257 = 28,     /**< cp1257 *//**< CNcomment:���޵ĺ��� */
    HI_CHARSET_CODETYPE_CP1258 = 29,     /**< cp1258 *//**< CNcomment:Խ�� */
    HI_CHARSET_CODETYPE_CP874  = 30,     /**< cp874 *//**< CNcomment:̩�� */
    HI_CHARSET_CODETYPE_UNICODE_32LE=31, /**< unicode_32 little-endian *//**< CNcomment:32λС�˵�UNICODE */
    HI_CHARSET_CODETYPE_UNICODE_32BE=32, /**< unicode_32 big-endian *//**< CNcomment:32λ��˵�UNICODE */
    HI_CHARSET_CODETYPE_CP949  = 33,     /**< cp874 *//**< CNcomment:���ģ�CP949�ַ��� */
    HI_CHARSET_CODETYPE_BULT,
} HI_CHARSET_CODETYPE_E;

/** Verion of charset library */
/** CNcomment:�汾�Ŷ��� */
typedef struct hiCHARSET_LIB_VERSION_S
{
    HI_U8 u8VersionMajor;    /**< Major version accessor element */
    HI_U8 u8VersionMinor;    /**< Minor version accessor element */
    HI_U8 u8Revision;        /**< Revision version accessor element */
    HI_U8 u8Step;            /**< Step version accessor element */
} HI_CHARSET_LIB_VERSION_S;

/** Stream detector API  */
/** CNcomment:����̽����API */
typedef struct hiCHARSET_DETECTOR_FUN_S
{
    /**
    \brief Create detector according to user data. CNcomment:����˽�����ݣ�����̽����CNend
    \attention \n
    none
    \param [out] pDetectHandle  Handle of detector
    \param [in] u32UserData  User data

    \retval ::HI_SUCCESS Create successful.
    \retval ::HI_FAILURE Create fail.

    \see \n
    none
    */
    HI_S32 (*detect_create)(HI_HANDLE *pDetectHandle, HI_U32 u32UserData);

    /**
    \brief According to the data stream, detecting a coding type.CNcomment:������������̽���������CNend
    \attention \n
    none
    \param [in]  handle Handle of detector
    \param [in]  u8Input Input data
    \param [in]  u32Inlen Length of input data
    \param [in]  s32UserData User data
    \param [out] s32CodeType Code type of input data

    \retval ::HI_SUCCESS  Detect successful.
    \retval ::HI_FAILURE  Detect fail, value of s32CodeType is invalid.

    \see \n
    none
    */
    HI_S32 (*identstream)(HI_HANDLE handle, const HI_U8 *u8Input, const HI_S32 s32Inlen, HI_S32 s32UserData, HI_S32 *s32CodeType);

    /**
    \brief Set user command and user data.CNcomment:�첽�����û����������CNend
    \attention \n
    none
    \param [in] handle Handle of detector
    \param [in] u32Cmd  User command
    \param [in] pArg  User data

    \retval ::HI_SUCCESS Set successful.
    \retval ::HI_FAILURE Parameter invalid.

    \see \n
    none
    */
    HI_S32 (*detect_setDataAsync)(HI_HANDLE handle,HI_U32 u32Cmd, HI_VOID *pArg);

    /**
    \brief Destroy detector. CNcomment:�ر�̽����CNend
    \attention \n
    none
    \param [in] pDetectHandle Handle of detector

    \retval ::HI_SUCCESS Destroy successful.
    \retval ::HI_FAILURE Handle invalid.

    \see \n
    none
    */
    HI_S32 (*detect_close)(HI_HANDLE pDetectHandle);
} HI_CHARSET_DETECTOR_FUN_S;

/** Stream transcoder API  */
/** CNcomment:ת����API */
typedef struct hiCHARSET_CONVERTER_FUN_S
{
    /**
    \brief Find a transcoder, according to the source data encoding type and target data encoding type.
    CNcomment:����ת��Դ�������ͺ�Ŀ�ı������ͣ�����ʶ����CNend
    \attention \n
    none
    \param [in] s32SrcType  Source data encoding type
    \param [in] s32DstType  Target data encoding type

    \retval ::HI_SUCCESS Find successfule.
    \retval ::HI_FAILURE Not found.

    \see \n
    none
    */
    HI_S32 (*conv_find)(HI_S32 s32SrcType, HI_S32 s32DstType);

    /**
    \brief Create a transcoder.CNcomment:����ת����CNend
    \attention \n
    none
    \param [out] pConvHandle  Handle of the transcoder
    \param [in] u32UserData  User data

    \retval ::HI_SUCCESS Create successful.
    \retval ::HI_FAILURE Create fail.

    \see \n
    none
    */
    HI_S32 (*conv_create)(HI_HANDLE *pConvHandle, HI_U32 u32UserData);

    /**
    \brief Transcode data stream.CNcomment:��������������ת��CNend
    \attention \n
    none
    \param [in]  pConvHandle Handle of transcoder
    \param [in]  u8Input Input data
    \param [in]  u32Inlen Length of input data
    \param [in] s32SrcType  Source data encoding type
    \param [in] s32DstType  Target data encoding type
    \param [out] u8Output Transcoded data
    \param [out] u32Outlen Length of transcoded data
    \param [out] s32Usedlen Used length

    \retval ::HI_SUCCESS  Transcode successful.
    \retval ::HI_FAILURE  Transcode fail.

    \see \n
    none
    */
    HI_S32 (*convstream)(HI_HANDLE pConvHandle, const HI_U8 *u8Input, const HI_S32 s32Inlen, HI_U8 *u8Output, HI_S32 *s32Outlen, HI_S32 s32SrcType, HI_S32 s32DstType, HI_S32 *s32Usedlen);

    /**
    \brief Set user command and user data.CNcomment:�첽�����û����������CNend
    \attention \n
    none
    \param [in] handle Handle of transcoder
    \param [in] u32Cmd  User command
    \param [in] pArg  User data

    \retval ::HI_SUCCESS Set successful.
    \retval ::HI_FAILURE Parameter invalid.

    \see \n
    none
    */
    HI_S32 (*conv_setDataAsync)(HI_HANDLE handle,HI_U32 u32Cmd, HI_VOID *pArg);

    /**
    \brief Destroy transcoder.CNcomment:�ر�ת����CNend
    \attention \n
    none
    \param [in] pConvHandle Handle of transcoder

    \retval ::HI_SUCCESS Destroy successfule.
    \retval ::HI_FAILURE Invalid handle.

    \see \n
    none
    */
    HI_S32 (*conv_close)(HI_HANDLE pConvHandle);
} HI_CHARSET_CONVERTER_FUN_S;

/*******************************************************
HI_CHARSET_S g_stCharset_entry =
{
    .pszDllName  = (const HI_PCHAR )"libhicharset.so",
    .stLibVersion = {1, 0, 0, 0},
    .pszCharSetDesc = (const HI_PCHAR )"hicharset detector and converter",
    .u32Priority   = 0,

    .stDetectorFun.detect_create = ,
    .stDetectorFun.identstream   = ,
    .stDetectorFun.detect_setDataAsync = ,
    .stDetectorFun.detect_close  = ,

    .stConverterFun.conv_find   = ,
    .stConverterFun.conv_create = ,
    .stConverterFun.convstream  = ,
    .stConverterFun.conv_setDataAsync = ,
    .stConverterFun.conv_close = ,

    .next           = NULL,
    .pDllModule     = NULL,
    .u32PrivateData = 0,
};
********************************************************/

typedef struct hiCHARSET_S
{
    /**< Charset dll library name *//**< CNcomment:�ַ������� */
    const HI_CHAR *pszDllName;

    /**< Version of charset library *//**< CNcomment:�ַ����汾�� */
    const HI_CHARSET_LIB_VERSION_S stLibVersion;

    /**< Description of the charset library. *//**< CNcomment:�ַ������� */
    const HI_CHAR *pszCharSetDesc;

    /**< Priority of the charset library *//**< CNcomment:���ȼ�����0x0 - 0xFFFFFFFF��0xFFFFFFFF���ȼ���ߣ���������Ч */
    HI_U32 u32Priority;

    /**< Interface of the detector */
    /**< CNcomment:�ַ���̽�������ʽӿڣ�̽����ʵ�ֱ�����ѭ���¹���:
         1������̽������̽�⾫��Ҳ�ߣ����ȼ�Խ�ߣ������ȷ�ϱ������ͣ���ʧ�ܷ���
         2������ַ���û��ʵ��̽�������ܣ����뽫�ò�������ΪNULL;
         */

    HI_CHARSET_DETECTOR_FUN_S stDetectorFun;

    /**< Interface of the detector */
    /**< CNcomment:ת�����ӿڣ��ýӿ���ѭ���¹���:
         1������ַ���û��ʵ�ָýӿڣ����뽫�ò�������ΪNULL;
         2���ò����������ⲿָ������: �ַ������Բ�ʵ�ָýӿڣ�
         */

    HI_CHARSET_CONVERTER_FUN_S stConverterFun;

    struct hiCHARSET_S *next;

    /**< Handle of the dll library */
    /**< CNcomment:��̬���� */
    HI_VOID  *pDllModule;

    /**< User data */
    /**< CNcomment:˽������ */
    HI_U32 u32PrivateData;
} HI_CHARSET_S;

/** Get the minimum of the character encoding value. */
/** CNcomment:��ȡ�ַ�������Сֵ */
extern HI_S32 HI_CHARSET_GetMinCodetype();

/** Get max of the character encoding value . */
/** CNcomment:��ȡ�ַ��������ֵ */
extern HI_S32 HI_CHARSET_GetMaxCodetype();

/** @} */  /** <!-- ==== Structure Definition end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_SVR_CHARSET_NORM_H__ */
