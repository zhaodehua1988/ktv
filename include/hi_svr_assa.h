/**
 \file hi_svr_assa.h
 \brief ASS subtitle parsing
 \author Shenzhen Hisilicon Co., Ltd.
 \date 2008-2018
 \version 1.0
 \author
 \date 2010-02-10
 */

#ifndef _HI_SVR_ASSA_H_
#define _HI_SVR_ASSA_H_

#include <stdio.h>
#include "hi_type.h"
#include "hi_unf_so.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/***************************** Structs Definition ******************************/
/** \addtogroup     Hiplayer */
/** @{ */  /** <!--[Hiplayer] */

/***************************** Macro Definition ******************************/

/** Number of Styles fields */
/** CNcomment:Styles�ֶθ��� */
#define STYLEMATCH_MAX (24)

/** Number of Events fields */
/** CNcomment:Events�ֶθ��� */
#define EVENTMATCH_MAX (10)

/** Length of the key field in the Format field */
/** CNcomment:������format�ֶ��ڵĹؼ��ֶγ��� */
#define FORMAT_FIELD_LEN (256)

/** Length of the key field in the Events field */
/** CNcomment:������Events�ֶ��ڵĹؼ��ֳ��� */
#define EVENT_FIELD_LEN (128)

/** Maximum length of the Dialog field */
/** CNcomment:Dialog�ֶε���󳤶� */
#define DIALOG_FIELD_LEN (2048)


/**
*When the data structure is extended, ensure that the new members are 4-byte aligned and new members are placed after the existing members.
* Field information structure in the script information
*/
/**
*CNcomment:��չ���ݽṹ��ʱ����ע��,������Ա��ò��õȿ��ֽڵĳ�Ա�ҷ������
* Script Info ����ֶ���Ϣ�ṹ CNend
*/
typedef struct hiSO_SUBTITLE_ASS_SCRIPTINFO_S
{
    HI_BOOL *pbFlag;                    /**<Whether the field is a Script Info field.*//**< CNcomment:�Ƿ�ΪScript Info�ֶ� */

    HI_CHAR *pszTitle;                  /**<Title (subtitle description)*//**< CNcomment:����,����Ļ������ */

    HI_CHAR *pszScriptAuthor;           /**<Subtitle creator*//**< CNcomment:������Ļ������ */

    HI_CHAR *pszTranslate;              /**<Initial subtitle translator*//**< CNcomment:���������Ļ���� */

    HI_CHAR *pszEdition;                /**<Initial subtitle editor*//**< CNcomment:����༭��Ļ���� */

    HI_CHAR *pszTimedAuthor;            /**<Time line creator*//**< CNcomment:�����ʱ������Ա */

    HI_CHAR *pszSynchPoint;             /**<Start time of loading subtitles*//**< CNcomment:��ʼ������Ļ��ʱ��� */

    HI_CHAR *pszUpdateAuthor;           /**<Subtitle updater*//**< CNcomment:����Ļ���и��µ���Ա */

    HI_CHAR *pszUpdateDetails;          /**<Update details*//**< CNcomment:���µľ�����Ϣ */

    HI_CHAR *pszScriptType;             /**<Version description: v4+/v4*//**< CNcomment:�汾˵��:v4+/v4 */

    HI_CHAR *pszCollisions;             /**<How to move subtitles when two subtitles overlap*//**< CNcomment:��������Ļ�ص�ʱ,��ν�������ƶ� */

    HI_CHAR *pszPlayResY;              /**<Video height reference standard of the file*//**< CNcomment:�ļ���ʹ�õ���Ƶ�߶Ȳο���׼ */

    HI_CHAR *pszPlayResX;              /**<Video width reference standard of the file*//**< CNcomment:�ļ���ʹ�õ���Ƶ��Ȳο���׼ */

    HI_CHAR *pszPlayDepth;             /**<Color depth*/

    HI_CHAR *pszTimer;                 /**<Speed timer for loading subtitles (in percentage)*/

    HI_CHAR *pszWrapStyle;             /**<Defines the default wrapping style.*/

}HI_SO_SUBTITLE_ASS_SCRIPTINFO_S;

