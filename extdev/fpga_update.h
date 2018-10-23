#ifndef  _FPGA_UPDATE_UPDATE_H_
#define  _FPGA_UPDATE_UPDATE_H_
#include "wv_common.h"

WV_S32  TSK_UPDATE_FPGA_Open();
WV_S32  TSK_UPDATE_FPGA_RecvBin(WV_U8 fpgaPort,WV_U32 len,WV_U8 *pData);
WV_S32  TSK_UPDATE_FPGAGetUpdateNum(WV_U16 *pTotalPack,WV_U16 *pUpdatedPack);
#endif 
