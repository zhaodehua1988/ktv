#include "his_go.h"
#include "his_dis.h"
#include "tsk_go.h" 
#include "tsk_scene.h"
#include <sys/time.h>
#include "tsk_fpga.h"
#include "fpga_conf.h"
#include "tsk_player.h"
#include "tsk_scene.h"
#include "hi_go_gdev.h"

#define  DEBUGSCENE  16
#define  DEFAULTSCENE 0
#define  COLOR_NUM     (512+64)
#define  GRID_RECT_PIX  10
#define  RECT_W   3840
#define  RECT_H   1080
#define  DRAW_EXIT_STATE 0
#define  DRAW_GRID_STATE 1
#define  FILL_FREC_STATE 2
#define  FILL_GRID_STATE 3
#define  DRAW_MAPLINE_STATE  4

//usb and mobile 图片和视频位置信息保存路径
#define  TSK_GO_PLAY_POSFILE  "./env/playPosFile.txt"
#define  TSK_GO_PLAY_BACKANI_FILE  "./env/BackAni.txt"
#define  TSK_GO_TEXT_LIB_FILE "/usr/lib/DroidSansFallbackLegacy.ttf"

static WV_U32 *pColorTable;
typedef struct TSK_GO_DEV_E 
{
	WV_THR_HNDL_T 	thrHndl; 
	WV_U32      	open;
	WV_U32      	close;
	HIS_DEV_HIGO_E goDev;  
	FPGA_SPLIT_INFO_S splitInfo;
	WV_U32 drawState; 
	WV_U32 playerMode;
	WV_U32 colorKeyEna;
	WV_U32 showPosEna;//显示视频图片位置使能
}TSK_GO_DEV_E;
TSK_GO_DEV_E   gGoDev;

typedef struct TSK_GO_LIGHT_S
{
	WV_U32 color;
	HI_RECT rect;
}TSK_GO_LIGHT_S;

static TSK_GO_LIGHT_S gLightData;
//static TSK_GO_MOV_PIC_POS_S gMovPicPos[3]; 
typedef struct TSK_GO_POS_E
{
	TSK_GO_MOV_PIC_POS_S pos[3];
	WV_U16 outlineChn;
	WV_U16 outlineId;
}TSK_GO_POS_E;
static TSK_GO_POS_E gUsbPos; 

static TSK_GO_MOV_PIC_POS_S gBackAniPos;

/*****************************************************************************
WV_U32 TSK_GO_DrawLine(WV_U16 x0,WV_U16 y0,WV_U16 x1,WV_U16 y1,WV_U32 color);
*****************************************************************************/
inline WV_S32 TSK_GO_DrawLine(WV_U16 x0,WV_U16 y0,WV_U16 x1,WV_U16 y1,WV_U32 color)
{
	
	
	HI_GO_DrawLine(gGoDev.goDev.layerSurfHndl,x0,y0,x1,y1,color);
	
	return WV_SOK;
}

/*****************************************************************************
WV_S32 TSK_GO_DrawMapLine(WV_U32 dataLen,WV_S8 *pData);
*****************************************************************************/
WV_S32 TSK_GO_DrawMapLine(WV_U32 dataLen,WV_S8 *pData)
{


	if(	gGoDev.playerMode != TSK_PLAYER_MODE_1920_1080)
	{
		TSK_PLAYER_ChangeMode(TSK_PLAYER_MODE_1920_1080);
		gGoDev.playerMode = TSK_PLAYER_MODE_1920_1080;
		//FPGA_CONF_ResetA();
	}		


		if(gGoDev.drawState != DRAW_MAPLINE_STATE)
		{
	
			WV_U16 ena;
			ena = FPGA_CONF_GetOutput();
			ena =ena & 0xff; 
			TSK_FPGA_outConf(ena);
			gGoDev.drawState = DRAW_MAPLINE_STATE;
			FPGA_SPLIT_INFO_S splitInfo;
			TSK_FPGA_GetSplit(&splitInfo);
			TSK_FPGA_SplitConf(splitInfo);
			//printf("***gGoDev.drawState != DRAW_MAPLINE_STATE***");
		}
		gGoDev.drawState = DRAW_MAPLINE_STATE;
		WV_U32 *pRcv32;
		WV_U16 *pRcv16;
		WV_U32 backColor;
		WV_U32 lineColor;
		WV_U32 point_num,x,y,r=3,EllipseNum;
		HI_RECT Rect;
		pRcv16 = (WV_U16 *)pData;
		pRcv32 = (WV_U32 *)pData;
	
		backColor = ntohl(pRcv32[2042]) | 0xff000000;
		lineColor = ntohl(pRcv32[2043]) | 0xff000000;


		EllipseNum = pRcv16[0];
		WV_S32 i,ellipseColor;
		ellipseColor=ntohl(pRcv32[1]);

		point_num = (dataLen - (2040 * 4 + 8+12))/4;//point num

		Rect.x = 0;
		Rect.y = 0;
		Rect.w = 3840;
		Rect.h = 1080;

		WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, backColor, HIGO_COMPOPT_NONE) ); 

		for(i=0;i<EllipseNum;i++)
		{

			x = pRcv16[2*i+4]/2;
			y = pRcv16[2*i+1+4]/2;
	
			if((x-r) <0)
			{
				x = r;
			}		

			if((y-r) <0)
			{
				y = r;
			}
			Rect.x = x-r;
			Rect.y = y-r;
			Rect.w = 2*r;
			Rect.h = 2*r;	
			HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, lineColor, HIGO_COMPOPT_NONE);
								
		}
  
	//set current point
	
	WV_U16 *p=(WV_U16 *)&pData[2040 * 4 + 8+12];
	for(i=0;i<point_num-1;i++)
	{	
		//printf("[%d]x0[%d]y0[%d]x1[%d]y1[%d] \n",i,p[2*i]/2,p[2*i+1]/2,p[2*i+2]/2,p[2*i+3]/2);
		TSK_GO_DrawLine(p[2*i]/2,p[2*i+1]/2,p[2*i+2]/2,p[2*i+3]/2,lineColor);	
	}
	i--;
	TSK_GO_DrawLine(p[0]/2,p[1]/2,p[2*i+2]/2,p[2*i+3]/2,lineColor);	
	HIS_GO_RefreshLayer(&gGoDev.goDev);

	return WV_SOK;
}

