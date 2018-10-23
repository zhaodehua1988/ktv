/******************************************************************************

  Copyright (C), 2001-2014, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_unf_edid.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013-8-28
  Description   :
  History       :
  1.Date        : 2013-8-28
    Author      : wx 178178
    Modification: Created file

*******************************************************************************/
#ifndef __HI_UNF_EDID_H__
#define __HI_UNF_EDID_H__

#include "hi_unf_common.h"
#include "hi_unf_audio.h"
#include "hi_unf_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*************************** Structure Definition ****************************/
/** \addtogroup      EDID */
/** @{*/  /** <!-- [EDID] */

/**EDID Audio type enum*//**CNcomment:HDMI ��Ƶ���� ö�� */
typedef enum hiUNF_EDID_AUDIO_FORMAT_CODE_E
{
    HI_UNF_EDID_AUDIO_FORMAT_CODE_RESERVED  = 0x00,  /**<Audio coding type, refer stream,default type*//**<CNcomment:��Ƶ�������ͣ��ο���������ȱʡ��ʽ */    
    HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM,               /**<Audio coding PCM type*//**<CNcomment:��Ƶ����PCM��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_AC3,               /**<Audio coding AC3 type*//**<CNcomment:��Ƶ����AC3��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_MPEG1,             /**<Audio coding MPEG1 type*//**<CNcomment:��Ƶ����MPEG1��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_MP3,               /**<Audio coding MP3 type*//**<CNcomment:��Ƶ����MP3��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_MPEG2,             /**<Audio coding MPEG2 type*//**<CNcomment:��Ƶ����MPEG2��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_AAC,               /**<Audio coding AAC type*//**<CNcomment:��Ƶ����AAC��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_DTS,               /**<Audio coding DTS type*//**<CNcomment:��Ƶ����DTS��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_ATRAC,             /**<Audio coding ATRAC type*//**<CNcomment:��Ƶ����ATRAC��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_ONE_BIT,           /**<Audio coding ONE BIT AUDIO type*//**<CNcomment:��Ƶ����ONE_BIT��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_DDP,               /**<Audio coding DDPLUS type*//**<CNcomment:��Ƶ����DDPLUS��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_DTS_HD,            /**<Audio coding DTS HD type*//**<CNcomment:��Ƶ����DTS HD��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_MAT,               /**<Audio coding MAT type*//**<CNcomment:��Ƶ����MAT��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_DST,               /**<Audio coding DST type*//**<CNcomment:��Ƶ����DST��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_WMA_PRO,           /**<Audio coding WMA PRO type*//**<CNcomment:��Ƶ����WMA PRO��ʽ */
    HI_UNF_EDID_AUDIO_FORMAT_CODE_BUTT,
}HI_UNF_EDID_AUDIO_FORMAT_CODE_E;

/**the max audio smprate count*//**CNcomment:�����Ƶ�����ʸ��� */
#define MAX_SAMPE_RATE_NUM 8

/**the max audio bit depth count*//**CNcomment:�����Ƶ������ȸ��� */
#define MAX_BIT_DEPTH_NUM  6

/**EDID Audio Info struct*//**CNcomment:EDID ��Ƶ��Ϣ�ṹ�� */
typedef struct hiUNF_EDID_AUDIO_INFO_S
{
    HI_UNF_EDID_AUDIO_FORMAT_CODE_E enAudFmtCode; /**<Audio coding type*//**<CNcomment:��Ƶ�������� */ 
    HI_UNF_SAMPLE_RATE_E            enSupportSampleRate[MAX_SAMPE_RATE_NUM]; /**<Audio sample rate*//**<CNcomment:��Ƶ������ */ 
    HI_U32                          u32SupportSampleRateNum; /**<Audio sample rate num*//**<CNcomment:��Ƶ���������� */ 
    HI_U8                           u8AudChannel;      /**<Aud Channel of the coding type*//**<CNcomment:�������Ͷ�Ӧ����Ƶͨ������ */ 
    
    HI_UNF_BIT_DEPTH_E              bSupportBitDepth[MAX_BIT_DEPTH_NUM];/*sample bit depth,when audio format code is HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM*//**<CNcomment:PCM��������չ��ϢΪ������� */
    HI_U32                          u32SupportBitDepthNum; /*sample bit depth Num,when audio format code is HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM*//**<CNcomment:PCM��������չ��ϢΪ������ȸ��� */
    HI_U32                          u32MaxBitRate;  /**enter max bit rate,when audio format code is HI_UNF_EDID_AUDIO_FORMAT_CODE_AC3 - HI_UNF_EDID_AUDIO_FORMAT_CODE_ATRAC**//**<CNcomment:AC3-ATRACʱ ��չ��Ϣ����Ϊ����������*/             
}HI_UNF_EDID_AUDIO_INFO_S;

