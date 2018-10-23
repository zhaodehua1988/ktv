/******************************************************************************

  Copyright (C), 2001-2014, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_unf_pes.h
  Version       : Initial Draft
  Author        : l00168554
  Created       : 2011/07/05
  Description   :

*******************************************************************************/
/**
 * \file
 * \brief  Describes the information about the Packetized elementary stream parse information. CNcomment: �ṩPES stream ����������Ϣ CNend
 */
#ifndef __HI_UNF_PES_H__
#define __HI_UNF_PES_H__

#include "hi_unf_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*************************** Structure Definition ****************************/
/** \addtogroup      PES */
/** @{ */  /** <!-- ��PES�� */

#ifdef HI_ADVCA_FUNCTION_RELEASE
#define HI_FATAL_PES
#define HI_ERR_PES
#define HI_WARN_PES
#define HI_INFO_PES
#else
/**control print, if undefined HI_ADVCA_FUNCTION_RELEASE, pes component  will print some information*/
/**CNcomment:��ӡ���ƿ��أ����û����HI_ADVCA_FUNCTION_RELEASE��������ӡ*/
#define HI_FATAL_PES printf
/**control print, if undefined HI_ADVCA_FUNCTION_RELEASE, pes component  will print some information*/
/**CNcomment:��ӡ���ƿ��أ����û����HI_ADVCA_FUNCTION_RELEASE��������ӡ*/
#define HI_ERR_PES   printf
/**control print, if undefined HI_ADVCA_FUNCTION_RELEASE, pes component  will print some information*/
/**CNcomment:��ӡ���ƿ��أ����û����HI_ADVCA_FUNCTION_RELEASE��������ӡ*/
#define HI_WARN_PES  printf
/**control print, if undefined HI_ADVCA_FUNCTION_RELEASE, pes component  will print some information*/
/**CNcomment:��ӡ���ƿ��أ����û����HI_ADVCA_FUNCTION_RELEASE��������ӡ*/
#define HI_INFO_PES  printf
#endif

#if 0
#define HI_FATAL_PES(fmt...) \
            HI_TRACE(HI_LOG_LEVEL_FATAL, HI_DEBUG_ID_AVPLAY, fmt)

#define HI_ERR_PES(fmt...) \
            HI_TRACE(HI_LOG_LEVEL_ERROR, HI_DEBUG_ID_AVPLAY, fmt)

#define HI_WARN_PES(fmt...) \
            HI_TRACE(HI_LOG_LEVEL_WARNING, HI_DEBUG_ID_AVPLAY, fmt)

#define HI_INFO_PES(fmt...) \
            HI_TRACE(HI_LOG_LEVEL_INFO, HI_DEBUG_ID_AVPLAY, fmt)
#endif

/**the largest node number for dvb format to store information*/
/**CNcomment:DVD��ʽ�洢��Ϣ�����ڵ���Ŀ*/
#define PES_MAX_NODE 10

/**Defines the PES stream information attributes.*/
/**CNcomment:����PES������Ϣ�Ľṹ�� */
typedef struct hiUNF_PES_INFO_S
{
    HI_U32 streamNum;      /**<the index of pes stream information*//**<CNcomment: PES ������Ϣ���� */
    struct
    {
        HI_U32 StreamType;      /**<specifying the type of the elementary stream See 13818-1*//**<CNcomment: PES ����Ԫ�����ͣ�����ο�13818-1*/
        HI_U32 StrmID;          /**<indicating the value of the stream_id field in the PES packet headers of PES packets in which this elementary stream is stored.*//**<CNcomment: PES ��ͷ���е�stream_id*/
    }node[PES_MAX_NODE];

}HI_UNF_PES_INFO_S;

/**Defines PES Stream Type*/
/**CNcomment:����PES��������ö������ */
enum
{
    HI_UNF_PES_STREAM_TYPE_NULL,                /**<NULL*/
    HI_UNF_PES_STREAM_TYPE_VIDEO_MPEG2,         /**<MPEG2*/
    HI_UNF_PES_STREAM_TYPE_VIDEO_MPEG4,         /**<MPEG4 DIVX4 DIVX5*/
    HI_UNF_PES_STREAM_TYPE_VIDEO_H264,          /**<H264*/
    HI_UNF_PES_STREAM_TYPE_VIDEO_H263,          /**<H263*/
    HI_UNF_PES_STREAM_TYPE_VIDEO_UNKNOW,        /**<VIDEO UNKNOW*/