/**
*Information structure of the fields in Styles
*The fields must be arranged, and newly added fields must be placed after existing fields.
*The following are supported fields:
* Name, Fontname, Fontsize, PrimaryColour, SecondaryColour,
* TertiaryColour, BackColour, Bold, Italic, Underline,
* StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle,
* Outline, Shadow, Alignment, MarginL, MarginR, MarginV, AlphaLevel, Encoding
*/
/**
* CNcomment:Styles ����ĸ����ֶ���Ϣ�ṹ
* ���밴˳�����и����ֶ�,����������չ�ֶ�ʱ,ֻ�ܷ��ں���
* ֧�ֵ��ֶ�:
* Name, Fontname, Fontsize, PrimaryColour, SecondaryColour,
* TertiaryColour, BackColour, Bold, Italic, Underline,
* StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle,
* Outline, Shadow, Alignment, MarginL, MarginR, MarginV, AlphaLevel, Encoding CNend
*/
typedef struct hiSO_SUBTITLE_ASS_STYLE_S
{
    HI_CHAR *pszName;                           /**<Style name, case-sensitive*/
    HI_CHAR *pszFontName;                       /**<Font name in Windows, case-sensitive*/
    HI_CHAR *pszFontSize;                       /**<Font size*/

    /**The color format contains the alpha channel. (AABBGGRR) */
    HI_CHAR *pszPrimaryColour;                  /**<Primary color*//**< CNcomment:��Ҫ��ɫ */
    HI_CHAR *pszSecondaryColour;                /**<Secondary color*//**< CNcomment:��Ҫ��ɫ */
    HI_CHAR *pszTertiaryColour;                 /**<Subtitle outline color*//**< CNcomment:������ɫ */
    HI_CHAR *pszBackColour;                     /**<Background color*//**< CNcomment:��Ӱ��ɫ */

    /** bool value is -1/0=true/false change to 1/0 */
    HI_CHAR *pszBold;                           /**<-1 indicates bold, and 0 indicate regular.*//**< CNcomment:-1 Ϊ���� , 0 Ϊ���� */
    HI_CHAR *pszItalic;                         /**<-1 indicates italic, and 0 indicates regular.*//**< CNcomment:-1 Ϊб�� , 0 Ϊ���� */
    HI_CHAR *pszUnderline;                      /**<[-1 or 0] underscore*//**< CNcomment:[-1 ���� 0] �»��� */
    HI_CHAR *pszStrikeOut;                      /**< [-1 or 0] strike or strikeout*//**< CNcomment:[-1 ���� 0] �л���/ɾ���� */

    /**Variables values can be floating-point numbers.*/
    HI_CHAR *pszScaleX;                         /**<Text scaling width (in percentage)*//**< CNcomment:�޸����ֵĿ��.Ϊ�ٷ��� */
    HI_CHAR *pszScaleY;                         /**<Text scaling text (in percentage)*//**< CNcomment:�޸����ֵĸ߶�.Ϊ�ٷ��� */
    HI_CHAR *pszSpacing;                        /**<Text spacing (in pixel)*//**< CNcomment:���ּ�Ķ����϶.Ϊ������ */
    HI_CHAR *pszAngle;                          /**<Rotation angle based on the Z axis. The angle can be a fractional number. The origin is defined by alignment variable.*//**< CNcomment:�� Z �������ת�Ķ���,ԭ����alignment�����˶���.����ΪС�� */
    HI_CHAR *pszBorderStyle;                    /**< 1 = border+shadow, 3 = background with a single color*//**< CNcomment:1=�߿�+��Ӱ,3=��ɫ����. */
    HI_CHAR *pszOutline;                        /**<Text outline width (in pixel)*//**< CNcomment:�����������,Ϊ������ */
    HI_CHAR *pszShadow;                         /**<Shadow depth (in pixel)*//**< CNcomment:��Ӱ����� , Ϊ������ */
    HI_CHAR *pszAlignment;                      /**<Define the subtitle position.*//**< CNcomment:������Ļ��λ�� */
    HI_CHAR *pszMarginL;                        /**<Margin between the subtitle area and the left border (in pixel)*//**< CNcomment:��Ļ�ɳ������������Ե�ľ���,Ϊ������ */
    HI_CHAR *pszMarginR;                        /**<Margin between the subtitle area and the right border (in pixel)*//**< CNcomment:��Ļ�ɳ����������ұ�Ե�ľ���,Ϊ������ */
    HI_CHAR *pszMarginV;                        /**<Vertical margin*//**< CNcomment:��ֱ���� */
    HI_CHAR *pszAlphaLevel;                     /**<This variable is used to define the transparency of ASS subtitles. It is not present in ASS.*//**< CNcomment:SSA ��Ļ��������͸����,Not present in ASS */
    HI_CHAR *pszEncoding;                       /**<Specify the character set or encoding mode of the font.*//**< CNcomment:ָ��������ַ�������뷽ʽ */

    struct hiSO_SUBTITLE_ASS_STYLE_S *pstNext;  /**<Point to the next Styles information.*//**< CNcomment:ָ����һ��Styles��Ϣ */

}HI_SO_SUBTITLE_ASS_STYLE_S;

