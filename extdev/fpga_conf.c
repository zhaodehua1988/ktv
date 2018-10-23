#include"fpga_conf.h"
#include"his_spi.h"
#include "tsk_player.h" 
#include "tsk_scene.h" 
typedef struct FPGA_CONF_VERSION
{
	WV_U16 ver;
	WV_U16 id;
	WV_U16 num;
	    
}FPGA_CONF_VERSION ;
static FPGA_CONF_VERSION  gVersion;


static unsigned int screenMode;//0:3840*1080;1:3840*2160;2:1920*1080;

static unsigned short gOutput=0xffff;
/*******************************************************************
WV_U16 FPGA_CONF_GerVer()
*******************************************************************/
WV_U16 FPGA_CONF_GerVer()
{
	WV_U16 version;

	HIS_SPI_FpgaRd(0x01,&version);
	return version;
}

/*******************************************************************
 WV_S32 FPGA_CONF_ResetA()
*******************************************************************/
WV_S32 FPGA_CONF_ResetA()
  {
	WV_S32 ret = 0;
	WV_U16 rd28;
	WV_U16 rd30;
	WV_U16 rd31;
	WV_U16 rdb0;
	WV_U16 rdb1;

	WV_S32 i,chl;
	chl = FPGA_CONF_GetOutChl_Num();
	if(chl == 2){
		WV_RET_ADD(	HIS_SPI_FpgaWd(0xb3,1),ret);
		usleep(12000);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0xb3,0),ret);
		
			for(i=0;i<100;i++){

			WV_RET_ADD(	HIS_SPI_FpgaRd(0xb0,&rdb0),ret);  	
			WV_RET_ADD(	HIS_SPI_FpgaRd(0xb1,&rdb1),ret);  	

			if(rdb0==1 && rdb1==1){
				WV_printf("***************FPGA Reset OK,channel num is [%d]\n",chl);		
				break;	
			}
				usleep(100000);  	
			}
				
	}
	else if(chl == 4){
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x0d,1),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x33,1),ret);
		
		usleep(12000);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x0d,0),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x33,0),ret);
		for(i=0;i<100;i++){	
			WV_RET_ADD(	HIS_SPI_FpgaRd(0x28,&rd28),ret);  	
			WV_RET_ADD(	HIS_SPI_FpgaRd2(0x30,&rd30),ret);
			WV_RET_ADD(	HIS_SPI_FpgaRd2(0x31,&rd31),ret);
			if(rd28==1 && rd30==1 &&rd31==1){
				WV_printf("***************FPGA Reset OK,output channel num is [%d]\n",chl);		
				break;	
			}
				usleep(100000);  	
		}
	}
	else if(chl == 6){
		WV_RET_ADD(	HIS_SPI_FpgaWd(0xb3,1),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x33,1),ret);
		usleep(12000);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0xb3,0),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x33,0),ret);
		for(i=0;i<100;i++){	
			
			WV_RET_ADD(	HIS_SPI_FpgaRd(0xb0,&rdb0),ret);  	
			WV_RET_ADD(	HIS_SPI_FpgaRd(0xb1,&rdb1),ret); 	
			WV_RET_ADD(	HIS_SPI_FpgaRd2(0x30,&rd30),ret);
			WV_RET_ADD(	HIS_SPI_FpgaRd2(0x31,&rd31),ret);
			if(rdb0==1 && rdb1==1 && rd30==1 &&rd31==1){
				WV_printf("***************FPGA Reset OK,output channel num is [%d]\n",chl);		
				break;	
			}
				usleep(100000);  	
		}		
		
	}
	else if(chl == 8){

		WV_RET_ADD(	HIS_SPI_FpgaWd(0x0d,1),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x33,1),ret);
		
		usleep(12000);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x0d,0),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x33,0),ret);
		for(i=0;i<100;i++){	
			WV_RET_ADD(	HIS_SPI_FpgaRd(0x28,&rd28),ret);  	
			WV_RET_ADD(	HIS_SPI_FpgaRd2(0x30,&rd30),ret);
			WV_RET_ADD(	HIS_SPI_FpgaRd2(0x31,&rd31),ret);
			if(rd28==1 && rd30==1 &&rd31==1){
				WV_printf("***************FPGA Reset OK,output channel num is [%d]\n",chl);		
				break;	
			}
				usleep(100000);  	
		}
	
	}else{
		WV_printf("error: FPGA_CONF_ResetA(),channel num [%d]\n ",chl);
		return WV_EFAIL;
	}

    return ret;
  }

 /*******************************************************************
 WV_S32 FPGA_CONF_RqMcu();
 *******************************************************************/
 
 WV_S32 FPGA_CONF_RqMcu()
  {
    WV_S32 ret = 0; 
	WV_S32 i;
	WV_U16 data = 1;
     WV_RET_ADD(	HIS_SPI_FpgaWd(0x29,1),ret);
  
	for(i=0;i<1000;i++){

		TSK_FPGA_Read(0x29,&data);

		if(data==0){
			break;	
		}		

		usleep(10000);
	}

    return ret;
  }  
     
 /*******************************************************************
 WV_S32 FPGA_CONF_ClrBuf();
 *******************************************************************/
 WV_S32 FPGA_CONF_ClrBuf()
  {
     WV_S32 ret = 0;
     WV_S32 i; 
     WV_RET_ADD(HIS_SPI_FpgaWd(0xe,1),ret);
     WV_U16 data = 1;
     
     for(i=0;i<100;i++){

	TSK_FPGA_Read(0x0e,&data);

	if(data==0){
		break;	
	}		
	
	usleep(10000);
     }

	
    return ret;
  }
  
