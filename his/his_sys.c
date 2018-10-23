#include "his_sys.h"
#include "his_snd.h"
#include "his_so.h"
#include "his_demux.h"
#include "his_iic.h"
#include "his_dis.h"
#include "his_hdmi.h"
#include "his_venc.h"
#include "his_avplay.h"
#include "his_chnl.h"
#include "hi_common.h"
#include "hi_unf_video.h"
#include "his_spi.h"
#include "his_reg.h"
#include "his_fb.h"
#include "his_player.h"
#include"tsk_cast.h"
#include"tsk_usb.h"
#include"his_temperature.h" 
#include"his_gpio.h" 
#include"his_go.h" 
#include"his_fpga.h"
 
 
typedef struct  HIS_SYS_CONF_S
{
HI_UNF_ENC_FMT_E  disFormat;
} 
HIS_SYS_CONF_S;

static  HIS_SYS_CONF_S   gHisConf;
 
// WV_AVPLAY_CHNL_E  gPlayChnl[2];

/*************************************************************************************
WV_S32  HIS_SYS_ConfInit();
*************************************************************************************/
WV_S32  HIS_SYS_ConfInit()
{
  //gHisConf.disFormat = HI_UNF_ENC_FMT_3840X2160_30;
  //gHisConf.disFormat = HI_UNF_ENC_FMT_1080P_60; 
 // gHisConf.disFormat = HI_UNF_ENC_FMT_VESA_2560X1600_60_RB;
  gHisConf.disFormat = HI_UNF_ENC_FMT_BUTT;
	
    return WV_SOK;
}
/*************************************************************************************
WV_S32  HIS_SYS_Init();
*************************************************************************************/
WV_S32   HIS_SYS_Init()
{
  //WV_S8  ipLocal[20] = "192.168.15.98";
  //WV_S8  ipRx[20] = "231.0.20.100";

  HI_SYS_Init(); 
  HIS_IO_Init();
  HIS_IIC_Init();
  HIS_REG_Init();
  HIS_TEMP_Init();
  HIS_SPI_Init();
  HIS_SYS_ConfInit();
  HIS_SND_Init();
  HIS_SO_Init();
  HIS_DEMUX_Init(); 

  HIS_FPGA_Init();
  HIS_DIS_Init(gHisConf.disFormat,1);
  HIS_HDMI_Init(gHisConf.disFormat);  
  HIS_AVP_Init();
  HIS_VENC_Init();
  HIS_PLAYER_Init();
 //usb power on
 //HIS_IO_usbPw(1);
//TSK_USB_Creat();
 
 
 //memset(gPlayChnl,0,2*sizeof( WV_AVPLAY_CHNL_E)); 
  //chnnel creat
  /*
  gPlayChnl[0].type = 0;
  gPlayChnl[0].vEecType = HI_UNF_VCODEC_TYPE_HEVC;
  
  gPlayChnl[0].winRect.s32X = 0;
  gPlayChnl[0].winRect.s32Y = 4;
  gPlayChnl[0].winRect.s32Width = 3840;
  gPlayChnl[0].winRect.s32Height = 1080; 
  

	gPlayChnl[0].socketRx.addrLocal.sin_addr.s_addr = inet_addr(ipLocal); 
	bzero(&( gPlayChnl[0].socketRx.addrLocal.sin_zero),8);

	gPlayChnl[0].socketRx.addrLocal.sin_family = AF_INET;
	gPlayChnl[0].socketRx.addrLocal.sin_port = htons(7001);
	
    gPlayChnl[0].socketRx.addrRx.sin_addr.s_addr = inet_addr(ipRx); 
    bzero(&( gPlayChnl[0].socketRx.addrRx.sin_zero),8);
   
    gPlayChnl[0].socketRx.addrRx.sin_family = AF_INET;
    gPlayChnl[0].socketRx.addrRx.sin_port = htons(7001); 
    HIS_CHNL_Creat(&gPlayChnl[0]); 
   
    gPlayChnl[0].type = 1;
    gPlayChnl[0].vEecType = HI_UNF_VCODEC_TYPE_HEVC;
    gPlayChnl[0].winRect.s32X = 0;
    gPlayChnl[0].winRect.s32Y = 4;
    gPlayChnl[0].winRect.s32Width = 3840;
    gPlayChnl[0].winRect.s32Height = 1080; 
    sprintf(gPlayChnl[0].fileName, "%s", "./1526.mp4"); 
	//sprintf(gPlayChnl[1].fileName, "%s", "/opt/video/1545.mp4");     
    //sprintf(gPlayChnl[1].fileName, "%s", "./2504.mkv"); BeautifulSea.mkv
    //sprintf(gPlayChnl[1].fileName, "%s", "BeautifulSea.mkv");
    //sprintf(gPlayChnl[0].fileName, "%s", "/opt/video/Chimei.mp4");
    //sprintf(gPlayChnl[0].fileName, "%s", "/opt/video/BeautyHevc.mkv");
    //sprintf(gPlayChnl[0].fileName, "%s", "/opt/usbdisk/Sony.4K.DEMO_Feel.The.Beauty_HEVC.mkv");  
    gPlayChnl[0].socketRx.addrLocal.sin_addr.s_addr = inet_addr(ipLocal); 
	bzero(&( gPlayChnl[0].socketRx.addrLocal.sin_zero),8);

	gPlayChnl[0].socketRx.addrLocal.sin_family = AF_INET;
	gPlayChnl[0].socketRx.addrLocal.sin_port = htons(7001);
	
    gPlayChnl[0].socketRx.addrRx.sin_addr.s_addr = inet_addr(ipRx); 
    bzero(&( gPlayChnl[0].socketRx.addrRx.sin_zero),8);
   
    gPlayChnl[0].socketRx.addrRx.sin_family = AF_INET;
    gPlayChnl[0].socketRx.addrRx.sin_port = htons(7001); 
    sleep(10);
    HIS_CHNL_Creat(&gPlayChnl[0]); 
    
//chennl file    
    gPlayChnl[1].type = 1;
    gPlayChnl[1].vEecType = HI_UNF_VCODEC_TYPE_HEVC;
    gPlayChnl[1].winRect.s32X = 960;
    gPlayChnl[1].winRect.s32Y = 274;
    gPlayChnl[1].winRect.s32Width = 1920;
    gPlayChnl[1].winRect.s32Height = 540; 
     sprintf(gPlayChnl[1].fileName, "%s", "./1526.mp4"); 
	//sprintf(gPlayChnl[1].fileName, "%s", "/opt/video/1545.mp4");     
    //sprintf(gPlayChnl[1].fileName, "%s", "./2504.mkv"); BeautifulSea.mkv
    //sprintf(gPlayChnl[1].fileName, "%s", "BeautifulSea.mkv");
    //sprintf(gPlayChnl[1].fileName, "%s", "BeautifulSea.mkv");  
    gPlayChnl[1].socketRx.addrLocal.sin_addr.s_addr = inet_addr(ipLocal); 
	bzero(&( gPlayChnl[1].socketRx.addrLocal.sin_zero),8);

	gPlayChnl[1].socketRx.addrLocal.sin_family = AF_INET;
	gPlayChnl[1].socketRx.addrLocal.sin_port = htons(7001);
	
    gPlayChnl[1].socketRx.addrRx.sin_addr.s_addr = inet_addr(ipRx); 
    bzero(&( gPlayChnl[1].socketRx.addrRx.sin_zero),8);
   
    gPlayChnl[1].socketRx.addrRx.sin_family = AF_INET;
    gPlayChnl[1].socketRx.addrRx.sin_port = htons(7001); 
    
    HIS_CHNL_Creat(&gPlayChnl[1]); 
    // 
      */ 
  
  HIS_GO_Init();
  HIS_FB_Open(); 
  //TASK_CAST_Open();
     
  return WV_SOK;
}
/*************************************************************************************
WV_S32  HIS_SYS_DeInit();
*************************************************************************************/

WV_S32 HIS_SYS_DeInit()
{
   HIS_FB_Close();
   HIS_GO_DeInit();

   //TASK_CAST_Close(); 
  //
   // HIS_CHNL_Destroy(&gPlayChnl[0]); 
   // HIS_CHNL_Destroy(&gPlayChnl[1]);  
  //
   HIS_PLAYER_DeInit();
   HIS_VENC_DeInit();
   HIS_AVP_DeInit();
   HIS_HDMI_DeInit();
   HIS_DIS_DeInit();
	HIS_FPGA_DeInit();
   HIS_SND_DeInit();
   HIS_SO_DeInit();
   HIS_DEMUX_DeInit(); 
   HIS_IIC_DeInit();
   HIS_SPI_DeInit();
   HIS_TEMP_DeInit();
   //TSK_USB_Destroy();
   HIS_IO_DeInit();
   HIS_REG_DeInit();
   HI_SYS_DeInit();
  return WV_SOK;
}

