#include "sys_licences.h"
#include "time.h"
#include "tsk_scene.h"
#include "sys_env.h"  
#include "tsk_fpga.h"
#include "fpga_conf.h"
#define SYS_INFO_FILE_PATH  "./env/info.txt"
#define SYS_INFO_MAX_LINE 1024 
#define SYS_INFO_MAX_LEN  1024
#define SYS_INFO_VER_MAX_LEN 1024*4

//#define SYS_INFO_DEBUG_MODE
#ifdef  SYS_INFO_DEBUG_MODE 
#define SYS_INFO_printf(...) \
  				 do {\
  				  printf("\n\rLIC::" __VA_ARGS__);\
  				  fflush(stdout); \
  				  } while(0)
#else 
  #define SYS_INFO_printf(...) 
#endif



#define SYS_INFO_VERSION_FPGA_2_4_1  "version 2 in 4 out ver 1\r\n not support 2 mov, screen max size(3840*1080)"
#define SYS_INFO_VERSION_FPGA_2_4_2  "version 2 in 4 out ver 2\r\n support 2 mov, screen max size(3840*2160)\r\n"
#define SYS_INFO_VERSION_FPGA_2_4_3  "version 2 in 4 out ver 3\r\n add outLine max size(3840*2160) \r\n"
#define SYS_INFO_VERSION_FPGA_2_4_4  "version 2 in 4 out ,fpga version 4\r\n add outLine max size(3840*2160) \r\n"
#define SYS_INFO_VERSION_FPGA_2_4_5  "version 2 in 4 out ,fpga version 5\r\n "

#define SYS_INFO_VERSION_FPGA_1_6_1  "version 1 in 6 out ver 1\r\n support 2 mov, screen max size(3840*2160)\r\n"
#define SYS_INFO_VERSION_FPGA_1_6_2  "version 1 in 6 out ver 2\r\n  add outLine max size(3840*2160) \r\n"
#define SYS_INFO_VERSION_FPGA_1_6_3  "version 1 in 6 out ver 3\r\n  add outLine max size(3840*2160) \r\n"
#define SYS_INFO_VERSION_FPGA_1_6_4  "version 1 in 6 out,fpga version 4 \r\n add outLine max size(3840*2160) \r\n"
#define SYS_INFO_VERSION_FPGA_1_6_5  "version 1 in 6 out,fpga version 5\r\n "
#define SYS_INFO_VERSION_FPGA_1_6_6  "version 1 in 6 out,fpga version 6\r\n "

#define SYS_INFO_VERSION_FPGA_0_6_1  "version 0 in 6 out ver 1\r\n support 2 mov, screen max size(3840*2160)\r\n"
#define SYS_INFO_VERSION_FPGA_0_6_2  "version 0 in 6 out ver 2\r\n add outLine max size(3840*2160) \r\n"
#define SYS_INFO_VERSION_FPGA_0_6_3  "version 0 in 6 out ver 3\r\n add outLine max size(3840*2160) \r\n"
#define SYS_INFO_VERSION_FPGA_0_6_4  "version 0 in 6 out ver 4\r\n add outLine max size(3840*2160) \r\n"
#define SYS_INFO_VERSION_FPGA_0_6_5  "version 0 in 6 out,fpga version 5\r\n "
#define SYS_INFO_VERSION_FPGA_0_6_6  "version 0 in 6 out,fpga version 6\r\n "



#define SYS_INFO_TEMP_WARN_VAL 70
typedef struct SYS_INFO_DATA
{

	WV_U16 cpuTemp;
	WV_U16 fpgaIn;
	WV_U16 fpgaOut;
	WV_U32 num;

}SYS_INFO_DATA;
typedef struct SYS_INFO_DEV_E
{
	WV_THR_HNDL_T 	thrHndl; 
	WV_U32      	open;
	WV_U32      	close;
	WV_U32          infoUse;// 0 is not use 1 is use
	WV_U32          tempNum;
	WV_U32          lineNum;
	WV_S8           *pVer;
	SYS_INFO_DATA   info[SYS_INFO_MAX_LINE];
}SYS_INFO_DEV_E;

static SYS_INFO_DEV_E gSysInfoDev;