/*******************************************************************
 WV_S32 FPGA_CONF_SetOutlineStart();
 *******************************************************************/
 
WV_S32 FPGA_CONF_SetOutlineStart()
{
	WV_U16 data=0;

	screenMode = TSK_PLAYER_GetMode();
	if(screenMode != 0 && screenMode != 1)
	{ 
		//printf("get map end screenMode=%d\n",screenMode);
		TSK_PLAYER_ChangeMode(1);
	} 	
    WV_S32 ret = 0; 
    WV_RET_ADD(	HIS_SPI_FpgaWd(0xb,0),ret);	
	usleep(10000);	
	WV_RET_ADD(	HIS_SPI_FpgaRd(0xb,&data),ret);
	if(data != 0 )
	{
	    WV_RET_ADD(	HIS_SPI_FpgaWd(0xb,0),ret);		
	}
    return ret;
}
  
/*******************************************************************
 WV_S32 FPGA_CONF_SetOutlineEnd();
*******************************************************************/
 WV_S32 FPGA_CONF_SetOutlineEnd()
 {
    WV_S32 ret = 0;
    WV_U16 data = 1;
    WV_S32 i; 
    WV_S32 num=0; 
    WV_RET_ADD(	HIS_SPI_FpgaWd(0x27,0),ret);
	
    WV_RET_ADD(	HIS_SPI_FpgaWd(0xe,1),ret);
	
    for(i=0;i<1000;i++){

		TSK_FPGA_Read(0x0e,&data);
		if(data==0){
			break;	
		}
		usleep(10000);
    }	
	 WV_RET_ADD(	HIS_SPI_FpgaWd(0xb,3),ret);

	for(i=0;i<10;i++){

		TSK_FPGA_Read(0x0b,&data);
		if(data==3){
			break;	
		}
		HIS_SPI_FpgaWd(0xb,3);
		usleep(10000);
	} 	 

	TSK_PLAYER_ChangeMode(screenMode);	

    return ret;
  }
 
/*******************************************************************
 WV_S32 FPGA_CONF_SetLightStart();
 *******************************************************************/
 
 WV_S32 FPGA_CONF_SetLightStart()
 {
    WV_S32 ret = 0;  
	screenMode = TSK_PLAYER_GetMode();
	if(screenMode != 0 && screenMode != 1)
	{ 
		TSK_PLAYER_ChangeMode(1);
	} 
    return ret;
}
  
/*******************************************************************
 WV_S32 FPGA_CONF_SetLightEnd();
*******************************************************************/
 