    HI_UNF_PES_STREAM_TYPE_AUDIO_AAC,           /**<AAC*/
    HI_UNF_PES_STREAM_TYPE_AUDIO_MP3,           /**<MP2 MP3*/
    HI_UNF_PES_STREAM_TYPE_AUDIO_AC3,           /**<AC3*/
    HI_UNF_PES_STREAM_TYPE_AUDIO_EAC3,          /**<EAC3*/
    HI_UNF_PES_STREAM_TYPE_AUDIO_DTS,           /**<DTS*/
    HI_UNF_PES_STREAM_TYPE_AUDIO_PCM,           /**<PCM*/
    HI_UNF_PES_STREAM_TYPE_AUDIO_DDPLUS,        /**<DDPLUS*/
    HI_UNF_PES_STREAM_TYPE_AUDIO_UNKNOW,        /**<AUDIO UNKNOW*/

    HI_UNF_PES_STREAM_TYPE_BUTT
};

/**Defines PES Stream Parse Method */
/**CNcomment:����PES������������ö������ */
typedef enum
{
    HI_UNF_PES_PARSE_TYPE_REGULAR = 0,          /**<believe peslen, protocol parse *//**<CNcomment: ��Ϊpes������׼ȷ������Э�����̽��� */
    HI_UNF_PES_PARSE_TYPE_ADJUST,               /**<when peslen is not correct, search pes header to parse , only for video element*//**<CNcomment: ������pes�����򣬲�������pesͷȥ����������������Ƶ��*/

    HI_UNF_PES_PARSE_TYPE_BUTT
}HI_UNF_PES_PARSE_TYPE_E;

/**Defines PES  Open parameter, this is used as parma[in] for api HI_UNF_PES_Open.*/
/**CNcomment: ����PES�򿪲�������ΪHI_UNF_PES_Open�ӿڵ�������� */
typedef struct hiUNF_PES_OPEN_S
{
    HI_HANDLE                      hAVPlay;             /**<AVPlay Handle *//**<CNcomment: AVPlay��� */
    HI_U32                         u32VdecType;         /**<PES Video type, if do not open video decoder, please select HI_UNF_PES_STREAM_TYPE_VIDEO_UNKNOW */
                                                        /**<CNcomment: PES��Ƶ���ͣ������Ƶ������û�д򿪣���ѡ��HI_UNF_PES_STREAM_TYPE_VIDEO_UNKNOW */
    HI_U32                         u32AudioType;        /**<PES Audio type, if do not open video decoder, please select HI_UNF_PES_STREAM_TYPE_AUDIO_UNKNOW */
                                                        /**<CNcomment: PES��Ƶ���ͣ������Ƶ������û�д򿪣���ѡ��HI_UNF_PES_STREAM_TYPE_AUDIO_UNKNOW */
    HI_U32                         u32PESBufferSize;    /**<PES packet Max size is 64KB, so it should be >= 100KB.IF PES length may equals 0,you need buffer bigger than 1MB.*/
                                                        /**<CNcomment: PES�����Ϊ64KB�����Ը�ֵ����Ϊ100KB �����pes������Ϊ0����ֵ����Ϊ1MB*/
    HI_UNF_PES_PARSE_TYPE_E        enPESParseType;      /**<*PES STREAM Parse Type *//**<CNcomment:PES������������*/
    HI_BOOL                        bIsDVD;              /**<*whether PES STREAM is DVD format, HI_TRUE means DVD format*//**<CNcomment:PES�����Ƿ�ΪDVD��ʽ��HI_TRUE��DVD��ʽ*/
}HI_UNF_PES_OPEN_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      PES */
/** @{ */  /** <!-- ��PES�� */



