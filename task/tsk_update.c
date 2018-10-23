#include"tsk_update.h"
#include<stdio.h>
#define  TSK_UP_PATH_NAME	 "/usb/update/"
#define  TSK_UP_AUDIOLIB_PATH    "/usb/Audiolib/"	
#define  TSK_UP_OTHERLIB_PATH    "/usb/OtherLib/"	
#define  TSK_UP_FILE_NAME	 "up.ini"
#define  TSK_UP_		
#define  BENCHMARK_YEAR         2016
#define  TSK_UP_PWD            "swkj2017"   
#define  TSK_CP_FILE_NUM	20
typedef struct TSK_UP_UPDATE_INFO 
{
	WV_U32	updateFlag; 
	WV_S8   updateInfo[20];     
	WV_U32	manufactureFlag; 
	WV_U32	year;	        
	WV_U32	mon;	        
	WV_U32	day;	        
	WV_U32	num;                
	WV_U32	upHelloFlag;
	WV_S8	helloVer[20];
	WV_U32	upMovFlag;	
	WV_U32	upPngFlag; 
	WV_U32	upOutlineFlag; 
	WV_U32  upFpgaInFlag;
	WV_U32  upFpgaOutFlag;
			
}TSK_UP_UPDATE_INFO;

static WV_CONF_DEV_E  *pUpEnv;
static TSK_UP_UPDATE_INFO upInfo;

/******************************************************************************
WV_S32  TSK_UP_Get(WV_S8 *pName ,WV_S8 * pValue);
******************************************************************************/
WV_S32  TSK_UP_Get(WV_S8 *pName ,WV_S8 * pValue)
{
	return WV_CONF_Get(pUpEnv,pName,pValue);
}
/******************************************************************************
WV_S32  TSK_UP_Set(WV_S8 *pName ,WV_S8 * pValue);
******************************************************************************/
WV_S32  TSK_UP_Set(WV_S8 *pName ,WV_S8 * pValue)
{
	return WV_CONF_Set(pUpEnv,pName,pValue);
}

/******************************************************************************
WV_S32  TSK_UP_GetU32(WV_S8 *pName ,WV_U32 * pValue);
******************************************************************************/
WV_S32  TSK_UP_GetU32(WV_S8 *pName ,WV_U32 * pValue)
{

	WV_S8  Value[WV_CONF_VALUE_MAX_LEN];
	WV_S32 ret = 0;
	ret = WV_CONF_Get(pUpEnv,pName,Value);
	if(ret == WV_EFAIL)
	{
		WV_ERROR("no the env  %s\r\n",pName);
  	return WV_EFAIL;
	}
	WV_STR_S2v(Value,pValue);
	return WV_SOK;
}

/******************************************************************************
WV_S32  TSK_UP_SetU32(WV_S8 *pName ,WV_U32 * pValue);
******************************************************************************/
WV_S32  TSK_UP_SetU32(WV_S8 *pName ,WV_U32 data)
{

	WV_S8  Value[WV_CONF_VALUE_MAX_LEN];
	sprintf(Value,"%d",data);
	
	return WV_CONF_Set(pUpEnv,pName,Value);
}
/******************************************************************************
WV_S32  TSK_UP_Save();
******************************************************************************/
WV_S32 TSK_UP_Save(){
	return WV_CONF_Save(pUpEnv);
}