/**EDID Audio speaker enum*//**CNcomment:EDID ��Ƶ����������ö�� */
typedef enum hiUNF_EDID_AUDIO_SPEAKER_E
{
    HI_UNF_EDID_AUDIO_SPEAKER_FL_FR   ,
    HI_UNF_EDID_AUDIO_SPEAKER_LFE     , 
    HI_UNF_EDID_AUDIO_SPEAKER_FC      , 
    HI_UNF_EDID_AUDIO_SPEAKER_RL_RR   ,
    HI_UNF_EDID_AUDIO_SPEAKER_RC      ,
    HI_UNF_EDID_AUDIO_SPEAKER_FLC_FRC , 
    HI_UNF_EDID_AUDIO_SPEAKER_RLC_RRC ,
    HI_UNF_EDID_AUDIO_SPEAKER_FLW_FRW ,
    HI_UNF_EDID_AUDIO_SPEAKER_FLH_FRH ,
    HI_UNF_EDID_AUDIO_SPEAKER_TC      ,
    HI_UNF_EDID_AUDIO_SPEAKER_FCH     ,
    HI_UNF_EDID_AUDIO_SPEAKER_BUTT    ,
} HI_UNF_EDID_AUDIO_SPEAKER_E;

/**EDID MANUFACTURE Info struct*//**CNcomment:EDID��������Ϣ */
typedef struct hiUNF_EDID_MANUFACTURE_INFO_E
{
    HI_U8      u8MfrsName[4];            /**<Manufacture name*//**<CNcomment:�豸���̱�ʶ */
    HI_U32     u32ProductCode;           /**<Product code*//**<CNcomment:�豸ID */
    HI_U32     u32SerialNumber;          /**<Serial numeber of Manufacture*//**<CNcomment:�豸���к� */
    HI_U32     u32Week;                  /**<the week of manufacture*//**<CNcomment:�豸��������(��) */
    HI_U32     u32Year;                  /**<the year of manufacture*//**<CNcomment:�豸��������(��) */
    HI_U8      u8pSinkName[14];          /**<Sink Name*//**<CNcomment:�豸����*/
}HI_UNF_EDID_MANUFACTURE_INFO_S;

