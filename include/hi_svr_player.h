/**
 \file
 \brief Server (SVR) player module. CNcomment:svr playerģ��CNend
 \author HiSilicon Technologies Co., Ltd.
 \date 2008-2018
 \version 1.0
 \author
 \date 2010-02-10
 */

#ifndef __HI_SVR_PLAYER_H__
#define __HI_SVR_PLAYER_H__

#include "hi_type.h"
#include "hi_svr_format.h"
#include "hi_svr_metadata.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*************************** Structure Definition ****************************/
/** \addtogroup      Hiplayer */
/** @{ */  /** <!-- [Hiplayer]*/

/** Invalid handle */
/** CNcomment:�Ƿ���� */
#define HI_SVR_PLAYER_INVALID_HDL        (0)

/** Normal playing speed */
/** CNcomment:���������ٶ�*/
#define HI_SVR_PLAYER_PLAY_SPEED_NORMAL  (1024)

/** File type */
/** CNcomment:���õ��ļ����� */
typedef enum hiSVR_PLAYER_MEDIA_PARAM_E
{
    HI_SVR_PLAYER_MEDIA_STREAMFILE = (1 << 0),    /**< Audio and video (AV) media file *//**< CNcomment:����Ƶý���ļ� */
    HI_SVR_PLAYER_MEDIA_SUBTITLE   = (1 << 1),    /**< Subtitle file *//**< CNcomment:��Ļ�ļ� */
    HI_SVR_PLAYER_MEDIA_BUTT       = (1 << 2)
} HI_SVR_PLAYER_MEDIA_PARAM_E;

/** The play mode of the player */
/** CNcomment:���ò������������� */
typedef enum hiSVR_PLAYER_PLAY_MODE_E
{
    HI_SVR_PLAYER_PLAYMODE_NORMAL = 0x0, /**< Normal play mode *//**< CNcomment: ��������ģʽ */
    HI_SVR_PLAYER_PLAYMODE_PRELOAD = 0x1,/**< Preload play mode. This mode will be used only in network playing *//**< CNcomment: Ԥ���ز���ģʽ */
    HI_SVR_PLAYER_PLAYMODE_BUTT
} HI_SVR_PLAYER_PLAY_MODE_E;

/** Player attribute ID */
/** CNcomment:����������ID */
typedef enum hiSVR_PLAYER_ATTR_E
{
    HI_SVR_PLAYER_ATTR_STREAMID = 0x0,         /**< Set or obtain the stream ID. This attribute is applicable to multi-audio multi-video streams. The parameter is ::HI_SVR_PLAYER_STREAMID_S. *//**< CNcomment:����/��ȡ����ţ���������Զ���Ƶ������Ƶ�������������Ϊ::HI_SVR_PLAYER_STREAMID_S */
    HI_SVR_PLAYER_ATTR_WINDOW_HDL,             /**< Obtain the window ID. The parameter is the HI_U32 variable. *//**< CNcomment:��ȡwindow id������ΪHI_U32���� */
    HI_SVR_PLAYER_ATTR_AVPLAYER_HDL,           /**< Obtain the audio/video play (AVPlay) handle. The parameter is the HI_HANDLE variable. *//**< CNcomment:��ȡAVPlayer���������ΪHI_HANDLE���� */
    HI_SVR_PLAYER_ATTR_SUBTITLE_HDL,           /**< Obtain the subtitle handle. The parameter is the HI_HANDLE variable. *//**< CNcomment:��ȡSubtitle���������ΪHI_HANDLE���� */
    HI_SVR_PLAYER_ATTR_SO_HDL,                      /**< Obtain the subtitle output (SO) handle. The parameter is the HI_HANDLE variable. *//**< CNcomment:��ȡso���������ΪHI_HANDLE���� */
    HI_SVR_PLAYER_ATTR_AUDTRACK_HDL,          /**< Obtain the audio track handle. The parameter is the HI_HANDLE variable. *//**< CNcomment:��ȡ��Ƶtrack���������ΪHI_HANDLE���� */
    HI_SVR_PLAYER_ATTR_SYNC,                          /**< Set the sync attribute. The parameter is ::HI_SVR_PLAYER_SYNC_ATTR_S. *//**< CNcomment:��������Ƶ����Ļʱ���ƫ�ƣ�����Ϊ::HI_SVR_PLAYER_SYNC_ATTR_S */
    HI_SVR_PLAYER_ATTR_VSINK_HDL,              /**< set the vsink handle. The parameter is HI_SVR_VSINK_S* */
    HI_SVR_PLAYER_ATTR_BUTT
} HI_SVR_PLAYER_ATTR_E;

/************************************Player Event Start***********************************/

/** Error information of the player */
/** CNcomment:������������Ϣ */
typedef enum hiSVR_PLAYER_ERROR_E
{
    HI_SVR_PLAYER_ERROR_NON = 0x0,
    HI_SVR_PLAYER_ERROR_VID_PLAY_FAIL,         /**< The video fails to be played. *//**< CNcomment: ��Ƶ������ʧ�� */
    HI_SVR_PLAYER_ERROR_AUD_PLAY_FAIL,         /**< The audio fails to be played. *//**< CNcomment: ��Ƶ������ʧ�� */
    HI_SVR_PLAYER_ERROR_SUB_PLAY_FAIL,         /**< The subtitle fails to be played. *//**< CNcomment: ��Ļ������ʧ�� */
    HI_SVR_PLAYER_ERROR_PLAY_FAIL,             /**< The file fails to be played. *//**< CNcomment: ����Ƶ����ʧ�� */
    HI_SVR_PLAYER_ERROR_TIMEOUT,               /**< Operation timeout. For example, reading data timeout. *//**< CNcomment: ������ʱ�� ���ȡ���ݳ�ʱ */
    HI_SVR_PLAYER_ERROR_NOT_SUPPORT,           /**< The file format is not supportted. *//**< CNcomment: �ļ���ʽ��֧�� */
    HI_SVR_PLAYER_ERROR_UNKNOW,                /**< Unknown error. *//**< CNcomment: δ֪���� */
    HI_SVR_PALYER_ERROR_BUTT,
} HI_SVR_PLAYER_ERROR_E;

/** Buffer status, the conditions of the event report can be configured by setting ::HI_FORMAT_BUFFER_CONFIG_S, when use default buffer configuration:
 1. When the total size of data in hiplayer's buffer and decoder's buffer is smaller than 512KBytes or the total duration is smaller than 200 ms, the::HI_SVR_PLAYER_BUFFER_START event is reported.
 2. When the total size of data in hiplayer's buffer and decoder's buffer is 0 byte, the ::HI_SVR_PLAYER_BUFFER_EMPTY event is reported.
 3. When the total size of data in hiplayer's buffer and decoder's buffer is greater than or equal to 4 MB or the total duration is greater than or equal to 50 seconds, the player notify the ::HI_SVR_PLAYER_BUFFER_ENOUGH event.
 4. When the total size of data in hiplayer's buffer and decoder's buffer is greater than or equal to 5 MB or the total duration is greater than or equal to 60 seconds, the ::HI_SVR_PLAYER_BUFFER_FULL event is reported.  */
/** CNcomment:����״̬������Ϣ,���¼��ϱ�������,��Ҫͨ��::HI_FORMAT_BUFFER_CONFIG_S,�����á�
  Ĭ��: ��hiplayer��������decoder�������ڵ�����Ƶ���ݺ�С�� 512KBytes�����ܲ��ŵ�ʱ��С��200ms, �ϱ�::HI_SVR_PLAYER_BUFFER_START
  ��hiplayer��������decoder�������ڵ�����Ƶ���ݺ�Ϊ0 bytesʱ���ϱ�::HI_SVR_PLAYER_BUFFER_EMPTY
  ��hiplayer��������decoder������������Ƶ���ݺʹ��ڵ���4M bytes ���� �ܲ��ŵ�ʱ�����ڵ���50��, �ϱ�::HI_SVR_PLAYER_BUFFER_ENOUGH
  ��hiplayer��������decoder������������Ƶ���ݺʹ��ڵ���5M bytes ���� �ܲ��ŵ�ʱ�����ڵ���60��, �ϱ�::HI_SVR_PLAYER_BUFFER_FULL
 */
typedef enum hiSVR_PLAYER_BUFFER_E
{
    HI_SVR_PLAYER_BUFFER_EMPTY=0,   /**< The buffer is empty. *//**< CNcomment:����Ϊ�� */
    HI_SVR_PLAYER_BUFFER_START,     /**< The data in the buffer is insufficient.*//**< CNcomment:���忪ʼ�¼� */
    HI_SVR_PLAYER_BUFFER_ENOUGH,    /**< The data in the buffer is sufficient *//**< CNcomment:�����㹻,���Բ��� */
    HI_SVR_PLAYER_BUFFER_FULL,      /**< The buffer is full. *//**< CNcomment:����Ϊ�� */
    HI_SVR_PLAYER_BUFFER_BUTT
} HI_SVR_PLAYER_BUFFER_E;

