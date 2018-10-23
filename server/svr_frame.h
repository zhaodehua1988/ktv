#ifndef _SVR_FRAME_H_
#define _SVR_FRAME_H_
#include"wv_common.h"
typedef struct SVR_FRAME_HEAD_E 
{ 
 WV_U32      sync;
 WV_U8       cmdL0;
 WV_U8       cmdL1; 
 WV_U16      cnt;
 WV_U16      arg1;
 WV_U16      arg2;
 WV_U32      dataNum;   
} SVR_FRAME_HEAD_E;

#define SVR_SYNC_WORD         0xa55a5a5a 

#define SVR_FRAME_DATA_MAX_LEN   1920*1092*4 


WV_S32  SVR_FRAME_Analyze(SVR_FRAME_HEAD_E * pHead, WV_U16 *pCnt);
WV_S32  SVR_FRAME_SyncPre(WV_U16  sync);
WV_S32  SVR_FRAME_Sync(WV_U16  sync); 
#endif