/**EDID Colorimety Info struct*//**CNcomment:EDID ɫ����Ϣ�ṹ�� */
typedef struct hiUNF_EDID_COLORIMETRY_S
{
    HI_BOOL    bxvYCC601      ;          /**<The sink is support xcYCC601 or not flag*//**<CNcomment:�Ƿ�֧��xvYCC601��ɫ��ʽ */
    HI_BOOL    bxvYCC709      ;          /**<The sink is support xvYCC709 or not flag*//**<CNcomment:�Ƿ�֧��xvYCC709��ɫ��ʽ */
    HI_BOOL    bsYCC601       ;          /**<The sink is support sYCC601 or not flag*//**<CNcomment:�Ƿ�֧��sYCC601��ɫ��ʽ */
    HI_BOOL    bAdobleYCC601  ;          /**<The sink is support AdobleYCC601 or not flag*//**<CNcomment:�Ƿ�֧��AdobleYCC601��ɫ��ʽ */ 
    HI_BOOL    bAdobleRGB     ;          /**<The sink is support AdobleRGB or not flag*//**<CNcomment:�Ƿ�֧��AdobleRGB��ɫ��ʽ */  
    HI_BOOL    bBT2020cYCC    ;          /**<The sink is support BT2020cYCC or not flag*//**<CNcomment:�Ƿ�֧��BT2020cYCC��ɫ��ʽ */  
    HI_BOOL    bBT2020YCC     ;          /**<The sink is support BT2020YCC or not flag*//**<CNcomment:�Ƿ�֧��BT2020YCC��ɫ��ʽ */  
    HI_BOOL    bBT2020RGB     ;          /**<The sink is support BT2020RGB or not flag*//**<CNcomment:�Ƿ�֧��BT2020RGB��ɫ��ʽ */  
} HI_UNF_EDID_COLORIMETRY_S;

/**EDID color space Info struct*//**CNcomment:EDID ɫ�ʿռ���Ϣ�ṹ�� */
typedef struct hiUNF_EDID_COLOR_SPACE_S
{
    HI_BOOL    bRGB444        ;          /**<The sink is support RGB444 or not flag*//**<CNcomment:< �Ƿ�֧��RGB444��ʾ */
    HI_BOOL    bYCbCr422      ;          /**<The sink is support YCbCr422 or not flag*//**<CNcomment:< �Ƿ�֧��YCbCr422��ʾ */ 
    HI_BOOL    bYCbCr444      ;          /**<The sink is support YCbCr444 or not flag*//**<CNcomment:< �Ƿ�֧��YCbCr444��ʾ */
} HI_UNF_EDID_COLOR_SPACE_S;

/**EDID cec address Info struct*//**CNcomment:EDID cec��ַ��Ϣ�ṹ�� */
typedef struct hiUNF_EDID_CEC_ADDRESS_S
{
    HI_BOOL    bPhyAddrValid   ;          /**<the flag of phyiscs address is valid or not*//**<CNcomment:CEC�����ַ�Ƿ���Ч��־ */  
    HI_U8      u8PhyAddrA      ;          /**<phyiscs address A of CEC*//**<CNcomment:CEC�����ַA */ 
    HI_U8      u8PhyAddrB      ;          /**<phyiscs address B of CEC*//**<CNcomment:CEC�����ַB */ 
    HI_U8      u8PhyAddrC      ;          /**<phyiscs address C of CEC*//**<CNcomment:CEC�����ַC */ 
    HI_U8      u8PhyAddrD      ;          /**<phyiscs address D of CEC*//**<CNcomment:CEC�����ַD */  
} HI_UNF_EDID_CEC_ADDRESS_S;

/**EDID deep color Info struct*//**CNcomment:EDID ��ɫ��Ϣ�ṹ�� */
typedef struct hiUNF_EDID_DEEP_COLOR_S
{
    HI_BOOL    bDeepColorY444   ;         /**<the Deep Color support YCBCR444 or not*//**<CNcomment:�Ƿ�֧�� YCBCR 4:4:4  Deep Color ģʽ */
    HI_BOOL    bDeepColor30Bit  ;         /**<the Deep Color support 30 bit  or not *//**<CNcomment:�Ƿ�֧��Deep Color 30bit ģʽ */
    HI_BOOL    bDeepColor36Bit  ;         /**<the Deep Color support 36 bit  or not *//**<CNcomment:�Ƿ�֧��Deep Color 36bit ģʽ */
    HI_BOOL    bDeepColor48Bit  ;         /**<the Deep Color support 48 bit  or not *//**<CNcomment:�Ƿ�֧��Deep Color 48bit ģʽ */
} HI_UNF_EDID_DEEP_COLOR_S;