/*******************************************************************************************************
WV_S32 TSK_GO_DrawGrig(WV_U32 chl,WV_U32 backColor,WV_U32 lineColor,WV_U32 row ,WV_U32 col,WV_U16 *pBuf)
*******************************************************************************************************/
WV_S32 TSK_GO_DrawGrig(WV_U32 chl,WV_U32 backColor,WV_U32 lineColor,WV_U32 row ,WV_U32 col,WV_U16 *pBuf)
{


	TSK_PLAYER_ChangeMode(TSK_PLAYER_MODE_3840_1080);
	gGoDev.playerMode = TSK_PLAYER_MODE_3840_1080;

				
	WV_S32 i,j;
	WV_U16 *pTemp;
	HI_RECT Rect;
	WV_U32   offset; 
 	gGoDev.drawState = DRAW_GRID_STATE;
	backColor=backColor | 0xff000000;
	lineColor=lineColor | 0xff000000; 

	if(chl == 0)
	{
		Rect.x = 0;
		Rect.y = 0;
		Rect.w = 1920;
		Rect.h = 1080;

		offset = 0;
	}
	else{   
	        Rect.x = 1920;
		Rect.y = 0;
		Rect.w = 1920;
		Rect.h = 1080;
		offset = 1920;

	}
		
		
	if(row<2 || col < 2 ){

		WV_printf("row [%d],col[%d],error\n",row,col);
		return WV_EFAIL;
	}
	WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, backColor, HIGO_COMPOPT_NONE) );   
	//draw row line
	
	for(i=0;i<row; i++){
		pTemp = pBuf + col *2 *i;

		for(j=0; j<col -1; j++){

			TSK_GO_DrawLine(pTemp[j*2] +offset,pTemp[j*2+1],pTemp[j*2+2]+offset,pTemp[j*2+3], lineColor);
			
					
		}
		
	}

	//draw col line

	for(i=0; i<row-1; i++){
		
		pTemp  = pBuf + col*2 *i;

		for(j=0;j<col;j++){
			
			TSK_GO_DrawLine(pTemp[j*2] +offset ,pTemp[j*2+1],pTemp[j*2+col*2]+offset,pTemp[j*2+col*2+1], lineColor); 
		
		}
	
	}
	
	//fresh layer
	//HIS_GO_RefreshLayer(&gGoDev.goDev);
	
	return WV_SOK;
}
/*****************************************************************************
WV_S32 TSK_GO_DrawGrid_EXIT();
*****************************************************************************/
WV_S32 TSK_GO_DrawGrid_EXIT()
{
	
	WV_U32 ret;
	WV_U16 outPutEna;
	HI_RECT Rect;
	WV_S32 chl,i;
	chl=FPGA_CONF_GetOutChl_Num();
	//Rect.x = 0;
	//Rect.y = 0;
	//Rect.w = RECT_W;
	//Rect.h = RECT_H+4;
	FPGA_CONF_SetOutput(0);
	//WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0, HIGO_COMPOPT_NONE) );   		
	HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, NULL, 0, HIGO_COMPOPT_NONE);
	HIS_GO_RefreshLayer(&gGoDev.goDev);
		
	if(gGoDev.drawState  == FILL_FREC_STATE){
		//FPGA_CONF_SetSplit(&gGoDev.splitInfo);
		TSK_FPGA_SplitConf(gGoDev.splitInfo);
	}else if(gGoDev.drawState  == DRAW_GRID_STATE){
		
		FPGA_SPLIT_INFO_S splitInfo;
		TSK_FPGA_GetSplit(&splitInfo);
		TSK_FPGA_SplitConf(splitInfo);
	}
	gGoDev.drawState = DRAW_EXIT_STATE;

	TSK_PLAYER_ChangeMode(TSK_PLAYER_MODE_1920_1080);
	gGoDev.playerMode = TSK_PLAYER_MODE_1920_1080;

 	HIS_FB_ClrFpga();
	//scene open
	TSK_SCENE_SceneOpen();
	outPutEna = TSK_FPGA_GetOutConf();
	FPGA_CONF_SetOutput(outPutEna);
	//FPGA_CONF_ResetA();
	//TSK_SCENE_Restart();

	return WV_SOK;
}
/*****************************************************************************
WV_S32 TSK_GO_DrawGrid_Start();
*****************************************************************************/
WV_S32 TSK_GO_DrawGrid_Start()
{
	WV_U16 outPutEna;
	WV_S32 chl,i;

	for(i=0;i<3;i++)
	{
		TSK_PLAYER_setWin(i,0,0,0,0);
	}
	TSK_SCENE_SetWinChange(1);
	TSK_SCENE_SceneClose();
	FPGA_SPLIT_INFO_S Info ;
	FPGA_CONF_GetSplitDefault(&Info);
	FPGA_CONF_SetSplit(&Info);

	TSK_PLAYER_ChangeMode(TSK_PLAYER_MODE_3840_1080);
	
	return WV_SOK;
}
/*****************************************************************************
WV_S32 TSK_GO_DrawPoitForLight(WV_U32 color,WV_U32 x,WV_U32 y);
*****************************************************************************/
WV_S32 TSK_GO_DrawPoitForLight(WV_U32 color,WV_U32 x,WV_U32 y)
{

	
	HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &gLightData.rect,gLightData.color , HIGO_COMPOPT_NONE);
	HIS_GO_RefreshLayer(&gGoDev.goDev);
	HI_RECT Rect;

	if(x >=5 )
	{
		Rect.x = x-5;
	}else{
		Rect.x = x;
	}
	if(y >=5)
	{
		Rect.y = y-5;
	}else{
		Rect.y = y;
	}
	Rect.w = 10;
	Rect.h = 10;
	
	gLightData.rect.x = Rect.x;
	gLightData.rect.y = Rect.y;
	gLightData.rect.w = Rect.w;
	gLightData.rect.h = Rect.h;


	color |= 0xff000000;
	//WV_printf("---------draw point for light x=%d,y=%d,w=%d,h=%d \n",Rect.x,Rect.y,Rect.w,Rect.h);
	HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect,color , HIGO_COMPOPT_NONE);
	

	//fresh layer
	HIS_GO_RefreshLayer(&gGoDev.goDev);

	return WV_SOK;
}

/*****************************************************************************
WV_S32 TSK_GO_DrawGrid_Test(WV_U32 row,WV_U32 col);
*****************************************************************************/

WV_S32 TSK_GO_DrawGrid_Test(WV_U32 chl,WV_U32 backColor,WV_U32 lineColor,WV_U32 row,WV_U32 col){


	WV_U16 gridBuf[128*128*2];
	WV_S32 i,j;
	WV_U16 *pTemp;
	pTemp = gridBuf;		
	for(i=0; i<row; i++){
	
	
		for(j=0;j<col;j++){
			
		
			pTemp[j*2]=(1900/col)*j;					
			pTemp[j*2+1] = (1000/row)*i;
		}
		pTemp=pTemp+col*2;
	}

	
	TSK_GO_DrawGrig(chl,backColor,lineColor,row ,col,gridBuf);

	return WV_SOK;
}

/*****************************************************************************
WV_S32 TSK_GO_DrawLine_Test(WV_U32 row,WV_U32 col);
*****************************************************************************/
WV_S32 TSK_GO_DrawLine_Test(WV_U16 x0,WV_U16 y0,WV_U16 x1,WV_U16 y1){

 
	HI_RECT Rect;

	Rect.x = 0;
	Rect.y = 0;
	Rect.w = RECT_W;
	Rect.h = RECT_H;

	HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0xff000000, HIGO_COMPOPT_NONE) ;
	HI_GO_DrawLine(gGoDev.goDev.layerSurfHndl,x0,y0,x1,y1,0xffffffff);
	HIS_GO_RefreshLayer(&gGoDev.goDev);


	return WV_SOK;
}