WV_S32 FPGA_CONF_SetLightEnd()
{
	WV_S32 ret = 0;
	WV_U32 lightEna;
	WV_U16 data=0;
	lightEna=TSK_SCENE_GetLightEna(); 
	WV_S32 i=0;
 	if(lightEna == 0)
	{
		for(i=0;i<10;i++)
		{
			WV_RET_ADD(	HIS_SPI_FpgaWd(0x26,0x3),ret);
			usleep(10000);	
			WV_RET_ADD(	HIS_SPI_FpgaRd(0X26,&data),ret);
			if(data == 0x3) break;

		}
	}else if(lightEna == 1){
		for(i=0;i<10;i++)
		{
			WV_RET_ADD(	HIS_SPI_FpgaWd(0x26,0),ret);
			usleep(10000);	
			WV_RET_ADD(	HIS_SPI_FpgaRd(0X26,&data),ret);
			if(data == 0) break;
		}
	}

	TSK_PLAYER_ChangeMode(screenMode);
	return ret;
} 
   
/*******************************************************************
 WV_S32 FPGA_CONF_SetMapStart();
 *******************************************************************/
 
 WV_S32 FPGA_CONF_SetMapStart()
  {
    WV_S32 ret = 0;
	//outFlag = FPGA_CONF_GetOutput();
	//FPGA_CONF_SetOutput(0);
	screenMode = TSK_PLAYER_GetMode();
	
	if(screenMode != 0 && screenMode != 1)
	{ 
		WV_printf("get map end screenMode=%d\n",screenMode);
		TSK_PLAYER_ChangeMode(1);
	}   
	// WV_RET_ADD(	HIS_SPI_FpgaWd2(0x0f,0xff),ret);//  
    return ret;
  }
  
/*******************************************************************
 WV_S32 FPGA_CONF_SetMapEnd();
*******************************************************************/
 
WV_S32 FPGA_CONF_SetMapEnd()
{
	WV_S32 ret = 0;
	WV_printf("set map end screenMode=%d\n",screenMode);
	TSK_PLAYER_ChangeMode(screenMode);
	//FPGA_CONF_SetOutput(outFlag);
   // WV_RET_ADD(	HIS_SPI_FpgaWd2(0x0f,0x0f),ret);//  
    return ret;
} 
/*******************************************************************
 WV_S32 FPGA_CONF_CleanWinFram();
*******************************************************************/
 
WV_S32 FPGA_CONF_CleanWinFram()
{
	WV_S32 ret;
   WV_RET_ADD(	HIS_SPI_FpgaWd(0xe,1),ret);
   WV_printf("fpga clean win fram \n");	
	return WV_SOK;  
}
/*******************************************************************
 WV_S32 FPGA_CONF_SetWin(WV_U16  num, FPGA_WIN_INFO_S * pInfo);
*******************************************************************/
 