/** Player status */
/** CNcomment:PLAYER״̬ */
typedef enum hiSVR_PLAYER_STATE_E
{
    HI_SVR_PLAYER_STATE_INIT = 0,    /**< The player is in the initial state. It changes to the initial state after being created. *//**< CNcomment:��������ǰ���ڳ�ʼ״̬��create�󲥷�������ini״̬ */
    HI_SVR_PLAYER_STATE_DEINIT,      /**< The player is deinitialized. *//**< CNcomment:�������Ѿ�ȥ��ʼ״̬ */
    HI_SVR_PLAYER_STATE_PLAY,       /**< The player is in the playing state. *//**< CNcomment:��������ǰ���ڲ���״̬ */
    HI_SVR_PLAYER_STATE_FORWARD,    /**< The player is in the fast forward state. *//**< CNcomment:��������ǰ���ڿ��״̬ */
    HI_SVR_PLAYER_STATE_BACKWARD,   /**< The player is in the rewind state. *//**< CNcomment:��������ǰ���ڿ���״̬ */
    HI_SVR_PLAYER_STATE_PAUSE,      /**< The player is in the pause state. *//**< CNcomment:��������ǰ������ͣ״̬ */
    HI_SVR_PLAYER_STATE_STOP,       /**< The player is in the stop state. *//**< CNcomment:��������ǰ����ֹͣ״̬ */
    HI_SVR_PLAYER_STATE_BUTT
} HI_SVR_PLAYER_STATE_E;

/** Event type */
/** CNcomment:�¼����� */
typedef enum hiSVR_PLAYER_EVENT_E
{
    HI_SVR_PLAYER_EVENT_STATE_CHANGED = 0x0,   /**< Player status change event. The parameter type is ::HI_SVR_PLAYER_STATE_E. *//**< CNcomment:������״̬ת���¼�����������Ϊ::HI_SVR_PLAYER_STATE_E */
    HI_SVR_PLAYER_EVENT_SOF,           /**< Event indicating that file playing starts or a file is rewound to the file header. The parameter type is HI_U32. The value ::HI_SVR_PLAYER_STATE_PLAY indicates that file playing starts and the value ::HI_SVR_PLAYER_STATE_BACKWARD indicates that a file is rewound to the file header. */
                                       /**< CNcomment:�ļ���ʼ���Ż���˵��ļ�ͷ�¼�����������ΪHI_U32��ֵΪ::HI_SVR_PLAYER_STATE_PLAY��ʾ��ʼ���ţ�����ֵΪ::HI_SVR_PLAYER_STATE_BACKWARD��ʾ���˵��ļ�ͷ */
    HI_SVR_PLAYER_EVENT_EOF,           /**< Event indicating that a file is played till the end of the file. There is no parameter. *//**< CNcomment:�ļ����ŵ�β�¼����޲��� */
    HI_SVR_PLAYER_EVENT_PROGRESS,      /**< Event indicating the current progress of the player. This event is reported once every 300 ms. The parameter is ::HI_SVR_PLAYER_PROGRESS_S. *//**< CNcomment:��������ǰ���Ž����¼���ÿ��300ms�ϱ�һ�θ��¼�������ֵΪ::HI_SVR_PLAYER_PROGRESS_S */
    HI_SVR_PLAYER_EVENT_STREAMID_CHANGED,  /**< Stream ID change event. The parameter is ::HI_SVR_PLAYER_STREAMID_S. *//**< CNcomment:stream id �����仯�¼�������Ϊ::HI_SVR_PLAYER_STREAMID_S */
    HI_SVR_PLAYER_EVENT_SEEK_FINISHED,      /**< Seek operation is complete. The parameter is HI_U32. When the value of the parameter is ::HI_FAILURE, the seek operation fails. When the value of the parameter is ::HI_SUCCESS, the seek operation succeeds. When the value of the parameter is ::HI_FORMAT_ERRO_ENDOFFILE, the file is read to the end. */
                                       /**< CNcomment:Seek������ɣ�����ΪHI_U32��ֵΪ::HI_FAILURE Seekʧ�ܣ�::HI_SUCCESS seek�ɹ���::HI_FORMAT_ERRO_ENDOFFILE �ļ���ȡ���ļ�β */
    HI_SVR_PLAYER_EVENT_CODETYPE_CHANGED,  /**< Event of indicating the byte encoding configuration is complete. The parameter is ::HI_SVR_PLAYER_SUB_CODETYPE_S. *//**< CNcomment:�����ַ���������¼����¼�����::HI_SVR_PLAYER_SUB_CODETYPE_S */
    HI_SVR_PLAYER_EVENT_DOWNLOAD_PROGRESS,   /**< Current download progress of the player. The event is reported every 300 ms. The parameter is ::HI_SVR_PLAYER_PROGRESS_S. *//**< CNcomment:��������ǰ���ؽ��ȣ�ÿ��300ms�ϱ�һ�θ��¼�������ֵΪ::HI_SVR_PLAYER_PROGRESS_S */
    HI_SVR_PLAYER_EVENT_BUFFER_STATE,        /**< Reporting buffer status. The parameter type is ::HI_SVR_PLAYER_BUFFER_S. *//**< CNcomment:����״̬�ϱ�,��������Ϊ::HI_SVR_PLAYER_BUFFER_S */
    HI_SVR_PLAYER_EVENT_FIRST_FRAME_TIME,    /**< The display time of the first frame from setting the setMedia. The parameter is ::HI_U32, in the unit of ms. Not supported now*//**< CNcomment:������ý��setMedia��ʼ��һ֡��ʾʱ��,����Ϊ::HI_U32,��λΪms .Ŀǰ��֧��*/

    HI_SVR_PLAYER_EVENT_ERROR,               /**< Event of indicating that an error occurs in the player. The parameter is ::HI_SVR_PLAYER_ERROR_E.*//**< CNcomment:������������Ϣ�¼�������Ϊ::HI_SVR_PLAYER_ERROR_E */
    HI_SVR_PLAYER_EVENT_NETWORK_INFO,       /**< Report the network status. The parameter is ::HI_FORMAT_NET_STATUS_S. *//**< CNcomment:����״̬�ϱ�, ����Ϊ::HI_FORMAT_NET_STATUS_S */
    HI_SVR_PLAYER_EVENT_DOWNLOAD_FINISH,    /**< File download finish, no parameter. *//**< CNcomment:�ļ��������, �޲��� */
    HI_SVR_PLAYER_EVENT_UPDATE_FILE_INFO, /**< The file information has been updated. *//**< CNcomment:�ļ���Ϣ�Ѹ��� */
    HI_SVR_PLAYER_EVENT_USER_PRIVATE = 100, /**< Private event of user define, HI_SVR_PLAYER_EVENT_S:pu8Data is addr of user parameter *//**< CNcomment:�û������˽���¼���HI_SVR_PLAYER_EVENT_S:pu8Data����Ϊ�¼�������ַ */
    HI_SVR_PLAYER_EVENT_BUTT
} HI_SVR_PLAYER_EVENT_E;

/** Information of playing progress */
/** CNcomment:�����¼���Ϣ���� */
typedef struct hiSVR_PLAYER_PROGRESS_S
{
    HI_U32 u32Progress;    /**< Progress of current playing. The value ranges from 0 to 100. *//**< CNcomment:����ֵ, ֵ0-100 */
    HI_S64 s64Duration;    /**< The duration (in the unit of ms) of  current playing or downloading ,when used for downloading event(e.g. HI_SVR_PLAYER_EVENT_DOWNLOAD_PROGRESS),the duration of data in the decoder's buffer is included*//**< CNcomment:��ǰ���Ž��Ȼ�����ʱ��,��λΪms ,�����������¼�ʱ(��HI_SVR_PLAYER_EVENT_DOWNLOAD_PROGRESS��)������decoder �����е�����ʱ��*/
    HI_S64 s64BufferSize;     /**< The total size(in the unit of byte) of data in the hiplayer's buffer and decoder's buffer*//**< CNcomment:hiplayer�ڲ��������ݺ�decoder���������ܴ�С,��λΪbytes */
} HI_SVR_PLAYER_PROGRESS_S;

/** Information of the buffer event */
/** CNcomment:HI_SVR_PLAYER_EVENT_BUFFER_STATE �����¼����� */
typedef struct hiSVR_PLAYER_BUFFER_S
{
    HI_SVR_PLAYER_BUFFER_E eType;        /**< Buffer type *//**< CNcomment:��������::HI_SVR_PLAYER_BUFFER_E */
    HI_FORMAT_BUFFER_STATUS_S stBufStat; /**< Buffer status *//**< CNcomment:����״̬::HI_FORMAT_BUFFER_STATUS_S */
} HI_SVR_PLAYER_BUFFER_S;

/** Player event callback parameters */
/** CNcomment:�������¼��ص����� */
typedef struct hiSVR_PLAYER_EVENT_S
{
    HI_SVR_PLAYER_EVENT_E  eEvent;    /**< Event type *//**< CNcomment:�¼����� */
    HI_U32  u32Len;                   /**< Event parameter length, in the unit of byte. *//**< CNcomment:�¼��������ȣ��ֽ�Ϊ��λ */
    HI_U8   *pu8Data;                 /**< Start address of event parameter data *//**< CNcomment:�¼�����������ʼ��ַ */
} HI_SVR_PLAYER_EVENT_S;

