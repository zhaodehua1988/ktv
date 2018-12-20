#include"svr_cmd.h"
#include"tsk_fpga.h"
#include"his_fb.h"
#include "svr_cmd_define.h"  
#include "tsk_go.h"
#include "sys_ip.h"
#include "svr.h"
#include "fpga_conf.h"
#include "tsk_conf.h"
#include "tsk_scene.h"
#include "fpga_conf.h"
#include "wv_ver.h"
#include "sys_info.h"
#include "tsk_mobileShow.h"
#include "svr_control_cmd.h"
#include "fpga_update.h"
#include "wv_sqlite3.h"
#include "tsk_text.h"
#include "sys_env.h"
#include "sys_file.h"
#define  SVR_CMD_MAX_NUM    128
#define  POINT_MAX_COL      128
#define  POINT_MAX_RAW      128
#define  POINT_SHIFT        1920


static SVR_CMD_FUNC_E  gFuncTable[SVR_CMD_MAX_NUM]; 
/********************************************************************
WV_S32  SVR_CMD_Ack(SVR_FRAME_HEAD_E * pHead); 
********************************************************************/
WV_S32  SVR_CMD_Ack(SVR_FRAME_HEAD_E * pHead,WV_S32 erro)
{ 	

	 pHead -> cmdL0 |= 0x80;
	 if(erro != WV_SOK)
	  {
	  pHead -> cmdL0 |= 0x40;  
	  }
	  
	pHead -> dataNum =0; 
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_HandProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_HandProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{   
	WV_S32 num,verLen,id;
	WV_S8 version[20];
	
 	num = FPGA_CONF_GetOutChl_Num();
	id = FPGA_CONF_GetVersion_ID();
	if(num < 0){
		pHead ->cmdL0 |= 0x40;
	}	
	pHead -> cmdL0 |= 0x80;
	//argv1 hello version  
        SYS_ENV_GetVersion(&pHead->arg1);
        printf("arg1 = [%04x]\n",pHead->arg1);

	//argv2 fpga version;	
	pHead -> arg2 = FPGA_CONF_GerVer();
	//chn num
	if(id == 0)
	{
		pHead->cmdL1 = num;
	}
	if(id == 1)
	{
		pHead->cmdL1 = num*10 + id;
	}
	pHead->dataNum = 5;
	SYS_LIC_GetTimeOut(pData);
    pHead->dataNum = 25;
    memcpy(&pData[6], SOFTVER_DATE, 10);
    pData[16] = 0x20;
    memcpy(&pData[17], SOFTVER_TIME, 8);

	return WV_SOK;
}
/********************************************************************

WV_S32  SVR_CMD_MapProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32  SVR_CMD_MapProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)

{ 

	WV_U8  channel,cmd;
	WV_U32 len; 
	WV_S32 ret = 0;; 
	WV_S32 num;

 	num = FPGA_CONF_GetOutChl_Num();	 
	len  =  pHead->dataNum;
	cmd  =  (pHead->cmdL1);

	channel = (pHead -> cmdL1) & 0x0f;
	if(channel > (num -1))
	{
		SVR_CMD_Ack(pHead,-1);
		return WV_SOK;
	}
	
	FPGA_CONF_SetMapStart();
	if((cmd & 0x80)  == 0x80)
	{
		
		WV_RET_ADD( TSK_FPGA_TransMap(pData,len),ret); 
		WV_RET_ADD( TSK_FPGA_SetMap(channel),ret); 
		WV_RET_ADD(HIS_FB_ClrFpga(),ret);
	 }  
	if((cmd & 0x40)  == 0x40) 
	{
		
		WV_RET_ADD( TSK_FPGA_TransMap(pData,len),ret); 
		WV_RET_ADD( TSK_FPGA_SetMap(channel),ret); 
		WV_RET_ADD( HIS_FB_ClrFpga(),ret);
		WV_RET_ADD( TSK_FPGA_SaveMap(channel),ret); 
	} 
    FPGA_CONF_SetMapEnd();
	HIS_FB_ClrFpga();
	SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}

/********************************************************************

WV_S32  SVR_CMD_OutProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32  SVR_CMD_OutProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{  
	WV_U8  channel,cmd;
	WV_U32 len; 
	WV_S32 ret = 0;; 
	WV_S32 num,id;
	
	num = FPGA_CONF_GetOutChl_Num();
	id = FPGA_CONF_GetVersion_ID();
	WV_U32 playerMode;
	if((num == 2) || (num == 6)){
		if(id != 1)
		{
			WV_printf("error: SVR_CMD_OutProc(),fpga channel num is [%d]\n",num);
			SVR_CMD_Ack(pHead,-1);     	 
			return WV_SOK;
		}		
	}	 

	len  =  pHead->dataNum;
	cmd  =  pHead->cmdL1;
	channel = (pHead -> cmdL1) & 0x0f;

	playerMode =TSK_PLAYER_GetMode();

	if((cmd & 0x80)  == 0x80)
	{
		WV_RET_ADD( TSK_FPGA_TransOut(pData,len),ret); 
		FPGA_CONF_SetOutlineStart();		 	
		WV_RET_ADD( TSK_FPGA_SetOut(channel),ret); 
		WV_RET_ADD( HIS_FB_ClrFpga(),ret);
		FPGA_CONF_SetOutlineEnd();
	 }  

	if((cmd & 0x40)  == 0x40) 
	{
		WV_RET_ADD( TSK_FPGA_TransOut(pData,len),ret); 
		FPGA_CONF_SetOutlineStart(); 	
		WV_RET_ADD( TSK_FPGA_SetOut(channel),ret); 
		WV_RET_ADD( HIS_FB_ClrFpga(),ret);
		FPGA_CONF_SetOutlineEnd();
		WV_RET_ADD( TSK_FPGA_SaveOut(channel),ret);   
	}
 
	SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_SplitProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_SplitProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{  
	WV_U8  channel,cmd;
	WV_U32 len; 
	WV_S32 ret = 0;; 
	WV_S32 num;

	num = FPGA_CONF_GetOutChl_Num();
	len  =  pHead->dataNum;
	cmd  =  pHead->cmdL1;
	channel = (pHead -> cmdL1) & 0x0f;
	if(((channel < 8) && (channel < num)) ||((channel >=8 ) && ((channel-8) < num))) {
     
		if((cmd & 0x80)  == 0x80)
		{
			WV_RET_ADD( TSK_FPGA_TransLight(pData,len),ret); 
			WV_RET_ADD( FPGA_CONF_SetLightStart(),ret);	
			WV_RET_ADD( TSK_FPGA_SetLight(channel),ret);
			WV_RET_ADD(HIS_FB_clrFillRect(),ret);
			WV_RET_ADD( FPGA_CONF_SetLightEnd(),ret); 	
			//WV_RET_ADD( HIS_FB_ClrFpga(),ret);
		}      

		if((cmd & 0x40)  == 0x40) 
		{
		   WV_RET_ADD( TSK_FPGA_TransLight(pData,len),ret); 
			WV_RET_ADD( FPGA_CONF_SetLightStart(),ret);	
			WV_RET_ADD( TSK_FPGA_SetLight(channel),ret);
			WV_RET_ADD(HIS_FB_clrFillRect(),ret);
			WV_RET_ADD( FPGA_CONF_SetLightEnd(),ret); 
			WV_RET_ADD( TSK_FPGA_SaveLight(channel),ret); 
		}
	}
	//debug mode	
	if((cmd & 0x20)  == 0x20) 
	{
		WV_U32* pRcv;
		pRcv = (WV_U32*)pData;
		WV_U32 colorNum;
		//judge recve color len whether equal colorNum	
		//WV_printf("pRcv0[%d],pRcv1[%d],pRcv2[%08x]\n",pRcv[0],pRcv[1],pRcv[2]);
		switch(pRcv[0]){
			case  1: 
			   //rect back color
			   if((pRcv[1]*4+8) == len){
				colorNum = pRcv[1];				
				pRcv+=2;
				
				WV_RET_ADD(TSK_GO_FillRectDebug(colorNum,pRcv),ret);
				
			   }else{

				WV_printf("error: color num[%d],recv color[%d]\n",pRcv[1],(len -8)/4);			
			       }
			    break;  

			case  2:
				//draw line for light area
				
				ret = TSK_GO_DrawPoitForLight(pRcv[1],pRcv[2],pRcv[3]);//WV_S32 TSK_GO_DrawPoitForLight(WV_U32 color,WV_U32 x,WV_U32 y);
				
				break;
			default :
			    break;
		
		}
		//WV_RET_ADD( TSK_GO_FillRectLine(),ret);
	}

	SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_ColorProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_ColorProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{  
	WV_U8  channel,cmd;
	WV_U32 len; 
	WV_S32 ret = 0;
	WV_S32 num;

	num = FPGA_CONF_GetOutChl_Num();	
	len  =  pHead->dataNum;
	cmd  =  pHead->cmdL1;
	channel = (pHead -> cmdL1) & 0x0f;
	if(channel > (num-1)){
		SVR_CMD_Ack(pHead,-1);
		return WV_SOK;			
	}
	//trans mode
	if((cmd & 0x80)  == 0x80)
	{
	   	WV_RET_ADD( TSK_FPGA_TransColor(pData,len),ret); 	
			WV_RET_ADD( TSK_FPGA_SetColor(channel),ret);
        	WV_RET_ADD( HIS_FB_ClrFpga(),ret); 
	}       
	//save mode
	if((cmd & 0x40)  == 0x40) 
	{
		
		WV_RET_ADD( TSK_FPGA_TransColor(pData,len),ret); 		
		WV_RET_ADD( TSK_FPGA_SetColor(channel),ret);
        	WV_RET_ADD( HIS_FB_ClrFpga(),ret); 
		WV_RET_ADD( TSK_FPGA_SaveColor(channel),ret); 
	}
	//debug mode
	if((cmd & 0x20) == 0x20)
	{
		WV_U16 *pRcv=(WV_U16 *)pData;
		if(len < 16){
			printf("error: color debug loadlen=[%d]\n",len);
			ret = -1;		
			SVR_CMD_Ack(pHead,ret);			
			return WV_SOK;
		}
		WV_U16 borderColor = pRcv[0];
		WV_U16 splitNum = pRcv[1];
		FILLRECT_GRID grid;
		//splitNum 
		if(len < 16+splitNum*4){
			printf("error: color debug loadlen=[%d]\n",len);
			ret = -1;		
			SVR_CMD_Ack(pHead,ret);			
			return WV_SOK;		
		}		
				
		WV_U32 i=0;
		printf("\r\nColor Debug \n");
		/*		
		for(i=0;i<len/2;i++){
			printf("pRcv[%d] = [%d]",i,pRcv[i]);

		}
		*/
		grid.w     = pRcv[2];
		grid.h     = pRcv[3];
		grid.row   = pRcv[4];
		grid.col   = pRcv[5];
		grid.start = pRcv[6];
		grid.end   = pRcv[7];
		pRcv += 8;
		for(i=0; i<splitNum; i++){
		
			grid.x[i]  = pRcv[0];
			grid.y[i]  = pRcv[1];
			pRcv+=2;
		}

		WV_RET_ADD(TSK_GO_FillRect_Start(),ret);
		WV_RET_ADD(TSK_GO_FillRectGrid(borderColor,splitNum,grid),ret);
	}
	
	SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}