/*****************************************************************************
WV_S32 TSK_GO_FillRect_Start();
*****************************************************************************/
WV_S32 TSK_GO_FillRect_Start()
{
	WV_S32 ret;	
	WV_S32 i,j,k;
	WV_S32 num=0;	
	WV_U32 color_R;
	WV_U32 color_G;
	WV_U32 color_B;
	memset(pColorTable,0,COLOR_NUM*4);
	WV_U32 *pTable = pColorTable;

	for(i=8; i>0; i--){ //R
		
		color_R = (i*32-1)<<16;
		
		for(j=8; j>0; j--){ //G
			
			color_G = (j*32-1)<<8;	
			
			for(k=8; k>0; k--){ //B

				color_B = k*32-1;
				pTable[num] = color_R | color_G | color_B | 0xff000000;
				//printf("table[%d][%08x]",num,pTable[num]);
				num++;
				
			}
			//printf("\n");
		}	
	}
		

	return WV_SOK;
}

/*****************************************************************************
WV_S32 TSK_GO_FillRect_EXIT();
*****************************************************************************/
WV_S32 TSK_GO_FillRect_EXIT()
{
	
	WV_U32 ret;

	HI_RECT Rect;

	Rect.x = 0;
	Rect.y = 0;
	Rect.w = RECT_W;
	Rect.h = RECT_H;

	
	WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0, HIGO_COMPOPT_NONE) );   
	
	HIS_GO_RefreshLayer(&gGoDev.goDev);
	//TSK_SCENE_Restart();//
	return WV_SOK;
}
/*****************************************************************************
WV_S32 TSK_GO_FillRectLine();
*****************************************************************************/
WV_S32 TSK_GO_FillRectLine(){

	WV_U32 color[40];
	memset(color,0x00,sizeof(color));
	WV_S32 i,j,k;
	WV_S32 num=0;
	
	WV_U32 color_R;
	WV_U32 color_G;
	WV_U32 color_B;

	for(i=0; i<3; i++){ //R

		color_R = (i*127)<<16;

		for(j=0; j<3; j++){ //G
		
			color_G = (j*127)<<8;

			for(k=0; k<3; k++){ //B
			
				color_B = (k*127);
				color[num] = color_R | color_G | color_B | 0xff000000;
				//printf("color=%8x",color[num]);
				num++;
			}
		}	
	}

	//printf("\ncolor num = [%d]\n",num);

	HI_RECT Rect;
	for(i=0;i<num;i++){	
		Rect.x = 0;		
		Rect.y = 0+40*i;
		Rect.w = RECT_W;
		Rect.h = 40;
	
		WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, color[i], HIGO_COMPOPT_NONE) ); 

	}

	HIS_GO_RefreshLayer(&gGoDev.goDev);	
	return WV_SOK;
}
/*****************************************************************************
WV_S32 TSK_GO_FillRectDebug(WV_U32 colorNum,WV_U32 *pColor);
*****************************************************************************/
WV_S32 TSK_GO_FillRectDebug(WV_U32 colorNum,WV_U32 *pColor)
{

	WV_U32 i;
	WV_U16 outPutEna;
	HI_RECT Rect;
	//change mode 0
	outPutEna = FPGA_CONF_GetOutput();
	FPGA_CONF_SetOutput(0);	
	

	TSK_PLAYER_ChangeMode(TSK_PLAYER_MODE_3840_1080);
	gGoDev.playerMode = TSK_PLAYER_MODE_3840_1080;

	FPGA_CONF_SetOutput(outPutEna);	
	
	Rect.x = 0;
	Rect.w = RECT_W;
	if(gGoDev.drawState != FILL_FREC_STATE){

		gGoDev.drawState = FILL_FREC_STATE;
		TSK_FPGA_GetSplit(&gGoDev.splitInfo);

		FPGA_SPLIT_INFO_S splitInfo;
		FPGA_CONF_GetSplitDefault(&splitInfo);
		FPGA_CONF_SetSplit(&splitInfo);	
	}

	if(colorNum == 0){
	
		Rect.h = RECT_H;	
		Rect.y = 0;
		WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0, HIGO_COMPOPT_NONE) );
	}else{
	
		Rect.h = RECT_H/colorNum;	
		for(i=0; i<colorNum; i++){	
		
			if(i==(colorNum -1))
			{
				Rect.h = RECT_H - Rect.h*(colorNum-1);
			}
			Rect.y = 0+Rect.h*i;
			pColor[i] |= 0xff000000;
			//WV_printf("color[%d][%08x]\n",i,pColor[i]);
			WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, pColor[i], HIGO_COMPOPT_NONE) ); 
		}

	}	
	//save back color 
	gLightData.color = pColor[0] | 0xff000000;
	HIS_GO_RefreshLayer(&gGoDev.goDev);	
	
}
/*****************************************************************************

WV_S32 TSK_GO_FillRectGrid(WV_U16 border,WV_U16 splitNum,FILLRECT_GRID grid);

*****************************************************************************/
WV_S32 TSK_GO_FillRectGrid(WV_U16 border,WV_U16 splitNum,FILLRECT_GRID grid)
{
	
	WV_U32 i,j,k;
	HI_RECT Rect;
	HI_RECT RectOut;
	WV_U32 num=grid.start;
	//set back color
		
	Rect.x = 0;
	Rect.y = 0;
	Rect.w = RECT_W;
	Rect.h = RECT_H;
		
	if(border == 0){  //backcolor is 0xffffffff
		WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0xffffffff, HIGO_COMPOPT_NONE) );   	
		HIS_GO_RefreshLayer(&gGoDev.goDev);	
	}else if(border == 1){ //backcolor is 0xff000000
		WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0xff000000, HIGO_COMPOPT_NONE) );   	
		HIS_GO_RefreshLayer(&gGoDev.goDev);			
	}
	
	RectOut.w = grid.w-GRID_RECT_PIX;//
	RectOut.h = grid.h-GRID_RECT_PIX;//
			
	Rect.w = grid.w;
	Rect.h = grid.h;
				
	for(i=0; i<splitNum; i++){
		
		for(j=0; j<grid.row; j++){

			Rect.y = grid.y[i] + j*grid.h  ;
			RectOut.y = grid.y[i] + j*grid.h +GRID_RECT_PIX/2;
			for(k=0; k<grid.col; k++){


				//if(k==0){ //the first col
					Rect.x = grid.x[i] + k*grid.w;					
				//}
		
				RectOut.x = grid.x[i] + k*grid.w+GRID_RECT_PIX/2;
				if(border==0){

					WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0xff000000, HIGO_COMPOPT_NONE) ); 
				}else if(border == 1){
					WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0xffffffff, HIGO_COMPOPT_NONE) ); 
				}				
				WV_CHECK_RET( HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &RectOut, pColorTable[num], HIGO_COMPOPT_NONE) ); 
				num++;
				if(num>grid.end){
					goto end;
				
				}
			}
			
			

		}