/************************************Player Event End************************************/

/** Type of the registered dynamic link library (DLL) */
/** CNcomment:ע��Ķ�̬������ */
typedef enum hiSVR_PLAYER_DLLTYPE_E
{
    HI_SVR_PLAYER_DLL_PARSER = 0x0,    /**< DLLs for parsing audio and video media files and subtitle files *//**< CNcomment:����Ƶý���ļ���������Ļ������̬�� */
    HI_SVR_PLAYER_DLL_BUTT
} HI_SVR_PLAYER_DLLTYPE_E;

/** Playing speed flag */
/** CNcomment:�����ٶȱ�ʶ */
typedef enum hiSVR_PLAYER_PLAY_SPEED_E
{
    HI_SVR_PLAYER_PLAY_SPEED_1X2_SLOW_FORWARD  = HI_SVR_PLAYER_PLAY_SPEED_NORMAL/2,      /**< 1/2 speed slow forward *//**< CNcomment:1/2�������� */
    HI_SVR_PLAYER_PLAY_SPEED_2X_FAST_FORWARD   = 2 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,      /**< 2 x speed fast forward *//**< CNcomment:2���ٿ�� */
    HI_SVR_PLAYER_PLAY_SPEED_4X_FAST_FORWARD   = 4 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,      /**< 4 x speed fast forward *//**< CNcomment:4���ٿ�� */
    HI_SVR_PLAYER_PLAY_SPEED_8X_FAST_FORWARD   = 8 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,      /**< 8 x speed fast forward *//**< CNcomment:8���ٿ�� */
    HI_SVR_PLAYER_PLAY_SPEED_16X_FAST_FORWARD  = 16 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,     /**< 16 x speed fast forward *//**< CNcomment:16���ٿ�� */
    HI_SVR_PLAYER_PLAY_SPEED_32X_FAST_FORWARD  = 32 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,     /**< 32 x speed fast forward *//**< CNcomment:32���ٿ�� */
    HI_SVR_PLAYER_PLAY_SPEED_64X_FAST_FORWARD  = 64 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,     /**< 64 x speed fast forward *//**< CNcomment:64���ٿ�� */
    HI_SVR_PLAYER_PLAY_SPEED_1X_BACKWARD       = -1 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,     /**< 1 x speed rewind *//**< CNcomment:1���ٵ��� */
    HI_SVR_PLAYER_PLAY_SPEED_2X_FAST_BACKWARD  = -2 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,     /**< 2 x speed rewind *//**< CNcomment:2���ٿ��� */
    HI_SVR_PLAYER_PLAY_SPEED_4X_FAST_BACKWARD  = -4 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,     /**< 4 x speed rewind *//**< CNcomment:4���ٿ��� */
    HI_SVR_PLAYER_PLAY_SPEED_8X_FAST_BACKWARD  = -8 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,     /**< 8 x speed rewind *//**< CNcomment:8���ٿ��� */
    HI_SVR_PLAYER_PLAY_SPEED_16X_FAST_BACKWARD = -16 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,    /**< 16 x speed rewind *//**< CNcomment:16���ٿ��� */
    HI_SVR_PLAYER_PLAY_SPEED_32X_FAST_BACKWARD = -32 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,    /**< 32 x speed rewind *//**< CNcomment:32���ٿ��� */
    HI_SVR_PLAYER_PLAY_SPEED_64X_FAST_BACKWARD = -64 * HI_SVR_PLAYER_PLAY_SPEED_NORMAL,    /**< 64 x speed rewind *//**< CNcomment:64���ٿ��� */
    HI_SVR_PLAYER_PLAY_SPEED_BUTT                                                          /**< Invalid speed value *//**< CNcomment:��Ч���ٶ�ֵ */
} HI_SVR_PLAYER_PLAY_SPEED_E;

/** The parameters need to be specified when the player is created. */
/** CNcomment:����������ʱ��ָ���Ĳ��� */
typedef struct hiSVR_PLAYER_PARAM_S
{
    HI_U32  u32DmxId;                 /**< Dmx ID. Not used now. *//**< CNcomment:dmx id��Ŀǰû��ʹ��*/
    HI_U32  u32PortId;                /**< Port ID. Not used now. *//**< CNcomment:port id��Ŀǰû��ʹ��*/
    HI_U32  x;                     /**< Coordinate of the video output window. This parameter is invalid in the case of hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL. *//**< CNcomment:��Ƶ����������꣬hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL���ò�����Ч */
    HI_U32  y;                     /**< Coordinate of the video output window. This parameter is invalid in the case of hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL. *//**< CNcomment:��Ƶ����������꣬hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL���ò�����Ч */
    HI_U32  w;                   /**< Width of the video output window. This parameter is invalid in the case of hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL. *//**< CNcomment:��Ƶ������ڿ�hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL���ò�����Ч */
    HI_U32  h;                   /**< Height of the video output window. This parameter is invalid in the case of hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL. *//**< CNcomment:��Ƶ������ڸߣ�hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL���ò�����Ч */
    HI_U32  u32MixHeight;             /**< Audio output mix weight. The value ranges from 0 to 100. This parameter is invalid in the case of hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL. *//**< CNcomment:��Ƶ�������Ȩ��0-100��hAVPlayer!=HI_SVR_PLAYER_INVALID_HDL���ò�����Ч */
    HI_HANDLE hAVPlayer;              /**< AVPlay created externally. A window device and a sound device are bound to the AVPlay and the window size and the sound volume are set.
                                                                    The AVPlay handle can be transferred to the HiPlayer, which will use the AVPlay for playing. If this parameter is set to
                                                                    HI_SVR_PLAYER_INVALID_HDL, the HiPlayer internally creates the AVPlay and window automatically. */
                                      /**< CNcomment:�ⲿ�Ѿ�������avplay������window,sound�������ú�window�Ĵ�Сλ�ã�sound��������
                                                                    ���Խ�avplay�������HiPlayer��HiPlayer����ʹ�ø�avplay���š�����ò�������Ϊ
                                                                    HI_SVR_PLAYER_INVALID_HDL��HiPlayer�ڲ����Զ�����avplayer��window */
    HI_HANDLE hVSink;                /**< vsink handle create external */
    HI_HANDLE hASink;                /**< asink handle create external */
    HI_U32   u32SndPort;             /**< Specified audio port number, 0: master audio, 1: slave audio  *//**< CNcomment:ָ����Ƶ�˿ںţ�0��������1������*/
    HI_U32   u32Display;             /**< Type of display, value is::HI_UNF_DISP_E *//**< CNcomment:display ���ͣ�ȡֵ�ο�::HI_UNF_DISP_E */
    HI_U32   u32VDecErrCover;   /**<Error concealment threshold of the output frames of a video decoder. The value 0 indicates that no frames are output if an error occurs; the value 100 indicates that all frames are output no matter whether errors occur.*/
                                               /**<CNcomment: ��Ƶ�����������֡�����������ޣ�0:���ִ��󼴲������100:���ܴ������ȫ�����*/
    HI_HANDLE hDRMClient;            /**< DRM client created externally. */
    HI_HANDLE hWindow;               /**< window handle create external */
} HI_SVR_PLAYER_PARAM_S;

/** Input media file */
/** CNcomment:�����ý���ļ� */
typedef struct hiSVR_PLAYER_MEDIA_S
{
    HI_CHAR  aszUrl[HI_FORMAT_MAX_URL_LEN];          /**< File path, absolute file path, such as /mnt/filename.ts. *//**< CNcomment:�ļ�·��������·������/mnt/filename.ts */
    HI_S32   s32PlayMode;                            /**< Set the mode of the player. The parameter is ::HI_SVR_PLAYER_PLAY_MODE_E *//**< CNcomment:���ò���ģʽ,����::HI_SVR_PLAYER_PLAY_MODE_E */
    HI_U32   u32ExtSubNum;                        /**< Number of subtitle files *//**< CNcomment:��Ļ�ļ����� */
    HI_CHAR  aszExtSubUrl[HI_FORMAT_MAX_LANG_NUM][HI_FORMAT_MAX_URL_LEN];  /**< Absolute path of a subtitle file, such as /mnt/filename.ts. *//**< CNcomment:��Ļ�ļ�·��������·������/mnt/filename.ts */
    HI_U32   u32UserData;                            /**< Create a handle by calling the fmt_open function, send the user data to the DEMUX by calling the fmt_invoke, and then call the fmt_find_stream function. */
                                                     /**< CNcomment:�û����ݣ�HiPlayer����͸�������ý�����fmt_open֮��ͨ��fmt_invoke�ӿڴ��ݸ����������ٵ���fmt_find_stream�ӿ� */
} HI_SVR_PLAYER_MEDIA_S;