/**Styles field administrative information that is obtained by parsing the ASS subtitle extra data*/
/** CNcomment:ass��Ļ��չ�����н����õ���styles�ֶι�����Ϣ */
typedef struct hiSO_SUBTITLE_ASS_STYLESV4_S
{
    HI_BOOL bVersionIsV4;                       /**<Whether the version is V4 or V4+. If not, an error occurs.*//**< CNcomment:�Ƿ�ΪV4��V4+,������� */

    HI_S32 s32StyleMatchTotal;                  /**<Number of identified file fields*//**< CNcomment:�ļ��ﱻʶ����ֶθ��� */

    HI_S32 s32StyleMatchIndex[STYLEMATCH_MAX];  /**<Information about all fields supported by Styles*//**< CNcomment:styles��֧�ֵ������ֶ���Ϣ */

    HI_SO_SUBTITLE_ASS_STYLE_S *pstStyle;       /**<Information about the supported fields in the storage file*//**< CNcomment:�洢�ļ��б�֧�ֵ��ֶ���Ϣ */

}HI_SO_SUBTITLE_ASS_STYLESV4_S;

/**Description of all fields in Dialogue*/
/** CNcomment:Dialogue ������ֶε�������Ϣ */
typedef struct hiSO_SUBTITLE_ASS_DIALOG_S
{
    HI_CHAR szMarked[EVENT_FIELD_LEN];          /**<Identifier line or graphics layer*//**< CNcomment:��ʶ��/����Ϊͼ�� */

    HI_CHAR szStart[EVENT_FIELD_LEN];           /**<Event start time 0:00:00:00 (hour:minute:second:millisecond)*//**< CNcomment:�¼���ʼ��ʱ��,��ʽΪ0:00:00:00(ʱ:��:��:�ٷ���) */

    HI_CHAR szEnd[EVENT_FIELD_LEN];             /**<Event end time 0:00:00:00 (hour:minute:second:millisecond)*//**< CNcomment:�¼�������ʱ��,��ʽΪ0:00:00:00(ʱ:��:��:�ٷ���) */

    HI_CHAR szStyle[EVENT_FIELD_LEN];           /**<Subtitle style*//**< CNcomment:������Ļ��ʹ�õķ�� */

    HI_CHAR szName[EVENT_FIELD_LEN];            /**<Player's name*//**< CNcomment:player's name, ��ɫ�� */

    HI_CHAR szMarginL[EVENT_FIELD_LEN];         /**<Spacing between the subtitle area and the left margin*//**< CNcomment:ʹ���µ������Ե�ľ��� */

    HI_CHAR szMarginR[EVENT_FIELD_LEN];         /**<Spacing between the subtitle area and the right margin*//**< CNcomment:ʹ���µ����ұ�Ե�ľ��� */

    HI_CHAR szMarginV[EVENT_FIELD_LEN];         /**<If subtitles are at the bottom of the screen, this variable indicates the spacing between the subtitle area and the screen bottom and the value is in the
unit of pixel. If subtitles are at the top of the screen, this variable indicates the spacing between the subtitle area and the screen top and the value is
in the unit of pixel. If subtitles are in the middle of the screen, this variable is ignored.*//**< CNcomment:ʹ���µĴ�ֱ���� */

    HI_CHAR szEffect[EVENT_FIELD_LEN];          /**<Transition effect. It can be empty.*//**< CNcomment:����Ч��.����Ϊ��ֵ */

    HI_CHAR szText[DIALOG_FIELD_LEN];           /**<Dialog subtitle area*//**< CNcomment:�԰���Ļ���� */

                                                /**<List of multi-language subtitles*//**< CNcomment:֧�ֶ�������Ļ���б� */
    struct hiSO_SUBTITLE_ASS_DIALOG_S *pstNext;

}HI_SO_SUBTITLE_ASS_DIALOG_S;