end:	
		num=grid.start;
	}


	HIS_GO_RefreshLayer(&gGoDev.goDev);
	return WV_SOK;
}

/*****************************************************************************
WV_S32 TSK_GO_FillRectGrid_Test();
*****************************************************************************/
WV_S32 TSK_GO_FillRectGrid_Test(){
	
	FILLRECT_GRID grid;
	WV_U16 splitNum = 2;
	WV_S32 i;
	grid.w=60;
	grid.h=60;
	grid.row=16;
	grid.col=32;
	grid.start = 0;
	grid.end = 511;
	
	for(i=0; i<splitNum;i++){
		grid.x[i] = i*1920;
		grid.y[i] = 0;
	}
	TSK_GO_FillRectGrid(0,splitNum,grid);
	return WV_SOK;
}

/***************************************************************

WV_S32 TSK_GO_DrawEllipse( WV_U16 *x ,WV_U16 *y,WV_U32 r,WV_U32 collor);

***************************************************************/
WV_S32 TSK_GO_DrawEllipse( WV_U16 x ,WV_U16 y,WV_U32 r,WV_U32 color)
{
		//printf("x=%d,y=%d,r=%d,color=%d\n",x,y,r,color);
		
		int offset = 1920;
		HI_RECT Rect;
		
		if((WV_S16)(x-r) <0)
		{
			x = r;
		}		
		
		if	((WV_S16)(y-r) <0)
		{

			y = r;
		}

		Rect.x = x-r+offset;
		Rect.y = y-r;
		Rect.w = 2*r;
		Rect.h = 2*r;	
		//HI_GO_DrawCircle  ( gGoDev.goDev.layerSurfHndl,x+offset,y,r,color);  
		//WV_S32 pointX[4],pointY[4];
		HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, color, HIGO_COMPOPT_NONE);
		
	return 0;
}



/*****************************************************************************

WV_S32 TSK_GO_ReadUsb_PicAndMov_Pos(WV_U8 saveMode,WV_U8 *pData);

*****************************************************************************/
WV_S32 TSK_GO_ReadUsb_PicAndMov_Pos(WV_U32 dataLen,WV_U8 *pData)
{
	WV_U32 readLen;
	FILE *fp;
	fp = fopen(TSK_GO_PLAY_POSFILE,"rb");
	if(fp == NULL)
	{
		WV_printf("fopen %s error",TSK_GO_PLAY_POSFILE);
		return WV_EFAIL;
	}	
	
	readLen = fread(pData,1,dataLen,fp);
	if(readLen != dataLen)
	{
		WV_printf("read PicAndMov pos file error \n");
		fclose(fp);
		return WV_EFAIL;
	}

	fclose(fp);
	return WV_SOK;
}

/*****************************************************************************

WV_S32 TSK_GO_SaveUsb_PicAndMov_Pos(WV_U8 saveMode,WV_U8 *pData);

*****************************************************************************/
WV_S32 TSK_GO_SaveUsb_PicAndMov_Pos(WV_U32 dataLen,WV_U8 *pData)
{
	WV_U32 writeLen;
	FILE *fp;
	fp = fopen(TSK_GO_PLAY_POSFILE,"wb+");
	if(fp == NULL)
	{
		WV_printf("fopen %s error",TSK_GO_PLAY_POSFILE);
		return WV_EFAIL;
	}

	writeLen = fwrite(pData,1,dataLen,fp);
	if(writeLen != dataLen)
	{
		WV_printf("write PicAndMov pos file error \n");
		fclose(fp);
		return WV_EFAIL;
	}

	fclose(fp);
	return WV_SOK;
}

/*****************************************************************************

WV_S32 TSK_GO_ReadUsb_BackAni_Conf(WV_U8 saveMode,WV_U8 *pData);

*****************************************************************************/
WV_S32 TSK_GO_ReadUsb_BackAni_Conf(WV_U32 dataLen,WV_U8 *pData)
{
	WV_U32 readLen;
	FILE *fp;
	fp = fopen(TSK_GO_PLAY_BACKANI_FILE,"rb");
	if(fp == NULL)
	{
		WV_printf("fopen %s error",TSK_GO_PLAY_BACKANI_FILE);
		return WV_EFAIL;
	}	
	
	readLen = fread(pData,1,dataLen,fp);
	if(readLen != dataLen)
	{
		WV_printf("read PicAndMov pos file error \n");
		fclose(fp);
		return WV_EFAIL;
	}

	fclose(fp);
	return WV_SOK;
}

/*****************************************************************************

WV_S32 TSK_GO_SaveUsb_BackAni_Conf(WV_U8 saveMode,WV_U8 *pData);

*****************************************************************************/
WV_S32 TSK_GO_SaveUsb_BackAni_Conf(WV_U32 dataLen,WV_U8 *pData)
{
	WV_U32 writeLen;
	FILE *fp;
	fp = fopen(TSK_GO_PLAY_BACKANI_FILE,"wb+");
	if(fp == NULL)
	{
		WV_printf("fopen %s error",TSK_GO_PLAY_BACKANI_FILE);
		return WV_EFAIL;
	}

	writeLen = fwrite(pData,1,dataLen,fp);
	if(writeLen != dataLen)
	{
		WV_printf("write PicAndMov pos file error \n");
		fclose(fp);
		return WV_EFAIL;
	}

	fclose(fp);
	return WV_SOK;
}


/*****************************************************************************

WV_S32 TSK_GO_GetUsb_PicAndMov_Pos(TSK_GO_MOV_PIC_POS_S * pMovPos,TSK_GO_MOV_PIC_POS_S * pPicPos);

*****************************************************************************/
WV_S32 TSK_GO_GetUsb_PicAndMov_Pos(TSK_GO_MOV_PIC_POS_S * pMovPos,TSK_GO_MOV_PIC_POS_S * pPicPos)
{
	pMovPos->startX = gUsbPos.pos[0].startX;
	pMovPos->startY = gUsbPos.pos[0].startY;
	pMovPos->endX = gUsbPos.pos[0].endX;
	pMovPos->endY = gUsbPos.pos[0].endY;

	pPicPos->startX = gUsbPos.pos[1].startX;
	pPicPos->startY = gUsbPos.pos[1].startY;
	pPicPos->endX = gUsbPos.pos[1].endX;
	pPicPos->endY = gUsbPos.pos[1].endY;

	return WV_SOK;
}
/*****************************************************************************

WV_S32 TSK_GO_GetOutline_Pos(TSK_GO_MOV_PIC_POS_S * pOutlinePos,WV_U16 *pChn,WV_U16 *pID);

*****************************************************************************/
WV_S32 TSK_GO_GetOutline_Pos(TSK_GO_MOV_PIC_POS_S * pOutlinePos,WV_U16 *pChn,WV_U16 *pID)
{

	pOutlinePos->startX = gUsbPos.pos[2].startX;
	pOutlinePos->startY = gUsbPos.pos[2].startY;
	pOutlinePos->endX = gUsbPos.pos[2].endX;
	pOutlinePos->endY = gUsbPos.pos[2].endY;
	*pChn = gUsbPos.outlineChn;
	*pID = gUsbPos.outlineId;

	return WV_SOK;
}