/** IDs of the AV and subtitle streams to be switched. */
/** CNcomment:Ҫ�л���������Ƶ����Ļ��ID  */
typedef struct hiSVR_PLAYER_STREAMID_S
{
    HI_U16 u16ProgramId;   /**< Program ID. The value is the subscript of the pastProgramInfo array in the ::HI_FORMAT_FILE_INFO_S structure and ranges from 0 to (u32ProgramNum - 1). */
                           /**< CNcomment:��Ŀid��ֵΪ::HI_FORMAT_FILE_INFO_S�ṹpastProgramInfo�����±꣬0-(u32ProgramNum - 1) */
    HI_U16 u16VidStreamId;   /**< Video stream ID. The value is the subscript of the pastVidStream array in the ::HI_FORMAT_PROGRAM_INFO_S structure and ranges from 0 to (u32VidStreamNum - 1). */
                           /**< CNcomment:��Ƶ��id��ֵΪ::HI_FORMAT_PROGRAM_INFO_S�ṹpastVidStream�����±꣬0-(u32VidStreamNum - 1)*/
    HI_U16 u16AudStreamId;   /**< Audio stream ID. The value is the subscript of the pastAudStream array in the ::HI_FORMAT_PROGRAM_INFO_S structure and ranges from 0 to (u32AudStreamNum - 1).*/
                           /**< CNcomment:��Ƶ��id��ֵΪ::HI_FORMAT_PROGRAM_INFO_S�ṹpastAudStream�����±꣬0-(u32AudStreamNum - 1)*/
    HI_U16 u16SubStreamId;  /**< Subtitle ID. The value is the subscript of the pastSubStream array in the ::HI_FORMAT_PROGRAM_INFO_S structure and ranges from 0 to (u32SubStreamNum - 1). */
                           /**< CNcomment:��Ļid��ֵΪ::HI_FORMAT_PROGRAM_INFO_S�ṹastSubTitle�����±꣬0-(u32SubStreamNum - 1) */
} HI_SVR_PLAYER_STREAMID_S;

/** Set the offset of the stream PTS */
/** CNcomment:��������Ƶ����Ļʱ��ƫ�� */
typedef struct hiSVR_PLAYER_SYNC_ATTR_S
{
    HI_S32 s32VidFrameOffset;    /**< offset (in the unit of ms) of the video stream. Add this offset to the PTS of the video stream. *//**< CNcomment:��Ƶ��ƫ��ֵ������Ƶ��ʱ������������Ӹ�offset����λms */
    HI_S32 s32AudFrameOffset;    /**< offset (in the unit of ms) of the audio stream. Add this offset to the PTS of the audio stream. *//**< CNcomment:��Ƶ��ƫ��ֵ������Ƶ��ʱ������������Ӹ�offset����λms */
    HI_S32 s32SubTitleOffset;  /**< offset (in the unit of ms) of the subtitle stream. Add this offset to the PTS of the subtitle stream. *//**< CNcomment:��Ļ��ƫ��ֵ������Ļ��ʱ������������Ӹ�offset����λms */
    HI_S32 s32SyncVidPlusTime;  /**<Plus time range during video synchronization*//**<CNcomment: ��Ƶͬ����ǰ��ʱ�䷶Χ */
    HI_S32 s32SyncVidNegativeTime;  /**<Negative time range during video synchronization*//**<CNcomment: ��Ƶͬ������ʱ�䷶Χ */
} HI_SVR_PLAYER_SYNC_ATTR_S;

/** Player information */
/** CNcomment:��������Ϣ */
typedef struct hiSVR_PLAYER_INFO_S
{
    HI_U32  u32Progress;                 /**< Playing progress. The value ranges from 0 to 100. *//**< CNcomment:���Űٷֱȣ�0-100 */
    HI_U64  u64TimePlayed;               /**< Elapsed time, in the unit of ms. *//**< CNcomment:�Ѳ���ʱ�䣬��λms */
    HI_S32  s32Speed;                    /**< Playing speed *//**< CNcomment:�������� */
    HI_SVR_PLAYER_STATE_E  eStatus;      /**< Playing status *//**< CNcomment:����״̬ */
} HI_SVR_PLAYER_INFO_S;

/** The result of setting the subtitle transcoding type */
/** CNcomment:Invoke����������Ļ�������͵��¼��ص����� */
typedef struct hiSVR_PLAYER_SUB_CODETYPE_S
{
    HI_U32 u32SubId;                    /**< Subtitle ID. For details, see ::HI_FORMAT_SUB_INFO_S:s32StreamIndex *//**< CNcomment:��ĻID���ο�::HI_FORMAT_SUB_INFO_S:s32StreamIndex���� */
    HI_U32 u32Result;                   /**< Result of setting the subtitle transcoding type *//**< CNcomment:invoke�����ֵ */
} HI_SVR_PLAYER_SUB_CODETYPE_S;

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API declaration *****************************/
/** \addtogroup      Hiplayer */
/** @{ */  /** <!-- [Hiplayer]*/

/**
\brief Player event callback function. The ::HI_SVR_PLAYER_RegCallback interface can be called to register the callback function. CNcomment:�������¼��ص�����������::HI_SVR_PLAYER_RegCallback�ӿ�ע��ûص�����CNend
\attention \n
None.
\param[out] hPlayer player handle. CNcomment:���������CNend
\param[out] pstruEvent event parameter. CNcomment: �¼�����CNend

\retval ::HI_SUCCESS

\see \n
None.
*/
typedef HI_S32 (*HI_SVR_PLAYER_EVENT_FN)(HI_HANDLE hPlayer, HI_SVR_PLAYER_EVENT_S *pstruEvent);