/********************************************************************

WV_S32 SYS_INFO_DevReset()

********************************************************************/
WV_S32 SYS_INFO_DevReset()
{
	
	sleep(1); 
    WV_CMD_GetReset(); 
	return 0;	
}
/********************************************************************

WV_S32 SYS_INFO_SaveInfoFile(SYS_INFO_DEV_E *pDev);

********************************************************************/
WV_S32 SYS_INFO_SaveInfoFile(SYS_INFO_DEV_E *pDev)
{

	pDev->infoUse = 1;

	FILE *fp;
	WV_S32 writeLen;
	WV_S8 *pData =(WV_S8 *)pDev->info;
	fp = fopen(SYS_INFO_FILE_PATH,"wb+");
	if(fp == NULL)
	{
		WV_printf("*********fopen %s  error **************\n",SYS_INFO_FILE_PATH);
		pDev->infoUse = 0;
		return -1;
	}


	writeLen = fwrite(pData,1,sizeof(SYS_INFO_DATA)*SYS_INFO_MAX_LINE,fp);
	if(writeLen != sizeof(SYS_INFO_DATA)*SYS_INFO_MAX_LINE )
	{
		WV_printf("*********fwrite %s  error **************\n",SYS_INFO_FILE_PATH);
		fclose(fp);
		pDev->infoUse = 0;
		return -1;
	}

	fclose(fp);
	pDev->infoUse = 0;
	return 0;
			
}


/********************************************************************

WV_S32 SYS_INFO_GetLineNum(SYS_INFO_DEV_E *pDev)

********************************************************************/
WV_S32 SYS_INFO_GetLineNum(SYS_INFO_DEV_E *pDev)
{
	WV_U32 lineNum=0;
	WV_U32 i;
	lineNum = pDev->info[0].num;
	for(i=0;i<SYS_INFO_MAX_LINE;i++)
	{
		if(pDev->info[i].num >lineNum)
		{
			lineNum = pDev->info[i].num;
		}
	}	
	
	pDev->lineNum = lineNum;

	//printf("get line num is [%d] \n",pDev->lineNum);
	if(lineNum == 0xffffffff)
	{
		memset((WV_S8 *)pDev->info,0,sizeof(SYS_INFO_DATA)*SYS_INFO_MAX_LINE);
		pDev->lineNum = 0;
		SYS_INFO_SaveInfoFile(pDev);
	}

	
	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_INFO_GetTemperature(SYS_INFO_DEV_E *pDev)

********************************************************************/
WV_S32 SYS_INFO_GetTemperature(SYS_INFO_DEV_E *pDev)
{
	
	while(pDev->infoUse == 1 )
	{
		usleep(100000);
	}

	WV_U16 cpuTemp,inTemp,outTemp;		
	TSK_FPGA_GetTemp(&cpuTemp,&inTemp,&outTemp);
	if(	pDev->tempNum == 0) //查询温度第一次获取到的数据不准确，所以放弃第一次查询的数据
	{	
		pDev->tempNum = 1;
		return WV_SOK;	
	}
	
	if(cpuTemp >SYS_INFO_TEMP_WARN_VAL || inTemp > SYS_INFO_TEMP_WARN_VAL || outTemp > SYS_INFO_TEMP_WARN_VAL)
	{
		 //sprintf(&pDev->pInfo[1024*pDev->lineNum],"[%d]cpu Temp：[%d] fpga in：[%d] fpga out:[%d]\r\n",pDev->lineNum,cpuTemp,inTemp,outTemp);
		
		WV_U32 minNum;
		WV_U32 i,j=0;
		minNum = pDev->info[0].num;
		for(i=0;i<SYS_INFO_MAX_LINE;i++)
		{
			if(pDev->info[i].num < minNum)
			{
				minNum = pDev->info[i].num;
				j=i;
			}
		}	

		pDev->lineNum++;	
		pDev->info[j].num = pDev->lineNum;
		if(cpuTemp > 200 || cpuTemp <0 || inTemp > 200 || inTemp < 0 || outTemp > 200 || outTemp <0)
		{
			return WV_SOK;//error temp data
		}
		pDev->info[j].cpuTemp = cpuTemp;
		pDev->info[j].fpgaIn = inTemp;
		pDev->info[j].fpgaOut = outTemp;
			
		//printf("pDev->info[%d],num[%d]get cputemp[%d],in[%d],out[%d]\n",j,pDev->info[j].num,pDev->info[j].cpuTemp,pDev->info[j].fpgaIn,pDev->info[j].fpgaOut);

		SYS_INFO_SaveInfoFile(pDev);		
	}
		
	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_INFO_GetVer(SYS_INFO_DEV_E *pDev);

********************************************************************/
WV_S32 SYS_INFO_GetVer(SYS_INFO_DEV_E *pDev)
{

	WV_U32 fpgaID,chlNum,fpgaVer;
	fpgaVer = FPGA_CONF_GerVer();
	fpgaID = FPGA_CONF_GetVersion_ID();//id == 0 ,no input ,id == 1 ,have input chn
	chlNum = FPGA_CONF_GetOutChl_Num();



	if(chlNum == 4){     //4chn 
		
		switch(fpgaVer)
		{
			case 1:
				sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_2_4_1);
				break;
			case 2:
				sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_2_4_2);
				break;
			case 3:
				sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_2_4_3);
				break;
			case 4:
				sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_2_4_4);
				break;
			case 5:
				sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_2_4_5);
				break;

			default:
				break;			
		}
	}
	else if(chlNum == 2 || chlNum == 6)	{

		if(fpgaID == 0)
		{
			switch(fpgaVer)
			{
				case 1:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_0_6_1);
					break;
				case 2:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_0_6_2);
					break;
				case 3:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_0_6_3);
					break;
				case 4:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_0_6_4);
					break;
				case 5:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_0_6_5);
					break;
				case 6:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_0_6_6);
					break;

				default:
					break;			
			}
		
		}else if(fpgaID == 1){
			
			switch(fpgaVer)
			{
				case 1:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_1_6_1);
					break;
				case 2:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_1_6_2);
					break;
				case 3:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_1_6_3);
					break;
				case 4:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_1_6_4);
					break;
				case 5:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_1_6_5);
					break;
				case 6:
					sprintf(pDev->pVer,"%s",SYS_INFO_VERSION_FPGA_1_6_6);
					break;

				default:
					break;			
			}
		
		}
			
	}

	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_INFO_GetInfo(SYS_INFO_DEV_E *pDev);

