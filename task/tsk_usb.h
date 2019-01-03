#ifndef  _TSK_USB_USB_H_
#define  _TSK_USB_USB_H_
#include "wv_common.h"  
#include "sys_env.h" 
#include "sys_ip.h"
WV_S32  TSK_USB_Open();
WV_S32  TSK_USB_Close();
WV_S32 TSK_USB_GetUpdate(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff);
#endif