/*****************************************************************************

WV_S32 TSK_GO_GetBackAni_Pos(TSK_GO_MOV_PIC_POS_S * pBackAniPos,WV_U16 *pChn,WV_U16 *pID);

*****************************************************************************/
WV_S32 TSK_GO_GetBackAni_Pos(TSK_GO_MOV_PIC_POS_S * pBackAniPos,WV_U16 *pChn,WV_U16 *pID)
{

	pBackAniPos->startX = gBackAniPos.startX;
	pBackAniPos->startY = gBackAniPos.startY;
	pBackAniPos->endX = gBackAniPos.endX;
	pBackAniPos->endY = gBackAniPos.endY;
	*pChn = gUsbPos.outlineChn;
	*pID = gUsbPos.outlineId;

	return WV_SOK;
}

/*****************************************************************************

WV_S32 TSK_GO_RectPicAndMovPos(WV_U16 BackAniEna,WV_U8 saveMode,WV_U32 dataLen,WV_U8 *pData);

*****************************************************************************/
WV_S32 TSK_GO_RectPicAndMovPos(WV_U16 BackAniEna,WV_U8 saveMode,WV_U32 dataLen,WV_U8 *pData)
{
	
	WV_S32 i;
	HI_RECT Rect;
	WV_U32 color;	
	TSK_GO_POS_E *pPos = (TSK_GO_MOV_PIC_POS_S *)pData;	
	WV_U16 *pTemp = (WV_U16 *)pData;
/*
	for(i=0;i<2;i++)
	{
		printf("pTemp[%d]=%d pTemp[%d]=%d pTemp[%d]=%d pTemp[%d]=%d \n",4*i,pTemp[4*i],4*i+1,pTemp[4*i+1],4*i+2,pTemp[4*i+2],4*i+3,pTemp[4*i+3]);
	}
*/

	if(dataLen != 28)
	{
		WV_printf("TSK_GO_RectPicAndMovPos get dataLen = %d (28)\n",dataLen);
		return WV_EFAIL;
	}


	if(gGoDev.showPosEna == 0 )
	{
		//TSK_SCENE_CloseWinAndAni();
		for(i = 0;i< HIS_HIGO_GODEC_NUM ; i++ )
		{
			TSK_GO_DecClose(i);
	    }
		TSK_SCENE_SetWinNum(0);
		TSK_SCENE_ConfWin();
		usleep(500000);
		gGoDev.showPosEna = 1;
	}

	TSK_GO_CleanLayer();
	memcpy(&gUsbPos,pData,dataLen);

	for(i=0;i<3;i++)
	{

		switch(i)
		{
			case 0:
				color = 0xffff0000;//red 
				break;
			case 1:
				color = 0xff00ff00;//green
				break;
			case 2:
				color = 0xff0000ff;//blue
				break;
			default:
				color = 0;
				break;
			 
		}
		//printf("startX[%d]= %d,startY[%d] =%d,endX[%d]=%d,endY[%d]=%d \n",i,pPos[i].startX,i,pPos[i].startY,i,pPos[i].endX,i,pPos[i].endY);
		if(gUsbPos.pos[i].startX != 0 || gUsbPos.pos[i].startY != 0 || gUsbPos.pos[i].endX != 0 || gUsbPos.pos[i].endY != 0 )
		{
			Rect.x = gUsbPos.pos[i].startX/2;
			Rect.y = gUsbPos.pos[i].startY/2;
			Rect.w = (gUsbPos.pos[i].endX-gUsbPos.pos[i].startX)/2;
			Rect.h = (gUsbPos.pos[i].endY-gUsbPos.pos[i].startY)/2;
			HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, color, HIGO_COMPOPT_NONE);
		}
					
	}


	HIS_GO_RefreshLayerForPic(&gGoDev.goDev);
	if(saveMode == 1)
	{
		TSK_GO_SaveUsb_PicAndMov_Pos(dataLen,pData);
		if(BackAniEna == 1)
		{

			//背景图片位置和开窗位置相同
			gBackAniPos.startX = gUsbPos.pos[2].startX;
			gBackAniPos.startY = gUsbPos.pos[2].startY;
			gBackAniPos.endX = gUsbPos.pos[2].endX;
			gBackAniPos.endY = gUsbPos.pos[2].endY;	
			TSK_GO_SaveUsb_BackAni_Conf(sizeof(TSK_GO_MOV_PIC_POS_S),(WV_U8 *)&gBackAniPos);
		}
		TSK_GO_CleanLayer();
		HIS_GO_RefreshLayerForPic(&gGoDev.goDev);
		TSK_SCENE_OpenWinAndAni();
		gGoDev.showPosEna = 0;
	}
	return WV_SOK;
}

/*****************************************************************************
WV_S32 TSK_GO_SetColorKeyFlag(WV_U32 ena);
*****************************************************************************/
WV_S32 TSK_GO_SetColorKeyFlag(WV_U32 ena)
{
	if(ena==1 || ena == 0 )
	{
		gGoDev.colorKeyEna = ena;
		return WV_SOK;
	}
	return WV_EFAIL;	
}
/*****************************************************************************
WV_S32 TSK_GO_RefreshLayerForPic();
*****************************************************************************/
WV_S32 TSK_GO_RefreshLayerForPic()
{
	HIS_GO_SetColorKey(gGoDev.colorKeyEna,0xff000000);
	HIS_GO_RefreshLayerForPic(&gGoDev.goDev);

	return WV_SOK;
}
/******************************************************************************

void *TSK_GO_Proc(Void * prm);
TSK_GO_OPEN的线程处理函数
******************************************************************************/
void * TSK_GO_Proc ( void * prm)
{
	TSK_GO_DEV_E    * pDev; 
	HIS_DEV_HIGO_E  * pGoDev; 
	WV_S32  fresh,ret,i;
	pDev = (TSK_GO_DEV_E *)prm;

	pGoDev = &(pDev -> goDev);  
	pDev-> open  = 1;
	pDev-> close  = 0;  

	while(pDev -> open == 1)
    {  
		  fresh = 0; 
			
		  for(i=0;i<TSK_HIGO_ANI_MAX_NUM;i++)
		  {
				
			  	ret = HIS_GODEC_Render(&(pGoDev->imgDev[i]));
			  	if(ret == 1)
			  	{
					fresh =1; 
			  	} 
		   }
		  if(fresh == 1)
		   {
				HIS_GO_SetColorKey(gGoDev.colorKeyEna,0xff000000);
				HIS_GO_RefreshLayerForPic(pGoDev);
		   }
		   else
		   {
		     usleep(300000);
		   }	
		   	
    } 
	pDev-> open  = 0;
	pDev-> close = 1;
	return NULL; 
} 
/***************************************************************

WV_S32 TSK_GO_DecOpen(WV_U32 aniId,WV_S8 *pFileName, WV_U32 x, WV_U32 y,WV_U32 w,WV_U32 h);

***************************************************************/
WV_S32 TSK_GO_DecOpen(WV_U32 aniId,WV_S8 *pFileName, WV_U32 x, WV_U32 y,WV_U32 w,WV_U32 h)
{ 
 
	WV_S32 ret=0;
    HIS_DEV_GODEC_E * pDev;
    //WV_S8 pFileName[20];
   
    if( aniId >= TSK_HIGO_ANI_MAX_NUM)
    {
       return WV_EFAIL;
    }
    
  if( aniId >= TSK_HIGO_FILE_MAX_NUM)
    {
       return WV_EFAIL;
    }
    pDev  =&(gGoDev.goDev.imgDev[aniId]);
    pDev->bkDisX = x/2;
    pDev->bkDisY = y/2;
    pDev->bkDisW = w/2;
    pDev->bkDisH = h/2;
	printf("tsk_go_decOpen x=%d,y=%d,w=%d,h=%d\n",x/2,y/2,w/2,h/2); 
    ret = HIS_GODEC_Open(pDev,pFileName,gGoDev.goDev.layerSurfHndl);
   return  ret; 
}