/**
\brief Initialize the player. CNcomment:��ʼ��Player CNend
\attention \n
The HI_S32 HI_SVR_PLAYER_Init interface can be called to return a success message after the initialization is successful. Multiple processes are not supported
. This interface must be called prior to other interfaces.
CNcomment:��ʼ���ɹ����ٵ��øýӿڷ��سɹ�����֧�ֶ���̣����������ӿ�ǰ�����ȵ��øýӿ�CNend
\param None. CNcomment:��CNend

\retval ::HI_SUCCESS The initialization is successful. CNcomment:��ʼ���ɹ�CNend
\retval ::HI_FAILURE The initialization fails. CNcomment:��ʼ��ʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Init(HI_VOID);

/**
\brief Deinitialize the player module by calling the HI_S32 HI_SVR_PLAYER_Deinit interface. The player module is not used any more. CNcomment:ȥ��ʼ��playerģ�飬����ʹ��playerģ�飬���øýӿ�ȥ��ʼ��playerģ��CNend
\attention \n
The ::HI_SVR_PLAYER_Destroy interface must be called to release the created player first. Otherwise, a failure is returned. The Deinit interface does not release the player resource. \n
This interface can be called to return a success message after the deinitialization is successful.
CNcomment:�����ȵ���::HI_SVR_PLAYER_Destroy�ӿ��ͷŵ���Ĳ��������ٵ��øýӿڣ�����᷵��ʧ�ܣ�Deinit�������ͷ�\n
��������Դ��ȥ��ʼ���ɹ����ٵ��øýӿڷ��سɹ�CNend
\param None. CNcomment:��CNend

\retval ::HI_SUCCESS The deinitialization is successful. CNcomment:ȥ��ʼ���ɹ�CNend
\retval ::HI_FAILURE The deinitialization fails and the created player is not released. CNcomment:ȥ��ʼ��ʧ�ܣ�û���ͷŵ���Ĳ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Deinit(HI_VOID);

/**
\brief Create a player. CNcomment:����һ��������CNend
\attention \n
This interface must be called after initialization. Only one player can be created. \n
CNcomment:�ýӿڱ����ڳ�ʼ������ã�֧����ഴ��һ��������\n CNend
\param[in] pstruParam player initialization attribute. CNcomment:��������ʼ������CNend
\param[out] phPlayer handle of the created player. CNcomment:�����Ĳ��������CNend

\retval ::HI_SUCCESS A player is created successfully and the player handle is valid. CNcomment:�����ɹ��������������ЧCNend
\retval ::HI_FAILURE A player fails to be created. The parameters are invalid or resources are insufficient. CNcomment:����ʧ�ܣ������Ƿ�����Դ����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Create(const HI_SVR_PLAYER_PARAM_S *pstruParam, HI_HANDLE *phPlayer);

/**
\brief Set the URL of the media file to be played. The URL format is absolute path + media file name. CNcomment:����Ҫ���ŵ�ý���ļ�url��ַ��url��ʽΪ:����·��+ý���ļ���CNend
\attention \n
The HI_SVR_PLAYER_SetMedia interface is a synchronous interface. It must be called after the ::HI_SVR_PLAYER_Create interface is called or after the ::
HI_SVR_PLAYER_Stop interface is called to stop the player.
This interface performs the following operations:
1. Queries the media file set by the DEMUX during parsing.
2. Queries subtitle files in the same directory as the audio and video media file automatically, and the queried subtitle files' name are the same as the name of the media file.
3. Performs the following steps if no AVPlay is specified:
   Creates an AVPlay and sets the audio and video attributes based on the parsed file attributes such as the AV encoding type.
   Creates a window, set the window size, and bind the window to the AVPlay.
   Creates a audio track(use device HI_UNF_SND_0),and bind it to the AVPlay.
The window display location and mixheight set for binding audio track to the AVPlay are specified when the ::HI_SVR_PLAYER_Create interface is called.
The window display location and mixheight are invalid if the AVPlay is specified externally.
CNcomment:ͬ���ӿڣ��ýӿڱ����ڵ���::HI_SVR_PLAYER_Create֮����ã����ߵ���::HI_SVR_PLAYER_Stop�ӿ�ֹͣ�����������
�ý���Ҫִ�����´���:
1�����ҽ������������õ�ý���ļ�
2������ý���ļ���ͬ��·���²�����ý���ļ�ͬ������Ļ�ļ�
3�����û��ָ��avplay��
   ����avplay�����ݽ��������ļ�������������Ƶ���ԣ�������Ƶ��������
   ����window������window����λ�ã���window�󶨵�avplay
   ����audio track(ʹ���豸HI_UNF_SND_0)������audio track�󶨵�avplay
window��ʾλ�ã��󶨵�avplay��audio track��mixheight(���avplay���ⲿָ��������ʾλ�ú�mixheight������Ч)��
�ڵ���::HI_SVR_PLAYER_Create�ӿ�ʱָ��CNend

\param[in] hPlayer handle of the player is created by calling the ::HI_SVR_PLAYER_Create interface. CNcomment:ͨ������::HI_SVR_PLAYER_Create�ӿڴ����Ĳ��������CNend
\param[in] eType media file. If only AV media files are specified, set this parameter to HI_SVR_PLAYER_MEDIA_STREAMFILE.\n
           If a subtitle file is specified, set this parameter to HI_SVR_PLAYER_MEDIA_STREAMFILE | HI_SVR_PLAYER_MEDIA_SUBTITLE.
           CNcomment:ý���ļ���ָֻ������Ƶý���ļ�����ò�������ΪHI_SVR_PLAYER_MEDIA_STREAMFILE��\n
            �����ָ������Ļ�ļ�������(HI_SVR_PLAYER_MEDIA_STREAMFILE | HI_SVR_PLAYER_MEDIA_SUBTITLE) CNend
\param[in] pstruMedia media file information. Only absolute path is supported.  \n
           The player searches for a subtitle file whose name is the same as the name of the audio and video media file in the same directory as the audio and video media file automatically.
            CNcomment:ý���ļ���Ϣ��ֻ֧�־���·�������������Զ�������Ƶý\n
            ���ļ�����Ŀ¼�²���������Ƶý���ļ�����ͬ����Ļ�ļ�CNend

\retval ::HI_SUCCESS The media file is set successfully. The ::HI_SVR_PLAYER_Play interface can be called to start playing the file. CNcomment:ý���ļ����óɹ�����ʱ����::HI_SVR_PLAYER_Play�ӿڿ��Կ�ʼ����CNend
\retval ::HI_FAILURE The media file fails to be set. CNcomment:ý���ļ�����ʧ��CNend
\retval ::HI_ERRNO_NOT_SUPPORT_FORMAT The file format is not supported. CNcomment:��֧�ֵ��ļ���ʽCNend
\retval ::HI_ERRNO_NOT_SUPPORT_PROTOCOL The protocol is not supported. CNcomment:��֧�ֵ�Э��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_SetMedia(HI_HANDLE hPlayer, HI_U32 eType, HI_SVR_PLAYER_MEDIA_S *pstruMedia);

/**
\brief Switch a new Demux source.This interface is not supported now. CNcomment:�����л�����Դ���ýӿڵ�ǰ��֧��CNend
\attention \n
The HI_SVR_PLAYER_SetSource interface is a synchronous interface. It must be called after the ::HI_SVR_PLAYER_Stop interface is called. Call
::HI_SVR_PLAYER_Play to play the new Demux source.
The handle of the new demux source is created by call the ::HI_SVR_FORMAT_Open function.
CNcomment:����::HI_SVR_PLAYER_Stopֹͣ�������󣬵��øýӿ����°�һ������Դ������::HI_SVR_PLAYER_Play�ӿ����²����µ�����Դ��������Դ���ͨ������::HI_SVR_FORMAT_Open��������CNend
\param[in] hPlayer player handle. CNcomment:���������CNend
\param[in] hFormat The handle of the new demux source. CNcomment:������Դ���CNend

\retval ::HI_SUCCESS The new demux source is set successfully. The ::HI_SVR_PLAYER_Play interface can be called to start playing the file. CNcomment:������Դ���óɹ�����ʱ����::HI_SVR_PLAYER_Play�ӿڿ��Կ�ʼ����CNend
\retval ::HI_FAILURE The new demux source fails to be set. CNcomment:������Դ����ʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_SetSource(HI_HANDLE hPlayer, HI_HANDLE hFormat);

/**
\brief Destroy a player instance. CNcomment:����һ��������ʵ��CNend
\attention \n
The HI_S32 HI_SVR_PLAYER_Destroy interface is called to destroy the player resource after the ::HI_SVR_PLAYER_Create interface is called to create a player.
CNcomment:����::HI_SVR_PLAYER_Create�����������󣬵��øýӿ����ٲ�������ԴCNend
\param[in] hPlayer player handle. CNcomment:���������CNend

\retval ::HI_SUCCESS The player is released successfully. CNcomment:�������ͷųɹ�CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Destroy(HI_HANDLE hPlayer);

/**
\brief Set player parameters. CNcomment:���ò���������CNend
\attention \n
The following operations are supported:
The ::HI_SVR_PLAYER_ATTR_STREAMID interface can be used to set the ID of the stream to be played. It can be called to set the audio streams to be played for
single-video multi-audio files. \n
Calling the HI_SVR_PLAYER_SetParam interface is an asynchronous operation during playing. Nevertheless, calling this interface is a synchronous operation
after the SetMedia operation is performed prior to playing. Values returned by the interface cannot be used to check whether the operation is successful.
The player notifies the application (APP) of the stream ID setting status by using the ::HI_SVR_PLAYER_EVENT_STREAMID_CHANGED event. The event parameter is ::
HI_SVR_PLAYER_STREAMID_S.
CNcomment:�ýӿڱ����ڵ���::HI_SVR_PLAYER_SetMedia�ӿں����CNend
CNcomment:֧�ֵĲ�������
::HI_SVR_PLAYER_ATTR_STREAMID : ���ò��ŵ���id�����ڵ���Ƶ������Ƶ�ļ�������ͨ���ò�������Ҫ���ŵ���Ƶ��\n
���Ź����е��øýӿ�Ϊ�첽����������ǰ��SetMedia����øýӿ�Ϊͬ���������첽��������ͨ���ӿڷ���ֵ���ж�\n
��id�Ƿ����óɹ�����������ͨ��::HI_SVR_PLAYER_EVENT_STREAMID_CHANGED�¼�֪ͨapp���¼�����Ϊ::HI_SVR_PLAYER_STREAMID_S��\n
::HI_SVR_PLAYER_ATTR_SYNC : ��������Ƶ����Ļ��ʱ���ƫ�ƣ�HiPlayer��ȡ����Ƶ֡��ʱ����������õ�ƫ��ֵ������ͬ��CNend

\param[in] hPlayer player handle. CNcomment:���������CNend
\param[in] eAttrId ID of the player parameter to be set. CNcomment:Ҫ���õĲ���������ID CNend
\param[in] pArg player parameter to be set. CNcomment:Ҫ���õĲ���������CNend

\retval ::HI_SUCCESS Parameters are set successfully. CNcomment:�������óɹ�CNend
\retval ::HI_FAILURE The operation fails. CNcomment:����ʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_SetParam(HI_HANDLE hPlayer, HI_SVR_PLAYER_ATTR_E eAttrId, const HI_VOID *pArg);

/**
\brief Obtain player parameters. CNcomment:��ȡ����������CNend
\attention \n
This interface must be called after the ::HI_SVR_PLAYER_SetMedia interface is called.
The following operations are supported:
::HI_SVR_PLAYER_ATTR_STREAMID: Obtain the ID of the stream that is played currently.
::HI_SVR_PLAYER_ATTR_WINDOW_HDL: Obtain the window handle created by the player.
::HI_SVR_PLAYER_ATTR_AVPLAYER_HDL: Obtain the AVPlay handle created by the player.
::HI_SVR_PLAYER_ATTR_SO_HDL: Obtain the SO module handle created by the player.
::HI_SVR_PLAYER_ATTR_AUDTRACK_HDL:Obtain the audio track handle created by the player.

CNcomment:�ýӿڱ����ڵ���::HI_SVR_PLAYER_SetMedia�ӿں����
֧�ֵĲ�������
::HI_SVR_PLAYER_ATTR_STREAMID :��ȡ��ǰ���ŵ���id
::HI_SVR_PLAYER_ATTR_WINDOW_HDL :��ȡ������������window���
::HI_SVR_PLAYER_ATTR_AVPLAYER_HDL :��ȡ������������avplay���
::HI_SVR_PLAYER_ATTR_SO_HDL :��ȡ��������������Ļ���ģ��(so)���
::HI_SVR_PLAYER_ATTR_AUDTRACK_HDL :��ȡ��������������Ƶtrack id CNend

\param[in] hPlayer player handle. CNcomment:���������CNend
\param[in] eAttrId player parameter ID. CNcomment:����������ID CNend
\param[out] pArg obtained player parameters. CNcomment:��ȡ�Ĳ���������CNend

\retval ::HI_SUCCESS Parameters are obtained successfully. CNcomment:��ȡ�����ɹ�CNend
\retval ::HI_FAILURE The operation fails. CNcomment:����ʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_GetParam(HI_HANDLE hPlayer, HI_SVR_PLAYER_ATTR_E eAttrId, HI_VOID *pArg);

/**
\brief Register a player event callback function. CNcomment:ע�Ქ�����¼��ص�����CNend
\attention \n
This interface must be called after the ::HI_SVR_PLAYER_Create interface is called. This interface is unrelated to the player status. This function cannot be called in any player callback event.
CNcomment:�ú��������ڵ���::HI_SVR_PLAYER_Create�ӿں���ã��ýӿ��벥����״̬�޹أ��ú���������player�κλص��¼��е���CNend
\param[in] hPlayer player handle. CNcomment:���������CNend
\param[in] pfnCallback player event callback function. The callback function definition is ::HI_SVR_PLAYER_EVENT_FN. CNcomment:�������¼��ص��������ص���������::HI_SVR_PLAYER_EVENT_FN CNend

\retval ::HI_SUCCESS The registration is successful. CNcomment:ע��ɹ�CNend
\retval ::HI_FAILURE The registration fails. CNcomment:����ʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_RegCallback(HI_HANDLE hPlayer, HI_SVR_PLAYER_EVENT_FN pfnCallback);

/**
\brief Start playing. CNcomment:��ʼ����CNend
\attention \n
This interface is an asynchronous interface. It must be called after the ::HI_SVR_PLAYER_SetMedia interface is called. If this interface is called after successful playing, HI_SUCCESS is returned. \n
Values returned by this interface cannot be used to check whether the playing is successful. The player notifies the APP of playing success or failure by using the ::HI_SVR_PLAYER_EVENT_STATE_CHANGED event. \n
The event parameter value is ::HI_SVR_PLAYER_STATE_PLAY.This interface can be called to restart playing after the playing stops.

CNcomment:�첽�ӿڣ��ýӿڱ����ڵ���::HI_SVR_PLAYER_SetMedia�ӿں���ã����ųɹ����ٵ��øýӿڷ���HI_SUCCESS��\n
����ͨ���ýӿڷ���ֵ���жϲ������Ƿ񲥷ųɹ�����������ͨ��::HI_SVR_PLAYER_EVENT_STATE_CHANGED �¼�֪ͨ\n
app���ųɹ����¼�����ֵΪ::HI_SVR_PLAYER_STATE_PLAY��ֹͣ���ź󣬿��Ե��øýӿ����²���CNend

\param[in] hPlayer player handle. CNcomment:���������CNend

\retval ::HI_SUCCESS The operation is valid. CNcomment:�Ϸ�����CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Play(HI_HANDLE hPlayer);

/**
\brief Stop playing. CNcomment:ֹͣ����CNend
\attention \n
This interface is an asynchronous interface. It can be called to stop playing during playing, fast forward, rewind, and pause. Values returned by this interface cannot be used to check whether playing is stopped successfully. \n
The player notifies the APP of stop success or failure by using the ::HI_SVR_PLAYER_EVENT_STATE_CHANGED event. The event parameter value is ::HI_SVR_PLAYER_STATE_STOP. \n
The ::HI_SVR_PLAYER_Play interface can be called to restart playing after playing is stopped.

CNcomment:�첽�ӿڣ����š���������ˡ���ͣ�����ж����Ե��øýӿ�ֹͣ���ţ�����ͨ���ýӿڷ���ֵ���жϲ�����\n
�Ƿ�ֹͣ�ɹ�����������ͨ��::HI_SVR_PLAYER_EVENT_STATE_CHANGED �¼�֪ͨappֹͣ�ɹ����¼�����ֵΪ::HI_SVR_PLAYER_STATE_STOP��\n
ֹͣ���ź󣬿��Ե���::HI_SVR_PLAYER_Play�ӿ����²���CNend

\param[in] hPlayer player handle. CNcomment:���������CNend

\retval ::HI_SUCCESS The operation is valid. CNcomment:�Ϸ�����CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Stop(HI_HANDLE hPlayer);

/**
\brief Pause playing.  CNcomment:��ͣ����CNend
\attention \n
This interface is an asynchronous interface. It can be called to pause playing but cannot be called during fast forward and rewind. Values returned by this interface cannot be used to check whether playing is paused successfully. \n
The player notifies the APP of pause success or failure by using the ::HI_SVR_PLAYER_EVENT_STATE_CHANGED event. The event parameter value is ::HI_SVR_PLAYER_STATE_PAUSE. \n
The ::HI_SVR_PLAYER_Resume interface can be called to resume playing after a successful pause.
CNcomment:�첽�ӿڣ����Ź����п��Ե��øýӿ���ͣ���ţ����������״̬�²��ܵ��øýӿڣ�����ͨ���ýӿڷ���ֵ���жϲ�����\n
�Ƿ���ͣ�ɹ�����������ͨ��::HI_SVR_PLAYER_EVENT_STATE_CHANGED �¼�֪ͨappֹͣ�ɹ����¼�����ֵΪ::HI_SVR_PLAYER_STATE_PAUSE��\n
��ͣ�ɹ��󣬿��Ե���::HI_SVR_PLAYER_Resume��������CNend

\param[in] hPlayer player handle. CNcomment:���������CNend

\retval ::HI_SUCCESS The operation is valid. CNcomment:�Ϸ�����CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Pause(HI_HANDLE hPlayer);

/**
\brief Resume playing. CNcomment:�ָ�����CNend
\attention \n
This interface is an asynchronous interface. It can be called to resume playing during pause, fast forward, and rewind. Values returned by this interface cannot be used to check whether playing is resumed successfully. \n
The player notifies the APP of resumption success or failure by using the ::HI_SVR_PLAYER_EVENT_STATE_CHANGED event. The event parameter value is ::HI_SVR_PLAYER_STATE_PLAY.

CNcomment:�첽�ӿڣ���ͣ�����������״̬�£����øýӿڻָ��������ţ�����ͨ���ýӿڷ���ֵ���жϲ������Ƿ�ָ�����\n
��������ͨ��::HI_SVR_PLAYER_EVENT_STATE_CHANGED �¼�֪ͨapp�ָ��ɹ����¼�����ֵΪ::HI_SVR_PLAYER_STATE_PLAY��CNend

\param[in] hPlayer player handle. CNcomment:���������CNend

\retval ::HI_SUCCESS The operation is valid. CNcomment:�Ϸ�����CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Resume(HI_HANDLE hPlayer);

/**
\brief Fast forward and rewind. CNcomment:���������CNend
\attention \n
This interface is an asynchronous interface. It must be called in play, pause or tplay status. The negative value of s32Speed indicates rewind and the
positive value of s32Speed indicates fast forward. Values returned by the interface cannot be used to check whether the fast forward or rewind is successful.
\n
The player notifies the APP of the fast forward or rewind status by using the ::HI_SVR_PLAYER_EVENT_STATE_CHANGED event. \n
The event parameter value is ::HI_SVR_PLAYER_STATE_FORWARD or ::HI_SVR_PLAYER_STATE_BACKWARD. The ::HI_SVR_PLAYER_Resume interface can be called to \n
resume normal playing after fast forward or rewind.
CNcomment:�첽�ӿڣ��ú������벥�š���ͣ����/����״̬�µ��ã�s32SpeedΪ����ʾ���ˣ�Ϊ����ʾ���������ͨ���ýӿ�\n
����ֵ���ж��Ƿ�������˳ɹ�����������ͨ��::HI_SVR_PLAYER_EVENT_STATE_CHANGED �¼�֪ͨapp���¼�����ֵΪ\n
::HI_SVR_PLAYER_STATE_FORWARD��::HI_SVR_PLAYER_STATE_BACKWARD����������˺�ͨ������::HI_SVR_PLAYER_Resume\n
�ӿڻָ��������š�CNend

\param[in] hPlayer player handle. CNcomment:���������CNend
\param[in] s32Speed playing speed. The value is HI_SVR_PLAYER_PLAY_SPEED_E. CNcomment:���ű���,ֵΪHI_SVR_PLAYER_PLAY_SPEED_E CNend

\retval ::HI_SUCCESS The operation is valid. CNcomment:�Ϸ�����CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_TPlay(HI_HANDLE hPlayer, HI_S32 s32Speed);

/**
\brief Seek to a specified location for playing. CNcomment:����ָ��λ�ò���CNend
\attention \n
This interface is an asynchronous interface. It must be called after the ::HI_SVR_PLAYER_SetMedia interface is called. It can be called to jump to a specified time point for playing.
CNcomment:�첽�ӿڣ��ýӿڱ����ڵ���::HI_SVR_PLAYER_SetMedia�ӿں���ã����š�ֹͣ״̬�£����øýӿ�����ָ��ʱ��㲥��CNend

\param[in] hPlayer player handle. CNcomment:���������CNend
\param[in] s64TimeInMs seeking time. The unit is ms. CNcomment:seekʱ�䣬��λms CNend

\retval ::HI_SUCCESS The operation is successful. CNcomment:Seek�ɹ�CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Seek(HI_HANDLE hPlayer, HI_S64 s64TimeInMs);

/**
\brief Seek to a specified location for playing. CNcomment:����ָ��λ�ò���CNend
\attention \n
This interface is an asynchronous interface. It must be called after the ::HI_SVR_PLAYER_SetMedia interface is called. It can be called to jump to a specified file position  for playing.
CNcomment:�첽�ӿڣ��ýӿڱ����ڵ���::HI_SVR_PLAYER_SetMedia�ӿں���ã����š�ֹͣ״̬�£����øýӿ�����ָ���ļ�λ�ò���CNend

\param[in] hPlayer player handle. CNcomment:���������CNend
\param[in] s64Offset offset(in bytes) of the starting position. CNcomment:����ļ���ʼλ�õ��ֽ�ƫ��CNend

\retval ::HI_SUCCESS The operation is successful. CNcomment:Seek�ɹ�CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_SeekPos(HI_HANDLE hPlayer, HI_S64 s64Offset);

/**
\brief Invoke operation. Application expand operation. The player will transparently transmits the invoke to the demux. CNcomment:invoke������app��չʹ�ã�player�Ὣ�ò���͸����demux CNend
\attention \n
It must be called after the ::HI_SVR_PLAYER_SetMedia interface is called.
CNcomment:�ýӿڱ����ڵ���::HI_SVR_PLAYER_SetMedia�ӿں����CNend

\param[in] hPlayer player handle. CNcomment:���������CNend
\param[in] u32InvokeId Operation ID of application expanding,the type of u32InvokeId is HI_FORMAT_INVOKE_ID_E. CNcomment:app��չ�Ĳ���id, �ο�HI_FORMAT_INVOKE_ID_E CNend
\param[in/out] pArg param of the invoking operation. CNcomment:��������CNend

\retval ::HI_SUCCESS The operation is successful. CNcomment:�����ɹ�CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_Invoke(HI_HANDLE hPlayer, HI_U32 u32InvokeId, HI_VOID *pArg);

/**
\brief Obtain information about the current open file, such as the file size, playing duration, file bit rate, video width, video height, coding format, frame rate, video bit rate, audio encoding, and audio bit rate.
CNcomment:��ȡ��ǰ���ļ���Ϣ�����ļ���С���ļ�����ʱ�������ʵȣ���Ƶ���ߣ������ʽ��֡�ʡ����ʣ���Ƶ���롢���ʵ�CNend
\attention \n
This interface must be called after the ::HI_SVR_PLAYER_SetMedia interface is called.
CNcomment:�ú��������ڵ���::HI_SVR_PLAYER_SetMedia�ӿں����CNend

\param[in] hPlayer player handle. CNcomment:���������CNend
\param[out] ppstruInfo obtained file information. CNcomment:��ȡ���ļ���ϢCNend

\retval ::HI_SUCCESS The file information is obtained successfully. CNcomment:��ȡ���ļ���ϢCNend
\retval ::HI_FAILURE The file information fails to be obtained. CNcomment:��ȡ�ļ���Ϣʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_GetFileInfo(HI_HANDLE hPlayer, HI_FORMAT_FILE_INFO_S **ppstruInfo);

/**
\brief Obtain the player information, such as the current playing status, playing progress, and elapsed time. CNcomment:��ȡ��������Ϣ���統ǰ����״̬�����Ž��ȡ��Ѳ���ʱ���CNend
\attention \n
This interface must be called after the ::HI_SVR_PLAYER_Create interface is called. The playing progress and elapsed time are valid only after the ::HI_SVR_PLAYER_Play interface is called.
CNcomment:�ú��������ڵ���::HI_SVR_PLAYER_Create�ӿں���ã����Ž��ȡ��Ѳ���ʱ��ֻ���ڵ���::HI_SVR_PLAYER_Play�ӿں���Ч
::HI_SVR_PLAYER_SetMediaδ����ǰ��������øýӿ�CNend

\param[in] hPlayer player handle. CNcomment:���������CNend
\param[out] pstruInfo player information. CNcomment:��������ϢCNend

\retval ::HI_SUCCESS The player information is obtained successfully. CNcomment:��������Ϣ��ȡ�ɹ�CNend
\retval ::HI_FAILURE The player information fails to be obtained. CNcomment:��������Ϣ��ȡʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_GetPlayerInfo(HI_HANDLE hPlayer, HI_SVR_PLAYER_INFO_S *pstruInfo);

/**
\brief Register a media file DEMUX or subtitle file DEMUX. The DEMUX must be a DLL and is implemented based on the specifications of the hi_svr_format.h header file.  CNcomment:ע��ý���ļ���������Ļ�������������������Ƕ�̬�⣬�Ұ�hi_svr_format.hͷ�ļ��淶ʵ��CNend
\attention \n
This interface must be called after the ::HI_SVR_PLAYER_Init interface is called.
DLLs such as libformat.so are stored in /usr/lib. The interface is called as follows:
HI_SVR_PLAYER_RegisterDynamic(HI_SVR_PLAYER_DLL_PARSER, "libformat.so")
CNcomment:�ýӿڱ����ڵ���::HI_SVR_PLAYER_Init�ӿں����
��̬�����/usr/libĿ¼�£���libformat.so������øýӿڷ�ʽΪ:
HI_SVR_PLAYER_RegisterDynamic(HI_SVR_PLAYER_DLL_PARSER, "libformat.so") CNend

\param[in] eDllType DDL type. CNcomment:��̬������CNend
\param[in] dllName DDL name. CNcomment:��̬������CNend

\retval ::HI_SUCCESS The DDL is registered successfully.  CNcomment:��̬��ע��ɹ�CNend
\retval ::HI_FAILURE The DDL fails to be registered. CNcomment:ע��ʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_RegisterDynamic(HI_SVR_PLAYER_DLLTYPE_E eDllType, const HI_CHAR *dllName);

/**
\brief Register a file and subtitle DEMUX object. This interface is not supported now.CNcomment:ʹ�ö���ķ�ʽע���ļ����������ýӿڵ�ǰ��֧��CNend
\attention \n
This interface must be called after the ::HI_SVR_PLAYER_Init interface is called.
CNcomment:�ýӿڱ����ڵ���::HI_SVR_PLAYER_Init�ӿں����CNend

\param[in] pEntry DEMUX object.

\retval ::HI_SUCCESS The object is registered successfully.  CNcomment:ע��ɹ�CNend
\retval ::HI_FAILURE The object fails to be registered. CNcomment:ע��ʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_RegisterEntry(HI_FORMAT_S * pEntry);


/**
\brief Remove all DDLs for parsing files and subtitles. CNcomment:ж�������ļ���������Ļ������̬��CNend
\attention \n
This interface must be called after the ::HI_SVR_PLAYER_Init interface is called.
CNcomment:�ýӿڱ����ڵ���::HI_SVR_PLAYER_Init�ӿں����CNend

\param[in] eDllType DDL type. CNcomment:��̬������CNend
\param[in] dllName DDL name. It is reserved. CNcomment:��̬�����ƣ�����������ݲ�ʹ��CNend

\retval ::HI_SUCCESS The DDLs are removed successfully. CNcomment:ж�سɹ�CNend
\retval ::HI_FAILURE The DDLs fail to be removed. CNcomment:ж��ʧ��CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_UnRegisterDynamic(HI_SVR_PLAYER_DLLTYPE_E eDllType, const HI_CHAR *dllName);

/**
\brief Enable or disable the player debug. CNcomment:��/�ر�player dbg��ϢCNend
\attention \n
This interface can be called in any state.
CNcomment:�ýӿڿ������κ�״̬����CNend

\param[in] bEnable enable value. HI_TRUE: enables the player log. HI_FALSE: disables the player log. CNcomment:ʹ��ֵ��HI_TRUE:��player��־��Ϣ��HI_FALSE:�ر���־��ϢCNend

\retval :: None.

\see \n
None.
*/
HI_VOID HI_SVR_PLAYER_EnableDbg(HI_BOOL bEnable);