/**
\brief Open  PES module  CNcomment:��PESģ�� CNend
\attention \n
N/A
\param[in] stOpenParam Open parameter.CNcomment:PES  ģ���������� CNend
\param[out] handle    Pointer to the handle of a pes instance .CNcomment:ָ�����ͣ���PESģ�������ɵ�PESʵ����� CNend
\retval ::HI_SUCCESS Success CNcomment: �ɹ� CNend
\retval ::HI_FAILURE  This API fails to be called due to system errors. CNcomment:����ϵͳ����API����ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PES_Open(HI_UNF_PES_OPEN_S *pstOpenParam, HI_HANDLE *handle);

/**
\brief  Close  PES module  CNcomment:�ر�PESģ�� CNend
\attention \n
N/A
\param[in] handle  the handle of a pes instance CNcomment:PESʵ����� CNend
\retval ::HI_SUCCESS Success  CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  This API fails to be called due to system errors. CNcomment:����ϵͳ����API����ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PES_Close(HI_HANDLE handle);

/**
\brief PES  Obtain PES buffer.CNcomment:��ȡPES buffer CNend
\attention \n
N/A
\param[in] handle  the handle of a pes instance CNcomment:PESʵ����� CNend
\param[in] u32ReqSize: Required buffer size,it must smaller than whole PS Buffer.Recommend it's a multiple of  whole PS Buffer's 1/N CNcomment:��Ҫ�Ļ����С������С������PS Buffer�Ĵ�С��������������С��1/N CNend
\param[out] pstData    Pointer to the returned buffer CNcomment:���ػ���ָ�� CNend
\retval ::HI_SUCCESS Success  CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  This API fails to be called due to system errors. CNcomment:����ϵͳ����API����ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PES_GetBuffer(HI_HANDLE handle, HI_U32 u32ReqLen, HI_UNF_STREAM_BUF_S *pstData);

/**
\brief  After filling data,send it to PES process module.CNcomment: PES����������������ݸ�PESģ�鴦�� CNend
\attention \n
It must be called after HI_UNF_PES_GetBuffer  CNcomment:  ������HI_UNF_PES_GetPSBuffer���ʹ�á� CNend
\param[in] handle  the handle of a pes instance CNcomment:PESʵ����� CNend
\param[in] u32ValidDatalen : Valid data length  CNcomment:��������ݳ��� CNend
\retval ::HI_SUCCESS Success  CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  This API fails to be called due to system errors. CNcomment:����ϵͳ����API����ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PES_PutBuffer(HI_HANDLE handle, HI_U32 u32ValidDatalen);

/**
\brief Obtain PES stream information CNcomment: ��ȡPES����Ϣ. CNend
\attention \n
N/A
\param[in] handle  the handle of a pes instance CNcomment:PESʵ����� CNend
\param[out] pstruPSInfo  Pointer to the returned PES stream information CNcomment: ����PES������Ϣָ�� CNend
\retval ::HI_SUCCESS Success  CNcomment:�ɹ� CNend
\retval ::HI_FAILURE  This API fails to be called due to system errors. CNcomment:����ϵͳ����API����ʧ�� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PES_GetInfo(HI_HANDLE handle, HI_UNF_PES_INFO_S *pstruPSInfo);

/**
\brief Set Video PES stream id CNcomment: ������ƵPES��ID. CNend
\attention \n
N/A
\param[in] handle  the handle of a pes instance CNcomment:PESʵ����� CNend
\param[in] videostreamid  Video PES stream id CNcomment: ��ƵPES��ID CNend
\retval ::HI_SUCCESS Success  CNcomment:�ɹ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PES_SetVideoStreamID(HI_HANDLE handle, HI_U32 videostreamid);

/**
\brief Set Audio PES stream id CNcomment: ������ƵPES��ID. CNend
\attention \n
N/A
\param[in] handle  the handle of a pes instance CNcomment:PESʵ����� CNend
\param[in] audiostreamid  Audio PES stream id CNcomment: ��ƵPES��ID CNend
\retval ::HI_SUCCESS Success  CNcomment:�ɹ� CNend
\see \n
N/A
*/
HI_S32 HI_UNF_PES_SetAudioStreamID(HI_HANDLE handle, HI_U32 audiostreamid);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/** @} */  /** <!-- ==== API declaration end ==== */

#endif /*__HI_UNF_PES_H__*/