/********************************************************************
WV_S32  SVR_CMD_OutResolutionProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_OutResolutionProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)

{  

	WV_U32 len; 
	WV_S32 ret = 0;; 
	FPGA_RESOLUTION_INFO_S outRes; 
	WV_U16 *pRcv;
	pRcv=(WV_U16*)pData;
	len  =  pHead->dataNum;

	outRes.frameRate = pHead->arg1;
	outRes.totalPix = pRcv[0];
	outRes.totalLine = pRcv[1];
	outRes.hsyncWidth = pRcv[2];
	outRes.vsyncWidth = pRcv[3];
	outRes.actHstart = pRcv[4];
	outRes.actHsize = pRcv[5];
	outRes.actVstart = pRcv[6];
	outRes.actVsize = pRcv[7];

	WV_RET_ADD(TSK_FPGA_ResolutionConf (outRes),ret);

	SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}
/********************************************************************
WV_S32 SVR_CMD_splitConf_Check(FPGA_SPLIT_INFO_S info);
********************************************************************/
WV_S32 SVR_CMD_splitConf_Check(FPGA_SPLIT_INFO_S info)
{
	WV_S32 ret=0;
	
	if(info.startY0 > 2160 || info.endY0 > 2160 ||\
		info.startY1 > 2160 || info.endY1 > 2160 ||\
		info.startY2 > 2160 || info.endY2 > 2160 ||\
		info.startY3 > 2160 || info.endY3 > 2160 ||\
		info.startY4 > 2160 || info.endY4 > 2160 ||\
		info.startY5 > 2160 || info.endY5 > 2160 ||\
		info.startY6 > 2160 || info.endY6 > 2160 ||\
		info.startY7 > 2160 || info.endY7 > 2160 ||\

		info.startX0 > 1919 || info.endX0 > 1919 ||\
		info.startX1 > 1919 || info.endX1 > 1919 ||\
		info.startX2 > 1919 || info.endX2 > 1919 ||\
		info.startX3 > 1919 || info.endX3 > 1919 ||\
		info.startX4 > 1919 || info.endX4 > 1919 ||\
		info.startX5 > 1919 || info.endX5 > 1919 ||\
		info.startX6 > 1919 || info.endX6 > 1919 ||\
		info.startX7 > 1919 || info.endX7 > 1919  )
	{

		WV_printf("----->>SVR_CMD_splitConf_Check() error \n");
		return WV_EFAIL;
	
	}
#if 0
			printf("\n");	
		printf("startX0=[%d],endX0=[%d],startY0=[%d],endY0=[%d]\n",info.startX0,info.endX0,info.startY0,info.endY0);	
		printf("startX1=[%d],endX1=[%d],startY1=[%d],endY1=[%d]\n",info.startX1,info.endX1,info.startY1,info.endY1);	
		printf("startX2=[%d],endX2=[%d],startY2=[%d],endY2=[%d]\n",info.startX2,info.endX2,info.startY2,info.endY2);	
		printf("startX3=[%d],endX3=[%d],startY3=[%d],endY3=[%d]\n",info.startX3,info.endX3,info.startY3,info.endY3);	
		printf("startX4=[%d],endX4=[%d],startY4=[%d],endY4=[%d]\n",info.startX4,info.endX4,info.startY4,info.endY4);	
		printf("startX5=[%d],endX5=[%d],startY5=[%d],endY5=[%d]\n",info.startX5,info.endX5,info.startY5,info.endY5);	
		printf("startX6=[%d],endX6=[%d],startY6=[%d],endY6=[%d]\n",info.startX6,info.endX6,info.startY6,info.endY6);	
		printf("startX7=[%d],endX7=[%d],startY7=[%d],endY7=[%d]\n",info.startX7,info.endX7,info.startY7,info.endY7);	
		
#endif

		return WV_SOK;

}

