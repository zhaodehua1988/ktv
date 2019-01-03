#ifndef _NET_UART_H_H_
#define _NET_UART_H_H_
#include "wv_common.h"
#include "svr_control_cmd.h"


#define NET_UART_DEBUG_MODE 
#ifdef  NET_UART_DEBUG_MODE 
#define NET_UART_printf(...) \
  				 do {\
  				  printf("\n\r[%s][%s]-%d:",__FILE__,__FUNCTION__, __LINE__);\
				  printf(__VA_ARGS__);\
  				  fflush(stdout); \
  				  } while(0)
#else 
  #define NET_UART_printf(...) 
#endif
WV_S32 NET_UART_Open();
WV_S32 NET_UART_Close();
WV_S32  NET_UART_ProjectorCmd(WV_U32 openProjector);
WV_S32 NET_UART_InfraredCmd(WV_U8 disable);
#endif
