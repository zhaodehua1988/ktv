#include"tsk_fpga.h"
#include"his_fb.h"
#include "hi_common.h"
#include "his_spi.h"
#include "fpga_conf.h"
#include "sys_env.h"
#include "sys_env.h"
#include "his_temperature.h"
#include "tsk_scene.h"
#define  TSK_FPGA_MAP_COL    1920  
#define  TSK_FPGA_MAP_ROW    1088


#define  TSK_FPGA_OUT_COL    128  
#define  TSK_FPGA_OUT_ROW    1080

#define  TSK_FPGA_SPLIT_COL    1024 
#define  TSK_FPGA_SPLIT_ROW    4

#define  TSK_FPGA_COLOR_COL    1024 
#define  TSK_FPGA_COLOR_ROW    2

#define  TSK_FPGA_CMD_VIDEO    0
#define  TSK_FPGA_CMD_MAP      1
#define  TSK_FPGA_CMD_OUT      2
#define  TSK_FPGA_CMD_SPLIT    3
#define  TSK_FPGA_CMD_COLOR    4
#define  TSK_FPGA_CMD_SPLIT_V    5

typedef struct TSK_FPGA_DEV_E 
{
    WV_THR_HNDL_T thrHndl;
    WV_U32      open;
    WV_U32      close;
    WV_U32      plug;
    WV_U32      mount;
    WV_U32      plugCnt;
    WV_S8       mapFile[30];
    WV_U8       *pMapBuf;
    WV_S8        outFile[30];
    WV_U8       *pOutBuf;
    WV_S8       splitFile[30];
    WV_U8       *pSplitBuf;
    WV_S8       colorFile[30];
    WV_U8       *pColorBuf;

    WV_U8       *pTestBuf;

    FPGA_SPLIT_INFO_S splitInfo;
    FPGA_RESOLUTION_INFO_S   resolution;
    WV_U16   outputEna;

}TSK_FPGA_DEV_E;

static TSK_FPGA_DEV_E gFpgaDev;

static int loopFlag;
/**************************************************************
WV_S32 TSK_FPGA_Read(WV_U16 addr,WV_U16 *pData);
**************************************************************/

WV_S32 TSK_FPGA_Read(WV_U16 addr,WV_U16 *pData){

    HIS_SPI_FpgaRd(addr,pData);
    return WV_SOK;
}

/*******************************************************************
 WV_S32 TSK_FPGA_RegisterConf();
*******************************************************************/

WV_S32 TSK_FPGA_RegisterConf()
{


    WV_S8  name[WV_CONF_NAME_MAX_LEN],value[WV_CONF_VALUE_MAX_LEN];

    // sprintf(name,"SceneAllConfig");
    //split y
    SYS_ENV_Register(0,"FpgaAllConfig",NULL,NULL);
    sprintf(name, "Split_startY0");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startY0");
    sprintf(name, "Split_endY0");
    sprintf(value, "%d",1079);
    SYS_ENV_Register(1 ,name,value,"Split_endY0");

    sprintf(name, "Split_startY1");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startY1");
    sprintf(name, "Split_endY1");
    sprintf(value, "%d",1079);
    SYS_ENV_Register(1 ,name,value,"Split_endY1");

    sprintf(name, "Split_startY2");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startY2");
    sprintf(name, "Split_endY2");
    sprintf(value, "%d",1079);
    SYS_ENV_Register(1 ,name,value,"Split_endY2");

    sprintf(name, "Split_startY3");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startY3");
    sprintf(name, "Split_endY3");
    sprintf(value, "%d",1079);
    SYS_ENV_Register(1 ,name,value,"Split_endY3");

    sprintf(name, "Split_startY4");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startY4");
    sprintf(name, "Split_endY4");
    sprintf(value, "%d",1079);
    SYS_ENV_Register(1 ,name,value,"Split_endY4");

    sprintf(name, "Split_startY5");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startY5");
    sprintf(name, "Split_endY5");
    sprintf(value, "%d",1079);
    SYS_ENV_Register(1 ,name,value,"Split_endY5");

    sprintf(name, "Split_startY6");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startY6");
    sprintf(name, "Split_endY6");
    sprintf(value, "%d",1079);
    SYS_ENV_Register(1 ,name,value,"Split_endY6");

    sprintf(name, "Split_startY7");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startY7");
    sprintf(name, "Split_endY7");
    sprintf(value, "%d",1079);
    SYS_ENV_Register(1 ,name,value,"Split_endY7");


    /*******************************************************************/
    //split x
    sprintf(name, "Split_startX0");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startX0");
    sprintf(name, "Split_endX0");
    sprintf(value, "%d",959);
    SYS_ENV_Register(1 ,name,value,"Split_endX0");
    //
    sprintf(name, "Split_startX1");
    sprintf(value, "%d",960);
    SYS_ENV_Register(1 ,name,value,"Split_startX1");
    sprintf(name, "Split_endX1");
    sprintf(value, "%d",1919);
    SYS_ENV_Register(1 ,name,value,"Split_endX1");
    //
    sprintf(name, "Split_startX2");
    sprintf(value, "%d",0);
    SYS_ENV_Register(1 ,name,value,"Split_startX2");
    sprintf(name, "Split_endX2");
    sprintf(value, "%d",959);
    SYS_ENV_Register(1 ,name,value,"Split_endX2");
    //
    sprintf(name, "Split_startX3");
    sprintf(value, "%d",960);
    SYS_ENV_Register(1 ,name,value,"Split_startX3");
    sprintf(name, "Split_endX3");
    sprintf(value, "%d",1919);
    SYS_ENV_Register(1 ,name,value,"Split_endX3");
    //
    sprintf(name, "Split_startX4");
    sprintf(value, "%d",960);
    SYS_ENV_Register(1 ,name,value,"Split_startX4");
    sprintf(name, "Split_endX4");
    sprintf(value, "%d",1919);
    SYS_ENV_Register(1 ,name,value,"Split_endX4");
    //
    sprintf(name, "Split_startX5");
    sprintf(value, "%d",960);
    SYS_ENV_Register(1 ,name,value,"Split_startX5");
    sprintf(name, "Split_endX5");
    sprintf(value, "%d",1919);
    SYS_ENV_Register(1 ,name,value,"Split_endX5");
    //
    sprintf(name, "Split_startX6");
    sprintf(value, "%d",960);
    SYS_ENV_Register(1 ,name,value,"Split_startX6");
    sprintf(name, "Split_endX6");
    sprintf(value, "%d",1919);
    SYS_ENV_Register(1 ,name,value,"Split_endX6");
    //
    sprintf(name, "Split_startX7");
    sprintf(value, "%d",960);
    SYS_ENV_Register(1 ,name,value,"Split_startX7");
    sprintf(name, "Split_endX7");
    sprintf(value, "%d",1919);
    SYS_ENV_Register(1 ,name,value,"Split_endX7");

    /***********************************************************/
    //set output
    sprintf(name, "Output_ena");
    sprintf(value, "%d",0x003f);
    SYS_ENV_Register(1 ,name,value,"output ena");

    //
    sprintf(name, "Output_fRate");
    sprintf(value, "%d",60);
    SYS_ENV_Register(1 ,name,value,"Output frame rate");

    sprintf(name, "Output_totalPix");
    sprintf(value, "%d",2200);
    SYS_ENV_Register(1 ,name,value,"Resolution totalPix");

    sprintf(name, "Output_totalLine");
    sprintf(value, "%d",1125);
    SYS_ENV_Register(1 ,name,value,"Resolution totalLine");

    sprintf(name, "Output_hsyncWidth");
    sprintf(value, "%d",44);
    SYS_ENV_Register(1 ,name,value,"Output hsyncWidth");

    sprintf(name, "Output_vsyncWidth");
    sprintf(value, "%d",5);
    SYS_ENV_Register(1 ,name,value,"Output vsyncWidth");

    sprintf(name, "Output_actHstart");
    sprintf(value, "%d",192);
    SYS_ENV_Register(1 ,name,value,"Output actHstart");


    sprintf(name, "Output_actHsize");
    sprintf(value, "%d",1920);
    SYS_ENV_Register(1 ,name,value,"Output actHsize1");

    sprintf(name, "Output_actVstart");
    sprintf(value, "%d",41);
    SYS_ENV_Register(1 ,name,value,"Output actVstart");

    sprintf(name, "Output_actVsize");
    sprintf(value, "%d",1080);
    SYS_ENV_Register(1 ,name,value,"Output actVsize");

    return WV_SOK;
}



/*******************************************************************
 WV_S32 TSK_FPGA_GetConf();
*******************************************************************/

