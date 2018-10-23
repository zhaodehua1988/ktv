#ifndef _SVR_CMD_H_
#define _SVR_CMD_H_
#include"wv_common.h" 
#include"svr_frame.h"

typedef WV_S32 (*SVR_CMD_PROFUNC)( SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData);

typedef struct SVR_CMD_FUNC_E{
	WV_U8  cmd;				//命令   
	WV_S8  cmdComment[WV_CMD_COMMENT_MAXLEN];		//comment
	SVR_CMD_PROFUNC  procFunc;					    //function
}SVR_CMD_FUNC_E;

typedef struct SVR_CMD_WINDOW_CONF{
		WV_U8  effCover;
		WV_U16 xoffset;
		WV_U16 yoffset;
		WV_U16 win_W;
		WV_U16 win_H;
		WV_U16 outID;
		WV_U16 videoID;												
	}SVR_CMD_WINDOW_CONF; 
/*******************************************/

/******************************************/
typedef struct SVR_CMD_OUT_RESOLUTION{

		WV_U16 totalPix;
		WV_U16 totalLine;
		WV_U16 hSyncWidth;
		WV_U16 vSyncWidth;	
		WV_U16 activeHStar;
		WV_U16 activeHSize;
		WV_U16 activeVStar;
		WV_U16 activeVSize;

	}SVR_CMD_OUT_RESOLUTION;

WV_S32  SVR_CMD_Init(); 
WV_S32  SVR_CMD_DeInit();
WV_S32   SVR_CMD_Register(WV_U8 cmd,WV_S8 *cmdComment,SVR_CMD_PROFUNC pFunc);
WV_S32   SVR_CMD_Print( WV_S8 *pPrntBuf);
WV_S32  SVR_CMD_Proc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData) ;
#endif