/**
\brief Obtain the player version. CNcomment:��ȡ�������汾��CNend
\attention \n
This interface can be called in any state.
CNcomment:�ýӿڿ������κ�״̬����CNend

\param[out] pstVersion player version. CNcomment:�������汾��CNend

\retval :: None.

\see \n
None.
*/
HI_VOID HI_SVR_PLAYER_GetVersion(HI_FORMAT_LIB_VERSION_S *pstVersion);

/** The parameters need to be specified when a media file is opened by user calling HI_SVR_FORMAT_Open() directly*/
/** CNcomment:�û�ֱ�ӵ���HI_SVR_FORMAT_Open()��ý���ļ�ʱ��Ҫָ���Ĳ���*/
typedef struct hiSVR_FORMAT_PARAMETER_S
{
    HI_FORMAT_BUFFER_CONFIG_S stBufConfig;     /**< buffer config, only used for network play */
    HI_S64          s64BufMaxSize;             /**< set buffer max peak(bytes), only used for network play, 0 is invalid */
    HI_CHAR         *pCookie;                  /**< http cookie info */
    HI_CHAR         *pHeaders;                 /**< http headers info */
    HI_FORMAT_HLS_START_MODE_E eHlsStartMode;  /**< hls start mode, only used for m3u8 play, default is HI_FORMAT_HLS_MODE_NORMAL */
    HI_HANDLE       hDRMClient;                /**< DRM Client handle created by user, only used for playing the drm file, 0 is invalid */
    HI_S32          s32DstCodeType;            /**< Dest codetype, value is::HI_CHARSET_CODETYPE_E, 0: mean not set */
    HI_VOID         *pArgCharsetMgr;           /**< Charset Mgr Func, value is::HI_CHARSET_FUN_S, 0 is invalid */
    HI_U32          u32Userdata;               /**< value is HI_SVR_PLAYER_MEDIA_S::u32UserData */
    HI_U8           *pUserAgent;               /**< User agent of connecting server */
    HI_U32          u32LogLevel;               /**< log level set by user, value is::HI_FORMAT_LOG_LEVEL_E */
    HI_U8           *pReferer;                 /**< http referer setting */
    HI_U32          u32NotSupportByteRange;  /**< if not support byte range in http protocol */
    HI_S32          s32HlsLiveStartNum;        /**< hls live stream start play number*/
} HI_SVR_FORMAT_PARAMETER_S;