WV_S32 TSK_FPGA_GetConf()
{


    WV_S8  name[WV_CONF_NAME_MAX_LEN];
    WV_U32 data,*pData;
    pData = & data;
    /**********************************************/
    //get y
    sprintf(name, "Split_startY0");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startY0 = data;
    sprintf(name, "Split_endY0");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endY0 = data;
    //
    sprintf(name, "Split_startY1");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startY1 = data;
    sprintf(name, "Split_endY1");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endY1 = data;
    //
    sprintf(name, "Split_startY2");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startY2 = data;
    sprintf(name, "Split_endY2");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endY2 = data;
    //
    sprintf(name, "Split_startY3");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startY3 = data;
    sprintf(name, "Split_endY3");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endY3 = data;
    //
    sprintf(name, "Split_startY4");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startY4 = data;
    sprintf(name, "Split_endY4");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endY4 = data;
    //
    sprintf(name, "Split_startY5");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startY5 = data;
    sprintf(name, "Split_endY5");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endY5 = data;
    //
    sprintf(name, "Split_startY6");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startY6 = data;
    sprintf(name, "Split_endY6");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endY6 = data;
    //
    sprintf(name, "Split_startY7");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startY7 = data;
    sprintf(name, "Split_endY7");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endY7 = data;

    /******************************************************/
    //get x
    sprintf(name, "Split_startX0");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startX0 = data;
    sprintf(name, "Split_endX0");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endX0 = data;;
    //
    sprintf(name, "Split_startX1");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startX1 = data;
    sprintf(name, "Split_endX1");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endX1 = data;
    //
    sprintf(name, "Split_startX2");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startX2 = data;
    sprintf(name, "Split_endX2");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endX2 = data;
    //
    sprintf(name, "Split_startX3");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startX3 = data;
    sprintf(name, "Split_endX3");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endX3 = data;
    //
    sprintf(name, "Split_startX4");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startX4 = data;
    sprintf(name, "Split_endX4");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endX4 = data;
    //
    sprintf(name, "Split_startX5");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startX5 = data;
    sprintf(name, "Split_endX5");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endX5 = data;
    //
    sprintf(name, "Split_startX6");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startX6 = data;
    sprintf(name, "Split_endX6");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endX6 = data;
    //
    sprintf(name, "Split_startX7");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.startX7 = data;
    sprintf(name, "Split_endX7");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.splitInfo.endX7 = data;
    //
    /*********************************************/
    sprintf(name, "Output_ena");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.outputEna = data;

    /*******************************************/
    sprintf(name, "Output_fRate");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.frameRate = data;

    sprintf(name, "Output_totalPix");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.totalPix = data;

    sprintf(name, "Output_totalLine");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.totalLine = data;

    sprintf(name, "Output_hsyncWidth");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.hsyncWidth = data;

    sprintf(name, "Output_vsyncWidth");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.vsyncWidth = data;

    sprintf(name, "Output_actHstart");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.actHstart = data;


    sprintf(name, "Output_actHsize");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.actHsize = data;

    sprintf(name, "Output_actVstart");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.actVstart = data;

    sprintf(name, "Output_actVsize");
    SYS_ENV_GetU32(name,pData);
    gFpgaDev.resolution.actVsize = data;

    return WV_SOK;
}

/*******************************************************************
 WV_S32 TSK_FPGA_SaveConf();
*******************************************************************/

WV_S32 TSK_FPGA_SaveConf()
{


    WV_S8  name[WV_CONF_NAME_MAX_LEN];
    WV_U32 data,*pData;
    pData = & data;
    /**********************************************/
    //save y 0
    sprintf(name, "Split_startY0");
    data =  gFpgaDev.splitInfo.startY0;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endY0");
    data =  gFpgaDev.splitInfo.endY0;
    SYS_ENV_SetU32(name,data);
    //  1
    sprintf(name, "Split_startY1");
    data =  gFpgaDev.splitInfo.startY1;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endY1");
    data =  gFpgaDev.splitInfo.endY1;
    SYS_ENV_SetU32(name,data);
    //  2
    sprintf(name, "Split_startY2");
    data =  gFpgaDev.splitInfo.startY2;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endY2");
    data =  gFpgaDev.splitInfo.endY2;
    SYS_ENV_SetU32(name,data);
    //  3
    sprintf(name, "Split_startY3");
    data =  gFpgaDev.splitInfo.startY3;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endY3");
    data =  gFpgaDev.splitInfo.endY3;
    SYS_ENV_SetU32(name,data);
    //  4
    sprintf(name, "Split_startY4");
    data =  gFpgaDev.splitInfo.startY4;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endY4");
    data =  gFpgaDev.splitInfo.endY4;
    SYS_ENV_SetU32(name,data);
    //  5
    sprintf(name, "Split_startY5");
    data =  gFpgaDev.splitInfo.startY5;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endY5");
    data =  gFpgaDev.splitInfo.endY5;
    SYS_ENV_SetU32(name,data);
    //  6
    sprintf(name, "Split_startY6");
    data =  gFpgaDev.splitInfo.startY6;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endY6");
    data =  gFpgaDev.splitInfo.endY6;
    SYS_ENV_SetU32(name,data);
    //  7
    sprintf(name, "Split_startY7");
    data =  gFpgaDev.splitInfo.startY7;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endY7");
    data =  gFpgaDev.splitInfo.endY7;
    SYS_ENV_SetU32(name,data);
    /**************************************************************/
    //save x
    sprintf(name, "Split_startX0");
    data =  gFpgaDev.splitInfo.startX0;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endX0");
    data =  gFpgaDev.splitInfo.endX0;
    SYS_ENV_SetU32(name,data);
    //
    sprintf(name, "Split_startX1");
    data =  gFpgaDev.splitInfo.startX1;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endX1");
    data =  gFpgaDev.splitInfo.endX1;
    SYS_ENV_SetU32(name,data);
    //
    sprintf(name, "Split_startX2");
    data =  gFpgaDev.splitInfo.startX2;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endX2");
    data =  gFpgaDev.splitInfo.endX2;
    SYS_ENV_SetU32(name,data);
    //
    sprintf(name, "Split_startX3");
    data =  gFpgaDev.splitInfo.startX3;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endX3");
    data =  gFpgaDev.splitInfo.endX3;
    SYS_ENV_SetU32(name,data);
    //
    sprintf(name, "Split_startX4");
    data =  gFpgaDev.splitInfo.startX4;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endX4");
    data =  gFpgaDev.splitInfo.endX4;
    SYS_ENV_SetU32(name,data);
    //
    sprintf(name, "Split_startX5");
    data =  gFpgaDev.splitInfo.startX5;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endX5");
    data =  gFpgaDev.splitInfo.endX5;
    SYS_ENV_SetU32(name,data);
    //
    sprintf(name, "Split_startX6");
    data =  gFpgaDev.splitInfo.startX6;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endX6");
    data =  gFpgaDev.splitInfo.endX6;
    SYS_ENV_SetU32(name,data);
    //
    sprintf(name, "Split_startX7");
    data =  gFpgaDev.splitInfo.startX7;
    SYS_ENV_SetU32(name,data);
    sprintf(name, "Split_endX7");
    data =  gFpgaDev.splitInfo.endX7;
    SYS_ENV_SetU32(name,data);
    //***********************************************
    sprintf(name, "Output_ena");
    data = gFpgaDev.outputEna ;
    SYS_ENV_SetU32(name,data);

    sprintf(name, "Output_fRate");
    data =  gFpgaDev.resolution.frameRate;
    SYS_ENV_SetU32(name,data);

    sprintf(name, "Output_totalPix");
    data =  gFpgaDev.resolution.totalPix;
    SYS_ENV_SetU32(name,data);

    sprintf(name, "Output_totalLine");
    data =  gFpgaDev.resolution.totalLine;
    SYS_ENV_SetU32(name,data);

    sprintf(name, "Output_hsyncWidth");
    data =  gFpgaDev.resolution.hsyncWidth;
    SYS_ENV_SetU32(name,data);

    sprintf(name, "Output_vsyncWidth");
    data =  gFpgaDev.resolution.vsyncWidth;
    SYS_ENV_SetU32(name,data);

    sprintf(name, "Output_actHstart");
    data =  gFpgaDev.resolution.actHstart;
    SYS_ENV_SetU32(name,data);


    sprintf(name, "Output_actHsize");
    data =  gFpgaDev.resolution.actHsize;
    SYS_ENV_SetU32(name,data);

    sprintf(name, "Output_actVstart");
    data =  gFpgaDev.resolution.actVstart;
    SYS_ENV_SetU32(name,data);


    sprintf(name, "Output_actVsize");
    data =  gFpgaDev.resolution.actVsize;
    SYS_ENV_SetU32(name,data);
    SYS_ENV_Save();
    return WV_SOK;
}


/********************************************************
WV_S32  TSK_FPGA_ConfInit();
****************************************************/
WV_S32  TSK_FPGA_ConfInit()
{

    TSK_FPGA_RegisterConf();
    TSK_FPGA_GetConf();


    return WV_SOK;
}


