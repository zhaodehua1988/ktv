#ifndef  _TSK_FPGA_H_
#define  _TSK_FPGA_H_
#include "wv_common.h"  
#include "fpga_conf.h" 
WV_S32  TSK_FPGA_Open();
WV_S32  TSK_FPGA_Close(); 

WV_S32 TSK_FPGA_Read(WV_U16 addr,WV_U16 *pData);

WV_S32  TSK_FPGA_TransMap(WV_U8 * pData,WV_U32 len);
WV_S32  TSK_FPGA_TransOut(WV_U8 * pData,WV_U32 len);
WV_S32  TSK_FPGA_TransLight(WV_U8 * pData,WV_U32 len);
WV_S32  TSK_FPGA_TransColor(WV_U8 * pData,WV_U32 len);

WV_S32  TSK_FPGA_SaveMap(WV_U8 channel );
WV_S32  TSK_FPGA_SaveOut(WV_U8 channel );
WV_S32  TSK_FPGA_SaveLight(WV_U8 channel);
WV_S32  TSK_FPGA_SaveColor(WV_U8 channel);

WV_S32  TSK_FPGA_GetMap(WV_U8 channel);
WV_S32  TSK_FPGA_GetOut(WV_U8 channel);
WV_S32  TSK_FPGA_GetLight(WV_U8 channel);  
WV_S32  TSK_FPGA_GetColor(WV_U8 channel);  

WV_S32  TSK_FPGA_SetMap (WV_U8 mapChnl);
WV_S32  TSK_FPGA_SetOut (WV_U8 outChnl);
WV_S32  TSK_FPGA_SetLight (WV_U8 splitChnl);
WV_S32  TSK_FPGA_SetColor (WV_U8 splitChnl);
WV_S32  TSK_FPGA_SavePointSplit (FPGA_SPLIT_INFO_S splitInfo );
WV_S32  TSK_FPGA_SetPointSplit (FPGA_SPLIT_INFO_S splitInfo );
WV_S32   TSK_FPGA_PointoutConf (WV_U16  ena);
WV_U16   TSK_FPGA_GetOutConf ();
//  for server
WV_S32  TSK_FPGA_SplitConf (FPGA_SPLIT_INFO_S splitInfo );
WV_S32  TSK_FPGA_GetSplit(FPGA_SPLIT_INFO_S* splitInfo );
WV_S32  TSK_FPGA_outConf (WV_U16  ena);
WV_S32  TSK_FPGA_ResolutionConf (FPGA_RESOLUTION_INFO_S   resolution);
WV_S32  TSK_FPGA_lightConf(WV_U16  ena);
WV_S32  TSK_FPGA_load();

WV_S32 TSK_FPGA_GetTemp(WV_U16* pCpu,WV_U16 *pFpgaIn,WV_U16 *pFpgaOut);
#endif
