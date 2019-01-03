#ifndef _TSK_HIS_GO_H_H__
#define _TSK_HIS_GO_H_H__
#include "wv_common.h"
#include "his_go.h"

#define TSK_HIGO_ANI_MAX_NUM   		HIS_HIGO_GODEC_NUM 
#define TSK_HIGO_FILE_MAX_NUM       16 

typedef struct FILLRECT_GRID 
{
	WV_U16 w;
	WV_U16 h;
	WV_U16 row;
	WV_U16 col;
	WV_U16 start;
	WV_U16 end;
	WV_U16 x[4];
	WV_U16 y[4];
				
}FILLRECT_GRID;    


typedef struct TSK_GO_MOV_PIC_POS_S
{
	WV_U16 startX;
	WV_U16 startY;
	WV_U16 endX;
	WV_U16 endY;
}TSK_GO_MOV_PIC_POS_S;

WV_S32 TSK_GO_Open();
WV_S32 TSK_GO_Close();
WV_S32 TSK_GO_FillRect_Start();
WV_S32 TSK_GO_FillRectLine();
WV_S32 TSK_GO_FillRectDebug(WV_U32 colorNum,WV_U32 *pColor);
WV_S32 TSK_GO_FillRectGrid(WV_U16 border,WV_U16 splitNum,FILLRECT_GRID grid);
WV_S32 TSK_GO_DrawGrig(WV_U32 chl,WV_U32 backColor,WV_U32 lineColor,WV_U32 row ,WV_U32 col,WV_U16 *pBuf);
WV_S32 TSK_GO_DrawPoitForLight(WV_U32 color,WV_U32 x,WV_U32 y);
WV_S32 TSK_GO_DrawGrid_Start();
WV_S32 TSK_GO_DrawGrid_EXIT();

WV_S32 TSK_GO_CleanLayer();
WV_S32 TSK_GO_RefreshLayer();
WV_S32 TSK_GO_DecOpen(WV_U32 aniId,WV_S8 *pFileName, WV_U32 x, WV_U32 y,WV_U32 w,WV_U32 h);
WV_S32 TSK_GO_DecClose( WV_U32 id);
WV_S32 TSK_GO_DrawMapLine(WV_U32 dataLen,WV_S8 *pData);
WV_S32 TSK_GO_RectPicAndMovPos(WV_U16 BackAniEna,WV_U8 saveMode,WV_U32 dataLen,WV_U8 *pData);
WV_S32 TSK_GO_SetColorKeyFlag(WV_U32 ena);
WV_S32 TSK_GO_GetUsb_PicAndMov_Pos(TSK_GO_MOV_PIC_POS_S * pMovPos,TSK_GO_MOV_PIC_POS_S * pPicPos);
WV_S32 TSK_GO_GetOutline_Pos(TSK_GO_MOV_PIC_POS_S * pOutlinePos,WV_U16 *pChn,WV_U16 *pID);
//WV_S32 HIS_GO_RefreshLayerForPic(HIS_DEV_HIGO_E * pDev);
WV_S32 TSK_GO_ShowText( WV_U32 x,WV_U32 y,WV_U32 w,WV_U32 h,WV_U32 color,WV_U32 size,WV_S8 *pText,WV_U32 codeType,WV_U32 border);
WV_S32 TSK_GO_CleanRect( WV_U32 x,WV_U32 y,WV_U32 w,WV_U32 h);
WV_S32 TSK_GO_RefreshLayerForPic();
WV_S32 TSK_GO_GetBackAni_Pos(TSK_GO_MOV_PIC_POS_S * pBackAniPos,WV_U16 *pChn,WV_U16 *pID);
WV_S32 TSK_GO_DrawEllipse( WV_U16 x ,WV_U16 y,WV_U32 r,WV_U32 color);
#endif
