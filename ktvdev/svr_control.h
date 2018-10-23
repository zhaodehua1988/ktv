#ifndef _SVR_CONTROL_H_
#define _SVR_CONTROL_H_
#include "wv_common.h"
#include "svr_control_cmd.h"


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

WV_S32 SVR_CONTROL_Send(WV_U8 *pData,WV_U32 dataLen);

#endif