WV_S32 FPGA_CONF_SetWin(WV_U16  num, FPGA_WIN_INFO_S * pInfo)
{
	WV_S32 ret = 0;  
	WV_S32 i;
	WV_U16  data;
	WV_RET_ADD(	HIS_SPI_FpgaRd(0x8,&data),ret);// stop win number
	data &= 0x3;//spi1 0x08 
	if(num == 0 || data==0){

		WV_RET_ADD(	HIS_SPI_FpgaWd(0x9,num),ret);// stop win number
		WV_RET_ADD(	HIS_SPI_FpgaWd(0xb,0),ret);
		return ret;
	}

	WV_RET_ADD(	HIS_SPI_FpgaWd(0xb,0),ret);	//stop overlay
	usleep(100000);
	WV_RET_ADD(	HIS_SPI_FpgaWd(0x9,num),ret);// set outline number

	for(i=0;i<num;i++)
	{
		//printf("\n\n***********outid[%d],x[%d],y[%d],w[%d],h[%d]************\n",pInfo[i].outId,pInfo[i].x,pInfo[i].y,pInfo[i].w,pInfo[i].h);
        //
		data  =((pInfo[i].h)>>4) |((pInfo[i].outId & 0xf )<<8) | ((pInfo[i].videoId & 0x1 )<<15); 
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x20,data),ret);//bit[7..0]:h[11..4]   bit[11..8]:outId[3..0]  bit[16]:videoId[0]
      //
		data  = ((pInfo[i].w) & 0xfff)  | ((pInfo[i].h & 0xf)<<12); 
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x20,data),ret);// bit[11..0] :w[11..0]   bit[15..12] : h[3..0]  
    //
		data  = (pInfo[i].y)>>4;
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x20,data),ret);// bit[7..0]  : y[11..4]     
    //
		data  = ((pInfo[i].x) & 0xfff)  | ((pInfo[i].y & 0xf)<<12); 
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x20,data),ret);// bit[11..0] :x[11..0]   bit[15..12] : y[3..0]
     
	}

   WV_RET_ADD(	HIS_SPI_FpgaWd(0x27,0),ret);
	
    WV_RET_ADD(	HIS_SPI_FpgaWd(0xe,1),ret);
	
    for(i=0;i<1000;i++){

		TSK_FPGA_Read(0x0e,&data);
		if(data==0){
			break;	
		}
		usleep(100000);
    }	
	 WV_RET_ADD(	HIS_SPI_FpgaWd(0xb,3),ret);

	for(i=0;i<10;i++){

		TSK_FPGA_Read(0x0b,&data);
		if(data==3){
			break;	
		}
		HIS_SPI_FpgaWd(0xb,3);
		usleep(100000);
	} 	 

	return ret;
  }


  
/*******************************************************************
 WV_S32 FPGA_CONF_GetSplit( FPGA_SPLIT_INFO_S * pInfo);
*******************************************************************/

WV_S32 FPGA_CONF_GetSplitDefault( FPGA_SPLIT_INFO_S * pInfo)
{
	pInfo->startX0 = 0;
	pInfo->endX0 = 959;
	pInfo->startY0 = 0;
	pInfo->endY0   = 1079;
	
	pInfo->startX1 = 0;
	pInfo->endX1 = 959;
	pInfo->startY1 = 0;
	pInfo->endY1   = 1079;

	pInfo->startX2 = 0;
	pInfo->endX2 = 959;
	pInfo->startY2 = 0;
	pInfo->endY2   = 1079;

	pInfo->startX3 =0;
	pInfo->endX3 = 959;
	pInfo->startY3 = 0;
	pInfo->endY3   = 1079;

	pInfo->startX4 = 0;
	pInfo->endX4 = 959;
	pInfo->startY4 = 0;
	pInfo->endY4   = 1079;

	pInfo->startX5 = 0;
	pInfo->endX5 = 959;
	pInfo->startY5 = 0;
	pInfo->endY5   = 1079;

	pInfo->startX6 = 0;
	pInfo->endX6 = 959;
	pInfo->startY6 = 0;
	pInfo->endY6   = 1079;

	pInfo->startX7 = 0;
	pInfo->endX7 = 959;
	pInfo->startY7 = 0;
	pInfo->endY7   = 1079;

}
#if 1
/*******************************************************************
 WV_S32 FPGA_CONF_SetSplit( FPGA_SPLIT_INFO_S * pInfo);
*******************************************************************/

 WV_S32 FPGA_CONF_SetSplit( FPGA_SPLIT_INFO_S * pInfo)
  {
		WV_S32 ret = 0;  
		WV_S32 i,chl;
		WV_U16 ena;
		chl = FPGA_CONF_GetOutChl_Num();
		ena = FPGA_CONF_GetOutput();
		//FPGA_CONF_SetOutput(0);
	  	switch(chl){
			case 2:
				//config chl 1
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x12,pInfo->startX0),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x13,pInfo->endX0),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x40,pInfo->startY0),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x41,pInfo->endY0),ret);// 
				//config chl 2
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x14,pInfo->startX1),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x15,pInfo->endX1),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x42,pInfo->startY1),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x43,pInfo->endY1),ret);//
				//start split
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x0c,0x3f),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x25,0x0),ret);//
				break;

			case 4:
				//config split info
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x10,pInfo->startY0),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x11,pInfo->endY0),ret);//
				
				//config chl 1
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x12,pInfo->startX0),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x13,pInfo->endX0),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x40,pInfo->startY0),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x41,pInfo->endY0),ret);// 
				//config chl 2
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x14,pInfo->startX1),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x15,pInfo->endX1),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x42,pInfo->startY1),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x43,pInfo->endY1),ret);// 
				//config chl 3
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x16,pInfo->startX2),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x17,pInfo->endX2),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x44,pInfo->startY2),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x45,pInfo->endY2),ret);//
				//config chl 4
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x18,pInfo->startX3),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x19,pInfo->endX3),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x46,pInfo->startY3),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x47,pInfo->endY3),ret);// 
				//start split
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x0c,0xf),ret);//
				//WV_RET_ADD(	HIS_SPI_FpgaWd(0x25,0x0),ret);//
				//WV_RET_ADD(	HIS_SPI_FpgaWd(0x0b,0),ret);//				
				break;

			case 6:
				//config chl 1
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x12,pInfo->startX0),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x13,pInfo->endX0),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x40,pInfo->startY0),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x41,pInfo->endY0),ret);// 
				//config chl 2
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x14,pInfo->startX1),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x15,pInfo->endX1),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x42,pInfo->startY1),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x43,pInfo->endY1),ret);// 
				//config chl 3
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x16,pInfo->startX2),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x17,pInfo->endX2),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x44,pInfo->startY2),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x45,pInfo->endY2),ret);// 
				//config chl 4
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x18,pInfo->startX3),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x19,pInfo->endX3),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x46,pInfo->startY3),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x47,pInfo->endY3),ret);// 
				//config chl 5	
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x1a,pInfo->startX4),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x1b,pInfo->endX4),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x48,pInfo->startY4),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x49,pInfo->endY4),ret);// 
				//config chl 6
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x1c,pInfo->startX5),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x1d,pInfo->endX5),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x4a,pInfo->startY5),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x4b,pInfo->endY5),ret);// 		
				//start split
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x0c,0x3f),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x25,0x0),ret);//		
				break;

			case 8:
				WV_printf("FPGA_CONF_SetSplit(),channel num [%d]\n ",chl);
				break;
			default:
				WV_printf("error: FPGA_CONF_SetSplit(),channel num [%d]\n ",chl);
				break;		
		}
		//FPGA_CONF_SetOutput(ena);

    return ret;
  }  