/**Events field administrative information that is obtained by parsing the ASS subtitle extra data*/
/** CNcomment:ass��Ļ��չ�����н����õ���Events�ֶι�����Ϣ */
typedef struct hiSO_SUBTITLE_ASS_EVENTS_S
{
    HI_S32 s32EventMatchTotal;                  /**<Number of supported fields*//**< CNcomment:��֧�ֵ��ֶ����� */

    HI_S32 as32EventMatchIndex[EVENTMATCH_MAX]; /**<All supported fields*//**< CNcomment:������֧�ֵ������ֶ� */

    HI_SO_SUBTITLE_ASS_DIALOG_S *pstDialog;  /**<Obtained Dialog information*//**< CNcomment:�õ���һ��Dialog��Ϣ */

}HI_SO_SUBTITLE_ASS_EVENTS_S;

/**Private extra information required for internal parsing*/
/** CNcomment:�ڲ�������Ҫ�õ���˽�и�����Ϣ */
typedef struct tagSO_SUBTITLE_ASS_EXTRADATA_S
{
    HI_CHAR* pszExtraData;                      /**<Subtitle extra data that is being parsing*//**< CNcomment:��ǰ���ڽ�������Ļ��չ���� */

    HI_S32 s32ExtraDataSize;                    /**<Extra data size*//**< CNcomment:��չ���ݵĳ��� */

}SO_SUBTITLE_ASS_EXTRADATA_S;

/**All resources for managing ASS subtitles*/
/** CNcomment:���ڹ������ASS��Ļ���õ���������Դ */
typedef struct hiSO_SUBTITLE_ASS_PARSE_S
{
    HI_SO_SUBTITLE_ASS_SCRIPTINFO_S *pstScript; /**<Subtitle Script information*//**< CNcomment:��Ļ��Script Info��Ϣ */

    HI_SO_SUBTITLE_ASS_STYLESV4_S stStyleV4;    /**<Subtitle Styles information*//**< CNcomment:��Ļ��Styles��Ϣ */

    HI_SO_SUBTITLE_ASS_EVENTS_S *pstEvents;       /**<Subtitle Events information*//**< CNcomment:��Ļ��Events��Ϣ */

    SO_SUBTITLE_ASS_EXTRADATA_S stAssExtra;     /**<Subtitle extra data*//**< CNcomment:��Ļ����չ������Ϣ */

}HI_SO_SUBTITLE_ASS_PARSE_S;

/** @} */  /** <!-- ==== Structure Definition end ==== */

/******************************* API declaration *****************************/
/** \addtogroup      Hiplayer */
/** @{ */  /** <!-- [Hiplayer] */

/**
\brief Parses a dialog message to obtain the dialog text. The message includes the dynamic display style of dialog texts. Other attributes are stored in the
handle.
CNcomment:������һ��Dialog��Ϣ,�����԰׵Ķ�̬��ʾ��ʽ,�������Իᱻ���ھ���� CNend
\attention \n
None.
\param[in]  pstDialog Dialog message to be parsed. CNcomment:��Ҫ������Dialog��Ϣ CNend
\param[out] pszOutHand The dialog texts that are obtained by parsing the dialog message is returned. CNcomment:������Ļ�н����õ���Dialog�԰����� CNend

\retval ::HI_SUCCESS    Parsing is successful. CNcomment:�����ɹ�CNend
\retval ::HI_FAILURE    Parsing fails. CNcomment:����ʧ��CNend
\see \n
None.
*/
HI_S32 HI_SO_SUBTITLE_ASS_GetDialogText(HI_SO_SUBTITLE_ASS_DIALOG_S *pstDialog,
                                                            HI_CHAR **pszOutHand, HI_S32 *s32OutLen );