/********************************************************************
WV_S32  SVR_CMD_splitConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_splitConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{  

	WV_U32 len; 
	WV_S32 ret = 0;
	WV_U16 *pRcv;
	WV_U16  ena;
	WV_U8  cmd;
	cmd = pHead->cmdL1;
	ena  =  pHead->arg1 & 0xffff;
	WV_U16 x,y,w,h;

	WV_S32 i;
	len  =  pHead->dataNum;
	FPGA_SPLIT_INFO_S info;
	pRcv=(WV_U16*)pData;
	if(len != 96){
		WV_printf(" len is [%d]\n",len);
		ret = -1;
		SVR_CMD_Ack(pHead,ret);
	     	 
		return WV_SOK;
	}
	if(cmd > 1){
		WV_printf("split cmd-->[%02x]\n",cmd);
		ret = -1;
   	    SVR_CMD_Ack(pHead,ret);
		return ret;
	}
	//set output ena



	info.startX0 = pRcv[16];
	info.endX0 = pRcv[17];
	info.startY0 = pRcv[18];
	info.endY0 = pRcv[19];

	info.startX1 = pRcv[20];
	info.endX1 = pRcv[21];
	info.startY1 = pRcv[22];
	info.endY1 = pRcv[23];

	info.startX2 = pRcv[24];
	info.endX2 = pRcv[25];
	info.startY2 = pRcv[26];
	info.endY2 = pRcv[27];

	info.startX3 = pRcv[28];
	info.endX3 = pRcv[29];
	info.startY3 = pRcv[30];
	info.endY3 = pRcv[31];

	info.startX4 = pRcv[32];
	info.endX4 = pRcv[33];
	info.startY4 = pRcv[34];
	info.endY4 = pRcv[35];

	info.startX5 = pRcv[36];
	info.endX5 = pRcv[37];
	info.startY5 = pRcv[38];
	info.endY5 = pRcv[39];

	info.startX6 = pRcv[40];
	info.endX6 = pRcv[41];
	info.startY6 = pRcv[42];
	info.endY6 = pRcv[43];

	info.startX7 = pRcv[44];
	info.endX7 = pRcv[45];
	info.startY7 = pRcv[46];
	info.endY7 = pRcv[47];


	if(cmd == 0)
	{
		//config output 
		WV_RET_ADD(TSK_FPGA_outConf(ena),ret);
		//config mov 
		for(i=0;i<3;i++)
		{
			x = pRcv[i*4+0];
			y = pRcv[i*4+1];
			w = pRcv[i*4+2];
			h = pRcv[i*4+3];
			WV_printf("mov[%d]set mov x[%d]y[%d]w[%d]h[%d]\n",i,x,y,w,h);
			if(((x+w)<=3840) && ((y+h)<= 2160) && ( w >= 200) && (w<=3840) && (h >= 200)&& (h<=2160)){
			
				TSK_SCENE_SetMovArea(i,1,x,y,w,h);
				TSK_SCENE_ConfMov(i);
			}else{
			
				TSK_SCENE_SetMovArea(i,0,x,y,w,h);
				TSK_SCENE_ConfMov(i);
			}
		}
		//config split
			ret =  SVR_CMD_splitConf_Check(info);//check info and config
			ret = TSK_FPGA_SplitConf (info ); //splitconf ena

		//cmd == 0 fpga reset
		//FPGA_CONF_ResetA();

	}else{
			//config output 
			WV_RET_ADD(TSK_FPGA_outConf(ena),ret);
			//config mov
			for(i=0;i<3;i++)
			{
				x = pRcv[i*4+0];
				y = pRcv[i*4+1];
				w = pRcv[i*4+2];
				h = pRcv[i*4+3];

				if(((x+w)<=3840) && ((y+h)<= 2160) && ( w >= 200) && (w<=3840) && (h >= 200)&& (h<=2160)){
					//WV_printf("mov[%d]set mov x[%d]y[%d]w[%d]h[%d]\n",i,x,y,w,h);
					TSK_SCENE_SetMovArea(i,1,x,y,w,h);
				}else{
	
					TSK_SCENE_SetMovArea(i,0,x,y,w,h);					
				}
			}

			//config split
			ret =  SVR_CMD_splitConf_Check(info);//check info and config
			//ret = TSK_FPGA_SplitConf (info ); //splitconf ena
			ret =  TSK_FPGA_SavePointSplit(info); //splitconf not save and set startX=960,endX=1919,startY=0,endY=1079;
			
			
	}
	
	SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}


/********************************************************************
WV_S32 SVR_CMD_windowConf_Check(FPGA_SPLIT_INFO_S info);
********************************************************************/

