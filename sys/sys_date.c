#include "sys_date.h" 
#include "sys_env.h" 
typedef struct SYS_DATE_CONF_E 
{ 
	WV_S8    year[20];
	WV_S8    mon[20];
	WV_S8    day[20]; 
	WV_S8    num[20];
 
}SYS_DATE_CONF_E;
 
static SYS_DATE_CONF_E gSysDateConf; 

/*******************************************************************
WV_S32 SYS_DATE_RegisterConf();
*******************************************************************/
WV_S32 SYS_DATE_RegisterConf()
{

	WV_S8  name[WV_CONF_NAME_MAX_LEN],value[WV_CONF_VALUE_MAX_LEN]; 
       	// sprintf(name,"SceneAllConfig"); 
	SYS_ENV_Register(0,"DateConfig",NULL,NULL);
	 
	sprintf(name, "DateYear");
	sprintf(value, "%d",2017);
	SYS_ENV_Register(1 ,name,value,"Dev make at year");
		 
	sprintf(name, "DateMon");
	sprintf(value, "%d",4);
	SYS_ENV_Register(1 ,name,value,"Dev make at mon");
		 
	sprintf(name, "DateDay");
	sprintf(value, "%d",11);
	SYS_ENV_Register(1 ,name,value,"Dev make at day");
		 
	sprintf(name, "DateNum");
	sprintf(value, "%d",100);
	SYS_ENV_Register(1 ,name,value,"Dev make No"); 

 return WV_SOK;    
}

/*******************************************************************
WV_S32 SYS_DATE_GetConf();
*******************************************************************/
WV_S32 SYS_DATE_GetConf()
{ 
	SYS_ENV_Get("DateYear",gSysDateConf.year); 
	SYS_ENV_Get("DateMon",gSysDateConf.mon);
	SYS_ENV_Get("DateDay",gSysDateConf.day);
	SYS_ENV_Get("DateNum",gSysDateConf.num); 
	WV_printf("\n**************************************************\n");
	WV_printf("\nID: %s.%s.%s.%s\n",gSysDateConf.year,gSysDateConf.mon,gSysDateConf.day,gSysDateConf.num);
         return WV_SOK;
}

/*******************************************************************
 WV_S32 SYS_DATE_SaveConf();
*******************************************************************/
WV_S32 SYS_DATE_SaveConf()
{ 

	SYS_ENV_Set("DateYear",gSysDateConf.year); 
	SYS_ENV_Set("DateMon",gSysDateConf.mon);
	SYS_ENV_Set("DateDay",gSysDateConf.day);
	SYS_ENV_Set("DateNum",gSysDateConf.num); 
        return WV_SOK;
}

/*******************************************************************
 WV_S32 SYS_DATE_SetYear(WV_S8 * pYear);
*******************************************************************/
WV_S32 SYS_DATE_SetYear(WV_S8 * pYear)
{
  	if(strcmp(pYear,gSysDateConf.year) != 0)
	{
		WV_CONF_CpyStr(pYear,gSysDateConf.year,20);
	  
	}  	 
	
	return WV_SOK;		 
}
/*******************************************************************
 WV_S32 SYS_DATE_GetYear(WV_S8 * pYear);
*******************************************************************/
WV_S32 SYS_DATE_GetYear(WV_S8 * pYear)
{
	WV_CONF_CpyStr(gSysDateConf.year,pYear,20); 
	
	return WV_SOK;		 
}
/*******************************************************************
 WV_S32 SYS_DATE_SetMon(WV_S8 * pMon);
*******************************************************************/
WV_S32 SYS_DATE_SetMon(WV_S8 * pMon)
{
  	if(strcmp(pMon,gSysDateConf.mon) != 0)
	{
		WV_CONF_CpyStr(pMon,gSysDateConf.mon,20);  
	}  	 
	
	return WV_SOK;		 
}