/**HDMI 3d enum type*/
/**CNcomment: HDMI 3d ö������ */
typedef enum hiUNF_EDID_3D_TYPE_E
{
    HI_UNF_EDID_3D_FRAME_PACKETING                 = 0x00, /**<3d type:Frame Packing*//**<CNcomment:3d ģʽ:֡��װ*/
    HI_UNF_EDID_3D_FIELD_ALTERNATIVE               = 0x01, /**<3d type:Field alternative*//**<CNcomment:3d ģʽ:������*/
    HI_UNF_EDID_3D_LINE_ALTERNATIVE                = 0x02, /**<3d type:Line alternative*//**<CNcomment:3d ģʽ:�н���*/
    HI_UNF_EDID_3D_SIDE_BY_SIDE_FULL               = 0x03, /**<3d type:Side by side full*//**<CNcomment:3d ģʽ:����ʽ ����ȫ��*/
    HI_UNF_EDID_3D_L_DEPTH                         = 0x04, /**<3d type:L+depth*//**<CNcomment:3d ģʽ:L+DEPTH*/
    HI_UNF_EDID_3D_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH = 0x05, /**<3d type:L+depth+Graphics+Graphics-depth*//**<CNcomment:3d ģʽ:L+depth+Graphics+Graphics-depth*/
    HI_UNF_EDID_3D_TOP_AND_BOTTOM                  = 0x06, /**<3d type:Top and Bottom*//**<CNcomment:3d ģʽ:����ģʽ*/
    HI_UNF_EDID_3D_SIDE_BY_SIDE_HALF               = 0x08, /**<3d type:Side by side half*//**<CNcomment:3d ģʽ:����ʽ ���Ұ��*/
    HI_UNF_EDID_3D_BUTT,                          
}HI_UNF_EDID_3D_TYPE_E;

/**3d Info struct*/
/**CNcomment: 3d ��Ϣ�ṹ�� */
typedef struct hiUNF_EDID_3D_INFO_S
{
    HI_BOOL    bSupport3D  ;                         /**<flag of 3d*//**<CNcomment:�Ƿ�֧��3d*/
    HI_BOOL    bSupport3DType[HI_UNF_EDID_3D_BUTT] ; /**<supported 3d type*//**<CNcomment:֧�ֵ�3d����*/
} HI_UNF_EDID_3D_INFO_S;

/**EDID detailed Timing Info struct*/
/**CNcomment: EDID ��ϸʱ��*/
typedef struct hiUNF_EDID_TIMING_S
{
    HI_U32                        u32VFB;          /**<vertical front blank*//**<CNcomment:��ֱǰ����*/
    HI_U32                        u32VBB;          /**<vertical back blank*//**<CNcomment:��ֱ������*/
    HI_U32                        u32VACT;         /**<vertical active area*//**<CNcomment:��ֱ��Ч��*/
    HI_U32                        u32HFB;          /**<horizonal front blank*//**<CNcomment:ˮƽǰ����*/
    HI_U32                        u32HBB;          /**<horizonal back blank*//**<CNcomment:ˮƽ������*/
    HI_U32                        u32HACT;         /**<horizonal active area*/ /**<CNcomment:ˮƽ��Ч��*/
    HI_U32                        u32VPW;          /**<vertical sync pluse width*//**<CNcomment:��ֱ������*/
    HI_U32                        u32HPW;          /**<horizonal sync pluse width*/ /**<CNcomment:ˮƽ������*/
    HI_BOOL                       bIDV;            /**< flag of data valid signal is needed flip*//**<CNcomment:��Ч�����ź��Ƿ�ת*/
    HI_BOOL                       bIHS;            /**<flag of horizonal sync pluse is needed flip*//**<CNcomment:ˮƽͬ�������ź��Ƿ�ת*/
    HI_BOOL                       bIVS;            /**<flag of vertical sync pluse is needed flip*//**<CNcomment:��ֱͬ�������ź��Ƿ�ת*/
    HI_U32                        u32ImageWidth;   /**<image width *//**<CNcomment:ͼ���*/
    HI_U32                        u32ImageHeight;  /**<image height *//**<CNcomment:ͼ��� */
    HI_U32                        u32AspectRatioW; /**<aspect ratio width *//**<CNcomment:��߱� */
    HI_U32                        u32AspectRatioH; /**<aspect ratio height *//**<CNcomment:��߱� */
    HI_BOOL                       bInterlace;      /**<flag of interlace *//**<CNcomment:����м���� */
    HI_S32                        u32PixelClk;     /**<pixelc clk for this timing *//**<CNcomment:��ǰ��ʽ��Ӧ��ʱ�� */
} HI_UNF_EDID_TIMING_S;

