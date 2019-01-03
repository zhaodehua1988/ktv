#include "sys_licences.h"
#include "time.h"
#include "tsk_scene.h"
#include "tsk_player.h"
#define SYS_LICENCES_FILE_PATH  "/bin/lic.l"
#define SYS_LICENCES_FILE_MIN  "./env/time.min"
#define SYS_LICENCES_FILE_ID_PATH  "/bin/id.i"
#define SYS_LICENCES_FILE_OUTPUTENA_PATH  "/bin/out.o"
#define SYS_LICENCES_ID_LEN 16
#define SYS_LICENCES_BASE_TIME 12

//#define SYS_LICENCES_DEBUG_MODE
#ifdef  SYS_LICENCES_DEBUG_MODE 
#define SYS_LIC_printf(...) \
  				 do {\
  				  printf("\n\rLIC::" __VA_ARGS__);\
  				  fflush(stdout); \
  				  } while(0)
#else 
  #define SYS_LIC_printf(...) 
#endif

#define  SYS_LIC_PWD            "swkj2017"

typedef struct SYS_LICENCES_DEV_E
{
	WV_THR_HNDL_T 	thrHndl; 
	WV_U32      	open;
	WV_U32      	close;
	WV_U32 ena;    //if licences.lic is exist ena=1;else ena=0
	WV_U8 flag;   //1 not limit ;0 limit time
	WV_U32 time_h;   //hour
	WV_U8 time_m;   //min
	//WV_U32 secCont;   // if secCont=60 ,secCont=0;
	WV_S8  curentId[SYS_LICENCES_ID_LEN];
	WV_S8  OldId[SYS_LICENCES_ID_LEN];
	WV_U8  output;
}SYS_LICENCES_DEV_E;

static SYS_LICENCES_DEV_E gSysLicDev;

/********************************************************************

WV_S32  SYS_LIC_Encrypt(WV_U8 *pIn,WV_S8 *pwd,WV_U8 *pOut,WV_U32 len); 

********************************************************************/
WV_S32  SYS_LIC_Encrypt(WV_U8 *pIn,WV_S8 *pwd,WV_U8 *pOut,WV_U32 len)
{

	WV_S32 i,j=0;
	WV_U32 pwd_length= 0;
	pwd_length = strlen(pwd);
	for(i=0;i<len;i++)
	{
		pOut[i] = pIn[i] ^pwd[j>pwd_length ?j=0:j++];
		
	}

	return 0;
}