********************************************************************/
WV_S32 SYS_INFO_GetInfo(WV_U32 *pDataLen,WV_S8 *pData )
{
	

	while(gSysInfoDev.infoUse == 1 )
	{
		usleep(100);
	}	
	gSysInfoDev.infoUse = 1 ;
	
	WV_U32 dataLen = 0;	
	SYS_INFO_DATA *pInfo = gSysInfoDev.info;
	WV_S8 *pBuf = pData;
	WV_S32 verLen,i;
	verLen = strlen(gSysInfoDev.pVer);
	strncpy(pBuf,gSysInfoDev.pVer,verLen);//cp ver 
	pBuf +=verLen ;
	dataLen += verLen;
	WV_S8 buf[1024];

	for(i=0;i<SYS_INFO_MAX_LINE;i++)
	{

		if(gSysInfoDev.info[i].num !=0)
		{
			if(pInfo[i].cpuTemp > 200 || pInfo[i].cpuTemp < 0 || pInfo[i].fpgaIn > 200 || pInfo[i].fpgaIn < 0 || pInfo[i].fpgaOut >200 || pInfo[i].fpgaOut < 0)
			continue;
			memset(buf,0,sizeof(buf));
			sprintf(buf," cpuTemp:[%d],fpga1:[%d],fpga2:[%d]\r\n",pInfo[i].cpuTemp,pInfo[i].fpgaIn,pInfo[i].fpgaOut);
			//printf("[%d]cpuTemp：[%d],fpga1：[%d],fpga2:[%d]\r\n",pInfo[i].num,pInfo[i].cpuTemp,pInfo[i].fpgaIn,pInfo[i].fpgaOut);
			strncpy(pBuf,buf,strlen(buf));
			pBuf += strlen(buf);
			dataLen += strlen(buf);
		}	
	}
 	
	*pDataLen = dataLen;
	gSysInfoDev.infoUse = 0;
	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_INFO_ReadInfoFile(SYS_INFO_DEV_E *pDev);

********************************************************************/
WV_S32 SYS_INFO_ReadInfoFile(SYS_INFO_DEV_E *pDev)
{


	pDev->infoUse = 1;
	WV_S32 ret,readLen;	
	ret = access(SYS_INFO_FILE_PATH,W_OK);

	if(ret != 0){
		WV_printf("access file [%s] error\n",SYS_INFO_FILE_PATH);
		pDev->infoUse = 0;
		return WV_EFAIL;
	}

	FILE *fp;
	//open sceneCmd.dat
	fp = fopen(SYS_INFO_FILE_PATH,"rb+");
	if(fp == NULL)
	{
		WV_printf("fopen file [%s] error\n",SYS_INFO_FILE_PATH);
		pDev->infoUse = 0;
		return WV_EFAIL;
	}

	readLen = fread(pDev->info, 1, sizeof(SYS_INFO_DATA)*SYS_INFO_MAX_LINE, fp);

	if(readLen  !=  sizeof(SYS_INFO_DATA)*SYS_INFO_MAX_LINE)
	{
		fclose(fp);
		pDev->infoUse = 0;
		WV_printf(" *****read info file error *******\n");
		return WV_EFAIL;
	}
	fclose(fp);
	pDev->infoUse = 0;

	//printf(" end**SYS_INFO_ReadInfoFile(SYS_INFO_DEV_E *pDev)\n");
	return WV_SOK;
			
}



/********************************************************************

WV_S32 SYS_INFO_Init();

********************************************************************/
WV_S32 SYS_INFO_Init()
{
	
	//gSysInfoDev.pInfo = (WV_S8 *)malloc((SYS_INFO_MAX_LEN)*(SYS_INFO_MAX_LINE) + 2048);//pMovInfoDev= (TSK_CONF_MOV_INFO_E*)malloc(sizeof(TSK_CONF_MOV_INFO_E));

	//memset(gSysInfoDev.pInfo,0,(SYS_INFO_MAX_LEN)*(SYS_INFO_MAX_LINE));
	
	gSysInfoDev.pVer = (WV_S8*)malloc(SYS_INFO_VER_MAX_LEN);
	
	memset(gSysInfoDev.pVer,0,SYS_INFO_VER_MAX_LEN);
	memset(gSysInfoDev.info,0,sizeof(SYS_INFO_DATA)*SYS_INFO_MAX_LINE);
	SYS_INFO_ReadInfoFile(&gSysInfoDev);
	SYS_INFO_GetLineNum(&gSysInfoDev);
	SYS_INFO_GetVer(&gSysInfoDev);
	gSysInfoDev.tempNum = 0;
	gSysInfoDev.infoUse = 0;
	return WV_SOK;
}


/********************************************************************

WV_S32 SYS_INFO_DeInit();

********************************************************************/
WV_S32 SYS_INFO_DeInit()
{
	
	//free(gSysInfoDev.pInfo);
	free(gSysInfoDev.pVer);	
	return 0;

}


/********************************************************************

void * SYS_INFO_Proc( void * prm);

********************************************************************/
void * SYS_INFO_Proc( void * prm)
{

	SYS_INFO_DEV_E   * pDev; 

	pDev = (SYS_INFO_DEV_E *)prm;  
	pDev-> open  = 1;
	pDev-> close  = 0;  
	WV_S32 i;
  while(pDev -> open == 1)
  {  
		for(i=0;i<5;i++)
		{
			//get temp for 1 min 
			SYS_INFO_GetTemperature(pDev);
			sleep(60);	
		}

			
  }
 pDev-> open  = 0;
 pDev-> close = 1;
	return NULL;  

}

/********************************************************************

WV_S32 SYS_INFO_Open(); 

********************************************************************/
WV_S32 SYS_INFO_Open()
{

	//printf("*****sys info open *******\n");
	memset(&gSysInfoDev,0,sizeof(gSysInfoDev));

	SYS_INFO_Init();
	//printf("*****sys info init *******\n");
	WV_CHECK_RET( WV_THR_Create(&gSysInfoDev.thrHndl, SYS_INFO_Proc , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gSysInfoDev)); 
	return WV_SOK;	
}

/********************************************************************

WV_S32 SYS_INFO_Close(); 

********************************************************************/
WV_S32 SYS_INFO_Close()
{
	//printf("*****sys info open *******\n");
	 if(gSysInfoDev.open == 1)
	{
		 gSysInfoDev.open = 0;
   	     while(gSysInfoDev.close == 1) ;
         WV_CHECK_RET( WV_THR_Destroy(&gSysInfoDev.thrHndl));	
	}
	SYS_INFO_DeInit();
	return WV_SOK;	
}