WV_S32 SVR_CMD_windowConf_Check(WV_S32 i,FPGA_WIN_INFO_S info)
{
	
	WV_S32 ret = 0;

	TSK_SCENE_SetWin(i,info.outId,info.x,info.y, info.w, info.h, info.videoId);

	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_windowConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_windowConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{  
	WV_S32 i;
	WV_U32 len; 
	WV_S32 ret = 0;
	WV_U16 *pRcv;
	WV_U32 winNum;	 
	len  =  pHead->dataNum;
        FPGA_WIN_INFO_S info;
	pRcv=(WV_U16*)pData;
	pRcv+=4;  //
	winNum = pHead -> arg1;//

	TSK_SCENE_SetWinNum(winNum);

	for(i=0; i<winNum; i++)
	{
		info.x=pRcv[i*6];
		info.y=pRcv[i*6+1];
		info.w=pRcv[i*6+2];
		info.h=pRcv[i*6+3];
		info.outId=pRcv[i*6+4];
		info.videoId=pRcv[i*6+5];

		ret = TSK_SCENE_SetWin(i,info.outId,info.x,info.y, info.w, info.h, info.videoId);
		if(ret != 0 )
		{
     		SVR_CMD_Ack(pHead,ret);
			return WV_SOK;
		}
	}

	 WV_RET_ADD(TSK_SCENE_ConfWin(),ret);//Config window pasition

     SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}
/********************************************************************
WV_S32  SVR_CMD_movConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/

WV_S32  SVR_CMD_movConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)

{  

	WV_U32 len; 
	WV_S32 ret = 0;
	WV_U16 *pRcv;
	WV_U32 id;

	WV_U32 winNum;	 
	len  =  pHead->dataNum;

	pRcv=(WV_U16*)pData;

	id = pHead->arg1;
	if((id<0) || (id > 128)){

		ret = -1;				
		SVR_CMD_Ack(pHead,ret);		

	}else{
		
	 WV_RET_ADD(TSK_SCENE_SetMov(0,id),ret);
     WV_RET_ADD(TSK_SCENE_ConfMov(0),ret);
	
	}	

        SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}


/********************************************************************
WV_S32  SVR_CMD_gifConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_gifConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)

{  

	WV_S32 i;
	WV_U32 len; 
	WV_S32 ret = 0;
	WV_U16 *pRcv;
	WV_U32 num,id,x,y,w,h;	 

	len  =  pHead->dataNum;
	pRcv=(WV_U16*)pData;
	num = pHead->arg1;
	//WV_printf("aninum [%d]\n",num);
	TSK_SCENE_SetAniNum(num);

	for(i=0; i<num;i++)
	{
		x = pRcv[i*5];
		y = pRcv[i*5+1];
		w = pRcv[i*5+2];
		h = pRcv[i*5+3];
		id = pRcv[i*5+4];
		//WV_printf("x[%d]y[%d]w[%d]h[%d],id[%d]\n",x,y,w,h,id);

		WV_RET_ADD(TSK_SCENE_SetAni(i,id,x,y,w,h),ret);
	}
	
	WV_RET_ADD(TSK_SCENE_ConfAni(),ret);
     
        SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_sceneConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_sceneConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)

{  

	WV_U32 cmd;
	WV_U32 id;
	WV_U32 ret = 0;	
	cmd = pHead->cmdL1 & 0x03;
	id = pHead->arg1;
	
	
	//printf("------>SVR_CMD_sceneConfProc  cmd = [%d],id = [%d]\n",cmd,id);
	if(id > TSK_SCENE_MAX_NUM ){
		
		WV_printf("error scene id [%d]\n",id);
		ret = -1;
		SVR_CMD_Ack(pHead,ret);
		return WV_EFAIL;
	}
	//printf("scene cmd [%d\n],id[%d\n]",cmd,id);

	switch(cmd){

		case 0 :  WV_RET_ADD(TSK_SCENE_Change(TSK_SCENE_TYPE_NETDATA, id),ret);
			break;
		case 1 : WV_RET_ADD(TSK_SCENE_Save(id),ret);
			break;
		case 2:  WV_RET_ADD(TSK_SCENE_SetDefault(id),ret);
			break;
		default : WV_printf("set scene error cmd \n");			
			break;	
	}

	SVR_CMD_Ack(pHead,ret);
     	 
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_IPConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_IPConfProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	
	WV_U8 *pRcv;
	pRcv = pData;

	WV_U8 ip[20];
	WV_U8 mask[20];
	WV_U8 gateWay[20];
	WV_U8 dns[20];
	WV_U8 mac[20];
	WV_U16 *pPort;

	
						
	sprintf(ip,"%d.%d.%d.%d",pRcv[0],pRcv[1],pRcv[2],pRcv[3]);
	sprintf(mask,"%d.%d.%d.%d",pRcv[4],pRcv[5],pRcv[6],pRcv[7]);
	sprintf(gateWay,"%d.%d.%d.%d",pRcv[8],pRcv[9],pRcv[10],pRcv[11]);
	sprintf(dns,"%d.%d.%d.%d",pRcv[12],pRcv[13],pRcv[14],pRcv[15]);
	sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",0,pRcv[17],pRcv[18],pRcv[19],pRcv[20],pRcv[21]);//mac:00:**:**:**:**:**
	
   pPort = pRcv + 22;

	WV_printf("ip:%s\n",ip);
	WV_printf("mask:%s\n",mask);
	WV_printf("gateWay:%s\n",gateWay);
	WV_printf("dns:%s\n",dns);
	WV_printf("mac:%s\n",mac);
	WV_printf("port:%d\n",*pPort);
	
	//port
	
	WV_RET_ADD(SVR_PORT_SetConf(*pPort),ret);
	WV_RET_ADD(SVR_PORT_SaveConf(),ret);	

	//ip
	
	WV_RET_ADD(SYS_IP_SetIp(ip),ret); //ip
	if((pRcv[17]==0) && (pRcv[18]==0) && (pRcv[19]==0) && (pRcv[20]==0) && (pRcv[21]==0) ){
		WV_printf("mac error : [%s] \n",mac);
	}else{
		//WV_RET_ADD(SYS_IP_SetMac(mac),ret);//mac	
	}

	WV_RET_ADD(SYS_IP_SetMask(mask),ret);//mask
	WV_RET_ADD(SYS_IP_SetGw(gateWay),ret);//GW
	
	//SVR_CMD_Ack(pHead,ret);
	WV_RET_ADD(SYS_IP_SaveConf(),ret);
	ret = 0 ;
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;

}

/********************************************************************
WV_S32  SVR_CMD_DrawLineProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_DrawLineProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{

	WV_S32 ret = 0;
	WV_S32 cmd;
	WV_U16 chl;
	//WV_U8 mapEna;
	//WV_U8 drawEna;
	FPGA_SPLIT_INFO_S info	;	

	WV_U16 *pRcv16;
	WV_U32 *pRcv32;

	//WV_U16 *pBuf;
	WV_U32 backColor;
	WV_U32 lineColor;
	WV_U16 col;
	WV_U16 row;
	WV_U32 len;
	
	WV_U32 EllipseNum;
	len = pHead->dataNum;
	
	cmd = pHead->cmdL1;
	
	if(cmd<0 || cmd > 3)
	{
		WV_printf("SVR_CMD_DrawLineProc cmd or chl is error cmd[%d]chl[%d]\n",cmd,chl);	
		ret = WV_EFAIL;		
		//SVR_CMD_Ack(pHead,ret);
		//return WV_SOK;
	}
	
	if(cmd == 0){
			
		WV_printf("************ draw grid Debug Mode*********\n");
		//DebugFun
		TSK_GO_DrawGrid_Start();
		
	}
	if(cmd == 1){

	WV_printf("************ draw grid Debug exit*********\n");
	//Debug exit
		TSK_GO_DrawGrid_EXIT();
	
	}
	if(cmd == 2){
	//drawline


		chl = pHead->arg1;
		pRcv16 = (WV_U16 *)pData;
		pRcv32 = (WV_U32 *)pData;

		backColor = ntohl(pRcv32[2042]);
		lineColor = ntohl(pRcv32[2043]);

	
		col   = pRcv16[4088];
		row    = pRcv16[4089];
	
		TSK_GO_DrawGrig(chl,backColor,lineColor,row ,col,(WV_U16 *)&pRcv16[4090]);
		//draw ellipse[rect]
		EllipseNum = pRcv16[0];
		WV_S32 i,ellipseColor;
		ellipseColor=ntohl(pRcv32[1]);
		//printf("ellipseNum = %d\n",EllipseNum);
		for(i=0;i<EllipseNum;i++)
		{
			TSK_GO_DrawEllipse(pRcv16[2*i+4],pRcv16[2*i+1+4],5,lineColor);
			
		}
		//refresh layer
		TSK_GO_RefreshLayer();
	}
	if(cmd==3){
	//set
		pRcv32 = (WV_U32 *)pData;

		WV_printf("rcv = [0x%08x]",pRcv32[0]);
		//set mapEna;
		
		WV_U16 mapEna;
		WV_U8 drawEna; 
		mapEna = pRcv32[0] & 0xffff;
		WV_RET_ADD(TSK_FPGA_PointoutConf(mapEna),ret);
		drawEna = (pRcv32[0] & 0x00ff0000)>>16;
		
		//set split
		info.startY0 = 0;
		info.endY0 = 1079;
		info.startY1 = 0;
		info.endY1 = 1079;
		info.startY2 = 0;
		info.endY2 = 1079;
		info.startY3 = 0;
		info.endY3 = 1079;
		info.startY4 = 0;
		info.endY4 = 1079;
		info.startY5 = 0;
		info.endY5 = 1079;
		info.startY6 = 0;
		info.endY6 = 1079;
		info.startY7 = 0;
		info.endY7 = 1079;
		//chl 0 
		if((drawEna & 0x01)== 0 ){
			info.startX0 = 0;
			info.endX0 = 959;		
		}else{
			info.startX0 = 960;
			info.endX0 = 1919;
		}
		//chl 1 
		if((drawEna & 0x02)== 0){
			info.startX1 = 0;
			info.endX1 = 959;		
		}else{
			info.startX1 = 960;
			info.endX1 = 1919;
		}
		//chl 2 
		if((drawEna & 0x04)== 0 ){
			info.startX2 = 0;
			info.endX2 = 959;		
		}else{
			info.startX2 = 960;
			info.endX2 = 1919;
		}
		//chl 3 
		if((drawEna & 0x08)== 0 ){
			info.startX3 = 0;
			info.endX3 = 959;		
		}else{
			info.startX3 = 960;
			info.endX3 = 1919;
		}
		//chl 4 
		if((drawEna & 0x10)== 0 ){
			info.startX4 = 0;
			info.endX4 = 959;		
		}else{
			info.startX4 = 960;
			info.endX4 = 1919;
		}
			//chl 5 
		if((drawEna & 0x20)== 0 ){
			info.startX5 = 0;
			info.endX5 = 959;		
		}else{
			info.startX5 = 960;
			info.endX5 = 1919;
		}
			//chl 6 
		if((drawEna & 0x40)== 0 ){
			info.startX6 = 0;
			info.endX6 = 959;		
		}else{
			info.startX6 = 960;
			info.endX6 = 1919;
		}
			//chl7 
		if((drawEna & 0x80)== 0 ){
			info.startX7 = 0;
			info.endX7 = 959;		
		}else{
			info.startX7 = 960;
			info.endX7 = 1919;
		}
	
		WV_RET_ADD( TSK_FPGA_SetPointSplit(info),ret);
		
	}

	//WV_printf("cmd1 is [%d\n]" ,pHead->cmdL1);
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;

}
/********************************************************************
WV_S32  SVR_CMD_lightConf(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_lightConf(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{

	WV_U16 cmd;
	WV_U32 ret = 0;	
	cmd = pHead->cmdL1;
	//WV_printf("cmd is [%d]\n",cmd);
	if(cmd > 1){

		return WV_EFAIL;	
	}
	
	WV_RET_ADD(TSK_FPGA_lightConf(cmd),ret);
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;


}

/********************************************************************
WV_S32  SVR_CMD_FpgaUpdate(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_FpgaUpdate(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;

	switch(pHead->cmdL1)
	{
		case 0:	
			ret = TSK_UPDATE_FPGAGetUpdateNum(&pHead->arg1,&pHead->arg2);
			break;
		case 1:
		case 2:
			ret = TSK_UPDATE_FPGA_RecvBin(pHead->cmdL1,pHead->dataNum,pData);		
			break;
		default:
			break;

	}
	SVR_CMD_Ack(pHead,ret);

	return 0;
}

/********************************************************************
WV_S32  SVR_CMD_GetTemp(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetTemp(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	WV_U16 cpu;
	WV_U16 fpgaIn;
	WV_U16 fpgaOut;

	TSK_FPGA_GetTemp(&cpu,&fpgaIn,&fpgaOut);
	//WV_printf("cpu[%d],in[%d],out[%d]\n",cpu,fpgaIn,fpgaOut);
	pHead->arg1 = cpu;
	pHead->arg2 = (fpgaOut & 0x00ff) |((fpgaIn&0x00ff)<<8);

	WV_printf(" %04x \n",pHead->arg2);
	SVR_CMD_Ack(pHead,ret);
	return ret;
}

/********************************************************************
WV_S32  SVR_CMD_GetResolutionProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetResolutionProc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	WV_printf("get resolution cmd \n");	
	WV_U32 dataLen=0;
	WV_U8 pBuf[20];
	memset(pBuf,0,sizeof(pBuf));
	
	ret = TSK_CONF_GetResolution(pBuf,&dataLen);

	pHead -> cmdL0 |= 0x80;
	if(ret!= WV_SOK)
	  pHead -> cmdL0 |= 0x40;  
	  
	  
  pHead -> dataNum =dataLen;
  if(dataLen >0){
		memcpy(pData,pBuf,dataLen);
	} 
	return WV_SOK;
}
/********************************************************************
WV_S32  SVR_CMD_GetSplitConf(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetSplitConf(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	WV_printf("get GetSplitConf cmd \n");
	WV_U32 dataLen=0;
	WV_U8 pBuf[256];
	memset(pBuf,0,sizeof(pBuf));
	//get split info
	ret = TSK_CONF_GetSplit(pBuf,&dataLen);
	
	pHead -> cmdL0 |= 0x80;
	if(ret!= WV_SOK)
	  pHead -> cmdL0 |= 0x40;
  	pHead->dataNum = dataLen;
	if(dataLen > 0){
		memcpy(pData,pBuf,dataLen);
	}
	return WV_SOK;

}
/********************************************************************
WV_S32  SVR_CMD_GetMovInfo(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetMovInfo(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0,i;
	WV_printf("get GetMovInfo cmd \n");
	
	WV_U32 dataLen=0;
	
	//get mov info
	ret = TSK_CONF_GetMovInfo(pData,&dataLen);
	//get player vol
	WV_S32 vol[3];
	for(i=0;i<3;i++)
	{
		TSK_PLAYER_GetVolume(&vol[i],i);
	}
	pHead -> cmdL1 = (WV_U8)vol[0];
	pHead -> arg1 = (WV_U16)vol[1];
	pHead -> arg2 = (WV_U16)vol[2];

	pHead -> cmdL0 |= 0x80;
	if(ret!= WV_SOK)
	  pHead -> cmdL0 |= 0x40;

  	pHead->dataNum = dataLen;

	return WV_SOK;

}


/********************************************************************
WV_S32  SVR_CMD_UpdateMov(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_UpdateMov(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	WV_U32 dataLen;
	dataLen = pHead->dataNum;
	switch(pHead->cmdL1)
	{
		case 0:
			//get mov name
			ret = TSK_CONF_UpdateMovName(pData,dataLen);
			break;
		case 1:
			//get mov data
			ret = TSK_CONF_UpdateMov(pData,dataLen);
			break;
		case 2:
			//get mov end
			ret = TSK_CONF_UpdataMovEnd(pData,dataLen);
			break;
		default:
			break;

	}
	//WV_printf("get UpdateMov cmd \n");
	SVR_CMD_Ack(pHead,ret);

	return ret;
}
/********************************************************************
WV_S32  SVR_CMD_DeletMov(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_DeletMov(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	WV_U32 dataLen;
	dataLen = pHead->dataNum;
	WV_printf("get DeletMov cmd dataLen[%d],name=%s\n",dataLen,pData);
	ret = TSK_CONF_DeleteMov(pData ,dataLen);
	SVR_CMD_Ack(pHead,ret);
	return ret;
}

/********************************************************************
WV_S32  SVR_CMD_ChangeMov(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_ChangeMov(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	WV_U32 dataLen,movNum;
	dataLen = pHead->dataNum;
	movNum = pHead->cmdL1;
	
	ret = TSK_CONF_ChangeMov(movNum,pData ,dataLen);

	SVR_CMD_Ack(pHead,ret);
	return ret;
}

/********************************************************************
WV_S32  SVR_CMD_ReNameMov(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_ReNameMov(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	WV_U32 dataLen;
	dataLen = pHead->dataNum;
	WV_printf("get ReName mov cmd \n");
	ret = TSK_CONF_ReNameMov(pData ,dataLen);
	SVR_CMD_Ack(pHead,ret);
	
	return ret;
}

/********************************************************************
WV_S32  SVR_CMD_GetSceneInfo(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetSceneInfo(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;

	WV_U32 dataLen=0;
	WV_printf("get GetSceneInfo cmd \n");

	switch(pHead->cmdL1)
	{
		case 0:
			ret = TSK_CONF_GetSceneInfo(pData,&dataLen);
			break;
		case 1:
			dataLen=pHead->dataNum;
			ret = TSK_CONF_SaveNewScene(pData ,dataLen);
			dataLen=0;
			break;
		case 2:
			dataLen=pHead->dataNum;
			ret = TSK_CONF_ChangeScene(pData,dataLen);
			dataLen=0;
		case 3:
			dataLen=pHead->dataNum;
			TSK_CONF_SaveCurrentScene(pData,dataLen);
			dataLen=0;
			break;
		case 4:
			dataLen=pHead->dataNum;
			ret=TSK_CONF_SceneDelet(pData,dataLen);
			dataLen=0;
			break;
		default:
			break;
		
	}

	pHead -> cmdL0 |= 0x80;
	if(ret!= WV_SOK)
	  pHead -> cmdL0 |= 0x40;

	pHead->dataNum = dataLen;

	return ret;
}

/********************************************************************
WV_S32  SVR_CMD_GetMovUrl(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetMovUrl(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	
	WV_U8 url[64]={0};
	WV_U32 len,ret;

	len = pHead->dataNum;
	memcpy(url,pData,len);
	//ret=TSK_SCENE_PlayMovUrl(pHead->cmdL1,url,len);
	ret = TSK_Mobile_PlayMovUrl(pHead->cmdL1,url,len);
	SVR_CMD_Ack(pHead,ret);
	return 0;
}
/********************************************************************
WV_S32  SVR_CMD_GetPic(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetPic(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_printf("get pic cmd \n");
	WV_S32 ret=0;
	ret = TSK_Mobile_ShowPic(pData,pHead->dataNum,pHead->cmdL1);
	SVR_CMD_Ack(pHead,ret);
	return 0;;
}
/********************************************************************

WV_S32  SVR_CMD_GetControlProtocol(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

//获取控制协议包括串口协议和网络协议
case 0:串口协议文件
case 1:网络协议文件
case 2：点歌机歌曲数据库 
********************************************************************/
WV_S32  SVR_CMD_GetControlProtocol(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret;
	switch(pHead->cmdL1)
	{

		case 0:
			ret = TSK_UART_SetUartSceneCmd(pData,pHead->dataNum);
			break;
		case 1:
			ret = SVR_CONTROL_GetConfFileCmd(pData,pHead->dataNum);
			break;
		case 2:
			ret = SYS_FILE_GetConfFile(pData,pHead->dataNum,pHead->cmdL1,pHead->arg1);
			break;
		default:
			ret = -1;
			break;
	}
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_GetIdCode(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32 SVR_CMD_GetIdCode(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{	
	WV_S32 ret=0,len;

	ret = SYS_LICENCES_GetID(&len,pData);
	SVR_CMD_Ack(pHead,ret);
	pHead->dataNum = len;
	return 0;
}

/********************************************************************
WV_S32  SVR_CMD_SetLicences(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32 SVR_CMD_SetLicences(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{

	WV_S32 ret=0;
	WV_U8 id[16];
	//WV_U8 data[pHead->dataNum];
	if(pHead->dataNum < 16)
	{
		
		pHead->dataNum = 0;
		ret = -1;
		SVR_CMD_Ack(pHead,ret);
		return 0;
	}

	//SYS_LIC_Encrypt_ID(pData,WV_S8 *pwd,WV_S8 *pOut,WV_U32 len);
	memcpy(id,pData,16);
	ret = SYS_LICENCES_Check_ID(id);
	if(ret != 0 )
	{
		pHead->dataNum = 0;
		ret = -1;
		SVR_CMD_Ack(pHead,ret);
		return WV_SOK;
	}
	ret = SYS_LICENCES_WriteLic(pData,pHead->dataNum);
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}
/********************************************************************
WV_S32  SVR_CMD_GetDevInfo(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32 SVR_CMD_GetDevInfo(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{

	WV_S32 ret=0;
	WV_U32 dataLen;
	ret = SYS_INFO_GetInfo(&dataLen,pData );
	SVR_CMD_Ack(pHead,ret);
	
	pHead->dataNum = dataLen;
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_SetGif(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_SetGif(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	
	pHead->cmdL1 = pHead->cmdL1 & 0x0f;
	if(pHead->cmdL1 >= 16)// 
	{
		ret = -1;
		SVR_CMD_Ack(pHead,ret);
		return 0;
	}
	ret = TSK_CONF_SetGif(pData,pHead->dataNum,pHead->cmdL1 );
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_Reset(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_Reset(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	TSK_SCENE_SceneClose();
	SYS_INFO_DevReset();
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_GetMapLine(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetMapLine(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	//printf("SVR_CMD_GetMapLine\n");
	WV_S32 ret = 0;
	
	ret = TSK_GO_DrawMapLine(pHead->dataNum,pData);
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}


/********************************************************************
WV_S32  SVR_CMD_SetCascading(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_SetCascading(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_printf("SVR_CMD_SetCascading\n");
	WV_S32 ret = 0;
	
	ret = TSK_SCENE_SetCascading(pHead->cmdL1,pHead->arg1,pData);

	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}
/********************************************************************
WV_S32  SVR_CMD_GetCascading(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_GetCascading(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_printf("SVR_CMD_GetCascading\n");
	WV_S32 ret = 0;
	
	ret=TSK_SCENE_GetCascading(&pHead->cmdL1,&pHead->arg1,pData);
	
	pHead->dataNum = 4 * pHead->arg1 ;

	pHead -> cmdL0 |= 0x80;
	if(ret!= WV_SOK)
	{
	  pHead -> cmdL0 |= 0x40;
	}

	//SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}


/********************************************************************
WV_S32  SVR_CMD_SetPlayMode(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_SetPlayMode(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_printf("SVR_CMD_SetPlayMode\n");
	WV_S32 ret = 0;
	ret = TSK_SCENE_SetPlayMode(pHead->cmdL1);
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}

/********************************************************************
WV_S32  SVR_CMD_SetPlayWindow(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_SetPlayWindow(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_printf("SVR_CMD_SetPlayWindow\n");
	WV_S32 ret = -1;
	WV_U8 saveMode = 0;
	if((pHead->cmdL1 & 0x40) == 0x40)
	{
		saveMode = 1;		
	}

	ret=TSK_GO_RectPicAndMovPos(pHead->arg1,saveMode,pHead->dataNum,pData);	
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}
/********************************************************************

WV_S32  SVR_CMD_SaveAngle(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32  SVR_CMD_SaveAngle(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_printf("SVR_CMD_SaveAngle\n");
	WV_S32 ret = 0;

	ret=TSK_SCENE_SaveAngle(pHead->dataNum,pData);
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}
/********************************************************************

WV_S32  SVR_CMD_GetAngle(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32  SVR_CMD_GetAngle(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_printf("SVR_CMD_GetAngle\n");
	WV_S32 ret = 0;

	ret=TSK_SCENE_GetAngle((WV_U32*)(&pHead->dataNum),pData);
	
	pHead -> cmdL0 |= 0x80;
	if(ret != WV_SOK)
	{
		pHead -> cmdL0 |= 0x40;  
	}
	return WV_SOK;
}

/********************************************************************

WV_S32  SVR_CMD_SetVolume(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32  SVR_CMD_SetVolume(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	
	WV_printf("SVR_CMD_SetVolume 1[%d] ,2[%d], 3[%d]\n",pHead->cmdL1,pHead->arg1,pHead->arg2);
	WV_S32 ret = 0;

	TSK_PLAYER_SetVolume(pHead->cmdL1,0);
	TSK_PLAYER_SetVolume(pHead->arg1,1);
	TSK_PLAYER_SetVolume(pHead->arg2,2);

	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}
/********************************************************************

WV_S32  SVR_CMD_SaveMovType(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32  SVR_CMD_SaveMovType(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	ret = TSK_CONF_saveMovType(pData,pHead->dataNum);
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;	
}

/********************************************************************

WV_S32  SVR_CMD_SaveSceneType(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32  SVR_CMD_SaveSceneType(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	ret = TSK_CONF_SaveSceneType(pHead->arg1,pHead->arg2);//id ,type
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;	
}
/********************************************************************

WV_S32  SVR_CMD_SetText(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32 SVR_CMD_SetText(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{

	WV_S32 ret = 0;
	TSK_TEXT_PutText(pData,pHead->dataNum);
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}
/********************************************************************

WV_S32  SVR_CMD_ConfText(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32 SVR_CMD_ConfText(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{

	WV_printf("SVR_CMD_ConfText\n");
	WV_S32 ret = 0;
	WV_U8 saveMode = 0;
	if((pHead->cmdL1 & 0x40) == 0x40)
	{
		saveMode = 1;
		
	}
	ret=TSK_TEXT_GetConf(saveMode,pHead->dataNum,pData);	
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}
/********************************************************************

WV_S32  SVR_CMD_DEV_On_Off(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 

********************************************************************/
WV_S32  SVR_CMD_DEV_On_Off(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	//arg1 == 0 dev standby 待机 ,关投影
	if(pHead->arg1 == 0){
		TSK_SCENE_Standby(TSK_SCENE_TYPE_NETDATA); 
		NET_UART_ProjectorCmd(0);
	}else if(pHead->arg1 == 1){  //开机 ,开投影
		
		TSK_SCENE_StartingUP(TSK_SCENE_TYPE_NETDATA);
		NET_UART_ProjectorCmd(1);
	}
	SVR_CMD_Ack(pHead,ret);
	return WV_SOK;
}

/********************************************************************
WV_S32 SVR_CMD_CustomScene(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
自定义场景
*********************************************************************/
WV_S32 SVR_CMD_CustomScene(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	WV_U32 dataLen;
	dataLen = pHead->dataNum;
	ret = TSK_CONF_SaveCustomScene(pData ,dataLen);
	SVR_CMD_Ack(pHead,ret);
	return ret;
}

/********************************************************************
WV_S32 SVR_CMD_Fadeinout(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
淡入淡出
*********************************************************************/
WV_S32 SVR_CMD_Fadeinout(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData)
{
	WV_S32 ret = 0;
	ret = TSK_CONF_SaveFadeinout(pHead->cmdL1);
	SVR_CMD_Ack(pHead,ret);
	return ret;
}

/********************************************************************
WV_S32  SVR_CMD_Proc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData); 
********************************************************************/
WV_S32  SVR_CMD_Proc(SVR_FRAME_HEAD_E * pHead ,WV_U8 *pData) 
{
    WV_U8  cmd,type;
    WV_S32 ret = WV_SOK,i;
    cmd  =  pHead -> cmdL0  & 0x3F;
    type =  ((pHead -> cmdL0) >>7) & 1; 
    
    for(i=0;i<SVR_CMD_MAX_NUM ;i++)
	{
	   if(gFuncTable[i].cmd == cmd)
	   {
	   		ret = gFuncTable[i].procFunc(pHead,pData);
	     	break;
	   }
	}
  	if(i == SVR_CMD_MAX_NUM)
	{
			SVR_CMD_Ack(pHead,1);
			WV_ERROR("no this cmand \r\n");		
			return WV_EFAIL; 
	}  
     return  ret;
}
/******************************************************************************
WV_S32   SVR_CMD_Print( WV_S8 *pPrntBuf);
******************************************************************************/
WV_S32   SVR_CMD_Print( WV_S8 *pPrntBuf)
{
  WV_S32  i,len=0; 
  len += sprintf(pPrntBuf,"server cmand table:\r\n");
  for(i = 0; i< SVR_CMD_MAX_NUM; i++)
  	{ 
  	if(gFuncTable[i].cmd == 0)
  		{
  		  break;
  		}	
  	len += sprintf(pPrntBuf,"%3d[0x%02x]  :%s\r\n",gFuncTable[i].cmd,gFuncTable[i].cmd,gFuncTable[i].cmdComment);
  		
  	}
  return  len;
   
}


/******************************************************************************
WV_S32   SVR_CMD_Register(WV_U8 *cmd,WV_S8 *cmdComment,SVR_CMD_PROFUNC pFunc);
******************************************************************************/
WV_S32   SVR_CMD_Register(WV_U8 cmd,WV_S8 *cmdComment,SVR_CMD_PROFUNC pFunc)
{
	WV_S32 i;

	if(cmd == 0)
		{ 
		WV_ERROR("input erro\r\n");		 
		return WV_EFAIL;
		}	

	for(i=0; i < SVR_CMD_MAX_NUM ; i++)
		{
		 
		 if(gFuncTable[i].cmd == 0  ||  gFuncTable[i].cmd == cmd)
			{ 
			 gFuncTable[i].cmd = cmd;
			 strcpy(gFuncTable[i].cmdComment,cmdComment); 
			 gFuncTable[i].procFunc = pFunc; 
			 break;
			} 
		}
	 
	if(i == SVR_CMD_MAX_NUM)
		{
		WV_ERROR("table is full\r\n");		
		return WV_EFAIL; 
		}

	return WV_SOK;
	
	
}


/********************************************************************
WV_S32  SVR_CMD_Init(); 
********************************************************************/
WV_S32  SVR_CMD_Init()  
{
	memset(gFuncTable,0x0,sizeof(gFuncTable));
	SVR_CMD_Register( SVR_CMD_HANDSHARK,"[0x01]hand cmd",SVR_CMD_HandProc); 
	SVR_CMD_Register( SVR_CMD_MAP,"[0x02]map  cmd",SVR_CMD_MapProc); 
	SVR_CMD_Register( SVR_CMD_OUTLINE,"[0x03]out  cmd",SVR_CMD_OutProc); 
	SVR_CMD_Register( SVR_CMD_SPLIT,"[0x04]split  cmd",SVR_CMD_SplitProc); 
	SVR_CMD_Register( SVR_CMD_OUTRESOLUTION,"[0x05]OutResolution  cmd",SVR_CMD_OutResolutionProc); 
	SVR_CMD_Register( SVR_CMD_SPLITCONF,"[0x06]splitConf  cmd",SVR_CMD_splitConfProc); 
	SVR_CMD_Register( SVR_CMD_WINDOWCONF,"[0x07]windowConf  cmd",SVR_CMD_windowConfProc); 
	SVR_CMD_Register( SVR_CMD_MOVCONF,"[0x08]movConf  cmd",SVR_CMD_movConfProc); 
	SVR_CMD_Register( SVR_CMD_GIFCONF,"[0x09]gifConf  cmd",SVR_CMD_gifConfProc); 
	SVR_CMD_Register( SVR_CMD_SCENECONF,"[0x0A]sceneConf  cmd",SVR_CMD_sceneConfProc); 
	SVR_CMD_Register( SVR_CMD_IPCONF,"[0x0B]sceneConf  cmd",SVR_CMD_IPConfProc); 
	SVR_CMD_Register( SVR_CMD_DRAWLINECONF,"[0x0C]sceneConf  cmd",SVR_CMD_DrawLineProc); 
	SVR_CMD_Register( SVR_CMD_LIGHTCONF,"[0x0d]lightConf  cmd",SVR_CMD_lightConf);
 	SVR_CMD_Register( SVR_CMD_COLOR,"[0x0e]color  cmd",SVR_CMD_ColorProc); 
	SVR_CMD_Register( SVR_CMD_FPGAUPDATE ,"[0x0f]FpgaUpdate  cmd",SVR_CMD_FpgaUpdate);
 	SVR_CMD_Register( SVR_CMD_GETTEMP,"[0x10]get temp  cmd",SVR_CMD_GetTemp);
 	SVR_CMD_Register( SVR_CMD_GETRESOLUTION,"[0x11]get resolution  cmd",SVR_CMD_GetResolutionProc);
  	SVR_CMD_Register( SVR_CMD_GETSPLITCONF,"[0x12]get splitconf  cmd",SVR_CMD_GetSplitConf);
  	SVR_CMD_Register( SVR_CMD_GETMOVINFO,"[0x13]get mov info  cmd",SVR_CMD_GetMovInfo);
  	SVR_CMD_Register( SVR_CMD_UPDATEMOV,"[0x14] update mov info  cmd",SVR_CMD_UpdateMov);
  	SVR_CMD_Register( SVR_CMD_DELETMOV,"[0x15] delet mov  cmd",SVR_CMD_DeletMov);
  	SVR_CMD_Register( SVR_CMD_CHANGEMOV,"[0x16] change  mov info  cmd",SVR_CMD_ChangeMov);
	SVR_CMD_Register( SVR_CMD_RENAMEMOV,"[0x17] change  mov info  cmd",SVR_CMD_ReNameMov);
  	SVR_CMD_Register( SVR_CMD_GETSCENEINFO,"[0x18] get  scene info  cmd",SVR_CMD_GetSceneInfo);
  	SVR_CMD_Register( SVR_CMD_GETMOVURL,"[0x1a] get  mov url  cmd",SVR_CMD_GetMovUrl);
  	SVR_CMD_Register( SVR_CMD_GETPIC,"[0x1b] get  mov phone pic ",SVR_CMD_GetPic);
  	SVR_CMD_Register( SVR_CMD_GET_CONTROL_PROTOCOL,"[0x1c] get uart cmd file  ",SVR_CMD_GetControlProtocol);
  	SVR_CMD_Register( SVR_CMD_GET_ID,"[0x1d] get dev id ",SVR_CMD_GetIdCode);
  	SVR_CMD_Register( SVR_CMD_SET_LICENCES_CMD,"[0x1e] get dev id code",SVR_CMD_SetLicences);
  	SVR_CMD_Register( SVR_CMD_GET_DEVINFO,"[0x1f] get dev info ",SVR_CMD_GetDevInfo);
  	SVR_CMD_Register( SVR_CMD_SET_GIF,"[0x20] get dev id code",SVR_CMD_SetGif);
  	SVR_CMD_Register( SVR_CMD_SET_RESET,"[0x21] get dev id code",SVR_CMD_Reset);
	SVR_CMD_Register( SVR_CMD_GET_MAPLINE,"[0x22] get map effective coverage ",SVR_CMD_GetMapLine); //map有效区域

	SVR_CMD_Register( SVR_CMD_SET_CASCADING,"[0x23] get map effective coverage ",SVR_CMD_SetCascading); //设置级联
	SVR_CMD_Register( SVR_CMD_GET_CASCADING,"[0x24] get map effective coverage ",SVR_CMD_GetCascading); //查询级联信息
	SVR_CMD_Register( SVR_CMD_SET_PLAYMODE,"[0x25] get map effective coverage ",SVR_CMD_SetPlayMode);  //设置视频播放模式(随机和重播)
	SVR_CMD_Register( SVR_CMD_SET_PLAYWINDOW,"[0x26] get map effective coverage ",SVR_CMD_SetPlayWindow); //设置U盘（手机）视频和图片播放位置
	SVR_CMD_Register( SVR_CMD_SAVE_ANGLE ,"[0x27] get map effective coverage ",SVR_CMD_SaveAngle);  //保存输出图像旋转信息
	SVR_CMD_Register( SVR_CMD_GET_ANGLE,"[0x28] get map effective coverage ",SVR_CMD_GetAngle); //
	SVR_CMD_Register( SVR_CMD_SET_VOLUME,"[0x29] set player volume ",SVR_CMD_SetVolume); //
	SVR_CMD_Register( SVR_CMD_SAVE_MOV_TYPE,"[0x2a] save mov type class ",SVR_CMD_SaveMovType); // save mov type
	SVR_CMD_Register( SVR_CMD_SAVE_SCENE_TYPE,"[0x2b] save scene type class",SVR_CMD_SaveSceneType); // save scene type
	SVR_CMD_Register( SVR_CMD_SET_TEXT,"[0x2c] save scene type class",SVR_CMD_SetText); // 发送文字
	SVR_CMD_Register( SVR_CMD_CONF_TEXTSHOW,"[0x2d] conf text show pos",SVR_CMD_ConfText); // conf text
	SVR_CMD_Register( SVR_CMD_CONF_DEV_ON_OFF,"[0x2f] dev on/off cmd",SVR_CMD_DEV_On_Off); // conf text
    SVR_CMD_Register( SVR_CMD_CUSTOM_SCENE, "[0x30] custom scene", SVR_CMD_CustomScene); // 自定义场景
    SVR_CMD_Register( SVR_CMD_FADE_INOUT, "[0x31] fade inout", SVR_CMD_Fadeinout); // 淡入淡出


	return WV_SOK;  
}



/********************************************************************
WV_S32  SVR_CMD_DeInit(); 
********************************************************************/
WV_S32  SVR_CMD_DeInit()  
{
	 
	 
	return WV_SOK;  
}