/**
\brief Parses a dialog message. CNcomment:������һ��Dialog��Ϣ,�����԰׵Ķ�̬��ʾ��ʽ,�������Իᱻ���ھ����CNend
\attention \n
None.
\param[in]      pstSubInfo Subtitle message to be parsed. CNcomment:��Ҫ��������Ļ��ϢCNend
\param[in]      pstParseHand Parser handle. It must be successfully initialized. CNcomment:���������, �����Ǳ���ʼ���ɹ��ľ��CNend
\param[out]     pstEvents The dialog texts obtained by parsing the dialog message are returned. CNcomment:������Ļ�н����õ���Dialog CNend

\retval ::HI_SUCCESS    Parsing is successful. CNcomment:�����ɹ�CNend
\retval ::HI_FAILURE    Parsing fails. CNcomment:����ʧ��CNend
\see \n
None.
*/
HI_S32 HI_SO_SUBTITLE_ASS_StartParse(const HI_UNF_SO_SUBTITLE_INFO_S *pstSubInfo,
    HI_SO_SUBTITLE_ASS_DIALOG_S **pstDialog, HI_SO_SUBTITLE_ASS_PARSE_S *pstParseHand);

/**
\brief Initializes a parser handle. The internal attributes are available only after ::HI_SO_SUBTITLE_ASS_OutputParse is successfully called.
CNcomment:��ʼ��һ�����������,�ڲ���������Ҫ����::HI_SO_SUBTITLE_ASS_OutputParse�ӿڳɹ������ʹ��CNend
\attention \n
None.
\param[in]      pstParseHand Parser handle to be initialized. CNcomment:��Ҫ��ʼ���Ľ��������CNend

\retval ::HI_SUCCESS    Initialization is successful. CNcomment:��ʼ���ɹ�CNend
\retval ::HI_FAILURE    Initialization fails. CNcomment:��ʼ��ʧ��CNend
\see \n
None.
*/
HI_S32 HI_SO_SUBTITLE_ASS_InitParseHand(HI_SO_SUBTITLE_ASS_PARSE_S **pstParseHand);

/**
\brief Releases a parser handle. A handle can be released only after it is successfully initialized.
CNcomment:�ͷ�һ�����������,�����Ǳ���ʼ���ɹ���ľ�������ͷ�CNend
\attention \n
None.
\param[in]      pstParseHand Parser handle to be released. CNcomment:��Ҫ�ͷŵĽ��������CNend

\retval ::HI_SUCCESS   A handle is successfully released. CNcomment:�ͷųɹ�CNend
\retval ::HI_FAILURE   A handle fails to be released. CNcomment:�ͷ�ʧ��CNend
\see \n
None.
*/
HI_S32 HI_SO_SUBTITLE_ASS_DeinitParseHand(HI_SO_SUBTITLE_ASS_PARSE_S *pstParseHand);

/**
\brief Releases a Dialog linked list. CNcomment:�ͷ�һ��Dialog����CNend
\attention \n
None.
\param[in]      pstDialog Node of the linked list to be released. CNcomment:��Ҫ�ͷŵ�����ͷ���CNend

\retval ::HI_SUCCESS A linked list is successfully released.  CNcomment: �ͷųɹ�CNend
\retval ::HI_FAILURE   A linked list fails to be released. CNcomment:�ͷ�ʧ��CNend
\see \n
None.
*/
HI_S32 HI_SO_SUBTITLE_ASS_FreeDialog(HI_SO_SUBTITLE_ASS_DIALOG_S **pstDialog);

/**
\brief Prints the extra data corresponding to the ASS subtitles. CNcomment:��ӡ��ASS��Ļ����չ����CNend
\attention \n
None.
\param[in]      pstAssParse ASS parser handle. CNcomment:��ӡ��ASS���CNend

\retval :: None
\see \n
None.
*/
HI_VOID HI_SO_SUBTITLE_ASS_PrintExtraData(HI_SO_SUBTITLE_ASS_PARSE_S *pstParseHand);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _HI_SVR_ASSA_H_ */