#endif

/*******************************************************************
 WV_S32 FPGA_CONF_SetResolution(FPGA_RESOLUTION_INFO_S * pInfo);
*******************************************************************/
 
 WV_S32 FPGA_CONF_SetResolution( FPGA_RESOLUTION_INFO_S * pInfo)
  {
		WV_S32 ret = 0;  
		WV_S32 chl;
		WV_U16 ena;
		float fqM;
		WV_U32 fqL;
		WV_U32 fqH; 
	
		fqM =  pInfo->totalPix * pInfo->totalLine * pInfo->frameRate;  
		fqM =   fqM/1000000;
		fqH =(WV_U32 )fqM;
		fqM=(fqM -fqH);
		fqL = fqM*1024*64;
		fqL &= 0XFFFF; 

		chl = FPGA_CONF_GetOutChl_Num();
		ena = FPGA_CONF_GetOutput();
		switch(chl){

			case 2:
				//channel num is 2
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x8f,0),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x92,fqH),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x91,fqL),ret);// 	
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x96,pInfo->totalPix),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x97,pInfo->totalLine),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x98,pInfo->hsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x99,pInfo->vsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x9a,pInfo->actHstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x9b,pInfo->actHsize),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x9c,pInfo->actVstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x9d,pInfo->actVsize),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x93,1),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x93,0),ret);// 
				FPGA_CONF_SetOutput(ena);
				break;
			case 4:
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,0),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x12,fqH),ret);// 	
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x11,fqL),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x16,pInfo->totalPix),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x17,pInfo->totalLine),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x18,pInfo->hsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x19,pInfo->vsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1a,pInfo->actHstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1b,pInfo->actHsize),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1c,pInfo->actVstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1d,pInfo->actVsize),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x13,1),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x13,0),ret);//
				//WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,0),ret);//
				FPGA_CONF_SetOutput(ena);
				break;
			case 6:
				//set fpga 2
				WV_printf("SetResolution start \n");

				WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,0),ret);// 				
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x12,fqH),ret);// 	
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x11,fqL),ret);// 	
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x16,pInfo->totalPix),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x17,pInfo->totalLine),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x18,pInfo->hsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x19,pInfo->vsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1a,pInfo->actHstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1b,pInfo->actHsize),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1c,pInfo->actVstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1d,pInfo->actVsize),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x13,1),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x13,0),ret);//	
				//set fpga 1

				WV_RET_ADD(	HIS_SPI_FpgaWd(0x8f,0),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x92,fqH),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x91,fqL),ret);// 	
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x96,pInfo->totalPix),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x97,pInfo->totalLine),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x98,pInfo->hsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x99,pInfo->vsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x9a,pInfo->actHstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x9b,pInfo->actHsize),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x9c,pInfo->actVstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x9d,pInfo->actVsize),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x93,1),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x93,0),ret);// 
				FPGA_CONF_SetOutput(ena);
				break;		
			case 8:
				//set fpga2 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,0),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x12,fqH),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x11,fqL),ret);// 	
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x16,pInfo->totalPix),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x17,pInfo->totalLine),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x18,pInfo->hsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x19,pInfo->vsyncWidth),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1a,pInfo->actHstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1b,pInfo->actHsize),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1c,pInfo->actVstart),ret);// 
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x1d,pInfo->actVsize),ret);//  
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x13,1),ret);//
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x13,0),ret);//
				FPGA_CONF_SetOutput(ena);
				break;
			default:
				WV_printf("error: FPGA_CONF_SetResolution(),channel num is[%d]\n ",chl);	
				break;
		}
	
    return ret;
  }  
 

 /*******************************************************************
 WV_S32 FPGA_CONF_SetOutput(WV_U16  ena);
*******************************************************************/
WV_S32 FPGA_CONF_SetOutput(WV_U16  ena)
{
	ena &=  gOutput;

	//printf("set out ena %04x\n",ena);
	WV_S32 ret = 0; 
	WV_U16 data,fpga1,fpga2;
	WV_U16 rd=0;
	WV_S32 chl;
	data = ena & 0xffff;
	chl = FPGA_CONF_GetOutChl_Num();

	if(chl == 2){

		fpga1 = (data & 0x0003) | ((data & 0x0300) >>4);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x8f,fpga1),ret);// 
		usleep(10000);
		WV_RET_ADD(HIS_SPI_FpgaRd(0x8f,&rd),ret);     

		if(rd!=fpga1){
			WV_RET_ADD(	HIS_SPI_FpgaWd(0x8f,fpga1),ret);// 
		}

	}else if(chl == 4){
		fpga2 = (data & 0x000f) | ((data&0x0f00)>>4);
		//printf("set spi2 0xf=0x%2x\n",fpga2);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,fpga2),ret);// 
		usleep(10000);
		WV_RET_ADD(HIS_SPI_FpgaRd2(0xf,&rd),ret);     

		if(rd!=fpga2){
			WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,fpga2),ret);// 
		}

	}else if(chl == 6){

		fpga1 = (data & 0x0003) | ((data & 0x0300) >>4);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x8f,fpga1),ret);// 
		usleep(10000);
		WV_RET_ADD(HIS_SPI_FpgaRd(0x8f,&rd),ret);     

		if(rd!=fpga1){
			WV_RET_ADD(	HIS_SPI_FpgaWd(0x8f,fpga1),ret);// 
		}
		//
		fpga2 = ((data & 0x003c)>>2) | ((data & 0x3c00) >> 6);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,fpga2),ret);// 
		usleep(10000);
		WV_RET_ADD(HIS_SPI_FpgaRd2(0xf,&rd),ret);     

		if(rd!=fpga2){
			WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,fpga2),ret);// 
		}

	}else if(chl == 8){
		
		fpga2 = (data & 0x000f) | ((data&0x0f00)>>4);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,fpga2),ret);// 
		usleep(10000);
		WV_RET_ADD(HIS_SPI_FpgaRd2(0xf,&rd),ret);     

		if(rd!=fpga2){
			WV_RET_ADD(	HIS_SPI_FpgaWd2(0xf,fpga2),ret);// 
		}

	}	

		
	SYS_ENV_SetU32("Output_ena",data);
	//ena = data & 0xffff;
	//WV_printf("\nget ena [0x%04x],chl [%d]\n",data,chl);

	return ret;
}       
 /*******************************************************************
 WV_S32 FPGA_CONF_GetOutput(WV_U16  ena);
*******************************************************************/