/**
\brief get default open param.This interface is not supported now.
\attention \n
non
\param[out] pstDefaultParam default param

\retval ::NONE

\see \n
non
*/
HI_VOID HI_SVR_FORMAT_GetDefaultParam(HI_SVR_FORMAT_PARAMETER_S *pstDefaultParam);

/**
\brief open the media file.This interface is not supported now.
\attention \n
non
\param[in] filename url of the media file
\param[in] pstFormatParam param of open media file
\param[out] phandle handle of the open file

\retval ::HI_SUCCESS open success
\retval ::HI_FAILURE open fail, not support or param invalid

\see \n
non
*/
HI_S32 HI_SVR_FORMAT_Open(const HI_CHAR *filename, HI_HANDLE *phandle, const HI_SVR_FORMAT_PARAMETER_S *pstFormatParam);

/**
\brief find stream.This interface is not supported now.
\attention \n
non
\param[in] handle handle of the open file
\param pArg  no use

\retval ::HI_SUCCESS find stream successfully
\retval ::HI_FAILURE param invalid or can't play the stream file

\see \n
non
*/
HI_S32 HI_SVR_FORMAT_FindStream(HI_HANDLE handle, HI_VOID *pArg);

/**
\brief get media file info.This interface is not supported now.
\attention \n
non
\param[in] handle handle of the open file
\param[out] pstFmtInfo media file info

\retval ::HI_SUCCESS get file info success
\retval ::HI_FAILURE param invalid

\see \n
non
*/
HI_S32 HI_SVR_FORMAT_GetInfo(HI_HANDLE handle, HI_FORMAT_FILE_INFO_S *pstFmtInfo);