/***************************************************************

WV_S32 TSK_GO_RefreshLayer();

***************************************************************/	
WV_S32 TSK_GO_RefreshLayer()
{
	HIS_GO_RefreshLayer(&gGoDev.goDev);
	return 0;
}

/***************************************************************

WV_S32 TSK_GO_CleanLayer();

***************************************************************/

WV_S32 TSK_GO_CleanLayer()
{
	HI_RECT Rect;

	Rect.x = 0;
	Rect.y =0;
	Rect.w = RECT_W;
	Rect.h = RECT_H+8;	
	HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0, HIGO_COMPOPT_NONE);
	return 0;
}

/***************************************************************

WV_S32 TSK_GO_DecClose( WV_U32 id);

***************************************************************/
WV_S32 TSK_GO_DecClose( WV_U32 id)
{ 
    
	//printf("go dec close %d \n",id);
    HIS_DEV_GODEC_E * pDev; 
    if( id >= HIS_HIGO_GODEC_NUM )
    {
       return WV_EFAIL;
    }
    pDev  =&(gGoDev.goDev.imgDev[id]); 
    WV_CHECK_RET( HIS_GODEC_Close(pDev));
   return  WV_SOK; 
}
//#####################################################################################

/***************************************************************

WV_S32 TSK_GO_CleanRect( WV_U32 x,WV_U32 y,WV_U32 w,WV_U32 h);

***************************************************************/ 
WV_S32 TSK_GO_CleanRect( WV_U32 x,WV_U32 y,WV_U32 w,WV_U32 h)
{
	HI_RECT Rect;
	Rect.x = x;
	Rect.y = y;
	Rect.w = w;
	Rect.h = h;

	HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0, HIGO_COMPOPT_NONE);
return WV_SOK;
}


/***************************************************************

TSK_GO_ShowText( WV_U32 x,WV_U32 y,WV_U32 w,WV_U32 h,WV_U32 color,WV_S8 *pText,WV_U32 codeType,WV_U32 border  );
COdeType:0 GB2312,1 UTF-8
border:0 disable,1 enable
***************************************************************/
WV_S32 TSK_GO_ShowText( WV_U32 x,WV_U32 y,WV_U32 w,WV_U32 h,WV_U32 color,WV_U32 size,WV_S8 *pText,WV_U32 codeType,WV_U32 border)
{ 

	//printf("\nTSK_GO_ShowText,x=%d,y=%d,w=%d,h=%d,color=%x\n",x,y,w,h,color);
	HI_RECT rc = {0};
    HI_HANDLE hFont = HIGO_INVALID_HANDLE;
	WV_S32 s32Ret;

	HIGO_TEXT_INFO_S TextInfo;
	TextInfo.pSbcFontFile = TSK_GO_TEXT_LIB_FILE;
	TextInfo.pMbcFontFile = NULL;
	TextInfo.u32Size = size;

 	s32Ret =HI_GO_CreateTextEx(&TextInfo, &hFont);
    if (HI_SUCCESS != s32Ret) 
    {
		WV_ERROR("ret=%d\n",s32Ret);
      return WV_EFAIL;;       
    }
	//set charset to gb2312
	if(codeType == 0)
	{
 		s32Ret =HI_GO_SetInputCharSet  (hFont,HIGO_CHARSET_GB2312);
	}else{
		
		s32Ret =HI_GO_SetInputCharSet  (hFont,HIGO_CHARSET_UTF8);
	}
    if (HI_SUCCESS != s32Ret) 
    {
		WV_ERROR("ret=%d\n",s32Ret);
		goto err;    
    }
	//set text color 
	color |= 0xff000000;
    s32Ret = HI_GO_SetTextColor(hFont, color);
    if (HI_SUCCESS != s32Ret) 
    {
		WV_ERROR("ret=%d\n",s32Ret);
		goto err;   
    }

    rc.x = x;
    rc.y = y;
    rc.w = w;
    rc.h = h;

	if(border == 0){
		s32Ret =HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &rc, 0, HIGO_COMPOPT_NONE);
	}else{
		s32Ret = HI_GO_DrawRect(gGoDev.goDev.layerSurfHndl, &rc, 0xffffffff);
	}    
    if (HI_SUCCESS != s32Ret) 
    {
		WV_ERROR("ret=%d\n",s32Ret);
		goto err;    
    }
    
    s32Ret = HI_GO_TextOutEx(hFont, gGoDev.goDev.layerSurfHndl, pText, &rc, HIGO_LAYOUT_LEFT);

    if (HI_SUCCESS != s32Ret)
    {
		WV_ERROR("ret=%d\n",s32Ret);
		goto err;
    }

err:
	s32Ret= HI_GO_DestroyText(hFont);
	if (HI_SUCCESS != s32Ret)
	{
		WV_ERROR("ret=%d\n",s32Ret);
		return WV_EFAIL; 
	}
	return WV_SOK;
}