/********************************************************
WV_S32  TSK_FPGA_Init();
****************************************************/
WV_S32  TSK_FPGA_Init()
{
    gFpgaDev.pMapBuf = (WV_U8 *)malloc( TSK_FPGA_MAP_COL * TSK_FPGA_MAP_ROW * 4  +  2048);
    gFpgaDev.pTestBuf = (WV_U8 *)malloc( TSK_FPGA_MAP_COL * TSK_FPGA_MAP_ROW * 4  +  2048);
    gFpgaDev.pOutBuf = (WV_U8 *)malloc( TSK_FPGA_OUT_COL * TSK_FPGA_OUT_ROW * 4  +  2048);
    gFpgaDev.pSplitBuf = (WV_U8 *)malloc( TSK_FPGA_SPLIT_COL * TSK_FPGA_SPLIT_ROW * 4  +  2048);
    gFpgaDev.pColorBuf = (WV_U8 *)malloc( TSK_FPGA_COLOR_COL * TSK_FPGA_COLOR_ROW * 4  +  2048);

    loopFlag = 1;
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_DeInit();
****************************************************/
WV_S32  TSK_FPGA_DeInit()
{
    free(gFpgaDev.pMapBuf);
    free(gFpgaDev.pTestBuf);
    free(gFpgaDev.pOutBuf);
    free(gFpgaDev.pSplitBuf);
    free(gFpgaDev.pColorBuf);

    return WV_SOK;
}  


/********************************************************
WV_S32  TSK_FPGA_GetMap();
****************************************************/
WV_S32  TSK_FPGA_GetMap(WV_U8 channel)
{ 
    FILE    *fp = NULL;
    WV_U8  *pMapBuf,pFileBuf[TSK_FPGA_MAP_COL*4+16];
    WV_S32 i,j;
    WV_S32 Readlen,ReadNum;
    sprintf(gFpgaDev.mapFile, "./map/mapfile%d.dat",channel);
    fp = fopen(gFpgaDev.mapFile, "rb");
    if ( fp == NULL )
    {
        WV_ERROR("GetMap() fopen error\r\n");
        return WV_EFAIL;
    }

    pMapBuf = gFpgaDev.pMapBuf;
    ReadNum = 0;

    for(i = 0;i < TSK_FPGA_MAP_ROW ;i++   )
    {
        Readlen = fread(pFileBuf, sizeof(WV_S8),TSK_FPGA_MAP_COL*4, fp);
        if(Readlen != TSK_FPGA_MAP_COL*4)
        {
            //WV_ERROR("GetMap() fread error redlen = %d\r\n",Readlen);
            // WV_ERROR("GetMap() read [%s] error line[%d] redlen [%d]\r\n",gFpgaDev.mapFile,i,Readlen);
            memset(pFileBuf,0xFF,TSK_FPGA_MAP_COL*4);
        }

        for(j=0;j<TSK_FPGA_MAP_COL;j++)
        {

            *(pMapBuf+2)   =  pFileBuf[j*4+2];
            *(pMapBuf+1)   =  ((pFileBuf[j*4] & 0x0f)<<4)  | (pFileBuf[j*4 +3] & 0x0f);
            *(pMapBuf)     =  (((pFileBuf[j*4+1] & 0x0f)<<4)) | ((pFileBuf[j*4] & 0xf0)>>4);

            pMapBuf  += 3;
        }

        ReadNum += Readlen;
    }

    // WV_printf("map read num = %d[%d]\r\n",ReadNum,TSK_FPGA_MAP_COL *  TSK_FPGA_MAP_ROW);


    fclose(fp);
    return WV_SOK;
}




/********************************************************
WV_S32  TSK_FPGA_TransMap(WV_U8 * pData,WV_U32 len);
****************************************************/
WV_S32 TSK_FPGA_TransMap(WV_U8 * pData,WV_U32 len)
{ 
    WV_U8 *pMapBuf,* pDataBuf;
    WV_S32 i,j;

    //if(len < TSK_FPGA_MAP_ROW * TSK_FPGA_MAP_COL * 4)
    //	{
    //		return WV_EFAIL;
    //	}

    //WV_printf("TransMap get data len = %d\r\n",len);
    pMapBuf = gFpgaDev.pMapBuf;
    for(i = 0;i < TSK_FPGA_MAP_ROW ;i++   )
    {
        pDataBuf  = pData +  TSK_FPGA_MAP_COL  *4 *i;

        for(j=0;j<TSK_FPGA_MAP_COL;j++)
        {
            if((TSK_FPGA_MAP_COL  *4 *i +j*4) >len)
            {
                *pMapBuf      =  0xff;
                *(pMapBuf+1)  =  0xff;
                *(pMapBuf +2) =  0xff;
            }
            else
            {
                *(pMapBuf+2)   =  pDataBuf[j*4+2];
                *(pMapBuf+1)   =  ((pDataBuf[j*4] & 0x0f)<<4)  | (pDataBuf[j*4 +3] & 0x0f);
                *(pMapBuf)     =  (((pDataBuf[j*4+1] & 0x0f)<<4)) | ((pDataBuf[j*4] & 0xf0)>>4);
            }
            pMapBuf  += 3;
        }
    }

    return WV_SOK;
}




/********************************************************
WV_S32  TSK_FPGA_SaveMap(WV_U8 channel);
****************************************************/
WV_S32  TSK_FPGA_SaveMap(WV_U8 channel )
{ 

    FILE    *fp = NULL;
    WV_U8  *pMapBuf,pFileBuf[TSK_FPGA_MAP_COL*4+16];;
    WV_S32 i,j;
    WV_S32 writeLen;
    sprintf(gFpgaDev.mapFile, "./map/mapfile%d.dat",channel);
    fp = fopen(gFpgaDev.mapFile, "wb");

    if ( fp == NULL )
    {
        WV_ERROR("SaveMap() fopen error\r\n");
        return WV_EFAIL;
    }

    pMapBuf = gFpgaDev.pMapBuf;

    for(i = 0;i < TSK_FPGA_MAP_ROW ;i++   )
    {

        for(j=0;j<TSK_FPGA_MAP_COL;j++)
        {

            pFileBuf[j*4]     =   ((*(pMapBuf) & 0x0f) << 4 | (*(pMapBuf +1) & 0xf0) >> 4);
            pFileBuf[j*4 + 1] =   ((*(pMapBuf) & 0xf0) >> 4);
            pFileBuf[j*4 + 2] =   *(pMapBuf+2);
            pFileBuf[j*4 + 3] =   *(pMapBuf+1) & 0x0f;

            pMapBuf  += 3;
        }

        writeLen = fwrite(pFileBuf, sizeof(WV_S8),TSK_FPGA_MAP_COL*4, fp);

        if(writeLen != TSK_FPGA_MAP_COL*4)
        {

            WV_ERROR("SaveMap() write [%s] error line[%d] writelen [%d]\r\n",gFpgaDev.mapFile,i,writeLen);

        }

        // WV_ERROR("SaveMap() write [%s] error line[%d] writelen [%d]\r\n",gFpgaDev.mapFile,i,writeLen);
    }

    
    fclose(fp);
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_GetTest();
****************************************************/
WV_S32  TSK_FPGA_GetTest()
{ 

    WV_U8  *pMapBuf ;
    WV_S32 i,j;
    pMapBuf = gFpgaDev.pTestBuf;
    memset(pMapBuf,0xFF,TSK_FPGA_MAP_COL * TSK_FPGA_MAP_ROW * 3);

    for(i = 0;i < TSK_FPGA_MAP_ROW ;i++ )
    {

        for(j=0;j<TSK_FPGA_MAP_COL;j++)
        {


            *(pMapBuf  +2)  =    (j & 0xff);
            *(pMapBuf+1)    =    ((j>>8) & 0xf)  | ((i<<4) & 0xf0);
            *(pMapBuf)      =    (i>>4) & 0xff;
            pMapBuf        +=    3;
        }
    }

    return WV_SOK;
}


/********************************************************
WV_S32  TSK_FPGA_GetOut(WV_U8 channel);
****************************************************/
WV_S32  TSK_FPGA_GetOut(WV_U8 channel)
{ 
    FILE    *fp = NULL;
    WV_U8  *pOutBuf,pFileBuf[TSK_FPGA_OUT_COL*4+16];
    WV_S32 i,j;
    WV_S32 Readlen,ReadNum;

    sprintf(gFpgaDev.outFile, "./outline/outfile%d.dat",channel);
    fp = fopen(gFpgaDev.outFile, "rb");
    if ( fp == NULL )
    {
        WV_ERROR("GetOut() fopen error\r\n");
        return WV_EFAIL;
    }

    pOutBuf = gFpgaDev.pOutBuf;
    ReadNum = 0;

    for(i = 0;i < TSK_FPGA_OUT_ROW ;i++   )
    {
        Readlen = fread(pFileBuf, sizeof(WV_U8),TSK_FPGA_OUT_COL*3, fp);
        if(Readlen != TSK_FPGA_OUT_COL*3)
        {
            //WV_ERROR("GetOut() fread error redlen = %d\r\n",Readlen);
            memset(pFileBuf,0xFF,TSK_FPGA_OUT_COL*3);
        }
        //print filebuf

        for(j=0;j<TSK_FPGA_OUT_COL;j++)
        {

            *(pOutBuf +2) = pFileBuf[j*3]; // *pOutBuf = (((pFileBuf[j*4+1] & 0x0f)<<4)) | ((pFileBuf[j*4] & 0xf0)>>4);
            *(pOutBuf+1)  = pFileBuf[j*3 + 1];// *(pOutBuf+1) = ((pFileBuf[j*4] & 0x0f)<<4) | (pFileBuf[j*4 +3] & 0x0f);
            *pOutBuf      = pFileBuf[j*3 + 2]; // *(pOutBuf +2) = pFileBuf[j*4+2];

            pOutBuf  += 3;
        }

        ReadNum += Readlen;
    }

    //WV_printf("out read num = %d[%d]\r\n",ReadNum,TSK_FPGA_OUT_COL * TSK_FPGA_OUT_ROW*3);

    fclose(fp);
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_TransOut(WV_U8 * pData,WV_U32 len);
****************************************************/
WV_S32  TSK_FPGA_TransOut(WV_U8 * pData,WV_U32 len)
{

    WV_U8  *pOutBuf,*pFileBuf;
    WV_S32 i,j;


    if(len < TSK_FPGA_OUT_ROW * TSK_FPGA_OUT_COL * 3)
    {
        return WV_EFAIL;
    }

    pOutBuf = gFpgaDev.pOutBuf;

    for(i = 0;i < TSK_FPGA_OUT_ROW ;i++)
    {

        pFileBuf = pData + TSK_FPGA_OUT_COL * 3 * i;
        for(j=0;j<TSK_FPGA_OUT_COL;j++)
        {

            *(pOutBuf +2)  = pFileBuf[j*3];
            *(pOutBuf+1)   = pFileBuf[j*3 + 1];
            *pOutBuf      = pFileBuf[j*3 + 2];
            pOutBuf  += 3;
        }
    }
    return WV_SOK;

    //ReadNum += Readlen;

    //WV_printf("out read num = %d[%d]\r\n",ReadNum,TSK_FPGA_OUT_COL * TSK_FPGA_OUT_ROW*3);
} 


/********************************************************
WV_S32  TSK_FPGA_SaveOut(WV_U8 channel);
****************************************************/
WV_S32  TSK_FPGA_SaveOut(WV_U8 channel )
{ 
    FILE    *fp = NULL;
    WV_U8  *pOutBuf,pFileBuf[TSK_FPGA_OUT_COL*4+16];;
    WV_S32 i,j;
    WV_S32 writeLen;
    sprintf(gFpgaDev.outFile, "./outline/outfile%d.dat",channel);
    fp = fopen(gFpgaDev.outFile, "wb");
    if ( fp == NULL )
    {
        WV_ERROR("SaveOut() fopen error\r\n");
        return WV_EFAIL;
    }

    pOutBuf = gFpgaDev.pOutBuf;

    for(i = 0;i < TSK_FPGA_OUT_ROW ;i++   )
    {

        for(j=0;j<TSK_FPGA_OUT_COL;j++)
        {

            pFileBuf[j*3 + 2]= *pOutBuf;
            pFileBuf[j*3 + 1] =*(pOutBuf+1);
            pFileBuf[j*3]  = *(pOutBuf +2);
            pOutBuf  += 3;
        }

        writeLen = fwrite(pFileBuf, sizeof(WV_S8),TSK_FPGA_OUT_COL*3, fp);
        if(writeLen != TSK_FPGA_OUT_COL*3)
        {
            WV_ERROR("SaveOut() write [%s] error line[%d] writelen [%d]\r\n",gFpgaDev.mapFile,i,writeLen);

        }
    }

    
    fclose(fp);
    return WV_SOK;
}


/********************************************************
WV_S32  TSK_FPGA_GetLight(WV_U8 channel);
****************************************************/
WV_S32  TSK_FPGA_GetLight(WV_U8 channel )
{ 
    FILE    *fp = NULL;
    WV_U8  *pSplitBuf,pFileBuf[TSK_FPGA_SPLIT_COL*4+16] ;
    WV_S32 i,j;
    WV_S32 Readlen,ReadNum;
    sprintf(gFpgaDev.splitFile, "./split/splitfile%d.dat",channel);
    fp = fopen(gFpgaDev.splitFile, "rb");
    if ( fp == NULL )
    {
        WV_ERROR("GetLight(%d) fopen error\r\n",channel);
        return WV_EFAIL;
    }

    pSplitBuf = gFpgaDev.pSplitBuf;
    ReadNum = 0;

    for(i = 0;i < TSK_FPGA_SPLIT_ROW ;i++   )
    {
        Readlen = fread(pFileBuf, sizeof(WV_U8),TSK_FPGA_SPLIT_COL*3, fp);
        if(Readlen != TSK_FPGA_SPLIT_COL*3)
        {
            WV_ERROR("GetLight() fread error redlen = %d\r\n",Readlen);
            memset(pFileBuf,0xFF,TSK_FPGA_SPLIT_COL*3);
        }

        for(j=0;j<TSK_FPGA_SPLIT_COL;j++)
        {
            *pSplitBuf        = pFileBuf[j*3 + 2];
            *(pSplitBuf+1)    = pFileBuf[j*3 + 1];
            *(pSplitBuf +2)   = pFileBuf[j*3];
            pSplitBuf  +=3;
        }
        ReadNum += Readlen;
    }

    //WV_printf("split read num = %d[%d]\r\n",ReadNum,TSK_FPGA_SPLIT_COL * TSK_FPGA_SPLIT_ROW*3);


    
    fclose(fp);
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_TransLight(WV_U8 * pData,WV_U32 len);
****************************************************/
WV_S32  TSK_FPGA_TransLight(WV_U8 * pData,WV_U32 len)
{

    WV_U8  *pSplitBuf,*pFileBuf;
    WV_S32 i,j;

    if(len < TSK_FPGA_SPLIT_ROW * TSK_FPGA_SPLIT_COL * 3)
    {
        return WV_EFAIL;
    }
    pSplitBuf = gFpgaDev.pSplitBuf;
    for(i = 0;i < TSK_FPGA_SPLIT_ROW ;i++ )
    {

        pFileBuf = pData + TSK_FPGA_SPLIT_COL *3 * i;
        for(j=0;j<TSK_FPGA_SPLIT_COL;j++)
        {

            *(pSplitBuf +2)  = pFileBuf[j*3];
            *(pSplitBuf+1)  = pFileBuf[j*3 + 1];
            *pSplitBuf = pFileBuf[j*3 + 2];
            pSplitBuf  += 3;
        }
    }


    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_SaveLight(WV_U8 channel);
****************************************************/
WV_S32  TSK_FPGA_SaveLight(WV_U8 channel )
{ 
    FILE    *fp = NULL;
    WV_U8  *pSplitBuf,pFileBuf[TSK_FPGA_SPLIT_COL*4+16] ;
    WV_S32 i,j;
    WV_S32 writeLen;
    sprintf(gFpgaDev.splitFile, "./split/splitfile%d.dat",channel);
    fp = fopen(gFpgaDev.splitFile, "wb");
    if ( fp == NULL )
    {
        WV_ERROR("SaveOut() fopen error\r\n");
        return WV_EFAIL;
    }

    pSplitBuf = gFpgaDev.pSplitBuf;

    for(i = 0;i < TSK_FPGA_SPLIT_ROW ;i++   )
    {

        for(j=0;j<TSK_FPGA_SPLIT_COL;j++)
        {

            pFileBuf[j*3 + 2] = *pSplitBuf;
            pFileBuf[j*3 + 1] = *(pSplitBuf+1);
            pFileBuf[j*3]     = *(pSplitBuf +2);
            pSplitBuf  += 3;
        }

        writeLen = fwrite(pFileBuf, sizeof(WV_S8),TSK_FPGA_SPLIT_COL*3, fp);
        if(writeLen != TSK_FPGA_SPLIT_COL*3)
        {
            WV_ERROR("SaveOut() write [%s] error line[%d] writelen [%d]\r\n",gFpgaDev.splitFile,i,writeLen);

        }
    }

    
    fclose(fp);
    return WV_SOK;
}


/********************************************************
WV_S32  TSK_FPGA_GetColor(WV_U8 channel);
****************************************************/
WV_S32  TSK_FPGA_GetColor(WV_U8 channel )
{ 
    FILE    *fp = NULL;
    WV_U8  *pColorBuf,pFileBuf[TSK_FPGA_COLOR_COL*4+16] ;
    WV_S32 i,j;
    WV_S32 Readlen,ReadNum;
    sprintf(gFpgaDev.colorFile, "./color/colorfile%d.dat",channel);
    fp = fopen(gFpgaDev.colorFile, "rb");
    if ( fp == NULL )
    {
        WV_ERROR("GetColor() fopen error\r\n");
        return WV_EFAIL;
    }

    pColorBuf = gFpgaDev.pColorBuf;
    ReadNum = 0;

    for(i = 0;i < TSK_FPGA_COLOR_ROW ;i++   )
    {
        Readlen = fread(pFileBuf, sizeof(WV_U8),TSK_FPGA_COLOR_COL*3, fp);
        if(Readlen != TSK_FPGA_COLOR_COL*3)
        {
            WV_ERROR("GetColor() fread error redlen = %d\r\n",Readlen);
            memset(pFileBuf,0xFF,TSK_FPGA_COLOR_COL*3);
        }

        for(j=0;j<TSK_FPGA_COLOR_COL;j++)
        {


            *pColorBuf        = pFileBuf[j*3 + 2];
            *(pColorBuf+1)    = pFileBuf[j*3 + 1];
            *(pColorBuf +2)   = pFileBuf[j*3];
            pColorBuf  +=3;
        }
        ReadNum += Readlen;
    }

    //WV_printf("color read num = %d[%d]\r\n",ReadNum,TSK_FPGA_COLOR_COL * TSK_FPGA_COLOR_ROW*3);


    
    fclose(fp);
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_TransColor(WV_U8 * pData,WV_U32 len);
****************************************************/
WV_S32  TSK_FPGA_TransColor(WV_U8 * pData,WV_U32 len)
{

    WV_U8  *pColorBuf,*pFileBuf;
    WV_S32 i,j;

    if(len < TSK_FPGA_COLOR_ROW * TSK_FPGA_COLOR_COL * 3)
    {
        return WV_EFAIL;
    }
    pColorBuf = gFpgaDev.pColorBuf;
    for(i = 0;i < TSK_FPGA_COLOR_ROW ;i++ )
    {

        pFileBuf = pData + TSK_FPGA_COLOR_COL *3 * i;
        for(j=0;j<TSK_FPGA_COLOR_COL;j++)
        {

            *(pColorBuf +2)  = pFileBuf[j*3];
            *(pColorBuf+1)  = pFileBuf[j*3 + 1];
            *pColorBuf = pFileBuf[j*3 + 2];
            pColorBuf  += 3;
        }
    }


    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_SaveColor(WV_U8 channel);
****************************************************/
WV_S32  TSK_FPGA_SaveColor(WV_U8 channel )
{ 
    FILE    *fp = NULL;
    WV_S8  *pColorBuf,pFileBuf[TSK_FPGA_COLOR_COL*4+16] ;
    WV_S32 i,j;
    WV_S32 writeLen;
    sprintf((WV_S8 *)gFpgaDev.pColorBuf, "./Color/Colorfile%d.dat",channel);
    fp = fopen((WV_S8 *)gFpgaDev.pColorBuf, "wb");
    if ( fp == NULL )
    {
        WV_ERROR("SaveOut() fopen error\r\n");
        return WV_EFAIL;
    }

    pColorBuf =(WV_S8 *) gFpgaDev.pColorBuf;

    for(i = 0;i < TSK_FPGA_COLOR_ROW ;i++   )
    {

        for(j=0;j<TSK_FPGA_COLOR_COL;j++)
        {

            pFileBuf[j*3 + 2] = *pColorBuf;
            pFileBuf[j*3 + 1] = *(pColorBuf+1);
            pFileBuf[j*3]     = *(pColorBuf +2);
            pColorBuf  += 3;
        }

        writeLen = fwrite(pFileBuf, sizeof(WV_S8),TSK_FPGA_COLOR_COL*3, fp);
        if(writeLen != TSK_FPGA_COLOR_COL*3)
        {
            WV_ERROR("SaveOut() write [%s] error line[%d] writelen [%d]\r\n",gFpgaDev.colorFile,i,writeLen);

        }
    }

    
    fclose(fp);
    return WV_SOK;
}



/********************************************************
WV_S32  TSK_FPGA_SetCmd( WV_S8 * pCmdData ,WV_U8 cmdType, WV_U8 cmdChnl,WV_U32 cmdRow ,WV_U32 cmdCol);
********************************************************/

WV_S32  TSK_FPGA_SetCmd( WV_U8 * pCmdData ,WV_U8 cmdType, WV_U8 cmdChnl,WV_U32 cmdCol ,WV_U32 cmdRow)
{

    WV_S32 i,ret = 0;
    HI_U32    starMs,curMs;
    //WV_printf("\r\nTSK_FPGA_SetCmd %d   chnl [%d]  COL[%d] ROW[%d]\r\n" ,cmdType,cmdChnl,cmdCol,cmdRow);
    HI_SYS_GetTimeStampMs( &starMs);

    WV_RET_ADD( HIS_FB_SetFpga( pCmdData,cmdType,cmdChnl,cmdCol ,cmdRow),ret);
    HI_SYS_GetTimeStampMs( &curMs);
    HI_SYS_GetTimeStampMs  ( &starMs);
    //getchar();

    for(i=0;i<100;i++)
    {
        usleep(30000);
        HI_SYS_GetTimeStampMs  ( &curMs);
        if(curMs - starMs > 70 )
        {
            break;
        }
    }


    return ret;
}


/********************************************************
WV_S32  TSK_FPGA_SetMap (WV_U8 mapChnl);
********************************************************/
WV_S32  TSK_FPGA_SetMap (WV_U8 mapChnl)
{


    WV_CHECK_RET (TSK_FPGA_SetCmd( gFpgaDev.pMapBuf ,TSK_FPGA_CMD_MAP,mapChnl,TSK_FPGA_MAP_COL ,TSK_FPGA_MAP_ROW));

    return WV_SOK;
}
/********************************************************
WV_S32  TSK_FPGA_SetOut (WV_U8 outChnl);
********************************************************/
WV_S32  TSK_FPGA_SetOut (WV_U8 outChnl)
{

    //fresh frame buf
    WV_CHECK_RET (TSK_FPGA_SetCmd( gFpgaDev.pOutBuf ,TSK_FPGA_CMD_OUT,outChnl,TSK_FPGA_OUT_COL ,TSK_FPGA_OUT_ROW));
    
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_SetLight (WV_U8 lightChnl);
********************************************************/
WV_S32  TSK_FPGA_SetLight (WV_U8 lightChnl)
{

    //set light horizontal
    WV_S32 chlNum;
    chlNum=FPGA_CONF_GetOutChl_Num();
    if(chlNum == 4){
        if(lightChnl < 4){

            WV_CHECK_RET (TSK_FPGA_SetCmd( gFpgaDev.pSplitBuf ,TSK_FPGA_CMD_SPLIT,lightChnl+1,TSK_FPGA_SPLIT_COL ,TSK_FPGA_SPLIT_ROW));

        }
        if((lightChnl >= 8) && (lightChnl < 16)){
            lightChnl -= 8;
            //printf("light chl [%d]\n",lightChnl);
            WV_CHECK_RET (TSK_FPGA_SetCmd( gFpgaDev.pSplitBuf ,TSK_FPGA_CMD_SPLIT_V,lightChnl+1,TSK_FPGA_SPLIT_COL ,TSK_FPGA_SPLIT_ROW));
        }
    }else {
        if(lightChnl < 8){
            WV_CHECK_RET (TSK_FPGA_SetCmd( gFpgaDev.pSplitBuf ,TSK_FPGA_CMD_SPLIT,lightChnl,TSK_FPGA_SPLIT_COL ,TSK_FPGA_SPLIT_ROW));
        }
        //set light vertical
        if((lightChnl >= 8) && (lightChnl < 16)){
            lightChnl -= 8;
            //printf("light chl [%d]\n",lightChnl);
            WV_CHECK_RET (TSK_FPGA_SetCmd( gFpgaDev.pSplitBuf ,TSK_FPGA_CMD_SPLIT_V,lightChnl,TSK_FPGA_SPLIT_COL ,TSK_FPGA_SPLIT_ROW));
        }

    }


    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_SetColor (WV_U8 splitChnl);
********************************************************/
WV_S32  TSK_FPGA_SetColor (WV_U8 colorChnl)
{

    //fresh frame buf

    WV_CHECK_RET (TSK_FPGA_SetCmd( gFpgaDev.pColorBuf ,TSK_FPGA_CMD_COLOR,colorChnl+1,TSK_FPGA_COLOR_COL ,TSK_FPGA_COLOR_ROW));

    return WV_SOK;
}


/********************************************************
WV_S32  TSK_FPGA_SplitConf (FPGA_SPLIT_INFO_S splitInfo );
********************************************************/
WV_S32  TSK_FPGA_SplitConf (FPGA_SPLIT_INFO_S splitInfo )
{
    gFpgaDev.splitInfo = splitInfo;
    FPGA_CONF_SetSplit(&gFpgaDev.splitInfo);
    TSK_FPGA_SaveConf();
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_SetPointSplit (FPGA_SPLIT_INFO_S splitInfo );
********************************************************/
WV_S32  TSK_FPGA_SetPointSplit (FPGA_SPLIT_INFO_S splitInfo )
{
    //gFpgaDev.splitInfo = splitInfo;
    FPGA_CONF_SetSplit(&splitInfo);
    //TSK_FPGA_SaveConf();
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_SavePointSplit (FPGA_SPLIT_INFO_S splitInfo );
********************************************************/
WV_S32  TSK_FPGA_SavePointSplit (FPGA_SPLIT_INFO_S splitInfo )
{
    gFpgaDev.splitInfo = splitInfo;
    TSK_FPGA_SaveConf();

    FPGA_SPLIT_INFO_S OldPoint;
    FPGA_CONF_GetSplitDefault( &OldPoint);
    FPGA_CONF_SetSplit(&OldPoint);
    return WV_SOK;
}
/********************************************************
WV_S32  TSK_FPGA_GetSplit(FPGA_SPLIT_INFO_S *splitInfo );
********************************************************/
WV_S32  TSK_FPGA_GetSplit(FPGA_SPLIT_INFO_S* splitInfo )
{

    splitInfo->startY0 = gFpgaDev.splitInfo.startY0;
    splitInfo->endY0 = gFpgaDev.splitInfo.endY0;
    splitInfo->startY1 = gFpgaDev.splitInfo.startY1;
    splitInfo->endY1 = gFpgaDev.splitInfo.endY1;
    splitInfo->startY2 = gFpgaDev.splitInfo.startY2;
    splitInfo->endY2 = gFpgaDev.splitInfo.endY2;
    splitInfo->startY3 = gFpgaDev.splitInfo.startY3;
    splitInfo->endY3 = gFpgaDev.splitInfo.endY3;
    splitInfo->startY4 = gFpgaDev.splitInfo.startY4;
    splitInfo->endY4 = gFpgaDev.splitInfo.endY4;
    splitInfo->startY5 = gFpgaDev.splitInfo.startY5;
    splitInfo->endY5 = gFpgaDev.splitInfo.endY5;
    splitInfo->startY6 = gFpgaDev.splitInfo.startY6;
    splitInfo->endY6 = gFpgaDev.splitInfo.endY6;
    splitInfo->startY7 = gFpgaDev.splitInfo.startY7;
    splitInfo->endY7 = gFpgaDev.splitInfo.endY7;

    splitInfo->startX0 = gFpgaDev.splitInfo.startX0;
    splitInfo->endX0 = gFpgaDev.splitInfo.endX0;
    splitInfo->startX1 = gFpgaDev.splitInfo.startX1;
    splitInfo->endX1 = gFpgaDev.splitInfo.endX1;
    splitInfo->startX2 = gFpgaDev.splitInfo.startX2;
    splitInfo->endX2 = gFpgaDev.splitInfo.endX2;
    splitInfo->startX3 = gFpgaDev.splitInfo.startX3;
    splitInfo->endX3 = gFpgaDev.splitInfo.endX3;
    splitInfo->startX4 = gFpgaDev.splitInfo.startX4;
    splitInfo->endX4 = gFpgaDev.splitInfo.endX4;
    splitInfo->startX5 = gFpgaDev.splitInfo.startX5;
    splitInfo->endX5 = gFpgaDev.splitInfo.endX5;
    splitInfo->startX6 = gFpgaDev.splitInfo.startX6;
    splitInfo->endX6 = gFpgaDev.splitInfo.endX6;
    splitInfo->startX7 = gFpgaDev.splitInfo.startX7;
    splitInfo->endX7 = gFpgaDev.splitInfo.endX7;


    return WV_SOK;
}


/********************************************************
WV_S32  TSK_FPGA_PointoutConf (WV_U16  ena);
********************************************************/
WV_S32   TSK_FPGA_PointoutConf (WV_U16  ena)
{
    FPGA_CONF_SetOutput(ena);
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_GetOutConf ();
********************************************************/
WV_U16   TSK_FPGA_GetOutConf ()
{
    return gFpgaDev.outputEna;
}

/********************************************************
WV_S32  TSK_FPGA_outConf (WV_U16  ena);
********************************************************/
WV_S32   TSK_FPGA_outConf (WV_U16  ena)
{
    gFpgaDev.outputEna = ena;
    FPGA_CONF_SetOutput(gFpgaDev.outputEna);
    TSK_FPGA_SaveConf();
    return WV_SOK;
}
/********************************************************
WV_S32  TSK_FPGA_lightConf (WV_U32  ena);
********************************************************/
WV_S32   TSK_FPGA_lightConf(WV_U16  ena)
{

    WV_U16 date;
    if(ena==0){
        date=0x03;
        HIS_SPI_FpgaWd(0x26,date);
        TSK_SCENE_SetLightEna(ena);
    }else if(ena==1){
        date=0;
        HIS_SPI_FpgaWd(0x26,date);
        TSK_SCENE_SetLightEna(ena);
    }

    return WV_SOK;
}

/********************************************************
WV_S32   TSK_FPGA_ResolutionConf (FPGA_RESOLUTION_INFO_S   resolution);
********************************************************/
WV_S32   TSK_FPGA_ResolutionConf (FPGA_RESOLUTION_INFO_S   resolution)
{
    gFpgaDev.resolution = resolution;
    FPGA_CONF_SetResolution(&gFpgaDev.resolution);
    TSK_FPGA_SaveConf();
    return WV_SOK;
}
/****************************************************************************

WV_S32 TSK_FPGA_GetSplitCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_GetSplitCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    prfBuff += sprintf(prfBuff,"split Info :\r\n");

    prfBuff += sprintf(prfBuff,"split Y1 : %d  --  %d \r\n",gFpgaDev.splitInfo.startY0,gFpgaDev.splitInfo.endY0 );
    prfBuff += sprintf(prfBuff,"split X1 : %d  --  %d \r\n",gFpgaDev.splitInfo.startX0, gFpgaDev.splitInfo.endX0);
    prfBuff += sprintf(prfBuff,"split Y2 : %d  --  %d \r\n",gFpgaDev.splitInfo.startY1,gFpgaDev.splitInfo.endY1 );
    prfBuff += sprintf(prfBuff,"split X2 : %d  --  %d \r\n",gFpgaDev.splitInfo.startX1, gFpgaDev.splitInfo.endX1);
    prfBuff += sprintf(prfBuff,"split Y3 : %d  --  %d \r\n",gFpgaDev.splitInfo.startY2,gFpgaDev.splitInfo.endY2 );
    prfBuff += sprintf(prfBuff,"split X3 : %d  --  %d \r\n",gFpgaDev.splitInfo.startX2, gFpgaDev.splitInfo.endX2);
    prfBuff += sprintf(prfBuff,"split Y4 : %d  --  %d \r\n",gFpgaDev.splitInfo.startY3,gFpgaDev.splitInfo.endY3 );
    prfBuff += sprintf(prfBuff,"split X4 : %d  --  %d \r\n",gFpgaDev.splitInfo.startX3, gFpgaDev.splitInfo.endX3);
    prfBuff += sprintf(prfBuff,"split Y5 : %d  --  %d \r\n",gFpgaDev.splitInfo.startY4,gFpgaDev.splitInfo.endY4 );
    prfBuff += sprintf(prfBuff,"split X5 : %d  --  %d \r\n",gFpgaDev.splitInfo.startX4, gFpgaDev.splitInfo.endX4);
    prfBuff += sprintf(prfBuff,"split Y6 : %d  --  %d \r\n",gFpgaDev.splitInfo.startY5,gFpgaDev.splitInfo.endY5 );
    prfBuff += sprintf(prfBuff,"split X6 : %d  --  %d \r\n",gFpgaDev.splitInfo.startX5, gFpgaDev.splitInfo.endX5);
    prfBuff += sprintf(prfBuff,"split Y7 : %d  --  %d \r\n",gFpgaDev.splitInfo.startY6,gFpgaDev.splitInfo.endY6 );
    prfBuff += sprintf(prfBuff,"split X7 : %d  --  %d \r\n",gFpgaDev.splitInfo.startX6, gFpgaDev.splitInfo.endX6);
    prfBuff += sprintf(prfBuff,"split Y8 : %d  --  %d \r\n",gFpgaDev.splitInfo.startY7,gFpgaDev.splitInfo.endY7);
    prfBuff += sprintf(prfBuff,"split X8 : %d  --  %d \r\n",gFpgaDev.splitInfo.startX7, gFpgaDev.splitInfo.endX7);

    return WV_SOK;
}

/****************************************************************************

WV_S32 TSK_FPGAS_CMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_SetSplitCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    if(argc<24)
    {
        prfBuff += sprintf(prfBuff,"input erro ! argc= %d\r\n",argc);
        prfBuff += sprintf(prfBuff,"set split  startY1  endY1 startX1 endX1 startY2  endY2 startX2 endX2 startY3  endY3 startX3 endX3 startY4  endY4 startX4 endX4 startY5  endY5 startX5 endX5 startY6  endY6 startX6 endX6\r\n");
        return 0;
    }

    FPGA_SPLIT_INFO_S  info;
    WV_U32 data, *pData;
    pData = &data;
    //set 0
    WV_ASSERT_RET(WV_STR_S2v(argv[0],pData));
    info.startY0 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[1],pData));
    info.endY0 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[2],pData));
    info.startX0 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[3],pData));
    info.endX0 = data;
    //set 1
    WV_ASSERT_RET(WV_STR_S2v(argv[4],pData));
    info.startY1 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[5],pData));
    info.endY1 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[6],pData));
    info.startX1 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[7],pData));
    info.endX1 = data;
    //set 2
    WV_ASSERT_RET(WV_STR_S2v(argv[8],pData));
    info.startY2 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[9],pData));
    info.endY2 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[10],pData));
    info.startX2 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[11],pData));
    info.endX2 = data;
    //set 3
    WV_ASSERT_RET(WV_STR_S2v(argv[12],pData));
    info.startY3 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[13],pData));
    info.endY3 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[14],pData));
    info.startX3 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[15],pData));
    info.endX3 = data;
    //set 4
    WV_ASSERT_RET(WV_STR_S2v(argv[16],pData));
    info.startY4 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[17],pData));
    info.endY4 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[18],pData));
    info.startX4 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[19],pData));
    info.endX4 = data;
    //set 5
    WV_ASSERT_RET(WV_STR_S2v(argv[20],pData));
    info.startY5 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[21],pData));
    info.endY5 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[22],pData));
    info.startX5 = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[23],pData));
    info.endX5 = data;

    /*
        WV_ASSERT_RET(WV_STR_S2v(argv[2],pData));
         info.startX0 = data;
        WV_ASSERT_RET(WV_STR_S2v(argv[3],pData));
         info.endX0 = data;
        WV_ASSERT_RET(WV_STR_S2v(argv[4],pData));
         info.startX1 = data;

        WV_ASSERT_RET(WV_STR_S2v(argv[5],pData));
         info.endX1 = data;
        WV_ASSERT_RET(WV_STR_S2v(argv[6],pData));
         info.startX2 = data;
        WV_ASSERT_RET(WV_STR_S2v(argv[7],pData));
         info.endX2 = data;
        WV_ASSERT_RET(WV_STR_S2v(argv[8],pData));
         info.startX3 = data;
        WV_ASSERT_RET(WV_STR_S2v(argv[9],pData));
         info.endX3 = data;
*/
    gFpgaDev.splitInfo = info;
    FPGA_CONF_SetSplit(&info);
    TSK_FPGA_SaveConf();

    return WV_SOK;
}



/****************************************************************************
WV_S32 TSK_FPGA_GetResolutionCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_GetResolutionCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    prfBuff += sprintf(prfBuff,"out Resolution Info :\r\n");
    prfBuff += sprintf(prfBuff,"frame Rate : %5d\r\n",gFpgaDev.resolution.frameRate);
    prfBuff += sprintf(prfBuff,"Hor :%5d:%5d:%5d :%5d\r\n",gFpgaDev.resolution.totalPix, gFpgaDev.resolution.actHstart,\
                       gFpgaDev.resolution.actHsize,gFpgaDev.resolution.hsyncWidth);
    prfBuff += sprintf(prfBuff,"Ver :%5d :%5d :%5d:%5d\r\n",gFpgaDev.resolution.totalLine, gFpgaDev.resolution.actVstart,\
                       gFpgaDev.resolution.actVsize,gFpgaDev.resolution.vsyncWidth);
    return WV_SOK;
}



/****************************************************************************
WV_S32 TSK_FPGA_SetResolutionCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_setResolutionCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    if(argc<9)
    {
        prfBuff += sprintf(prfBuff,"set resolution  frameRate totalPix actHstart actHsize totalLine  actVstart  actVsize  hsyncWidth  vsyncWidth\r\n");
        return 0;
    }
    FPGA_RESOLUTION_INFO_S  info;
    WV_U32 data, *pData;
    pData = &data;
    WV_ASSERT_RET(WV_STR_S2v(argv[0],pData));
    info.frameRate  = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[1],pData));
    info.totalPix  = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[2],pData));
    info.actHstart  = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[3],pData));
    info.actHsize  = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[4],pData));
    info.totalLine  = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[5],pData));
    info.actVstart  = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[6],pData));
    info.actVsize  = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[7],pData));
    info.hsyncWidth  = data;
    WV_ASSERT_RET(WV_STR_S2v(argv[8],pData));
    info.vsyncWidth  = data;


    gFpgaDev.resolution = info;
    FPGA_CONF_SetResolution(&info);
    TSK_FPGA_SaveConf();
    return WV_SOK;
}


/****************************************************************************
WV_S32 TSK_FPGA_GetResolutionCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_GetOutputCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    prfBuff += sprintf(prfBuff,"output Info :\r\n");
    prfBuff += sprintf(prfBuff,"frame Rate : 0x%08x\r\n",gFpgaDev.outputEna);
    return WV_SOK;
}


/****************************************************************************
WV_S32 TSK_FPGA_SetMapCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_SetMapCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    if(argc<1)
    {
        prfBuff += sprintf(prfBuff,"set map  < id> \r\n");
        return 0;
    }
    WV_U32  id;
    WV_ASSERT_RET(WV_STR_S2v(argv[0],&id));
    FPGA_CONF_SetMapStart();
    TSK_FPGA_GetMap(id);
    TSK_FPGA_SetMap(id);
    FPGA_CONF_SetMapEnd();
    HIS_FB_ClrFpga();

    return WV_SOK;
}


/****************************************************************************
WV_S32 TSK_FPGA_SetOutlineCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_SetOutlineCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    if(argc<1)
    {
        prfBuff += sprintf(prfBuff,"set Outline  < id> \r\n");
        return 0;
    }
    WV_U32  id;
    WV_ASSERT_RET(WV_STR_S2v(argv[0],&id));

    FPGA_CONF_SetOutlineStart();
    TSK_FPGA_GetOut(id);
    TSK_FPGA_SetOut(id);
    HIS_FB_ClrFpga();
    FPGA_CONF_SetOutlineEnd();
    return WV_SOK;
}

/****************************************************************************
WV_S32 TSK_FPGA_SetLoopStop()

****************************************************************************/
WV_S32 TSK_FPGA_SetLoopStop()
{
    loopFlag = 0;
    return WV_SOK;
}


#define TSK_FPGA_X 1919
#define TSK_FPGA_END_Y 1080
/****************************************************************************
WV_S32 TSK_FPGA_SetSplitLoop()

****************************************************************************/
WV_S32 TSK_FPGA_SetSplitLoop()
{
#if 1
    WV_S32  i;
    FPGA_SPLIT_INFO_S split;
    memcpy(&split,&gFpgaDev.splitInfo,sizeof(FPGA_SPLIT_INFO_S));
    loopFlag = 1;
    for(i=0;i<40;i++)
    {
        split.startX0 = split.startX0 +8;
        split.endX0 = split.endX0 + 8;
        split.startX1 = split.startX1 +8;
        split.endX1 = split.endX1 + 8;
        split.startX2 = split.startX2 +8;
        split.endX2 = split.endX2 + 8;

        if(split.endX0 > TSK_FPGA_X)
        {
            split.startX0 = 0;
            split.endX0 = 639;//359;
            if(split.startY0 == 0)
            {
                split.startY0 = TSK_FPGA_END_Y;
                split.endY0 = TSK_FPGA_END_Y + TSK_FPGA_END_Y -1;
            }else if(split.startY0 == TSK_FPGA_END_Y)
            {
                split.startY0 = 0;
                split.endY0 = TSK_FPGA_END_Y -1 ;
            }

        }

        if(split.endX1 > TSK_FPGA_X)
        {
            split.startX1 = 0;
            split.endX1 = 639;//359;
            if(split.startY1 == 0)
            {
                split.startY1 = TSK_FPGA_END_Y;
                split.endY1 = TSK_FPGA_END_Y + TSK_FPGA_END_Y -1;
            }else if(split.startY1 == TSK_FPGA_END_Y)
            {
                split.startY1 = 0;
                split.endY1 = TSK_FPGA_END_Y -1 ;
            }
        }

        if(split.endX2 > TSK_FPGA_X)
        {
            split.startX2 = 0;
            split.endX2 = 639;
            if(split.startY2 == 0)
            {
                split.startY2 = TSK_FPGA_END_Y;
                split.endY2 = TSK_FPGA_END_Y + TSK_FPGA_END_Y -1;
            }else if(split.startY2 == TSK_FPGA_END_Y)
            {
                split.startY2 = 0;
                split.endY2 = TSK_FPGA_END_Y -1 ;
            }

        }

        FPGA_CONF_SetSplit(&split);
        //printf("11x0[%d-%d] y0[%d-%d] x1[%d-%d]y1[%d-%d] x2[%d-%d] y2[%d-%d]\n",split.startX0,split.endX0,split.startY0,split.endY0,split.startX1,split.endX1,split.startY1,split.endY1,split.startX2,split.endX2,split.startY2,split.endY2);
        usleep(30000);
    }
#endif
    //TSK_FPGA_SaveConf();
    return WV_SOK;
}

/****************************************************************************
WV_S32 TSK_FPGA_GetTemp(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_GetTemp(WV_U16* pCpu,WV_U16 *pFpgaIn,WV_U16 *pFpgaOut)
{
    WV_S32 ret = 0;
    WV_U16 t;
    WV_S32 chl;
    WV_S16 tmpCpu;
    chl = FPGA_CONF_GetOutChl_Num();
    if(chl==4){
        HIS_SPI_FpgaRd(0x30,&t);
        *pFpgaIn = (t*503.975)/4096-273.15;
        //pFpgaIn=&tmp;
        //WV_printf("fpga in tmp [%d]   ",*pFpgaIn);

        HIS_SPI_FpgaRd2(0x34,&t);
        *pFpgaOut = (t*503.975)/4096-273.15;

        //WV_printf("fpga out tmp [%d]   ",*pFpgaOut);

        HIS_TEMP_ReadTemp(&tmpCpu);
        *pCpu = tmpCpu;
        //WV_printf("tmpCpu[%d]\n",*pCpu);
    }else if (chl==2){
        HIS_SPI_FpgaRd(0x30,&t);
        *pFpgaIn = (t*503.975)/4096-273.15;
        //pFpgaIn=&tmp;
        //WV_printf("fpga in tmp [%d]   ",*pFpgaIn);
        *pFpgaOut = 0;

        HIS_TEMP_ReadTemp(&tmpCpu);
        *pCpu = tmpCpu;
        //WV_printf("tmpCpu[%d]\n",*pCpu);


    }else if(chl==6){

        HIS_SPI_FpgaRd(0xB4,&t);
        *pFpgaIn = (t*503.975)/4096-273.15;
        //pFpgaIn=&tmp;
        //WV_printf("fpga in tmp [%d]   ",*pFpgaIn);

        HIS_SPI_FpgaRd2(0x34,&t);
        *pFpgaOut = (t*503.975)/4096-273.15;

        //WV_printf("fpga out tmp [%d]   ",*pFpgaOut);

        HIS_TEMP_ReadTemp(&tmpCpu);
        *pCpu = tmpCpu;
        //WV_printf("tmpCpu[%d]\n",*pCpu);
    }
    return ret;


}

/****************************************************************************
WV_S32 TSK_FPGA_GetTempCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_GetTempCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    if(argc<1)
    {
        prfBuff += sprintf(prfBuff,"get temperture  < num> \r\n");
        return 0;
    }
    WV_U32  id,i;



    WV_U16 cpu,fpgaIn,fpgaOut;

    WV_ASSERT_RET(WV_STR_S2v(argv[0],&id));

    for(i=0;i<id;i++){
        TSK_FPGA_GetTemp(&cpu,&fpgaIn,&fpgaOut)	;
        sleep(2);
    }



    return WV_SOK;
}





/****************************************************************************
WV_S32 TSK_FPGA_SetLightCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_SetLightCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
    if(argc<1)
    {
        prfBuff += sprintf(prfBuff,"set light  < id> \r\n");
        return 0;
    }
    WV_U32  id;
    WV_ASSERT_RET(WV_STR_S2v(argv[0],&id));

    FPGA_CONF_SetLightStart();
    TSK_FPGA_GetLight(id);
    TSK_FPGA_SetLight(id);
    FPGA_CONF_SetLightEnd();
    HIS_FB_ClrFpga();

    return WV_SOK;
}

/****************************************************************************

WV_S32 TSK_FPGA_SetCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_FPGA_SetCMD(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{

    WV_U32 temp;
    WV_U8   chennl;


    if(argc<1)
    {
        prfBuff += sprintf(prfBuff,"set fbcmd  < CMD> <Chnnl>  CMD : map/out/split/clr/test\r\n");
        return 0;
    }
    chennl = 0;
    if(argc > 1)
    {
        WV_ASSERT_RET(WV_STR_S2v(argv[1],&temp));
        chennl =  temp & 0xFF;
    }/*
        if(chennl<1)
        {
          chennl = 1;
        }
        if(chennl> 4)
        {
          chennl = 4;
        }
        */
    if(strcmp(argv[0],"map") ==0)
    {
        prfBuff += sprintf(prfBuff,"runing set fbcmd  map \r\n");
        FPGA_CONF_SetMapStart();
        TSK_FPGA_GetMap(chennl);
        TSK_FPGA_SetMap(chennl);
        HIS_FB_ClrFpga();
        FPGA_CONF_SetMapStart();
    }

    if(strcmp(argv[0],"out") ==0)
    {
        prfBuff += sprintf(prfBuff,"runing set fbcmd  out \r\n");
        FPGA_CONF_SetOutlineStart();
        TSK_FPGA_GetOut(chennl);
        TSK_FPGA_SetOut(chennl);
        HIS_FB_ClrFpga();
        FPGA_CONF_SetOutlineEnd();

    }

    if(strcmp(argv[0],"light") ==0)
    {
        prfBuff += sprintf(prfBuff,"runing set fbcmd  light \r\n");
        // HIS_FB_SetFpga( gFpgaDev.pSplitBuf ,3,chennl,TSK_FPGA_SPLIT_COL ,TSK_FPGA_SPLIT_ROW);
        FPGA_CONF_SetLightStart();
        TSK_FPGA_SetLight(chennl);

        HIS_FB_ClrFpga();

        FPGA_CONF_SetLightEnd();

    }


    if(strcmp(argv[0],"clr") ==0)
    {
        prfBuff += sprintf(prfBuff,"runing set fbcmd  clr \r\n");

        HIS_FB_ClrFpga();
    }

    if(strcmp(argv[0],"test") ==0)
    {

        if(chennl == 5)
        {
            HIS_FB_SetFpga( gFpgaDev.pTestBuf,0,0,TSK_FPGA_MAP_COL   ,1088);
        }
        else
        {
            TSK_FPGA_GetMap(chennl);
            HIS_FB_SetFpga( gFpgaDev.pMapBuf,0,0,TSK_FPGA_MAP_COL   ,1080);
        }

    }

    return WV_SOK;
}
/********************************************************
WV_S32  TSK_FPGA_load();
********************************************************/
WV_S32  TSK_FPGA_load()
{
    WV_S32 i,j,k;
    WV_S32 chl=0,id, ret=0;
    //WV_U16 ena = 0;
    chl = FPGA_CONF_GetOutChl_Num();
    if(chl == -1)
    {
        WV_printf("error:fpga load get out chl is %d \n",chl);
        return WV_EFAIL;
    }
    id = FPGA_CONF_GetVersion_ID();
    //FPGA_CONF_SetOutput(ena);
    /*****************fpga reset ******************/
    FPGA_CONF_ResetA();
    /*****************set resolution***************/
    FPGA_CONF_SetResolution( &gFpgaDev.resolution);
    /**********load mapfile local position ********/
    FPGA_CONF_SetMapStart();
    for(i=0; i<chl; i ++)
    {
        TSK_FPGA_GetMap(i);
        TSK_FPGA_SetMap(i);
    }
    FPGA_CONF_SetMapEnd();
    /***************get test***********************/

    //	 TSK_FPGA_GetTest();

    /************load outfile local pasition*********/
    WV_printf("**************set outline******\n");
    if((chl == 4) || (chl == 8) ){


        FPGA_CONF_SetOutlineStart();

        for(j=0;j<16; j++)
        {
            TSK_FPGA_GetOut(j);
            TSK_FPGA_SetOut(j);
        }
        FPGA_CONF_SetOutlineEnd();

    }
    if((chl == 6) || (chl == 2) )
    {
        if(id == 1)
        {
            //WV_printf("**************set out 6 1 ********************\n");
            FPGA_CONF_SetOutlineStart();

            for(j=0;j<16; j++)
            {
                TSK_FPGA_GetOut(j);
                TSK_FPGA_SetOut(j);
            }
            FPGA_CONF_SetOutlineEnd();

        }

    }

    /*************load lightfile local pasition*********/
    WV_printf("**************set light******\n");
    if(chl==4){
        FPGA_CONF_SetLightStart();
        for(k=0;k<chl;k++)
        {
            ret = TSK_FPGA_GetLight(k);
            if(ret != 0 ) continue;
            TSK_FPGA_SetLight(k);

        }

        for(k=0;k<chl+8;k++)
        {
            ret = TSK_FPGA_GetLight(k);
            if(ret != 0 ) continue;
            TSK_FPGA_SetLight(k);
        }
        FPGA_CONF_SetLightEnd();
    }else {

        FPGA_CONF_SetLightStart();
        for(k=0;k<chl;k++)
        {
            ret = TSK_FPGA_GetLight(k);
            if(ret != 0 ) continue;
            TSK_FPGA_SetLight(k);

        }

        for(k=8;k<chl+8;k++)
        {
            ret = TSK_FPGA_GetLight(k);
            if(ret != 0 ) continue;
            TSK_FPGA_SetLight(k);
        }

        FPGA_CONF_SetLightEnd();
    }
    /*****************set split**********************/
    FPGA_CONF_SetSplit(&gFpgaDev.splitInfo);

    /*************************************************/

    HIS_FB_ClrFpga();
    FPGA_CONF_ClrBuf();
    FPGA_CONF_SetOutput(gFpgaDev.outputEna);
    
    return WV_SOK;
}

/********************************************************
WV_S32  TSK_FPGA_Open();
********************************************************/

WV_S32  TSK_FPGA_Open()
{
    //先注册一些和FPGA相关的终端命令
    WV_CMD_Register("set","fbcmd","set cmd by fb",TSK_FPGA_SetCMD);
    WV_CMD_Register("get","Output","get output ionfo",TSK_FPGA_GetOutputCMD);
    WV_CMD_Register("set","resol","set resolution ",TSK_FPGA_setResolutionCMD);
    WV_CMD_Register("get","resol","get resolution",TSK_FPGA_GetResolutionCMD);
    WV_CMD_Register("set","split","set split info",TSK_FPGA_SetSplitCMD);
    WV_CMD_Register("get","split","get split info",TSK_FPGA_GetSplitCMD);
    WV_CMD_Register("set","light","set light file",TSK_FPGA_SetLightCMD);
    WV_CMD_Register("set","map","set map file",TSK_FPGA_SetMapCMD);
    WV_CMD_Register("set","outline","set outline file",TSK_FPGA_SetOutlineCMD);
    WV_CMD_Register("get","temp","get temperture ",TSK_FPGA_GetTempCMD);
    WV_CMD_Register("set","loop","get split loop ",TSK_FPGA_SetSplitLoop);
    WV_CMD_Register("set","loopStop","set split loop stop ",TSK_FPGA_SetLoopStop);
    TSK_FPGA_ConfInit();
    TSK_FPGA_Init();
    //get ver and channel num

    //TSK_FPGA_GetTest();


    WV_U16 ena=0;
    FPGA_CONF_SetOutput(ena);   //设置FPGA输出


    HIS_FB_ClrFpga();//
    FPGA_CONF_RqMcu();//请求单片机复位

    return WV_SOK;
}


/********************************************************
WV_S32  TSK_FPGA_Close();
********************************************************/

WV_S32  TSK_FPGA_Close()
{
    FPGA_CONF_SetOutlineStart();
    TSK_FPGA_DeInit();
    return WV_SOK;
}