/**the max audio capability count*//**CNcomment:�����Ƶ���������� */
#define HI_UNF_EDID_MAX_AUDIO_CAP_COUNT  16

/**HDMI sink capability of interface*/
/**CNcomment: HDMI sink �ӿ������� */
typedef struct hiUNF_EDID_BASE_INFO_S
{
    HI_BOOL                         bSupportHdmi;           /**<The Device suppot HDMI or not,the device is DVI when nonsupport HDMI*//**<CNcomment:�豸�Ƿ�֧��HDMI�������֧�֣���ΪDVI�豸.*/
    HI_UNF_ENC_FMT_E                enNativeFormat;         /**<The sink native video format*//**<CNcomment:��ʾ�豸����ֱ��� */
    HI_BOOL                         bSupportFormat[HI_UNF_ENC_FMT_BUTT]; /**<video capability,HI_TRUE:support the video display format;HI_FALSE:nonsupport the video display foramt*//**<CNcomment:��Ƶ������,HI_TRUE��ʾ֧��������ʾ��ʽ��HI_FALSE��ʾ��֧�� */
    HI_UNF_EDID_3D_INFO_S           st3DInfo;               /**<3d Info*//**<CNcomment:3d ������ */
    HI_UNF_EDID_DEEP_COLOR_S        stDeepColor;            /**<deep color Info*//**<CNcomment:3d ������ */
    HI_UNF_EDID_COLORIMETRY_S       stColorMetry;           /**<colorimetry Info*//**<CNcomment:3d ������ */
    HI_UNF_EDID_COLOR_SPACE_S       stColorSpace;           /**<color space Info*//**<CNcomment:3d ������ */            
    
    HI_UNF_EDID_AUDIO_INFO_S        stAudioInfo[HI_UNF_EDID_MAX_AUDIO_CAP_COUNT];   /**<audio Info*//**<CNcomment:3d ������ */
    HI_U32                          u32AudioInfoNum;                                /**<num of audio Info*//**<CNcomment:3d ������ */
    HI_BOOL                         bSupportAudioSpeaker[HI_UNF_EDID_AUDIO_SPEAKER_BUTT];/**<speaker Info*//**<CNcomment:3d ������ */
    
    HI_U8                           u8ExtBlockNum;          /**<edid extend block num Info*//**<CNcomment:edid��չ����� */
    
    HI_U8                           u8Version;              /**<the version of manufacture*//**<CNcomment:�豸�汾�� */
    HI_U8                           u8Revision;             /**<the revision of manufacture*//**<CNcomment:�豸�Ӱ汾�� */
    HI_UNF_EDID_MANUFACTURE_INFO_S  stMfrsInfo;
 
    HI_UNF_EDID_CEC_ADDRESS_S       stCECAddr;              /**<cec address Info*//**<CNcomment:cec��ַ��Ϣ */
    HI_BOOL                         bSupportDVIDual;        /**<the DVI support dual-link or not *//**<CNcomment:�Ƿ�֧�� DVI dual-link ���� */
    HI_BOOL                         bSupportsAI;            /**<support AI or not *//**<CNcomment:�Ƿ�֧�� Supports_AI ģʽ */
    HI_UNF_EDID_TIMING_S            stPerferTiming;         /**<first detailed timing Info*//**<CNcomment:VESA�����ϸ��ʽ��Ϣ */          

} HI_UNF_EDID_BASE_INFO_S;