WV_U16 FPGA_CONF_GetOutput()
{
	WV_U32 data;
	WV_U16 ena;
	SYS_ENV_GetU32("Output_ena",&data);
	ena = data & 0xffff;
	//printf("get out put ena %04x\n",ena);
	return ena;	
}
 /*******************************************************************
 WV_S32 FPGA_CONF_SetGlobalOutput(WV_U16  ena);
*******************************************************************/

WV_U16 FPGA_CONF_SetGlobalOutput(WV_U16 ena)
{
	WV_U16 curentEna;
	gOutput = ena & 0xff;
	gOutput |= 0xff00;

	curentEna=FPGA_CONF_GetOutput();
	curentEna &= gOutput;
	FPGA_CONF_SetOutput(curentEna);
	return WV_SOK;	
}
 /*******************************************************************
 WV_S32 FPGA_CONF_GetVersion(FPGA_CONF_VERSION *pDev);
*******************************************************************/
WV_S32 FPGA_CONF_GetVersion()
{
	WV_S32 ret = 0;
	WV_U16 ver;
	WV_U16 id;
	WV_S32 i;
	WV_S32 num=0;	
	WV_U16 readFpga1 = 0;
	WV_U16 readFpga2 = 0;

	for(i=0;i<50;i++){

		HIS_SPI_FpgaRd(0x80,&readFpga1);
		HIS_SPI_FpgaRd2(0x80,&readFpga2);			
		num++;
		if((readFpga1 == 0x5aa5) && (readFpga2==0x5aa5)){
		
			break;
		}
		usleep(100000);
		
	}


	WV_RET_ADD(HIS_SPI_FpgaRd(0x00,&id),ret);
	WV_RET_ADD(HIS_SPI_FpgaRd(0x01,&ver),ret);
	WV_printf("\nfpga id [0x%04x]\n",id);
 	if(ret != 0){
		WV_RET_ADD(HIS_SPI_FpgaRd(0x00,&id),ret);
		WV_RET_ADD(HIS_SPI_FpgaRd(0x01,&id),ret);
	}
	gVersion.id = id;
	gVersion.ver = ver;
	//num
	if(gVersion.id == 0x5f75){//use overlay
		gVersion.num = 4;		
	}else if(gVersion.id == 0x7532){//
		gVersion.num = 6;		
	}else if(gVersion.id == 0x3132)
	{
		gVersion.num = 6;
	}
	if(gVersion.num == 6){

		if(readFpga2 == 0x5aa5){
			HIS_SPI_FpgaWd(0x34,1);//  	
		
		} else{
			HIS_SPI_FpgaWd(0x34,0);//
			gVersion.num = 2; 
		}
	}
	//WV_printf("\nchannel num is [%d]\n",gVersion.num);
}
 /*******************************************************************
 WV_S32 FPGA_CONF_GetVersion(WV_U16  ver);
*******************************************************************/
WV_S32 FPGA_CONF_GetOutChl_Num()
{
		WV_S32 num;
		num = gVersion.num;
		if((num != 2 ) && (num != 4) && (num != 6) && (num != 8 ) ){
			num = -1;
		}
		return num;
}
 /*******************************************************************

WV_S32 FPGA_CONF_GetVersion_ID();

	id=0 there is no input (except 4chn )
	id=1 there is one input 
*******************************************************************/
WV_S32 FPGA_CONF_GetVersion_ID()
{
		WV_S32 id;
		//id = gVersion.id;
		if((gVersion.id == 0x5f75) || (gVersion.id == 0x7532))
		{
			id = 0;
		}
		
		if(gVersion.id == 0x3132)
		{
			id = 1;
		}
		return id;
}