/******************************************************************************
WV_S32  TSK_UP_ResgistConf32(WV_S8 *pName ,WV_U32 * pValue);
******************************************************************************/
WV_S32  TSK_UP_ResgistConf()
{
	WV_S8 srcFile[20];
	WV_S8 desFile[20];
	WV_S32 i;

	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"UpdateFlag","0","UpdateFlag"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"UpdateInfo","0","UpdateInfo"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"ManufactureInfo","0","Manufacture Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"Year","2016","Year Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"Mon","10","Mon Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"Day","1","Day Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"Num","0","Num Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"UpHelloFlag","0","UpdataFlag Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"HelloVer","1.0.0.0","Hello version"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"UpMovFlag","0","MovFlag Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"UpPngFlag","0","PngFlag Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"UpOutlineFlag","0","CpFlag Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"UpFpgaInFlag","0","CpFlag Info"));
	WV_CHECK_FAIL( WV_CONF_Register(pUpEnv,1,"UpFpgaOutFlag","0","CpFlag Info"));

	return WV_SOK;

}
/******************************************************************************
WV_S32  TSK_UP_GetConf();
******************************************************************************/
WV_S32  TSK_UP_GetConf()
{
	WV_S32 i;
	WV_S8 *pTemp;
	WV_S8 srcFile[20];
	WV_S8 desFile[20];

	TSK_UP_GetU32("UpdateFlag",&upInfo.updateFlag);
	TSK_UP_Get("UpdateInfo",upInfo.updateInfo);
	TSK_UP_GetU32("ManufactureInfo",&upInfo.manufactureFlag);
	TSK_UP_GetU32("Year",&upInfo.year);
	TSK_UP_GetU32("Mon",&upInfo.mon);
	TSK_UP_GetU32("Day",&upInfo.day);
	TSK_UP_GetU32("Num",&upInfo.num);
	TSK_UP_GetU32("UpHelloFlag",&upInfo.upHelloFlag);
	TSK_UP_Get("HelloVer",upInfo.helloVer);
	TSK_UP_GetU32("UpMovFlag",&upInfo.upMovFlag);	
	TSK_UP_GetU32("UpPngFlag",&upInfo.upPngFlag);
	TSK_UP_GetU32("UpOutlineFlag",&upInfo.upOutlineFlag);
	TSK_UP_GetU32("UpFpgaInFlag",&upInfo.upFpgaInFlag);
	TSK_UP_GetU32("UpFpgaOutFlag",&upInfo.upFpgaOutFlag);
#if 1

	WV_printf("UpdateFlag = [%d]\n",upInfo.updateFlag);
	WV_printf("UpdateInfo = [%s]\n",upInfo.updateInfo);	
	WV_printf("ManufactureInfo = [%d]\n",upInfo.manufactureFlag);
	WV_printf("Year = [%d]\n",upInfo.year);
	WV_printf("Mon = [%d]\n",upInfo.mon);
	WV_printf("Day = [%d]\n",upInfo.day);
	WV_printf("Num = [%d]\n",upInfo.num);
	WV_printf("UpHelloFlag = [%d]\n",upInfo.upHelloFlag);
	WV_printf("HelloVer = [%s]\n",upInfo.helloVer);
	WV_printf("UpMovFlag = [%d]\n",upInfo.upMovFlag);
	WV_printf("UpPngFlag = [%d]\n",upInfo.upPngFlag);
	WV_printf("UpOutlineFlag = [%d]\n",upInfo.upOutlineFlag);
	WV_printf("UpFpgaInFlag = [%d]\n",upInfo.upFpgaInFlag);
	WV_printf("UpFpgaOutFlag = [%d]\n",upInfo.upFpgaOutFlag);
#endif 
	return WV_SOK;


}
/******************************************************************************
WV_S32 TSK_UP_ManuSetMac(TSK_UP_UPDATE_INFO date);
******************************************************************************/
WV_S32 TSK_UP_ManuSetMac(TSK_UP_UPDATE_INFO date)
{

	WV_S8 buf[6];	
	WV_S8 mac[20];
	
	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0x01;
	buf[3] = ((date.year - BENCHMARK_YEAR) & 0x07 << 5)| ((date.mon & 0x0f) << 1) | ((date.day & 0x1f) >> 4);
	buf[4] = ((date.day & 0x0f) << 4 ) | ((date.num & 0x0fff) >> 8);
	buf[5] = date.num & 0xff ;

	sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	
	WV_printf("%02x:%02x:%02x:%02x:%02x:%02x",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
	
	SYS_IP_SetMac(mac);
	SYS_IP_SaveUpdate();
	return WV_SOK;				
}

/******************************************************************************
WV_S32  TSK_UP_Encrypt(WV_S8 * in_fname,WV_S8 *pwd,WV_S8 *out_fname);
******************************************************************************/
WV_S32  TSK_UP_Encrypt(WV_S8 * in_fname,WV_S8 *pwd,WV_S8 *out_fname)
{
	FILE *fp_1,*fp_2;
	WV_S32 i=0;
	WV_U32 pwd_length= 0;	
	WV_S32 c;
	fp_1 = fopen(in_fname,"rb");

	if(fp_1 == NULL){
		printf("open src faile!\n");		
		return -1;
	}	
	fp_2 = fopen(out_fname,"wb");

	if(fp_2 == NULL){
		printf("open dec faile!\n");
		fclose(fp_1);		
		return -1;
	}
	pwd_length = strlen(pwd);
	while((c = fgetc(fp_1)) != EOF){
		
		fputc(c ^ pwd[i>pwd_length ?i=0:i++],fp_2);
	}
	fclose(fp_1);
	fclose(fp_2);
	return 0;
}

/******************************************************************************
WV_S32 TSK_UP_UpdateCheck();
******************************************************************************/
WV_S32 TSK_UP_UpdateCheck()
{
	WV_S32 ret;	
	WV_S8 srcFile[20];
	WV_S8 desFile[20];
	
	sprintf(srcFile,"%s%s",TSK_UP_PATH_NAME,TSK_UP_FILE_NAME);	//  /usb/update/up.ini
	sprintf(desFile,"./%s",TSK_UP_FILE_NAME);  //  ./update/up.ini
	//check srcFile	
	ret = WV_FILE_Access(srcFile);
	if(ret != WV_SOK){
		WV_printf("usb update check error!\n");
		return WV_EFAIL;
	}


	//check decrypt
	ret = TSK_UP_Encrypt(srcFile,TSK_UP_PWD,desFile);
	if(ret !=WV_SOK){
		WV_printf("usb update decrypt error!\n");
	}
	//check desFile
	ret = WV_FILE_Access(desFile);
	if(ret != WV_SOK){
		WV_printf("usb update check desFile error!\n");
		return WV_EFAIL;
	}

	WV_printf("usb update check ok!\n");
	return WV_SOK;
	
}
/******************************************************************************
WV_S32 TSK_UP_UpdateInfoCheck();
******************************************************************************/
WV_S32 TSK_UP_UpdateInfoCheck()
{
	WV_S8 dataEnv[20];
	WV_S8 dataUsb[20];
	
	SYS_ENV_Get("UpdateInfo",dataEnv);
	TSK_UP_Get("UpdateInfo",dataUsb);

	if(upInfo.updateFlag == 0){
	
		WV_printf("updateFlag [%d],don't need to update \n",upInfo.updateFlag);
		return WV_EFAIL;
	}
	if(strcmp(dataEnv,dataUsb) == 0 ){
			
		WV_printf("updateInfo [%s],error don't need to update\n",upInfo.updateInfo);
		return WV_EFAIL;	
	}	

	return WV_SOK;
}
/******************************************************************************
WV_S32 TSK_UP_UpManu();
******************************************************************************/
WV_S32 TSK_UP_UpManu()
{
	WV_U32 num;
	WV_S32 ret;
	if(upInfo.manufactureFlag == 1){

		WV_printf("\r\n-------->updata manufacture Info start..\n");		
		SYS_ENV_SetU32("DateYear",upInfo.year);
		SYS_ENV_SetU32("DateMon",upInfo.mon);
		SYS_ENV_SetU32("DateDay",upInfo.day);
		SYS_ENV_SetU32("DateNum",upInfo.num);
		WV_printf("\r\n-------->updata ID:%d.%d.%d.%d \n",upInfo.year,upInfo.mon,upInfo.day,upInfo.num);		
		TSK_UP_ManuSetMac(upInfo);
		SYS_ENV_Save();
		//write usb update.dat Num +1			
		num = upInfo.num;
		num++;
		TSK_UP_SetU32("Num",num);
		ret = TSK_UP_Save();
		system("sync");
		
		if(ret != WV_SOK){
			WV_printf("\r\n-------->updata num +1[%d] error\n",num);		
		}
		TSK_UP_GetU32("Num",&num);
		WV_printf("\r\n-------->updata manufacture end. num+1=[%d]..\n",num);
	}else{
		WV_printf("\r\n-------->don't updata manufacture Info\n");
	}	
	
	return WV_SOK;
	
}
/******************************************************************************
WV_S32 TSK_UP_UpHello();
******************************************************************************/
WV_S32 TSK_UP_UpHello()
{
	WV_S32 argc= 0;	
	WV_S8 **argv =NULL;
	WV_S8 prfBuff[1024*2];
	
	if(upInfo.upHelloFlag == 1){
		WV_S8 envVer[20];
		SYS_ENV_Get("EnvVer",envVer);
		
		if(strcmp(envVer,upInfo.helloVer) != 0){

			WV_printf("\r\n-------->updata hello  start..\n");	
			TSK_USB_GetUpdate(argc, argv, prfBuff);
			WV_printf("\r\n-------->updata hello end\n");

			strcpy(envVer,upInfo.helloVer);
			SYS_ENV_Set("EnvVer",envVer);	
			SYS_ENV_Save();		
		}else{
			  WV_printf("\r\n-------->hello is new ,don't need to update ,ver[%s]\n",envVer);			
		}	
		
	}else{
		WV_printf("\r\n-------->don't updata hello \n");
	}
		
}

/******************************************************************************
WV_S32 TSK_UP_UpMov();
******************************************************************************/
WV_S32 TSK_UP_UpMov()
{  
    WV_S32 ret = 0; 
    WV_S8 scrFile[64];
    WV_S8 desFile[64];
    WV_S8 scrPath[64];
    WV_S32  i;


	if(upInfo.upMovFlag == 1){	
		WV_printf("\r\n-------->updata mov  start...\n");
		//up mov start

 	sprintf(scrPath,"%smov",TSK_UP_PATH_NAME); 
	ret = WV_FILE_Access(scrPath);
	if(ret != WV_SOK)
	{
		WV_printf("%s file not exist !!! \r\n",scrPath); 
		return WV_SOK;
	}
	ret = WV_FILE_Access("./mov");
	if(ret != WV_SOK)
	{
		mkdir("./mov",0755);  
	}	 
	for(i = 0;i<128;i++)
	{
		sprintf(scrFile,"%s/mov%d.mp4",scrPath,i); 
		sprintf(desFile,"./mov/mov%d.mp4",i);
		ret = WV_FILE_Access(scrFile);
		if(ret != WV_SOK)
		{
			//WV_printf("%s file not exist !!! \r\n",scrFile); 
		 	continue;
		}
		WV_printf("cp %s start...\n",scrFile);  
		ret = WV_FILE_Cpy(scrFile,desFile);

		if(ret == WV_SOK)
		{
		 WV_printf("cpy %s to  %s ok \r\n",scrFile,desFile);  
		}
		else
		{ 
		 WV_printf("cpy %s to  %s erro !!!\r\n",scrFile,desFile);   
		}
	}	

		//up mov end
		WV_printf("\r\n-------->updata mov  end.\n");

	}else{
		WV_printf("\r\n-------->don't updata mov \n");	
	}
	

   
    
    return 0;

}
  
 

/******************************************************************************
WV_S32 TSK_UP_UpPng();
******************************************************************************/
WV_S32 TSK_UP_UpPng()
{
	WV_S32 ret = 0; 
	WV_S8 scrFile[64];
	WV_S8 desFile[64];
	WV_S8 scrPath[64];
	WV_S8 desPath[64];
	WV_S32  i,j;

	if(upInfo.upPngFlag == 1){

	//up png start					
	WV_printf("\r\n-------->updata png  start...\n");

	sprintf(scrPath,"%spng",TSK_UP_PATH_NAME); 	    

	ret = WV_FILE_Access(scrPath);
	if(ret != WV_SOK)
	{
		WV_printf("%s file not exist !!! \r\n",scrPath); 
		return WV_SOK;
	 }
	ret = WV_FILE_Access("./png");
	if(ret != WV_SOK)
	{
		mkdir("./png",0755);  
	}	 
	WV_S8 cmd[128];
	for(i = 0;i<16;i++)
	{ 
		sprintf(scrFile,"%s/png%d",scrPath,i); 
		ret = WV_FILE_Access(scrFile);
		if(ret != WV_SOK)
		{
			continue;
		}	

		sprintf(desFile,"./png/png%d",i); 
		ret = WV_FILE_Access(desFile);
		if(ret != WV_SOK)
		{
			mkdir(desFile,0755);  
		}
   
		j=0;
		//rm old file		
		memset(cmd,0,sizeof(cmd));
		sprintf(cmd,"rm %s/* -rf",desFile);
		system(cmd);
		while(1)
		{
			sprintf(scrFile,"%s/png%d/%d.png",scrPath,i,j); 
			sprintf(desFile,"./png/png%d/%d.png",i,j);
			ret = WV_FILE_Access(scrFile);
			if(ret != WV_SOK)
			{
				WV_printf("%s file not exist !!! \r\n",scrFile); 
				break;
			 } 
			 
		  	 ret = WV_FILE_Cpy(scrFile,desFile);
		   	if(ret == WV_SOK)
		  	{
				j++;
			 
		    }
		   	else
		   	{   
				break;
		   	}
		 }  
		 
		WV_printf("cpy png%d fiel  %d \r\n",i,j);   
	}
		//up png end	
		WV_printf("\r\n-------->updata png  end.\n");
		
	}else{
		WV_printf("\r\n-------->don't updata png \n");	
	}
	
}

/******************************************************************************
WV_S32 TSK_UP_UpOutline();
******************************************************************************/
WV_S32 TSK_UP_UpOutline()
{
	WV_S32 ret = 0; 
	WV_S8 scrFile[64];
	WV_S8 desFile[64];
	WV_S8 scrPath[64];
	WV_S32  i;
	
	if(upInfo.upOutlineFlag == 1){
					
		WV_printf("\r\n-------->updata out  start...\n");
		//start		
		sprintf(scrPath,"%sout",TSK_UP_PATH_NAME); 

		ret = WV_FILE_Access(scrPath);
		if(ret != WV_SOK)
		{
		   WV_printf("%s file not exist !!! \r\n",scrPath); 
		   return WV_SOK;
		 }

		ret = WV_FILE_Access("./outline");
		if(ret != WV_SOK)
		{
		   mkdir("./outline",0755);  
		}	 
	
		for(i = 0;i<16;i++)
		{
			sprintf(scrFile,"%s/outfile%d.dat",scrPath,i); 
			sprintf(desFile,"./outline/outfile%d.dat",i);
			ret = WV_FILE_Access(scrFile);
			if(ret != WV_SOK)
			{
			   WV_printf("%s file not exist !!! \r\n",scrFile); 
			   continue;
			 } 
			 
			ret = WV_FILE_Cpy(scrFile,desFile);
			if(ret == WV_SOK)
			{
			 WV_printf("cpy %s to  %s ok \r\n",scrFile,desFile);  
			}
			else
			{ 
			 WV_printf("cpy %s to  %s erro !!!\r\n",scrFile,desFile);   
			}
		}
		//end
		WV_printf("\r\n-------->updata out  end.\n");
	}else{
		WV_printf("\r\n-------->don't updata out \n");	
	}
    
	return WV_SOK;	
		
}

/******************************************************************************
WV_S32 TSK_UP_FpgaIn();
******************************************************************************/
WV_S32 TSK_UP_FpgaIn()
{
	if(upInfo.upFpgaInFlag == 1){
	
		WV_printf("\r\n-------->updata fpgaIn  start...\n");
		//start

		//end
		WV_printf("\r\n-------->updata fpgaIn  end\n");
	}else{
		WV_printf("\r\n-------->don't updata fpgaIn\n");		
	}
	
	return WV_SOK;
}
/******************************************************************************
WV_S32 TSK_UP_FpgaOut();
******************************************************************************/
WV_S32 TSK_UP_FpgaOut()
{
	if(upInfo.upFpgaOutFlag  == 1){
	
		WV_printf("\r\n-------->updata fpgaOut  start...\n");
		//start

		//end
		WV_printf("\r\n-------->updata fpgaOut  end\n");
	}else{
		WV_printf("\r\n-------->don't updata fpgaOut\n");		
	}
	
	return WV_SOK;
}
/******************************************************************************
WV_S32 TSK_UP_UpdateInit();
******************************************************************************/
WV_S32 TSK_UP_UpdateInit()
{
	WV_S32 i;
	
	WV_S32 ret;

	pUpEnv =  (WV_CONF_DEV_E *)malloc( sizeof(WV_CONF_DEV_E));	
	WV_CHECK_RET( WV_CONF_Creat(pUpEnv,TSK_UP_FILE_NAME,"./"));
	WV_ASSERT_RET( WV_CONF_Load(pUpEnv));		
	ret = TSK_UP_ResgistConf();
 	ret = TSK_UP_GetConf();


	return WV_SOK;
	
}

/******************************************************************************
WV_S32 TSK_UP_UpdateDeInit();
******************************************************************************/
WV_S32 TSK_UP_UpdateDeInit()
{
	WV_S32 i;
	WV_CONF_Destry(pUpEnv);
	free(pUpEnv);
	pUpEnv = NULL; 
	
	//sys "reset"	
	//WV_CMD_GetReset(); 
}

/******************************************************************************
WV_S32 TSK_UP_UpdateStart();
******************************************************************************/
WV_S32 TSK_UP_UpDateStart()
{
	TSK_UP_UpManu();
	TSK_UP_UpHello();
	TSK_UP_UpMov();
	TSK_UP_UpPng();
	TSK_UP_UpOutline();
	TSK_UP_FpgaIn();
	TSK_UP_FpgaOut();

	return WV_SOK;
	
}

/******************************************************************************
WV_S32 TSK_UP_UpdateEnd();
******************************************************************************/
WV_S32 TSK_UP_UpdateEnd()
{
	
	WV_S32 ret;	
	WV_S8 srcFile[20];
	WV_S8 desFile[20];
	
	//set upInfo

	SYS_ENV_Set("UpdateInfo",upInfo.updateInfo);
	SYS_ENV_Save();
			
	
	//check manufactureFlag
	if(upInfo.manufactureFlag == 1){
	
		sprintf(desFile,"%s%s",TSK_UP_PATH_NAME,TSK_UP_FILE_NAME);	//  /usb/update/update.ini
		sprintf(srcFile,"./%s",TSK_UP_FILE_NAME);  //  ./update/up.ini
		//check srcFile	
		
		ret = WV_FILE_Access(srcFile);
		if(ret != WV_SOK){
			WV_printf("usb update check error!\n");
			return WV_EFAIL;
		}
		//check decrypt
		ret = TSK_UP_Encrypt(srcFile,TSK_UP_PWD,desFile);
		if(ret !=WV_SOK){
			WV_printf("usb update decrypt error!\n");
		}
		//check desFile
		ret = WV_FILE_Access(desFile);
		if(ret != WV_SOK){
			WV_printf("usb update check desFile error!\n");
			return WV_EFAIL;
		}

	}


	WV_printf("usb update  ok!\n");

	return WV_SOK;
		
}
/******************************************************************************

WV_S32 TSK_UP_CpAudioLib(WV_S8 *pLibName);

******************************************************************************/
WV_S32 TSK_UP_CpAudioLib(WV_S8 *pLibName)
{

	WV_S32 ret;
	WV_S8 localFile[100];
	WV_S8 usbFile[100];

	sprintf(localFile,"/usr/lib/%s",pLibName);
	sprintf(usbFile,"/usb/Audiolib/%s",pLibName);
	
	ret = access(localFile,F_OK);
	if(ret == WV_SOK)
	{
		WV_printf("audio lib:/usr/lib/%s is exit\n",pLibName);
		return WV_SOK;
	}
	ret = access(usbFile,F_OK);
	if(ret != WV_SOK)
	{
		WV_printf("audio lib :/usb/Audiolib/%s is not exit\n",pLibName);
		return WV_SOK;		
	}
	
	ret = WV_FILE_Cpy(usbFile,localFile);

	if(ret == WV_SOK)
	{
		WV_printf("cpy %s to  %s ok \r\n",usbFile,localFile);  
	}
	else
	{ 
		WV_printf("cpy %s to  %s erro !!!\r\n",usbFile,localFile);   
	}
	return WV_EFAIL;
}
/******************************************************************************

WV_S32 TSK_UP_AudioLib();
升级音频库
******************************************************************************/
WV_S32 TSK_UP_AudioLib()
{
	WV_U32 ret;
	ret = access("/usb/Audiolib/",F_OK);
	if(ret != 0)
	{
		return WV_SOK;
	}
	
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.FFMPEG_ADEC.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.AMRWB.codec.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.MP3.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.MP2.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.AAC.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.DRA.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.TRUEHDPASSTHROUGH.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.AMRNB.codec.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.WMA.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.COOK.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.DTSPASSTHROUGH.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.AC3PASSTHROUGH.decode.so");
	ret |= TSK_UP_CpAudioLib("libHA.AUDIO.PCM.decode.so");

	if(ret != 0 )
	{
		return WV_EFAIL;
	}

	return WV_SOK;
	
}

/******************************************************************************

WV_S32 TSK_UP_CpOtherLib(WV_S8 *pLibName);

******************************************************************************/
WV_S32 TSK_UP_CpOtherLib(WV_S8 *pLibName)
{

	WV_S32 ret;
	WV_S8 localFile[100];
	WV_S8 usbFile[100];

	sprintf(localFile,"/usr/lib/%s",pLibName);
	sprintf(usbFile,"%s%s",TSK_UP_OTHERLIB_PATH,pLibName);
	
	ret = access(localFile,F_OK);
	if(ret == WV_SOK)
	{
		WV_printf("audio lib:/usr/lib/%s is exit\n",pLibName);
		return WV_SOK;
	}
	ret = access(usbFile,F_OK);
	if(ret != WV_SOK)
	{
		WV_printf("audio lib :/usb/OtherLib/%s is not exit\n",pLibName);
		return WV_SOK;		
	}
	
	ret = WV_FILE_Cpy(usbFile,localFile);

	if(ret == WV_SOK)
	{
		WV_printf("cpy %s to  %s ok \r\n",usbFile,localFile);  
	}
	else
	{ 
		WV_printf("cpy %s to  %s erro !!!\r\n",usbFile,localFile);   
	}
	return WV_EFAIL;
}
/******************************************************************************

WV_S32 TSK_UP_OtherLib();
升级其他库：包括sqlite3，libcharset，libconv
******************************************************************************/
WV_S32 TSK_UP_OtherLib()
{
	WV_U32 ret;
	ret = access("/usb/OtherLib/",F_OK);
	if(ret != 0)
	{
		return WV_SOK;
	}
	
	ret |= TSK_UP_CpOtherLib("libcharset.so.1.0.0");
	ret |= TSK_UP_CpOtherLib("libcharset.so");
	ret |= TSK_UP_CpOtherLib("libcharset.so.1");
	ret |= TSK_UP_CpOtherLib("libiconv.la");
	ret |= TSK_UP_CpOtherLib("libiconv.so");
	ret |= TSK_UP_CpOtherLib("libiconv.so.2");
	ret |= TSK_UP_CpOtherLib("libiconv.so.2.5.1");
	ret |= TSK_UP_CpOtherLib("libcharset.la");
	ret |= TSK_UP_CpOtherLib("libcharset.a");
	ret |= TSK_UP_CpOtherLib("libsqlite3.so.0.8.6");
	ret |= TSK_UP_CpOtherLib("libsqlite3.so");
	ret |= TSK_UP_CpOtherLib("libsqlite3.so.0");
	ret |= TSK_UP_CpOtherLib("libsqlite3.la");
	ret |= TSK_UP_CpOtherLib("libsqlite3.a");
	ret |= TSK_UP_CpOtherLib("DroidSansFallbackLegacy.ttf");

	if(ret != 0 )
	{
		return WV_EFAIL;
	}

	return WV_SOK;
	
}

/******************************************************************************
WV_S32 TSK_UP_Update();
******************************************************************************/
WV_S32 TSK_UP_Update(){

	WV_S32 ret=0;
	ret |=TSK_UP_AudioLib();
	ret |=TSK_UP_OtherLib();

	if(ret != 0 )
	{
		//reboot;
		WV_CMD_GetReset(); 
	}

	if(TSK_UP_UpdateCheck() == 0){
		TSK_UP_UpdateInit();
		
		if(TSK_UP_UpdateInfoCheck() == 0){
			TSK_UP_UpDateStart();
			TSK_UP_UpdateEnd();
			//reboot
			WV_CMD_GetReset(); 	
		}
		TSK_UP_UpdateDeInit();
		
	}

	return WV_SOK;
}