/**enum define about Video port*/
/**CNcomment:������Ƶ�ӿ�*/
typedef enum hiUNF_EDID_VIDEO_PORT_E
{
    HI_UNF_EDID_VIDEO_PORT_VGA   = 0x00,/**< VGA port *//**<CNcomment: VGA�ӿ� */
    HI_UNF_EDID_VIDEO_PORT_DVI,/**< DVI port *//**<CNcomment: DVI�ӿ� */
    HI_UNF_EDID_VIDEO_PORT_HDMI,/**< HDMI port *//**<CNcomment: HDMI �ӿ� */
    HI_UNF_EDID_VIDEO_PORT_BUTT
} HI_UNF_EDID_VIDEO_PORT_E;

/**enum define about timing protocol type*/
/**CNcomment:����ʱ���Э������*/
typedef enum hiUNF_EDID_TIMING_TYPE_E
{
    HI_UNF_EDID_TIMING_TYPE_DMT= 0x00,/**< DMT  protocol Timing *//**<CNcomment: DMTЭ��ʱ�� */
    HI_UNF_EDID_TIMING_TYPE_861,/**< 861D  protocol Timing *//**<CNcomment: 861DЭ��ʱ�� */
    HI_UNF_EDID_TIMING_TYPE_CVT,/**< CVT  protocol Timing *//**<CNcomment: CVTЭ��ʱ�� */
    HI_UNF_EDID_TIMING_TYPE_CVT_RB,/**< CVT_RB  protocol Timing *//**<CNcomment: CVT_RBЭ��ʱ�� */
    HI_UNF_EDID_TIMING_TYPE_GTF,/**< GTF  protocol Timing *//**<CNcomment: GTFЭ��ʱ�� */
    HI_UNF_EDID_TIMING_TYPE_BUTT
} HI_UNF_EDID_TIMING_TYPE_E;

/**enum define about timing type*/
/**CNcomment:����ʱ�������*/
typedef enum hiUNF_EDID_TIMING_ATTR_E
{
    HI_UNF_EDID_TIMING_ATTR_NONE = 0x00,/**< None *//**<CNcomment: �� */
    HI_UNF_EDID_TIMING_ATTR_PREFERRED_TIMING,/**< None *//**<CNcomment: ���ʱ��*/
    HI_UNF_EDID_TIMING_ATTR_PREFERRED_VERTICAL_FREQ,/**< None *//**<CNcomment: ���ˢ����*/
    HI_UNF_EDID_TIMING_ATTR_BUTT
} HI_UNF_EDID_TIMING_ATTR_E;

