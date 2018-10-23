#ifndef _SVR_CONTROL_CMD_H_
#define _SVR_CONTROL_CMD_H_
#include "wv_common.h"

#include "svr_control.h"
#include "svr_control_leishi.h"
#include "svr_control_shiyi.h"
#include "svr_control_yinchuang.h"
//#define SVR_CONTROL_DEBUG_MODE 
#ifdef  SVR_CONTROL_DEBUG_MODE 
  #define SVR_CONTROL_printf(...) \
  				 do {\
  				  printf("\n\r[%s][%s]-%d:",__FILE__,__FUNCTION__, __LINE__);\
				  printf(__VA_ARGS__);\
  				  fflush(stdout); \
  				  } while(0)
#else 
  #define SVR_CONTROL_printf(...) 
#endif  
typedef enum SVR_CONTROL_KTV_CHANGEMODE_E
{
	SVR_CONTROL_CHANGE_NULL = 0,
	SVR_CONTROL_CHANGE_MOV ,
	SVR_CONTROL_CHANGE_SCENE ,
}SVR_CONTROL_KTV_CHANGEMODE_E;

WV_S32 SVR_CONTROL_GetConfFileCmd(WV_U8 *pData,WV_S32 len);
WV_S32 SVR_CONTROL_ReadConfFileCmd();
WV_S32 SVR_CONTROL_GetKtvDev();
WV_S32 SVR_CONTROL_ReadConfFileCmd();
WV_S32 SVR_CONTROL_GetKtvNetPort();
WV_S32 SVR_CONTROL_GetKtvChangeMode();
WV_S32 SVR_CONTROL_GetTypeRound();
WV_S32 SVR_CONTROL_Cmd_Init();
WV_S32 SVR_CONTROL_GetKtvChangeMode();
//WV_S32 SVR_CONTROL_ReadConfFileCmd();
#endif


