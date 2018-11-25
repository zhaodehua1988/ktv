#include "wv_common.h"
#include "wv_cmd.h"
#include "wv_tty.h"
#include "his_sys.h"
#include "his_spi.h"
#include "his_fb.h"
#include "adi_7619.h"
#include"tsk_cast.h"
#include"tsk_usb.h"
#include"tsk_fpga.h"
#include"tsk_scene.h"
#include"fpga_conf.h"
#include "svr_udp.h"
#include"svr.h"
#include "tsk_text.h"
#include<sys/time.h>
#include "net_uart.h"
#include "ktv_shiyi.h"
#include "wv_file.h"
#include "sys_env.h"
#include "wv_file.h"
WV_S32  main()
{
	WV_S32 ret;
	WV_U32 rowNum;
        printf("********hello world ********\n");
        //打印版本信息
        printf("The version number of the program is %s\n",SYS_ENV_VERSION_NO);    //版本号的宏定义在sys_env.h里
        //WV_FILE_PRINTF_VERSION();                  //这里打印的版本信息，是读取conf64.ini里的

        WV_CMD_Create();
	SYS_ENV_Open();
	WV_TTY_Create();

	HIS_SYS_Init();
	SYS_DATE_Init();
	SYS_IP_Init();
	// ADV_7619_Init();
	//TASK_CAST_Open();
	
	TSK_PLAYER_Open();
	TSK_FPGA_Open();
	TSK_GO_Open();	
	TSK_SCENE_Init();
	SVR_Open();
	SVR_UDP_Open();
	TSK_UART_Open();
	TSK_CheckOut_Open();	
	//sleep(1);
	TSK_USB_ShowOpen();
	TSK_USB_Open();	
	SYS_LICENCES_Open();
	TSK_UART_SceneLoopOpen();
	SYS_INFO_Open();
	TSK_SCENE_SendSync();   //场景同步
	SVR_CONTROL_Open();
	TSK_UPDATE_FPGA_Open();
	WV_SQLITE3_Init();
	TSK_TEXT_Open();
	NET_UART_Open();
	KTV_SHIYI_Open();
    KTV_LEISHI_Open();
	TSK_SCENE_ConfAni();//解决开机背景图片显示不全的问题
	rowNum =0;
    while(1)
    {
       ret = WV_CMD_GetExit();
       if(ret == 1) break;
       usleep(100000);     
    }
	KTV_SHIYI_Close();
	KTV_LEISHI_Close();
	NET_UART_Close();
	TSK_TEXT_Close();
	SVR_CONTROL_Close();
	SYS_INFO_Close();	
	TSK_UART_SceneLoopClose();
	SYS_LICENCES_Close();
	TSK_USB_ShowClose();
	TSK_USB_Close();
	TSK_CheckOut_Close();
	TSK_UART_Close();
	SVR_UDP_Close();
	SVR_Close();
	TSK_SCENE_DeInit();
	TSK_GO_Close(); 
	TSK_FPGA_Close();
	TSK_PLAYER_Close();
	//TASK_CAST_Close();
	
     SYS_IP_DeInit(); 
     SYS_DATE_DeInit();
     HIS_SYS_DeInit(); 
     WV_TTY_Destroy();
     SYS_ENV_Close();     
     WV_CMD_Destroy(); 
  
     printf("********    bye     ********\n");
	return 0;
}
/**/