/**Detailed Timing Definitions Sync Signal Definitions*/
/**CNcomment:��ϸʱ���ж���ͬ������*/
typedef enum hiUNF_EDID_SYNC_TYPE_E
{
    /*Analog Sync Signal Definitions*/
    HI_UNF_EDID_SYNC_ACS_WS_GREEN    = 0x00,     /**<Analog Composite Sync - Without Serrations - Sync On Green Signal only*//**<CNcomment: ģ�⸴��ͬ��ȥ��ֱͬ��ʱ�ڵ�ˮƽ��ݣ���ɫͨ������ͬ��*/
    HI_UNF_EDID_SYNC_ACS_WS_ALL,                 /**<Analog Composite Sync - Without Serrations - Sync On all three (RGB) video signals*//**<CNcomment: ģ�⸴��ͬ��ȥ��ֱͬ��ʱ�ڵ�ˮƽ��ݣ�����ͨ������ͬ��*/
    HI_UNF_EDID_SYNC_ACS_DS_GREEN,               /**<Analog Composite Sync - With Serrations (H-sync during V-sync); - Sync On Green Signal only*//**<CNcomment: ģ�⸴��ͬ�����ִ�ֱͬ��ʱ�ڵ�ˮƽ��ݣ���ɫͨ������ͬ��*/
    HI_UNF_EDID_SYNC_ACS_DS_ALL,                 /**<Analog Composite Sync - With Serrations (H-sync during V-sync); - Sync On all three (RGB) video signals*//**<CNcomment: ģ�⸴��ͬ�����ִ�ֱͬ��ʱ�ڵ�ˮƽ��ݣ�����ͨ������ͬ��*/
    HI_UNF_EDID_SYNC_BACS_WS_GREEN,              /**<Bipolar Analog Composite Sync - Without Serrations; - Sync On Green Signal only*//**<CNcomment: ģ�⸴��ͬ��ȥ��ֱͬ��ʱ�ڵ�ˮƽ��ݣ�����ͨ������ͬ��*/
    HI_UNF_EDID_SYNC_BACS_WS_ALL,                /**<Bipolar Analog Composite Sync - Without Serrations; - Sync On all three (RGB) video signals*//**<CNcomment: ģ�⸴��ͬ��ȥ��ֱͬ��ʱ�ڵ�ˮƽ��ݣ�����ͨ������ͬ��*/
    HI_UNF_EDID_SYNC_BACS_DS_GREEN,              /**<Bipolar Analog Composite Sync - With Serrations (H-sync during V-sync); - Sync On Green Signal only*//**<CNcomment:  ����ģ�⸴��ͬ�����ִ�ֱͬ��ʱ�ڵ�ˮƽ��ݣ���ɫͨ������ͬ��*/
    HI_UNF_EDID_SYNC_BACS_DS_ALL,                /**<Bipolar Analog Composite Sync - With Serrations (H-sync during V-sync); - Sync On all three (RGB) video signals*//**<CNcomment:  ����ģ�⸴��ͬ�����ִ�ֱͬ��ʱ�ڵ�ˮƽ��ݣ���ɫͨ������ͬ��*/
    /*Digital Sync Signal Definitions*/
    HI_UNF_EDID_SYNC_DCS_WS,                     /**<Digital Composite Sync - Without Serrations*//**<CNcomment: ���ָ���ͬ����ȥ��ֱͬ���ڼ���ͬ������ź�*/
    HI_UNF_EDID_SYNC_DCS_DS,                     /**<Digital Composite Sync - With Serrations (H-sync during V-sync)*//**<CNcomment: ���ָ���ͬ�������ִ�ֱͬ���ڼ���ͬ������ź�*/
    HI_UNF_EDID_SYNC_DSS_VN_HN,                  /**<Digital Separate Sync Vsync(-) Hsync(-)*//**<CNcomment: ���ָ���ͬ����ˮƽ�ʹ�ֱͬ������(-)(-)*/
    HI_UNF_EDID_SYNC_DSS_VN_HP,                  /**<Digital Separate Sync Vsync(-) Hsync(+)*//**<CNcomment: ���ָ���ͬ����ˮƽ�ʹ�ֱͬ������(-)(+)*/
    HI_UNF_EDID_SYNC_DSS_VP_HN,                  /**<Digital Separate Sync Vsync(+) Hsync(-)*//**<CNcomment: ���ָ���ͬ����ˮƽ�ʹ�ֱͬ������(+)(-)*/
    HI_UNF_EDID_SYNC_DSS_VP_HP,                 /**<Digital Separate Sync Vsync(+) Hsync(+)*//**<CNcomment: ���ָ���ͬ����ˮƽ�ʹ�ֱͬ������(+)(+)*/
    HI_UNF_EDID_SYNC_BUTT
} HI_UNF_EDID_SYNC_TYPE_E;


/**Simple Timing Definitions*/
/**CNcomment:�����Ҫʱ��*/
typedef struct hiUNF_EDID_SIMPLE_TIMING_S
{
    HI_U32              u32Vact;    /**<  Active Line *//**<CNcomment: ��Ч��*/
    HI_U32              u32Hact;/**< Active Pixels *//**<CNcomment: ��Ч����*/
    HI_U32              u32VerFreq;/**<Ver Frequency *//**<CNcomment: ˢ���� */
    HI_UNF_EDID_TIMING_TYPE_E          enTimingType;/**< Timing protocol type *//**<CNcomment: ����ʱ���Э������*/
    HI_UNF_EDID_TIMING_ATTR_E          enTimingAttr;/**<timing type *//**<CNcomment: :����ʱ�������*/
} HI_UNF_EDID_SIMPLE_TIMING_S;