/****************************************************************************

WV_S32 TSK_GO_CmdGet(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_GO_CmdGet(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
WV_U32  id;
if(argc<1) 
  {
   	prfBuff += sprintf(prfBuff,"get higo <id> \r\n");
    return 0;
   } 
WV_ASSERT_RET(WV_STR_S2v(argv[0],&id)); 
 
 return HIS_GO_PrintDec(&gGoDev.goDev.imgDev[id],prfBuff);
 
prfBuff += sprintf(prfBuff,"no this cmd !  \r\nget hidec  <id>\r\n");  
return WV_SOK;
 
}


/****************************************************************************

WV_S32 TSK_GO_CmdSet(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_GO_CmdSet(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
WV_U32   x,y,w,h,id;
WV_U32 row,col,lc,bc,chl;
WV_S32   ret;
	if(argc<2) 
	  {
	   	prfBuff += sprintf(prfBuff,"set higo <id> < cmd> ..... cmd: file /pose /colorkey/ena/draw/drawexit/drawlight/rectline/rectstart/rectexit/rectgrid/drawE/FadeIn  \r\n");
		return WV_SOK;
	  } 
	WV_ASSERT_RET(WV_STR_S2v(argv[0],&id)); 
	if(id >= HIS_HIGO_GODEC_NUM)  
	{
		prfBuff += sprintf(prfBuff,"input id erro !!!  id < %d \r\n",HIS_HIGO_GODEC_NUM);
		return WV_SOK; 
	}
 
 if(strcmp(argv[1],"file") ==0)
    { 
		 if(argc<3)
		 {
		  	prfBuff += sprintf(prfBuff,"set higo <id> file  <fileName>\r\n"); 
		    return  WV_SOK;
		 }
		 ret = WV_FILE_Access(argv[2]);  
		 if(ret != WV_SOK)
		 {
		   prfBuff += sprintf(prfBuff,"no the file %s\r\n",argv[2]); 
		   return  WV_SOK;
		 }
      HIS_GODEC_Close( &(gGoDev.goDev.imgDev[id]));
      HIS_GODEC_Open(&(gGoDev.goDev.imgDev[id]),argv[2],gGoDev.goDev.layerSurfHndl);      
      return  WV_SOK;
      
    }
  if(strcmp(argv[1],"pos") ==0)
    { 
		 if(argc<6)
			 { 
				prfBuff += sprintf(prfBuff,"set higo <id>  pos <x> <y> <w> <h>\r\n"); 
				return  WV_SOK;
			 }
		 WV_STR_S2v(argv[2],&x);
		 WV_STR_S2v(argv[3],&y);
		 WV_STR_S2v(argv[4],&w);
		 WV_STR_S2v(argv[5],&h);
		 
		 if(x + w > 3840  || w < 64)
			 {
			   prfBuff += sprintf(prfBuff,"input erro!! x w\r\n"); 
				return  WV_SOK; 
				
			 }
		if(y + h > 1920  || h < 64)
			 {
			   prfBuff += sprintf(prfBuff,"input erro!! y h \r\n"); 
				return  WV_SOK; 
			 }
		 gGoDev.goDev.imgDev[id].bkDisX = x;
		 gGoDev.goDev.imgDev[id].bkDisY = y;
		 gGoDev.goDev.imgDev[id].bkDisW = w;
		 gGoDev.goDev.imgDev[id].bkDisH = h;  
		 return  WV_SOK;
    }  
    if(strcmp(argv[1],"colorkey") ==0)
    { 
		if(argc<3)
			{
			prfBuff += sprintf(prfBuff,"set higo <id>  colorkey   <ena> ena: 1/0 \r\n"); 
			return  WV_SOK;
			}
		WV_STR_S2v(argv[2],&x);
		if(x == 1)
			{  
			   gGoDev.goDev.imgDev[id].colorKeyEna = 1; 
			}   
		 else
			 {
			   gGoDev.goDev.imgDev[id].colorKeyEna = 0; 
			 }  
		return  WV_SOK;
    }       
    if(strcmp(argv[1],"pause") ==0)
    {
         
        gGoDev.goDev.imgDev[id].ena = 1;  
		return  WV_SOK;
    }
    if(strcmp(argv[1],"draw") == 0)
    {
	if(argc<7)
	{
		prfBuff += sprintf(prfBuff,"set draw <chl><row> <col> <backColor><lineColor>   \r\n"); 
			return  WV_SOK;
	}
		
	
	WV_STR_S2v(argv[2],&chl);
	WV_STR_S2v(argv[3],&row);
	WV_STR_S2v(argv[4],&col);
	WV_STR_S2v(argv[5],&bc);
	WV_STR_S2v(argv[6],&lc);
		
	TSK_GO_DrawGrid_Test(chl,bc,lc,row,col);
	return WV_SOK;
    }
    
    if(strcmp(argv[1],"drawline") == 0)
    {
	WV_U16 x0,y0,x1,y1;
	if(argc<6)
	{
		prfBuff += sprintf(prfBuff,"set drawline <x0><y0> <x1> <y1> \r\n"); 
			return  WV_SOK;
	}
	WV_STR_S2v(argv[2],&x0);
	WV_STR_S2v(argv[3],&y0);
	WV_STR_S2v(argv[4],&x1);
	WV_STR_S2v(argv[5],&y1);
	TSK_GO_DrawLine_Test(x0,y0,x1,y1);
	
	return WV_SOK;
    } 

    if(strcmp(argv[1],"drawexit") == 0)
    {
		TSK_GO_DrawGrid_EXIT();
		return WV_SOK;
    }
	
    if(strcmp(argv[1],"rectline") == 0)
    {
		TSK_GO_FillRectLine();	
		return WV_SOK;
    }  
	
    if(strcmp(argv[1],"rectexit") == 0)
    {
	
		TSK_GO_FillRect_EXIT();	
		return WV_SOK;
     }
    if(strcmp(argv[1],"rectstart") == 0)
    {
	
		TSK_GO_FillRect_Start();	
		return WV_SOK;
     }
    if(strcmp(argv[1],"rectgrid") == 0)
    {
	
		TSK_GO_FillRectGrid_Test();	
		return WV_SOK;
     }
    if(strcmp(argv[1],"drawlight") == 0)
    {
		if(argc<4)
		{
			prfBuff += sprintf(prfBuff,"set drawlight <color> <num> \r\n"); 
				return  WV_SOK;
		}
		WV_U32 color;
		WV_U32 num;
		WV_STR_S2v(argv[2],&color);
		WV_STR_S2v(argv[3],&num);

		
		return WV_SOK;
     }	
    if(strcmp(argv[1],"drawE") == 0)
    {
		WV_U32 x,y,r;

		WV_STR_S2v(argv[2],&x);
		WV_STR_S2v(argv[3],&y);

		WV_STR_S2v(argv[4],&r);
		//TSK_GO_DrawEllipse(x,y,r,0xffff0000);

		//HIS_GO_RefreshLayer(&gGoDev.goDev);	
		return 0;
	}
    if(strcmp(argv[1],"FadeIn") == 0)
    {
		//TSK_GO_FadeIn();
		return 0;
	}


    prfBuff += sprintf(prfBuff,"set higo <id> < cmd> ..... cmd: file /pose /colorkey  \r\n");; 
    prfBuff += sprintf(prfBuff,"no this CMD %s\r\n",argv[0]); 
    return WV_SOK;
 
}

/****************************************************************************

WV_S32 TSK_GO_CmdSetText(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_GO_CmdSetText(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{

	//TSK_GO_ShowText( );
	if(argc<6) 
	  {
	   	prfBuff += sprintf(prfBuff,"set text <x> <y> <w> <h> <color> <text>  \r\n");
		return WV_SOK;
	  } 
	WV_S32 x,y,w,h,color;
	WV_STR_S2v(argv[0],&x);
	WV_STR_S2v(argv[1],&y);
	WV_STR_S2v(argv[2],&w);
	WV_STR_S2v(argv[3],&h);
	WV_STR_S2v(argv[4],&color);
	WV_S32 i;
	for(i=0;i<100;i++)
	{
		if(x-i*5 < 0)
		{
			TSK_GO_ShowText( x-i*5,y,w,h,color,22,argv[5],1,1);
		}		
		usleep(500000);
	}
    prfBuff += sprintf(prfBuff,"TSK_GO_ShowText() \r\n"); 
    return WV_SOK;
 
}


/****************************************************************************

WV_S32 TSK_GO_SetBank(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_GO_SetBack(WV_S32 argc, WV_S8 **argv, WV_S8 *prfBuff)
{
	WV_U32 color;
	WV_U32 devAddr;
	WV_U8 buf[255] = {0};
	WV_S32 ret, i;

	HI_RECT Rect;
	Rect.x = 0;
	Rect.y = 0;
	Rect.w = 3840;
	Rect.h = 2160;

	if (argc < 1)
	{

		prfBuff += sprintf(prfBuff, "set back <cmd> //cmd like:color/clear\r\n");
		return 0;
	}
	if (strcmp(argv[0], "color") == 0)
	{
		if (argc < 2)
		{
			prfBuff += sprintf(prfBuff, "set back color <val>//val like:0xffff0000\r\n");
		}

		ret = WV_STR_S2v(argv[1], &color);
		if (ret != WV_SOK)
		{

			prfBuff += sprintf(prfBuff, "input erro!\r\n");
		}
		color = color | 0xff000000;
		HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, color, HIGO_COMPOPT_NONE);
		HIS_GO_RefreshLayer(&gGoDev.goDev);
	}else 	if (strcmp(argv[0], "clear") == 0)
	{
		HI_GO_FillRect(gGoDev.goDev.layerSurfHndl, &Rect, 0, HIGO_COMPOPT_NONE);
		HIS_GO_RefreshLayer(&gGoDev.goDev);
	}

}
  
/***************************************************************

WV_S32 TSK_GO_Open();

***************************************************************/