/**
\brief read a frame, free the frame call SVR_FORMAT_Free function.This interface is not supported now.
\attention \n
non
\param[in] handle handle of the open file
\param[in/out] pstFmtFrame frame info

\retval ::HI_SUCCESS read a frame success
\retval ::HI_FAILURE param invalid
\retval ::HI_FORMAT_ERRO_ENDOFFILE end of file

\see \n
non
*/
HI_S32 HI_SVR_FORMAT_Read(HI_HANDLE handle, HI_FORMAT_FRAME_S *pstFmtFrame);

/**
\brief free the frame.This interface is not supported now.
\attention \n
non
\param[in] handle handle of the open file
\param[in] pstFmtFrame frame info

\retval ::HI_SUCCESS success
\retval ::HI_FAILURE param invalid

\see \n
non
*/
HI_S32 HI_SVR_FORMAT_Free(HI_HANDLE handle, HI_FORMAT_FRAME_S *pstFmtFrame);

/**
\brief Extended Interface.This interface is not supported now.
\attention \n
non
\param [in] fmtHandle handle of the format
\param [in] u32InvokeId  command of the operation, value is ::HI_FORMAT_INVOKE_ID_E
\param [in] pArg parameter of the command

\retval ::HI_SUCCESS success
\retval ::HI_FAILURE param is invalid

\see \n
None.
*/
HI_S32 HI_SVR_FORMAT_Invoke(HI_HANDLE fmtHandle, HI_U32 u32InvokeId, HI_VOID *pArg);

/**
\brief seek file, if s64Pts is 0, seek to head of the file.This interface is not supported now.
\attention \n
non
\param[in] handle handle of the open file
\param[in] s32StreamIndex streamd index
\param[in] s64Pts seek time(ms)
\param[in] eFlag flag of seeking

\retval ::HI_SUCCESS seek success
\retval ::HI_FAILURE param invalid or not support seek

\see \n
non
*/
HI_S32 HI_SVR_FORMAT_Seek(HI_HANDLE handle, HI_S32 s32StreamIndex, HI_S64 s64Pts, HI_FORMAT_SEEK_FLAG_E eFlag);

/**
\brief seek file, offset is position of the file pointer.This interface is not supported now.
\attention \n
non
\param[in] handle handle of the open file
\param[in] s64Offsets offset in bytes
\param[in] s32Whence seek flag

\retval ::NON-HI_FAILURE the position of the file pointer
\retval ::HI_FAILURE not support

\see \n
non
*/
HI_S64 HI_SVR_FORMAT_SeekPos(HI_HANDLE handle, HI_S64 s64Offsets, HI_S32 s32Whence);

/**
\brief close media file, destroy handle.This interface is not supported now.
\attention \n
non
\param[in] handle handle of the open file

\retval ::HI_SUCCESS success
\retval ::HI_FAILURE param invalid

\see \n
non
*/
HI_S32 HI_SVR_FORMAT_Close(HI_HANDLE handle);

/**
\brief get MsgEvent.This interface is not supported now.
\attention \n
non
\param[in] handle handle of the open file
\param[out] pstMsg information of the message

\retval ::HI_SUCCESS success
\retval ::HI_FAILURE param invalid

\see \n
non
*/
HI_S32 HI_SVR_FORMAT_GetMsg(HI_HANDLE handle, HI_FORMAT_MSG_S *pstMsg);

/**
\brief Create a player for meta retriever. CNcomment:����һ������������ý��Ԫ���ݻ�ȡCNend
\attention \n
This interface must be called after initialization. Only one player can be created. \n
CNcomment:�ýӿڱ����ڳ�ʼ������ã�֧����ഴ��һ��������\n  CNend
\param[in] pstruParam player initialization attribute. CNcomment:��������ʼ������CNend
\param[in] pstruMedia media file information. Only absolute path is supported.\n
\param[out] phPlayer handle of the created player. CNcomment:�����Ĳ��������CNend

\retval ::HI_SUCCESS A player is created successfully and the player handle is valid. CNcomment:�����ɹ��������������ЧCNend
\retval ::HI_FAILURE A player fails to be created. The parameters are invalid or resources are insufficient. CNcomment:����ʧ�ܣ������Ƿ�����Դ����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_CreateMetaRetriever(const HI_SVR_PLAYER_PARAM_S *pstruParam,
                                            HI_SVR_PLAYER_MEDIA_S *pstruMedia, HI_HANDLE *phPlayer);

/**
\brief Destroy a player instance for meta retriever. CNcomment:����һ������ý��Ԫ���ݻ�ȡ�Ĳ�����ʵ��CNend
\attention \n
The HI_S32 HI_SVR_PLAYER_Destroy interface is called to destroy the player resource after the ::HI_SVR_PLAYER_CreateMetaRetriever interface is called to create a player.
CNcomment:����::HI_SVR_PLAYER_CreateMetaRetriever�����������󣬵��øýӿ����ٲ�������ԴCNend
\param[in] hPlayer player handle. CNcomment:���������CNend

\retval ::HI_SUCCESS The player is released successfully. CNcomment:�������ͷųɹ�CNend
\retval ::HI_FAILURE The operation is invalid. CNcomment:�Ƿ�����CNend

\see \n
None.
*/
HI_S32 HI_SVR_PLAYER_DestroyMetaRetriever(HI_HANDLE hPlayer);


/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_SVR_PLAYER_H__ */