/**Detailed Timing Definitions*/
/**CNcomment:������ϸʱ��*/
typedef struct hiUNF_EDID_DETAIL_TIMING_S		
{
    HI_UNF_EDID_TIMING_S	            stTiming; /**<  Detailed Timing *//**<CNcomment: ��ϸʱ��*/
    HI_UNF_EDID_SYNC_TYPE_E             enSyncType;/**Detailed Timing Definitions Sync Signal Definitions*//**CNcomment:��ϸʱ���ж���ͬ������*/
    HI_UNF_EDID_TIMING_ATTR_E           enTimingAttr;/**<timing type *//**<CNcomment: :����ʱ�������*/
} HI_UNF_EDID_DETAIL_TIMING_S;

/**EDID information*/
/**CNcomment:EDID������Ϣ*/
typedef struct hiUNF_EDID_INFO_S
{
    HI_UNF_EDID_BASE_INFO_S 		stEDIDBaseInfo;/**< EDID base information *//**<CNcomment: EDID������Ϣ */
    HI_UNF_EDID_VIDEO_PORT_E        enVideoPort;                  /**<video port  *//**<CNcomment: ��Ƶ�ӿ����� */
    HI_UNF_EDID_SIMPLE_TIMING_S*	pstSimpleTiming;	/**<Simple timing   *//**<CNcomment: ��Ҫʱ��*/
    HI_U32                          u32SimpleTimingNum;/**<Simple timing number *//**<CNcomment: ��Ҫʱ�����*/
    HI_UNF_EDID_DETAIL_TIMING_S*  	pstDetailTiming;	/**<Simple timing *//**<CNcomment: ��ϸʱ�����*/
    HI_U32                          u32DetailTimingNum;/**<Simple timing number *//**<CNcomment: ��ϸʱ�����*/
} HI_UNF_EDID_INFO_S;



/** @}*/  /** <!-- ==== Structure Definition End ====*/




/******************************* API Declaration *****************************/
/** \addtogroup      EDID*/
/** @{*/  /** <!-- [EDID] */


/** 
\brief parse the edid information. CNcomment:����EDID���� CNend
\attention \n
\param[in]  u32EdidLength EDID data length . CNcomment:EDID���� �Ľṹ ���� CNend
\param[in]  *pu8Edid  EDID data point . CNcomment:EDID���� ��ָ��CNend
\param[in]  *pstEdidInfo  EDID parse information . CNcomment:EDID������Ϣָ��CNend

\retval HI_SUCCESS   success.   CNcomment:�ɹ� CNend
\retval please refer to the err code definitino of mpi.CNcomment:��ο�MPI������ CNend
\see \n
CNcomment:�� CNend
*/
HI_S32 HI_UNF_EDID_EdidParse(HI_U8* pu8Edid, HI_U32 u32EdidLength, HI_UNF_EDID_INFO_S* pstEdidInfo);

/** 
\brief release the edid information. CNcomment:�ͷ�EDID��Ϣ CNend
\attention \n
\param[in]  *pstEdidInfo  EDID parse information . CNcomment:EDID������Ϣָ��CNend
\retval HI_SUCCESS   success.   CNcomment:�ɹ� CNend
\retval please refer to the err code definitino of mpi.CNcomment:��ο�MPI������ CNend
\see \n
CNcomment:�� CNend
*/
HI_VOID HI_UNF_EDID_EdidRelease(HI_UNF_EDID_INFO_S * pstEdidInfo);


/** @} */  /** <!-- ==== API declaration end ==== */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_UNF_EDID_H__ */