/*******************************************************************
WV_S32 SYS_DATE_GetMon(WV_S8 * pMon);
*******************************************************************/
WV_S32 SYS_DATE_GetMon(WV_S8 * pMon)
{

	WV_CONF_CpyStr(gSysDateConf.mon,pMon,20);  

	return WV_SOK;		 
}
/*******************************************************************
 WV_S32 SYS_DATE_SetDay(WV_S8 * pDay);
*******************************************************************/
WV_S32 SYS_DATE_SetDay(WV_S8 * pDay)
{
  	if(strcmp(pDay,gSysDateConf.day) != 0)
	{
		WV_CONF_CpyStr(pDay,gSysDateConf.day,20);  
	}  	 
	
	return WV_SOK;		 
}

/*******************************************************************
 WV_S32 SYS_DATE_GetDay(WV_S8 * pDay);
*******************************************************************/
WV_S32 SYS_DATE_GetDay(WV_S8 * pDay)
{

	WV_CONF_CpyStr(gSysDateConf.day,pDay,20);  

	return WV_SOK;		 
}
/*******************************************************************
 WV_S32 SYS_DATE_SetNum(WV_S8 * pNum);
*******************************************************************/
WV_S32 SYS_DATE_SetNum(WV_S8 * pNum)
{
  	if(strcmp(pNum,gSysDateConf.num) != 0)
	{
		WV_CONF_CpyStr(pNum,gSysDateConf.num,20);  
	}  	 
	
	return WV_SOK;		 
}
/*******************************************************************
 WV_S32 SYS_DATE_GetNum(WV_S8 * pNum);
*******************************************************************/
WV_S32 SYS_DATE_GetNum(WV_S8 * pNum)
{
  	
	WV_CONF_CpyStr(gSysDateConf.num,pNum,20);   	 
	
	return WV_SOK;		 
}
/****************************************************************************
WV_S32 SYS_DATE_CMDGet(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
****************************************************************************/
WV_S32 SYS_DATE_CMDGet(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{

	SYS_DATE_GetConf();
   prfBuff += sprintf(prfBuff,"ID Info:\r\n");
   prfBuff += sprintf(prfBuff,"year :  %s\r\n",gSysDateConf.year );
   prfBuff += sprintf(prfBuff,"mon :  %s\r\n",gSysDateConf.mon); 
   prfBuff += sprintf(prfBuff,"day :  %s\r\n",gSysDateConf.day ); 
   prfBuff += sprintf(prfBuff,"num :  %s\r\n",gSysDateConf.num ); 

   return WV_SOK; 
}

/****************************************************************************

WV_S32 SYS_DATE_CMDSet(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 SYS_DATE_CMDSet(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{
   WV_S8    year[20];
	WV_S8    mon[20];
	WV_S8    day[20]; 
	WV_S8    num[20];
   WV_S32 ret;
	SYS_DATE_GetConf();

	if(argc < 4)
	{
		prfBuff += sprintf(prfBuff,"set id  <year> <mon> <day> <num> like:set id 2017 5 1 12 \r\n"); 
		return WV_SOK; 
	}
		SYS_DATE_SetYear(argv[0]);
		SYS_DATE_SetMon(argv[1]);
		SYS_DATE_SetDay(argv[2]);
		SYS_DATE_SetNum(argv[3]);
		SYS_DATE_SaveConf();
				  
	return WV_SOK; 
}
/*******************************************************************
 WV_S32 SYS_DATE_Init();
*******************************************************************/
WV_S32 SYS_DATE_Init()
{ 

   SYS_DATE_RegisterConf();
   SYS_DATE_GetConf();  
   WV_CMD_Register("get","id","set id config",SYS_DATE_CMDGet); 
   WV_CMD_Register("set","id","set id config",SYS_DATE_CMDSet); 
   return WV_SOK;
} 

/*******************************************************************
 WV_S32 SYS_DATE_DeInit();
*******************************************************************/
WV_S32 SYS_DATE_DeInit()
{ 

   return WV_SOK;
} 