WV_S32 TSK_GO_Open()
{

	pColorTable = (WV_U32 *)malloc(COLOR_NUM*4);
	//注册一些和go相关的串口命令
	WV_CMD_Register("get","higo","get higo mode info ",TSK_GO_CmdGet);
	WV_CMD_Register("set","higo","set higo mode  ",TSK_GO_CmdSet);
	WV_CMD_Register("set","text","set higo mode  ",TSK_GO_CmdSetText);
	WV_CMD_Register("set","back","set higo mode  ",TSK_GO_SetBack);

	gGoDev.drawState = DRAW_EXIT_STATE;
	gGoDev.colorKeyEna = 1;
	memset(&gLightData,0,sizeof(gLightData));
   WV_CHECK_RET( HIS_GO_GetConf(&gGoDev.goDev,0));  //获取配置
   WV_CHECK_RET( HIS_GO_Open(&gGoDev.goDev));       //打开HIS_GO
	
	if(TSK_GO_ReadUsb_PicAndMov_Pos(28,(WV_U8 *)&gUsbPos) != 0 )
	{

		gUsbPos.pos[0].startX = 0;
		gUsbPos.pos[0].startY = 0;
		gUsbPos.pos[0].endX = 0;
		gUsbPos.pos[0].endY = 0;

		gUsbPos.pos[1].startX = 0;
		gUsbPos.pos[1].startY = 0;
		gUsbPos.pos[1].endX = 0;
		gUsbPos.pos[1].endY = 0;

		gUsbPos.pos[3].startX = 0;
		gUsbPos.pos[3].startY = 0;
		gUsbPos.pos[3].endX = 0;
		gUsbPos.pos[3].endY = 0;
		
	}

//static TSK_GO_MOV_PIC_POS_S gBackAniPos;
	if(TSK_GO_ReadUsb_BackAni_Conf(sizeof(TSK_GO_MOV_PIC_POS_S),(WV_U8 *)&gBackAniPos) != 0 )
	{
		gBackAniPos.startX = 0;
		gBackAniPos.startY = 0;
		gBackAniPos.endX = 0;
		gBackAniPos.endY = 0;		
	}

	//gRecvPicFlag=2;
   WV_CHECK_RET( WV_THR_Create(&gGoDev.thrHndl, TSK_GO_Proc , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gGoDev)); 
   return  WV_SOK; 
}

/***************************************************************

WV_S32 TSK_GO_Close();

***************************************************************/

WV_S32 TSK_GO_Close()
{ 
   if(gGoDev.open == 1)
   {
   	  gGoDev.open = 0;
   	  while(gGoDev.close == 1) ;
     WV_CHECK_RET( WV_THR_Destroy(&gGoDev.thrHndl));	
   } 
	 
   WV_CHECK_RET( HIS_GO_Close(&(gGoDev.goDev)) );  
   free(pColorTable);
   return  WV_SOK;
 
}

/****************************************************************************

WV_S32 TSK_GO_DisPlayBmp()
//显示BMP图
****************************************************************************/
WV_S32 TSK_GO_DisPlayBmp(HI_CHAR *FileName,const HI_RECT* pSrcRect,const HI_RECT* pDstRect)
{
    HI_HANDLE decHndl;
    HIGO_DEC_ATTR_S SrcDesc;
    HIGO_DEC_PRIMARYINFO_S decInfo;
    HI_HANDLE bkSurfHndl = 0;
    HIGO_BLTOPT_S BltOpt = {0};
    BltOpt.EnableScale = HI_TRUE;
    HI_HANDLE screenSurf[2];
    WV_S32 ret;

    SrcDesc.SrcType = HIGO_DEC_SRCTYPE_FILE;
    SrcDesc.SrcInfo.pFileName = FileName;
    WV_CHECK_RET( HI_GO_CreateDecoder(&SrcDesc, &(decHndl)));
    WV_CHECK_RET( HI_GO_DecCommInfo(decHndl, &(decInfo)));
    if(bkSurfHndl != 0)
    {
        WV_CHECK_RET( HI_GO_FreeSurface(bkSurfHndl));
        bkSurfHndl = 0;
        return WV_EFAIL;
    }
    WV_CHECK_RET( HI_GO_DecImgData(decHndl,0, NULL, &(bkSurfHndl)));
    WV_ASSERT_RET( HI_GO_Blit(bkSurfHndl, NULL, gGoDev.goDev.layerSurfHndl, NULL, &BltOpt));
    WV_CHECK_RET(HI_GO_FreeSurface(bkSurfHndl));
    ret = HIS_FB_SurfGetForPic(screenSurf);
    if(ret == WV_EFAIL)
    {
        return WV_EFAIL;
    }
    BltOpt.EnableScale = HI_TRUE;
    WV_CHECK_RET( HI_GO_Blit(gGoDev.goDev.layerSurfHndl,pSrcRect,screenSurf[0], pDstRect, &BltOpt));
    return  WV_SOK;
}