/********************************************************************

WV_S32  SYS_LIC_GetTimeOut(WV_S8 * in_fname,WV_S8 *pwd,WV_S8 *pOut); 

********************************************************************/
WV_S32 SYS_LIC_GetTimeOut(WV_U8 *pData)
{

	WV_U32 *pTime;
	pData[0] = gSysLicDev.flag;
	
	pTime = (WV_U32 *) &pData[1];
	*pTime = gSysLicDev.time_h;
	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_LICENCES_Check_ID(WV_U8 *pId); 

********************************************************************/
WV_S32 SYS_LICENCES_Check_ID(WV_U8 *pId)//[check id]
{
	WV_S32 ret=0 ;
	
	WV_S8 outId[SYS_LICENCES_ID_LEN];

	SYS_LIC_Encrypt(pId,SYS_LIC_PWD ,(WV_U8 *)outId,SYS_LICENCES_ID_LEN);
	ret = strncmp(gSysLicDev.curentId,outId,SYS_LICENCES_ID_LEN);
	if(ret != 0 )
	{
		SYS_LIC_printf("licences Check ID error,ret=[%d]\n",ret);
	}
	return ret;
}

/********************************************************************

WV_S32 SYS_LICENCES_MAKE_ID(); 

********************************************************************/
WV_S32 SYS_LICENCES_MAKE_ID()//[get id between 0~9]
{
	WV_S32 i,x=0,y=9;
	srand(time(0));
	for(i=0;i<SYS_LICENCES_ID_LEN;i++)
	{
		gSysLicDev.curentId[i]=x+rand()%(y-x+1);
	}

	WV_U8 *pId;
	pId = (WV_U8 *)gSysLicDev.curentId;
	
	for(i=0;i<SYS_LICENCES_ID_LEN;i++)
	{
		pId[i] = pId[i] + 0x30;
	}

	FILE *fp;
	WV_S32 writeLen;
	fp=fopen(SYS_LICENCES_FILE_ID_PATH,"wb");
	if(fp == NULL)
	{

		SYS_LIC_printf("write id:fopen licences id file error \n");
		return WV_EFAIL;
	}
	
	writeLen = fwrite(pId,1,SYS_LICENCES_ID_LEN,fp);
	if(writeLen != SYS_LICENCES_ID_LEN)
	{
		
		SYS_LIC_printf("write licences id error \n");
		fclose(fp);
		return WV_EFAIL;
	}
	fclose(fp);
	return WV_SOK;
}


/********************************************************************

WV_S32 SYS_LICENCES_READ_ID(); 

********************************************************************/
WV_S32 SYS_LICENCES_READ_ID()//[get id between 0~9]
{
	FILE *fp;
	WV_S32 readLen;
	fp = fopen(SYS_LICENCES_FILE_ID_PATH,"rb");
	if(fp == NULL)
	{
		SYS_LIC_printf("read id:fopen licences id file error \n");
		return WV_EFAIL;
	}
	readLen = fread(gSysLicDev.curentId,1,SYS_LICENCES_ID_LEN,fp);
	if(readLen != SYS_LICENCES_ID_LEN)
	{
		SYS_LIC_printf("read licences id error \n");
		fclose(fp);
		return WV_EFAIL;
	}
	
	fclose(fp);

	SYS_LIC_printf("read id :%s \n",gSysLicDev.curentId);	
	
	return WV_SOK;
}

/********************************************************************

WV_S32  SYS_LICENCES_GetID(WV_S32 *pLen,WV_U8 *pBuf);

********************************************************************/
WV_S32 SYS_LICENCES_GetID(WV_S32 *pLen,WV_U8 *pBuf)
{

	//SYS_LICENCES_MAKE_ID();
	SYS_LICENCES_READ_ID();
	*pLen = SYS_LICENCES_ID_LEN;
	memcpy(pBuf,(WV_U8 *)gSysLicDev.curentId,SYS_LICENCES_ID_LEN);
	return WV_SOK;
}
/********************************************************************

WV_S32 SYS_LICENCES_SaveMin(); 

********************************************************************/
WV_S32 SYS_LICENCES_SaveMin()
{
	FILE *fp;
	WV_S32 writeLen;	
	fp = fopen(SYS_LICENCES_FILE_MIN,"wb+");
	if(fp == NULL)
	{	
		SYS_LIC_printf("fopen min file error !\n");
		return WV_EFAIL;
	}
	
	writeLen = fwrite((WV_S8 *)&gSysLicDev.time_m,1,1,fp);
	if(writeLen != 1)
	{
		SYS_LIC_printf("write min file error !\n");
		fclose(fp);
		return WV_EFAIL;
	}
	fclose(fp);
	system("sync");
	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_LICENCES_ReadMin(); 

********************************************************************/
WV_S32 SYS_LICENCES_ReadMin()
{
	FILE *fp;
	WV_S32 readLen;	
	fp = fopen(SYS_LICENCES_FILE_MIN,"rb");
	if(fp == NULL)
	{	
		SYS_LIC_printf("fopen min file error !\n");
		gSysLicDev.time_m = 60;
		return WV_EFAIL;
	}
	
	readLen = fread((WV_S8 *)&gSysLicDev.time_m,1,1,fp);
	if(readLen != 1)
	{
		SYS_LIC_printf("read min file error !\n");
		fclose(fp);
		return WV_EFAIL;
	}
	fclose(fp);
	SYS_LIC_printf("this time min is %d \n",gSysLicDev.time_m);
	system("sync");
	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_LICENCES_SaveLic(); 

********************************************************************/
WV_S32 SYS_LICENCES_SaveLic()
{
	WV_U8 licIn[64];
	WV_U8 licOut[64];
	WV_S32 i,len,writeLen;	
	FILE *fp;
	for(i=0;i<SYS_LICENCES_ID_LEN;i++)
	{
		licIn[i]=gSysLicDev.OldId[i];
	}
	
	licIn[SYS_LICENCES_ID_LEN] = gSysLicDev.flag +0x30;

	sprintf((WV_S8 *)&licIn[SYS_LICENCES_ID_LEN+1],"%d",gSysLicDev.time_h);
	len = strlen((WV_S8 *)licIn);
	SYS_LIC_Encrypt(licIn,SYS_LIC_PWD,licOut,len);
	fp = fopen(SYS_LICENCES_FILE_PATH,"wb+");
	if(fp == NULL)
	{
		SYS_LIC_printf("licences save hour open lic file error \n");
		return WV_EFAIL;
	}
	writeLen = fwrite(licOut,1,len,fp);
	if(writeLen != len)
	{
		SYS_LIC_printf("licences save hour write error \n");
		fclose(fp);
		return WV_EFAIL;
	}

	fclose(fp);
	system("sync");
	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_LICENCES_SetBaseTime(); 

********************************************************************/
WV_S32 SYS_LICENCES_SetBaseTime()
{
	WV_S32 i;
	gSysLicDev.ena = 1;
	gSysLicDev.flag = 1;
	gSysLicDev.time_h = SYS_LICENCES_BASE_TIME;//12 h
	gSysLicDev.time_m = 60;
	SYS_LICENCES_MAKE_ID();	
	for(i=0;i<SYS_LICENCES_ID_LEN;i++)
	{
		gSysLicDev.OldId[i] = gSysLicDev.curentId[i] + 0x30;
	}	
	SYS_LICENCES_SaveLic();
	return WV_SOK;
}

/********************************************************************

WV_S32 SYS_LICENCES_SetOutEna(WV_U8 output);

********************************************************************/
WV_S32 SYS_LICENCES_SetOutEna(WV_U8 output)
{
	WV_S32 writeLen=0,ret = 0;	
	FILE *fp=NULL;
	fp = fopen(SYS_LICENCES_FILE_OUTPUTENA_PATH,"wb+");
	if(fp == NULL)
	{
		return WV_EFAIL;
	}
	writeLen = fwrite(&output,1,1,fp);
	if(writeLen != 1 )
	{
		printf("**************set output ena error *****************\n");
		ret = -1;
	}
	fclose(fp);
	system("sync");
	return ret;
}
/********************************************************************

WV_S32 SYS_LICENCES_GetOutEna(WV_U8 *pOutput); 

********************************************************************/
WV_S32 SYS_LICENCES_GetOutEna(WV_U8 *pOutput)
{
	WV_S32 readLen=0,ret = 0;	
	FILE *fp=NULL;
	WV_U8 temp;
	fp = fopen(SYS_LICENCES_FILE_OUTPUTENA_PATH,"rb");
	if(fp == NULL)
	{
		
		return WV_EFAIL;
	}
	readLen = fread(&temp,1,1,fp);
	if(readLen != 1 )
	{
		printf("**************get output ena error *****************\n");
		ret = -1;
	}else{
		*pOutput = temp;
	}
	fclose(fp);
	system("sync");
	return ret;
}
/********************************************************************

WV_S32 SYS_LICENCES_ReadLic(); 

********************************************************************/
WV_S32 SYS_LICENCES_ReadLic()
{
	FILE *fp=NULL;
	WV_S32 ret=0,i=0,j=0,readLen=0;
	WV_S8 bufIn[64];
	WV_S8 bufOut[64];
	WV_S8 time[64];
	memset(time,0,sizeof(time));
	memset(bufOut,0,sizeof(bufOut));
	memset(bufIn,0,sizeof(bufIn));

	WV_U16 output;
	ret = access(SYS_LICENCES_FILE_PATH,R_OK);	
	if(ret != 0 )
	{
		SYS_LICENCES_SetBaseTime();
 		SYS_LIC_printf("licences file is not exit\n");
		return ret;
	}
	//encrypt licences
	fp=fopen(SYS_LICENCES_FILE_PATH ,"rb");
	if(fp == NULL)
	{
		SYS_LIC_printf("open src faile!\n");		
		return -1;
	}
	readLen=fread(bufIn,1,sizeof(bufIn),fp);
	if(readLen <0 || readLen >=64)
	{
		SYS_LIC_printf("read licences file is error\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);

	ret = SYS_LIC_Encrypt((WV_U8 *)bufIn,SYS_LIC_PWD,(WV_U8 *)bufOut,readLen);

	if(ret != 0 )
	{
		SYS_LIC_printf("sys licences encrypt error !\n");
		return ret;
	}

	SYS_LIC_printf("encrypt:%s\n",bufOut);
	//read licences ID
	memcpy(gSysLicDev.OldId,bufOut,SYS_LICENCES_ID_LEN);	
	//get flag
	gSysLicDev.flag = bufOut[SYS_LICENCES_ID_LEN] - 0x30;//change ascii to num 
	if(gSysLicDev.flag != 0 && gSysLicDev.flag != 1)
	{
		
		SYS_LIC_printf("sys licences get flag error !\n");
		gSysLicDev.ena = 0;
		return WV_EFAIL;
	}
	//get time
	for(i=SYS_LICENCES_ID_LEN+1;i<strlen(bufOut);i++)
	{
		if(bufOut[i]=='?')
		{
			//get output 
			gSysLicDev.output=bufOut[i+1];
			output = gSysLicDev.output;
			FPGA_CONF_SetGlobalOutput(output);//配置fpga输出卡的全局输出开关
			SYS_LIC_printf("output = 0x%04x \n",output);
			SYS_LICENCES_SetOutEna(gSysLicDev.output);
			break;
		}
		time[j] = bufOut[i];

		j++;
	}
	gSysLicDev.time_h = atoi(time);
	
	gSysLicDev.ena = 1;
	SYS_LIC_printf("licences:ena=%d,flag=%d,time=%d h %d m,oldId=%s\n",gSysLicDev.ena,gSysLicDev.flag,gSysLicDev.time_h,gSysLicDev.time_m,gSysLicDev.OldId);	
	return ret;
}
/********************************************************************

WV_S32 SYS_LICENCES_WriteLic(WV_U8 *pLic,WV_S32 len); 

********************************************************************/
WV_S32 SYS_LICENCES_WriteLic(WV_U8 *pLic,WV_S32 len)
{
	FILE *fp;
	WV_S32 writeLen,ret;
	fp = fopen(SYS_LICENCES_FILE_PATH,"wb+");
	if(fp == NULL)
	{
		SYS_LIC_printf("write licences fopen error !\n");
		return WV_EFAIL;
	}
	
	writeLen = fwrite(pLic,1,len,fp);
	if(writeLen != len)
	{
		SYS_LIC_printf("write licences  error writeLen [%d]!=len[%d] !\n",writeLen,len);
		fclose(fp);
		return WV_EFAIL;
	}
	
	fclose(fp);

	ret = SYS_LICENCES_ReadLic();
	gSysLicDev.time_m = 60;
	SYS_LICENCES_SaveMin();
	
	if((gSysLicDev.flag == 1) || (gSysLicDev.time_h >0) )
	{
		TSK_SCENE_SceneOpen();
	}

	SYS_LICENCES_MAKE_ID();

	return ret;
}

/********************************************************************

WV_S32 SYS_LICENCES_Init(); 

********************************************************************/
WV_S32 SYS_LICENCES_Init()
{

	WV_S32 ret = 0;
	WV_U8 temp=0;
	WV_U16 output;
	ret=SYS_LICENCES_GetOutEna(&temp);
	 if(ret == 0 )
	{
		gSysLicDev.output = temp;
		output = temp;
		FPGA_CONF_SetGlobalOutput(output);
	}else{
		gSysLicDev.output = 0xff;
	}
		
	ret = SYS_LICENCES_ReadLic();//read licences
	SYS_LICENCES_ReadMin();//read min 
	return ret;
}

/********************************************************************

void * SYS_LICENCES_Proc( void * prm);

********************************************************************/
void * SYS_LICENCES_Proc( void * prm)
{

	SYS_LICENCES_DEV_E   * pDev; 

	pDev = (SYS_LICENCES_DEV_E *)prm;  
	pDev-> open  = 1;
	pDev-> close  = 0;  

  while(pDev -> open == 1)
  {  
		
		 if(pDev->ena == 0)//licences file not exit
		{
			SYS_LIC_printf("close scene \n");
			TSK_SCENE_SceneClose();
			TSK_PLAYER_FreezeWindowBlack();
			sleep(1);
			continue;
		}

		if(pDev->flag == 1)//not limit usrtime
		{
			SYS_LIC_printf("sys time not limit\n");
			//TSK_SCENE_SceneOpen();
			sleep(1);
			continue;
		}

		if(pDev->time_h != 0)
		{
			if(pDev->time_m > 60)
			{
				pDev->time_m = 60;
			}
			pDev->time_m--;
			if(pDev->time_m <=1)
			{
				pDev->time_h --; //time h --
				pDev->time_m = 60;
				SYS_LICENCES_SaveLic();
			}
			SYS_LIC_printf(" --------have %d h,%d m \n",pDev->time_h,pDev->time_m);
			SYS_LICENCES_SaveMin();
			sleep(60);
			
			continue;
		}
		
		pDev->ena = 0;
		SYS_LIC_printf("---------this is time out \n");
		sleep(1);   	
  }
 pDev-> open  = 0;
 pDev-> close = 1;
	return NULL;  

}

/********************************************************************

WV_S32 SYS_LICENCES_Open(); 

********************************************************************/
WV_S32 SYS_LICENCES_Open()
{
	memset(&gSysLicDev,0,sizeof(gSysLicDev));
	
	
	SYS_LICENCES_Init();

	WV_CHECK_RET( WV_THR_Create(&gSysLicDev.thrHndl, SYS_LICENCES_Proc , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gSysLicDev)); 
	return WV_SOK;	
}

/********************************************************************

WV_S32 SYS_LICENCES_Close(); 

********************************************************************/
WV_S32 SYS_LICENCES_Close()
{
	 if(gSysLicDev.open == 1)
	{
		 gSysLicDev.open = 0;
   	     while(gSysLicDev.close == 1) ;
         WV_CHECK_RET( WV_THR_Destroy(&gSysLicDev.thrHndl));	
	}
	
	return WV_SOK;	
}



