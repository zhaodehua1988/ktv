#include "tsk_uart.h"
#include "sys_env.h"
#include "his_spi.h"
#include "tsk_player.h"
#include "tsk_scene.h"
#include "wv_telnet.h"
#include "his_gpio.h"
#include "his_temperature.h"
#include "his_reg.h"
#include "net_uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <linux/serial.h>
#define TSK_UART_DEV_NAME  "/dev/ttyAMA1"  
#define TSK_SCENE_CMDFILE "./env/sceneCmd.dat"
#define TASK_UART_MAX_LEN  256  
#define TSK_UART_SCENE_NUM (TSK_SCENE_MAX_NUM) 
#define TSK_UART_WAG_NUM   2
#define TSK_UART_SCENE_CTRLCMD_NUM 13
#define TSK_UART_WIN_CMD_NUM 2
//#define TSK_UART_DEBUG_MODE 
#ifdef  TSK_UART_DEBUG_MODE 
#define TSK_UART_printf(...) \
  				 do {\
  				  printf("\n\rUART:" __VA_ARGS__);\
  				  fflush(stdout); \
  				  } while(0)
#else 
  #define TSK_UART_printf(...) 
#endif   

typedef struct TSK_UART_ID_S
{
 
WV_U8      Vender[2];
WV_U8      type;
WV_U8      ver[3];
WV_U8      random[4]; 
WV_U8      year;
WV_U8      mon;
WV_U8      day;
WV_U8      num[3];

}TSK_UART_ID_S;


typedef struct TSK_UART_SCENE_CMD_S
{
	WV_U8 SceneId;
	WV_U8 CmdLen;
	WV_U8 ScenceCmd[64];
	WV_U8 CmdEna;
	WV_U8 AckEna;
	WV_U8 AckLen;
	WV_U8 AckCmd[64]; 
}TSK_UART_SCENE_CMD_S;

typedef struct TSK_UART_DEV_E 
{
	WV_THR_HNDL_T thrHndl; 
	WV_U32      open;
	WV_U32      close;
	WV_S32      fd; 
	WV_U32      port;
	WV_U32      baudRate;
	WV_U32      dataBit;
	WV_U32      stopBit;
	WV_U32      windowMode; //mode 0 :正常模式， mode 1：串口控制所有窗口开关
	WV_U32      openDev;    //0：串口控制开关机无效  1：串口控制开关机有效
    WV_U32      openProjector;    //0：投影开关机是否有效
    WV_U32      typeRound;   //视频跟随分类进行循环播放
	WV_U32      rcvEroNum;
	WV_U32      checkEroNum;
	WV_U32      procEroNum;
	WV_U32      timeoutNum; 
	WV_U32      runNum;
	TSK_UART_ID_S       id;
	WV_S8       info[20];
	WV_U32      runTime;
	
	TSK_UART_SCENE_CMD_S sceneCmd[TSK_UART_SCENE_NUM]; 
	TSK_UART_SCENE_CMD_S wagCmd[TSK_UART_WAG_NUM];
	TSK_UART_SCENE_CMD_S sceneCtrl[TSK_UART_SCENE_CTRLCMD_NUM];
	
}TSK_UART_DEV_E;

typedef struct TSK_UART_LOOP_E 
{
	WV_THR_HNDL_T thrHndl; 
	WV_U32      open;
	WV_U32      close;
	WV_S32      loopEna;    //0 DisAbl ,1 enabl
	WV_S32      loopTime;	//min
	
}TSK_UART_LOOP_E;

static TSK_UART_LOOP_E gSceneLoop;

static TSK_UART_DEV_E gUartDev;
static WV_S32 gWagShow = -1;
//static WV_S32 gSceneCtrlCmd = -1;

/*******************************************************************
 WV_S32 TSK_UART_GetWindowMode();
*******************************************************************/
WV_S32 TSK_UART_GetWindowMode()
{
	//printf("winmode = %d \n",gUartDev.windowMode);
	return gUartDev.windowMode;
}

/*******************************************************************
 WV_S32 TSK_UART_GetWindowMode();
*******************************************************************/
WV_S32 TSK_UART_GetOpenDevMode()
{
	//printf("winmode = %d \n",gUartDev.windowMode);
	return gUartDev.openDev;
}
/*******************************************************************
WV_S32 TSK_UART_GetOpenprojectorMode();
*******************************************************************/
WV_S32 TSK_UART_GetOpenprojectorMode()
{
	//printf("winmode = %d \n",gUartDev.windowMode);
	return gUartDev.openProjector;
}

/*******************************************************************
 WV_S32 TSK_UART_RegisterConf();
*******************************************************************/
WV_S32 TSK_UART_RegisterConf()
{
 WV_S8  name[WV_CONF_NAME_MAX_LEN],value[WV_CONF_VALUE_MAX_LEN]; 
       	// sprintf(name,"SceneAllConfig"); 
		 SYS_ENV_Register(0,"UartConfig",NULL,NULL);
		 //
		 sprintf(name, "UartPort");
		 sprintf(value, "%d",1);
		 SYS_ENV_Register(1 ,name,value,"Uart port");
		 sprintf(name, "UartRate");
		 sprintf(value, "%d",4800);
		 SYS_ENV_Register(1 ,name,value,"Uart burd rate");
		 sprintf(name, "UartInfo");
		 sprintf(value, "%s","Bei Jing");
		 SYS_ENV_Register(1 ,name,value,"Dev info"); 

 return WV_SOK;    
}

/*******************************************************************
 WV_S32 TSK_UART_GetScene(WV_U32  id ,TSK_UART_DEV_E  * pDev);
*******************************************************************/
WV_S32 TSK_UART_GetConf(TSK_UART_DEV_E  * pDev)
{
   
     WV_U32 data,*pData; 
     pData = & data;
     WV_S8  name[WV_CONF_NAME_MAX_LEN]; 
         sprintf(name, "UartPort");
		 SYS_ENV_GetU32(name, & data);
		 pDev->port = data;  
		 //
		 sprintf(name, "UartRate");
		 SYS_ENV_GetU32(name,pData);
		 pDev->baudRate = data; 
		 //
		 sprintf(name, "UartInfo");
		 SYS_ENV_Get(name, pDev-> info ); 
		 
		 sprintf(name, "DateYear"); 
		 SYS_ENV_GetU32(name,pData);		 
		 pDev -> id.year =  data - 2016;
		 
	     	 sprintf(name, "DateMon"); 
		 SYS_ENV_GetU32(name,pData);		 
		 pDev -> id.mon =  ((data/10 & 0xf) <<4 )|  (data%10  & 0xf);
		 
		 sprintf(name, "DateDay"); 
		 SYS_ENV_GetU32(name,pData);		 
		 pDev -> id.day =  ((data/10 & 0xf) <<4 )|  (data%10  & 0xf);
		 
	  	 sprintf(name, "DateNum"); 
		 SYS_ENV_GetU32(name,pData);
		 //printf("DateNum = %d\r\n",data);
		 WV_U8 * PChar;	
		 PChar = (WV_U8 *) pData;		 	 
		 pDev -> id.num[0] = PChar[2];
		 pDev -> id.num[1] = PChar[1]; 
		 pDev -> id.num[2] = PChar[0]; 
		 
		 pDev -> id.Vender[0] = 'C';
		 pDev -> id.Vender[1] = 'Z';
		 pDev -> id.type = 1;
		 
		 
		 WV_U16 addr; 
		 WV_U16 ver; 
		 addr = 0;
		 HIS_SPI_FpgaRd(addr,&ver); 
		 pDev -> id.ver[0] = ver & 0xff;
		 addr = 0;
		 HIS_SPI_FpgaRd2(addr,&ver);
		 pDev -> id.ver[1] = ver & 0xff;
		 pDev -> id.ver[2] = 3;
		 pDev -> id.random[0] = 0x12; 
		 pDev -> id.random[1] = 0x34;
		 pDev -> id.random[2] = 0x56;
		 pDev -> id.random[3] = 0x78;
		 return WV_SOK;
}
/*******************************************************************
 WV_S32 TSK_UART_SaveConf(TSK_UART_DEV_E  * pDev);
*******************************************************************/

WV_S32 TSK_UART_SaveConf(TSK_UART_DEV_E  * pDev)
{
      
        WV_U32 data ; 
         WV_S8  name[WV_CONF_NAME_MAX_LEN]; 
          sprintf(name, "UartPort");
		 data = pDev->port;  
		 SYS_ENV_SetU32(name,data);
		 printf("save port [%d]\n",data);

		 sprintf(name, "UartRate");
		 data =  pDev->baudRate;  
		 SYS_ENV_SetU32(name,data);
		  printf("save rate [%d]\n",data);
		 sprintf(name, "UartInfo");
		  
		 SYS_ENV_Set(name,pDev->info); 
		 
		 sprintf(name, "DateYear"); 
		 data =  2016 +  pDev -> id.year;
		 SYS_ENV_SetU32(name,data); 
		 
		 
	     sprintf(name, "DateMon"); 
	      data =  (pDev -> id.mon & 0Xf)  +  10* ((pDev -> id.mon>> 4) & 0X0f);
		 SYS_ENV_SetU32(name,data);		 
		 
		 
		 sprintf(name, "DateDay"); 
	      data =  (pDev -> id.day & 0Xf ) +  10*((pDev -> id.day>> 4) & 0X0f );
		 SYS_ENV_SetU32(name,data);	
		 
	     sprintf(name, "DateNum");  
	      WV_U8 * PChar;	
		 PChar = (WV_U8 *)(&data);	
		 PChar[0] = pDev -> id.num[2];
		 PChar[1] = pDev -> id.num[1];
		 PChar[2] = pDev -> id.num[0];
		 PChar[3] = 0; 
		 SYS_ENV_SetU32(name,data); 
		 SYS_ENV_Save();
		 
     return WV_SOK;
}		  		 
/****************************************************************************************

WV_S32 TSK_UART_SetPort();

****************************************************************************************/

WV_S32 TSK_UART_SetPort(WV_U32 port)
{ 
    
	if(port == 1)
	{
		gUartDev.port = 1;
	}
	else
	{
		gUartDev.port = 0; 
	}
	 

	return HIS_IO_UartSel( gUartDev.port); 
	  
}

/****************************************************************************************

WV_S32 TSK_UART_GetPort();

****************************************************************************************/

WV_S32 TSK_UART_GetPort(WV_U32 * pPort)
{
	*pPort  = gUartDev.port;
	return WV_SOK; 
}
/****************************************************************************************

WV_S32 TSK_UART_GetSpeed();

****************************************************************************************/

WV_S32 TSK_UART_GetSpeed(WV_U32 * pBaud)
{
	*pBaud  = gUartDev.baudRate;
	return WV_SOK;    
}


/****************************************************************************************

WV_S32 TSK_UART_Set_Speci_Speed(WV_U32 baud)

****************************************************************************************/
#if 0
WV_S32 TSK_UART_Set_Speci_Speed(WV_U32 baud)
{ 
	
	struct serial_struct ss,ss_set;
	if((ioctl(gUartDev.fd,TIOCGSERIAL,&ss)) < 0 ){

		printf("baud:error to get the serial_struct info\n");
		return -1;
	}
	ss.flags = ASYNC_SPD_CUST;
	ss.custom_divisor = 115200 /baud;
	//ss.custom_divisor = ss.baud_base /baud;
	printf("ss.custom_divisor = %d \n",ss.custom_divisor);

	if((ioctl(gUartDev.fd,TIOCSSERIAL,&ss))<0)
	{
		printf("baud:error to set serial_struct\n");
		return -1;	
	}

	ioctl(gUartDev.fd,TIOCGSERIAL,&ss_set);
	printf("baud:success set baud to %d,custom_divisor=%d,baud_base=%d\n",baud,ss_set.custom_divisor,ss_set.baud_base);
	
}
#endif
WV_S32 TSK_UART_Set_Speci_Speed(WV_U32 baud)
{
	 WV_U32  regAddr_IBRD,regAddr_FBRD,regAddr_LCR_H ,data_IBRD,data_FBRD,data_LCR_H;
	 float temp,temp2,f_baud;
	 f_baud = baud;
	 regAddr_IBRD = 0xf8006024;
	 regAddr_FBRD = 0xf8006028;
	 regAddr_LCR_H = 0xf800602c;
	 temp = 187500.0/f_baud;
	//printf("temp****** = %f \n",temp);
	 data_IBRD = (int)temp;//3000000/(baud*16);
		 temp2 = temp - data_IBRD;
	 //data_FBRD = ((int)(temp*1000)%1000)*64;//b=(int)(x*1000)%1000
	data_FBRD = temp2*64;
	//printf("UART_IBRD[%08x],UART_FBRD[%08x]\n",data_IBRD,data_FBRD);

	HI_SYS_WriteRegister(regAddr_IBRD,data_IBRD);
	HI_SYS_WriteRegister(regAddr_FBRD,data_FBRD);

	HI_SYS_ReadRegister(regAddr_LCR_H,&data_LCR_H);
	HI_SYS_WriteRegister(regAddr_LCR_H,data_LCR_H);
	printf("UART_IBRD[%08x],UART_FBRD[%08x],LCR_H[%08x]\n",data_IBRD,data_FBRD,data_LCR_H);
	return 0;
}
/****************************************************************************************

WV_S32 TSK_UART_SetSpeed();

****************************************************************************************/
WV_S32 TSK_UART_SetSpeed(WV_U32 baud)
{ 
 
	
	WV_S32  ret,flag=0;
	struct  termios opt;
	if(gUartDev.fd < 0 )
		{
		TSK_UART_printf("no UART open !!!"); 
		return WV_EFAIL;
		}
 
	tcgetattr(gUartDev.fd, &opt);
	tcflush(gUartDev.fd, TCIOFLUSH);
	switch( baud)
    {
    case 2400:
        cfsetispeed(&opt, B2400);
        cfsetospeed(&opt, B2400);
         gUartDev.baudRate = 2400;
        break;
    case 4800:
        cfsetispeed(&opt, B4800);
        cfsetospeed(&opt, B4800);
         gUartDev.baudRate = 4800;
        break;
    case 9600:
        cfsetispeed(&opt, B9600);
        cfsetospeed(&opt, B9600);
         gUartDev.baudRate = 9600;
        break;
    case 19200:
	cfsetispeed(&opt, B19200);
	cfsetospeed(&opt, B19200);
	gUartDev.baudRate = 19200;
	break;
    case 38400:
	cfsetispeed(&opt, B38400);
	cfsetospeed(&opt, B38400);
	gUartDev.baudRate = 38400;
	break;   
    case 57600:
        cfsetispeed(&opt, B57600);
        cfsetospeed(&opt, B57600);
        gUartDev.baudRate = 57600;
        break;
	case 115200:
		//printf("\r\nsped set 115200\n");
		cfsetispeed(&opt, B115200);
		cfsetospeed(&opt, B115200);
		gUartDev.baudRate = 115200;
		break;
    default:
        cfsetispeed(&opt, B9600);
        cfsetospeed(&opt, B9600);
        gUartDev.baudRate = 9600;
		flag = 1; 
        break;
    } 
	ret = tcsetattr(gUartDev.fd,TCSANOW,&opt);
	if(ret < 0) 
	{
		 TSK_UART_printf(" UART SetSpeed  error  !!!"); 
	}
	tcflush(gUartDev.fd,TCIOFLUSH);

	if(flag == 1)
	{
		ret = TSK_UART_Set_Speci_Speed(baud);
		if(ret == 0)
		{
			gUartDev.baudRate = baud;
		}
	}

	return ret; 
}

/****************************************************************************************

WV_S32 TSK_UART_Setmode();

****************************************************************************************/
WV_S32 TSK_UART_Setmode(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret;
	struct  termios opt;
	if(pDev-> fd < 0 )
	{
		TSK_UART_printf("no UART open !!!"); 
		return WV_EFAIL;
	}



	tcgetattr(pDev->fd, &opt);
	tcflush(pDev->fd, TCIOFLUSH);
        // opt.c_cflag  |=  CLOCAL | CREAD;
	//opt.c_cflag &= ~CSIZE;
	//opt.c_cflag |= CS8;     //8 bit
	//opt.c_cflag &= ~PARENB; // no pare
	//opt.c_cflag &= ~CSTOPB; //
	if(pDev->stopBit == 1)
	{
		opt.c_cflag &= ~CSTOPB;	 // 1 stop
	}else if(pDev->stopBit == 2)
	{
		opt.c_cflag |= CSTOPB; // 2 stop
	}

	
	//opt.c_oflag  &= ~OPOST;	

	cfmakeraw(&opt);
	//opt.c_lflag |= ECHO;
   	tcflush(pDev->fd,TCIFLUSH); 
	opt.c_cc[VTIME] = 0;
	opt.c_cc[VMIN] = 1;
    
	ret = tcsetattr(pDev->fd,TCSANOW,&opt);
	if(ret < 0)
		 {
		TSK_UART_printf(" UART SetMode  error  !!!"); 
		}
   tcgetattr(pDev->fd, &opt);
   
   TSK_UART_printf(" UART SetMode  VTIME[%d]VMIN[%d]",opt.c_cc[VTIME],opt.c_cc[VMIN]); 

	return ret; 
}

/****************************************************************************************

WV_S32 TSK_UART_RecvCmd();

****************************************************************************************/
WV_S32 TSK_UART_RecvCmd(TSK_UART_DEV_E * pDev,WV_U8  *pBuf,WV_S32 *pLen)
{ 

		//printf("TSK_UART_RecvCmd\n");
		WV_S32  recvLen,i;
		 struct  termios opt;
		if(pDev-> fd < 0 )
		{
			TSK_UART_printf("no UART open !!!"); 
			return WV_EFAIL;
		} 

       /*************************/
        tcgetattr(pDev->fd, &opt); 
        opt.c_cc[VTIME] = 0;
	    opt.c_cc[VMIN] = 1; 
	    tcsetattr(pDev->fd,TCSANOW,&opt); 
	   /*************************/
		recvLen  = read(pDev->fd, pBuf,1); 
		

		if(recvLen != 1)
		{
		     return WV_EFAIL;			
		}
	 /*************************/
        tcgetattr(pDev->fd, &opt); 
        opt.c_cc[VTIME] = 1;
	    opt.c_cc[VMIN] = 0; 
	    tcsetattr(pDev->fd,TCSANOW,&opt); 
	   /*************************/
		
		//printf("pBuf[0] = %d ,%02x\n",pBuf[0],pBuf[0]);

		for(i =1;i< 128 ;i++)
		{
			recvLen  = read(pDev->fd, &(pBuf[i]),1); 
			if(recvLen != 1)
			{
				pDev->timeoutNum ++;
				*pLen = i;//data len
				break;		
			}
			
		} 

//printf recv cmd 

	printf("recv cmd:");
	for(i=0;i<*pLen;i++)
	{
		printf("%02x ",pBuf[i]);
	}
	printf("\n");

	return  WV_SOK;   
}
  
  
/****************************************************************************************

WV_S32 TSK_UART_Send(WV_U8  *pBuf ,WV_U32  len);

****************************************************************************************/

WV_S32 TSK_UART_Send(WV_S8  *pBuf ,WV_U32  len)
{ 

	 WV_S32  ret;
	 if(gUartDev.fd < 0 )
	 {
        TSK_UART_printf("no UART open !!!"); 
		return WV_EFAIL;
	 }
	 ret  = write(gUartDev.fd,pBuf,len); 

	 if(ret == -1)
	 {
        TSK_UART_printf("TSK_UART_Send errro !!!");  
	 }


	return ret; 
}  
/****************************************************************************************

WV_S32 TSK_UART_Init();

****************************************************************************************/

WV_S32 TSK_UART_Init(TSK_UART_DEV_E * pDev)
{ 
	WV_S32 ret = 0; 
	pDev->fd =  open(TSK_UART_DEV_NAME, O_RDWR);
	if(pDev-> fd < 0 )
	{
		TSK_UART_printf("UART open erro!!!"); 
		return WV_EFAIL;
	}
	TSK_UART_SetSpeed(pDev->baudRate);
	TSK_UART_Setmode(pDev);
	TSK_UART_SetPort(pDev->port);

return ret;  
}


/****************************************************************************************

WV_S32 TSK_UART_DeInit();

****************************************************************************************/

WV_S32 TSK_UART_DeInit(TSK_UART_DEV_E * pDev)
{ 
WV_S32 ret = 0; 
 
 ret = 	close(pDev -> fd);
		 
return ret;  
}

/****************************************************************************************

WV_S32 TSK_UART_SetUartSceneCmd(WV_U8 *pData);

****************************************************************************************/
WV_S32 TSK_UART_SetUartSceneCmd(WV_U8 *pData,WV_S32 len)
{
	FILE *fp;
	//open sceneCmd.dat
	fp = fopen(TSK_SCENE_CMDFILE,"wb+");
	if(fp == NULL)
	{
		TSK_UART_printf("fopen sceneCmd.dat error \n"); 
		return -1;
	}
	WV_S32 writeLen;
	writeLen =fwrite(pData,1,len,fp);
	if(writeLen != len)
	{
		TSK_UART_printf("fwrite uart cmd file error \n");
		fclose(fp);	
		return -1;
	}
	
	fclose(fp);
	return 0;	
	
}

/****************************************************************************************

WV_S32 TSK_UART_SceneAck();

****************************************************************************************/
WV_S32 TSK_UART_SceneAck(TSK_UART_DEV_E * pDev,WV_S32 id)
{
	WV_S32 ret;
	ret  = write(pDev->fd,pDev->sceneCmd[id].AckCmd,pDev->sceneCmd[id].AckLen); 
	
	if(ret != pDev->sceneCmd[id].AckLen)
	{
		TSK_UART_printf("TSK_UART ack temp  send info !!!");  
	}
	return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_UART_SceneCmd(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);

******************************************************************************/
WV_S32 TSK_UART_SceneCmd(TSK_UART_DEV_E  *pDev, WV_U8 *pBuf, WV_S32 len)
{
    WV_S32 i, j;

    for (i = 0; i < TSK_UART_SCENE_NUM; i++)
    {
        if (pDev->sceneCmd[i].CmdLen == len)
        {
            for (j = 0; j < len; j++)
            {
                if (pDev->sceneCmd[i].ScenceCmd[j] != pBuf[j])
                {
                    TSK_UART_printf("\nscene cmd error \n");
                    break;
                }
            }
            if (j == len && pDev->sceneCmd[i].CmdEna == 0)
            {
                if (gWagShow == 0x00)
                {
                    NET_UART_InfraredCmd(1);
                    gWagShow = 0x01;
                }
                TSK_SCENE_Change(TSK_SCENE_TYPE_UARTDATA, pDev->sceneCmd[i].SceneId);
                if (pDev->sceneCmd[i].AckEna == 1)
                {
                    //uart ack cmd 
                    TSK_UART_SceneAck(pDev, i);
                }
                pDev->sceneCmd[i].CmdEna = 1;
                break;
            }
        }
    }
    return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_UART_WagCmd(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);

******************************************************************************/
WV_S32 TSK_UART_WagCmd(TSK_UART_DEV_E  *pDev, WV_U8 *pBuf, WV_S32 len)
{
    WV_S32 i, j;

    for (i = 0; i < TSK_UART_WAG_NUM; i++)
    {
        if (pDev->wagCmd[i].CmdLen == len)
        {
            for (j = 0; j < len; j++)
            {
                if (pDev->wagCmd[i].ScenceCmd[j] != pBuf[j])
                {
                    TSK_UART_printf("\nwag cmd error \n");
                    break;
                }
            }
            if (j == len && pDev->wagCmd[i].CmdEna == 0)
            {
                if ((i == 0x00) || (i == 0x01))
                {
                    if (gWagShow != i)
                    {
                        NET_UART_InfraredCmd(i);
                        gWagShow = i;
                    }
                }
                pDev->wagCmd[i].CmdEna = 1;
                break;
            }
        }
    }
    return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_UART_SceneStrlCmd(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);
//具体scenestrl命令的应用：播放/暂停/上个场景/下个场景/音量+/音量-/停止/开机/待机/
******************************************************************************/
WV_S32 TSK_UART_SceneStrlApp(WV_S32 cmd)
{
	WV_printf("TSK_UART_SceneStrlApp[%d]\n",cmd);
	switch (cmd){
		case 0:TSK_SCENE_PlayerPlay();break;
		case 1:TSK_SCENE_PlayerPause();break;
		case 2:TSK_SCENE_PreScene();break;
		case 3:TSK_SCENE_NextScene();break;
		case 4:TSK_SCENE_PlayerVolUp();break;
		case 5:TSK_SCENE_PlayerVolDown();break;
		case 6:TSK_SCENE_PlayerStop();break;
		case 7:TSK_SCENE_StartingUP(TSK_SCENE_TYPE_NETDATA);break;
		case 8:TSK_SCENE_Standby(TSK_SCENE_TYPE_UARTDATA);break;
        case 9:TSK_SCENE_LockScene();break;
        case 10:NET_UART_ProjectorCmd(1);break;
        case 11:NET_UART_ProjectorCmd(0);break;
		default:break;
	}
    return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_UART_SceneStrlCmd(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);

******************************************************************************/
WV_S32 TSK_UART_SceneStrlCmd(TSK_UART_DEV_E  *pDev, WV_U8 *pBuf, WV_S32 len)
{
    WV_S32 i, j;

    for (i = 0; i < TSK_UART_SCENE_CTRLCMD_NUM; i++)
    {
        if (pDev->sceneCtrl[i].CmdLen == len)
        {
            for (j = 0; j < len; j++)
            {
                if (pDev->sceneCtrl[i].ScenceCmd[j] != pBuf[j])
                {
                    TSK_UART_printf("\nwag cmd error \n");
                    break;
                }
            }
            if (j == len )
            {
                TSK_UART_SceneStrlApp(i);
                //pDev->sceneCtrl[i].CmdEna = 1;
                break;
            }
        }
    }
    return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_UART_SceneCheck(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);

******************************************************************************/
WV_S32 TSK_UART_SceneCheck(TSK_UART_DEV_E  *pDev, WV_U8 *pBuf, WV_S32 len)
{
    WV_S32 i, j;

    for (i = 0; i < TSK_UART_SCENE_NUM; i++)
    {
        if (pDev->sceneCmd[i].CmdLen == len)
        {
            for (j = 0; j < len; j++)
            {
                if (pDev->sceneCmd[i].ScenceCmd[j] != pBuf[j])
                {
                    break;
                }
            }
            if ((j == len) && (pDev->sceneCmd[i].CmdEna == 0))
            {
                return WV_SOK;
            }
        }
    }
    if (i == TSK_UART_SCENE_NUM)
    {
        for (i = 0; i < TSK_UART_SCENE_NUM; i++)
        {
            if (pDev->sceneCmd[i].CmdLen == len)
            {
                for (j = 0; j < len; j++)
                {
                    if (pDev->sceneCmd[i].ScenceCmd[j] != pBuf[j])
                    {
                        break;
                    }
                }
                if (j == len && pDev->sceneCmd[i].CmdEna == 1)
                {
                    pDev->sceneCmd[i].CmdEna = 0;
                }
            }
        }
    }
    return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_UART_WagCheck(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);

******************************************************************************/
WV_S32 TSK_UART_WagCheck(TSK_UART_DEV_E  *pDev, WV_U8 *pBuf, WV_S32 len)
{
    WV_S32 i, j;

    for (i = 0; i < TSK_UART_WAG_NUM; i++)
    {
        if (pDev->wagCmd[i].CmdLen == len)
        {
            for (j = 0; j < len; j++)
            {
                if (pDev->wagCmd[i].ScenceCmd[j] != pBuf[j])
                {
                    break;
                }
            }
            if ((j == len) && (pDev->wagCmd[i].CmdEna == 0))
            {
                return WV_SOK;
            }
        }
    }
    if (i == TSK_UART_WAG_NUM)
    {
        for (i = 0; i < TSK_UART_WAG_NUM; i++)
        {
            if (pDev->wagCmd[i].CmdLen == len)
            {
                for (j = 0; j < len; j++)
                {
                    if (pDev->wagCmd[i].ScenceCmd[j] != pBuf[j])
                    {
                        break;
                    }
                }
                if (j == len && pDev->wagCmd[i].CmdEna == 1)
                {
                    pDev->wagCmd[i].CmdEna = 0;
                }
            }
        }
    }
    return WV_SOK;
}
/******************************************************************************

WV_S32 TSK_UART_WagCheck(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf,WV_S32 len);

******************************************************************************/
WV_S32 TSK_UART_SceneCtrlCheck(TSK_UART_DEV_E  *pDev, WV_U8 *pBuf, WV_S32 len)
{
    WV_S32 i, j;

    for (i = 0; i < TSK_UART_WAG_NUM; i++)
    {
        if (pDev->sceneCtrl[i].CmdLen == len)
        {
            for (j = 0; j < len; j++)
            {
                if (pDev->sceneCtrl[i].ScenceCmd[j] != pBuf[j])
                {
                    break;
                }
            }
            if ((j == len) && (pDev->sceneCtrl[i].CmdEna == 0))
            {
                return WV_SOK;
            }
        }
    }
    if (i == TSK_UART_WAG_NUM)
    {
        for (i = 0; i < TSK_UART_WAG_NUM; i++)
        {
            if (pDev->sceneCtrl[i].CmdLen == len)
            {
                for (j = 0; j < len; j++)
                {
                    if (pDev->sceneCtrl[i].ScenceCmd[j] != pBuf[j])
                    {
                        break;
                    }
                }
                if (j == len && pDev->sceneCtrl[i].CmdEna == 1)
                {
                    pDev->sceneCtrl[i].CmdEna = 0;
                }
            }
        }
    }
    return WV_SOK;
}

/****************************************************************************************

WV_S32 TSK_UART_GetUartMode(TSK_UART_DEV_E  *pDev);

****************************************************************************************/
WV_S32 TSK_UART_GetUartMode(TSK_UART_DEV_E  *pDev)
{
	return 0;
	
}

/******************************************************************************
void *TSK_UART_Proc(Void * prm);
******************************************************************************/
void * TSK_UART_Proc ( void * prm)
{
	TSK_UART_DEV_E  * pDev; 
	WV_S32 ret,cmdLen;
	WV_U8 recvBuf[TASK_UART_MAX_LEN];
	
	pDev = (TSK_UART_DEV_E  *) prm;

	pDev-> open  = 1;
	pDev-> close  = 0;  
	TSK_UART_printf("TSK_UART_begin");
	TSK_UART_Send("test1",5);		
	while(pDev -> open == 1)
	{  
		struct  timeval    tv;
		struct  timezone   tz;
		gettimeofday(&tv,&tz);
		pDev ->runTime = tv.tv_sec; 
				
		ret += TSK_UART_RecvCmd(pDev, recvBuf,&cmdLen);
		ret += TSK_UART_SceneCheck(pDev,recvBuf,cmdLen);
		ret += TSK_UART_SceneCmd(pDev,recvBuf,cmdLen);
        ret += TSK_UART_WagCheck(pDev, recvBuf, cmdLen);
        ret += TSK_UART_WagCmd(pDev, recvBuf, cmdLen);
		//ret += TSK_UART_SceneCtrlCheck(pDev, recvBuf, cmdLen);
		ret += TSK_UART_SceneStrlCmd(pDev, recvBuf, cmdLen);
		pDev ->runNum ++;  
		if(ret != WV_SOK)
		{ 
			pDev -> rcvEroNum ++;
			continue; 	 
		} 
	   
    }  
 pDev-> open  = 0;
 pDev-> close = 1;
 return NULL;  
} 


/****************************************************************************

WV_S32 TSK_UART_GetDev(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_UART_GetDev(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{
 prfBuff += sprintf(prfBuff,"port      : %d\r\n",gUartDev.port); 
 prfBuff += sprintf(prfBuff,"speed     : %d\r\n",gUartDev.baudRate); 
 prfBuff += sprintf(prfBuff,"info      : %s\r\n",gUartDev.info);
 prfBuff += sprintf(prfBuff,"Vender    : %c%c\r\n",gUartDev.id.Vender[0],gUartDev.id.Vender[1]); 
 prfBuff += sprintf(prfBuff,"version   : %d.%d.%d\r\n",gUartDev.id.ver[0],gUartDev.id.ver[1],gUartDev.id.ver[2]); 
 prfBuff += sprintf(prfBuff,"ID        : %d.%d.%d.%d\r\n",gUartDev.id.year+2016,gUartDev.id.mon,gUartDev.id.day,gUartDev.id.num[2] |( gUartDev.id.num[1]<<8 ) | (gUartDev.id.num[0]<<16));
 prfBuff += sprintf(prfBuff,"runTime   : %d\r\n",gUartDev.runTime);
 prfBuff += sprintf(prfBuff,"runNum    : %d\r\n",gUartDev.runNum);
 prfBuff += sprintf(prfBuff,"timeoutNum: %d\r\n",gUartDev.timeoutNum);
 prfBuff += sprintf(prfBuff,"rcvEroNum : %d\r\n",gUartDev.rcvEroNum);
 prfBuff += sprintf(prfBuff,"procEroNum: %d\r\n",gUartDev.procEroNum);
  
  
 return 0;
} 
 

/****************************************************************************

WV_S32 TSK_UART_SetDev(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_UART_SetDev(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{
  WV_U32 num; 
  WV_S32  ret;  
  
  if(argc<1) 
  {
   	prfBuff += sprintf(prfBuff,"set uart cmd  : port  speed  info  save\r\n");
    return 0;
  }
  
  //  cmd port
	if(strcmp(argv[0],"port") ==0)
	{
		if(argc<2) 
			{
			prfBuff += sprintf(prfBuff,"set uart port  num(0/1) \r\n");
			return 0;
			}
		ret =  WV_STR_S2v(argv[1],&num);
		if(ret != WV_SOK)
			{
			prfBuff += sprintf(prfBuff,"<Num> erro!\r\n"); 
			}
		prfBuff += sprintf(prfBuff,"set uart to  %d\r\n",num); 
		TSK_UART_SetPort(num);
		TSK_UART_SaveConf(&gUartDev);
		
		return 0;	  
	}
	
	
	if(strcmp(argv[0],"speed") ==0)
	{
		if(argc<2) 
			{
			prfBuff += sprintf(prfBuff,"set uart speed  baudRate: 2400/4800/9600/19200/38400/57600/115200 \r\n");
			return 0;
			}
		ret =  WV_STR_S2v(argv[1],&num);
		if(ret != WV_SOK)
			{
			prfBuff += sprintf(prfBuff,"<Num> erro!\r\n"); 
			}
		prfBuff += sprintf(prfBuff,"set uart speed to  %d\r\n",num);
		gUartDev.baudRate = num;
		TSK_UART_SetSpeed(gUartDev.baudRate);
		return 0;	  
	 }
	
   if(strcmp(argv[0],"info") ==0)
	{
		if(argc<2) 
			{
			prfBuff += sprintf(prfBuff,"set uart info  <infoAgs> \r\n");
			return 0;
			}
		 
		 
		prfBuff += sprintf(prfBuff,"set uart info :%s\r\n",argv[1]); 
		strcpy(gUartDev.info,argv[1]);  
		return 0;	  
	 }
   if(strcmp(argv[0],"id") ==0)
	{
		if(argc<5) 
			{
			prfBuff += sprintf(prfBuff,"set uart id  <year> <mon> <day> <num>\r\n");
			return 0;
			}
		WV_STR_S2v(argv[1],&num);
		gUartDev.id.year =   num -2016;
		WV_STR_S2v(argv[2],&num);
		gUartDev.id.mon =  (((num/10) & 0x0f)<<4) | ((num%10) & 0x0f)  ; 
		
		 WV_STR_S2v(argv[3],&num);
		gUartDev.id.day =  (((num/10) & 0x0f)<<4) | ((num%10) & 0x0f)  ; 
		
		 WV_STR_S2v(argv[4],&num);
		gUartDev.id.num[0] = (num >> 16) & 0xff; 
		gUartDev.id.num[1] = (num >> 8) & 0xff; 
		gUartDev.id.num[2] = num & 0xff;  
		prfBuff += sprintf(prfBuff,"set id : %d.%d.%d.%d\r\n",gUartDev.id.year+2016,gUartDev.id.mon,gUartDev.id.day,gUartDev.id.num[2] |( gUartDev.id.num[1]<<8 ) | (gUartDev.id.num[0]<<16));
		return 0;	  
	 }
	
 
   if(strcmp(argv[0],"save") ==0)
	{
		 
		prfBuff += sprintf(prfBuff,"save uart config \r\n"); 
		TSK_UART_SaveConf(&gUartDev);  
		return 0;	  
	 }	 
	
 return WV_SOK;
 	
}

/********************************************************

WV_S32 TSK_UART_AnalyzeSceneCmd(WV_S8 *cmd)

*********************************************************/
WV_S32 TSK_UART_AnalyzeSceneCmd(WV_S8 *buf)
{
    //char buf[128]="0f | F1 02 03 04 05 | 06 07 08 9f";
    int a, id, i, k = 0, j = 0;
    char temp[3];

    id = 0xFF;
    for (i = 0; i < TSK_UART_SCENE_NUM; i++)
    {
        if (gUartDev.sceneCmd[i].SceneId == 0xFF)
        {
            id = i;
            break;
        }
    }
    if (id >= TSK_UART_SCENE_NUM)
    {
        //printf("get scene id error \n");
        return -1;
    }
    memset(temp, 0, sizeof(temp));
    //************get scene id****************
    for (i = 0; i < strlen(buf); i++)
    {
        if (buf[i] != ' ')
        {
            if (buf[i] == '|')
            {
                break;
            }
            temp[j] = buf[i];
            j++;
            if (j >= 3)
            {
                return -1;
            }
        }
    }
    sscanf(temp, "%x", &a);
    gUartDev.sceneCmd[id].SceneId = a;
    //*****************get scene cmd ********************
    memset(temp, 0, sizeof(temp));
    j = 0;
    for (i = i + 1; i < strlen(buf); i++)
    {
        if (buf[i] == '|')
        {
            gUartDev.sceneCmd[id].CmdLen = k;
            gUartDev.sceneCmd[id].AckEna = 1;
            break;
        }
        if (buf[i] != ' ' && buf[i] != 0x0a)
        {
            temp[j] = buf[i];
            j++;
            if (j == 2)
            {
                j = 0;
                sscanf(temp, "%x", &a);
                gUartDev.sceneCmd[id].ScenceCmd[k] = (WV_U8)a;//get sceneCmd
                k++;
                memset(temp, 0, sizeof(temp));
            }
        }
        if (i == strlen(buf) - 1)
        {
            gUartDev.sceneCmd[id].CmdLen = k;
            gUartDev.sceneCmd[id].AckEna = 0;
        }
    }
    //******************get ack cmd ************************
    k = 0;
    j = 0;
    for (i = i + 1; i < strlen(buf); i++)
    {
        if (buf[i] != ' ')
        {
            temp[j] = buf[i];
            j++;
            if (j == 2)
            {
                j = 0;
                sscanf(temp, "%x", &a);
                gUartDev.sceneCmd[id].AckCmd[k] = (WV_U8)a;//get sceneCmd
                k++;
                memset(temp, 0, sizeof(temp));
            }
        }
        if (i == strlen(buf) - 1)
        {
            gUartDev.sceneCmd[id].AckLen = k - 1;
        }
    }
    return 0;
}

/********************************************************

WV_S32 TSK_UART_AnalyzeWagCmd(WV_S8 *cmd)

*********************************************************/
WV_S32 TSK_UART_AnalyzeWagCmd(WV_S8 *buf)
{
    //char buf[128]="0f | F1 02 03 04 05 | 06 07 08 9f";
    int a, id, i, k = 0, j = 0;
    char temp[3];

    memset(temp, 0, sizeof(temp));
    //************get wag id****************
    for (i = 0; i < strlen(buf); i++)
    {
        if (buf[i] != ' ')
        {
            if (buf[i] == '|')
            {
                break;
            }
            temp[j] = buf[i];
            j++;
            if (j >= 3)
            {
                return -1;
            }
        }
    }
    sscanf(temp, "%x", &a);
    id = a;
    if (id >= TSK_UART_WAG_NUM)
    {
        return -1;
    }
    //*****************get wag cmd ********************
    memset(temp, 0, sizeof(temp));
    j = 0;
    for (i = i + 1; i < strlen(buf); i++)
    {
        if (buf[i] == '|')
        {
            gUartDev.wagCmd[id].CmdLen = k;
            gUartDev.wagCmd[id].AckEna = 1;
            break;
        }
        if (buf[i] != ' ' && buf[i] != 0x0a)
        {
            temp[j] = buf[i];
            j++;
            if (j == 2)
            {
                j = 0;
                sscanf(temp, "%x", &a);
                gUartDev.wagCmd[id].ScenceCmd[k] = (WV_U8)a;//get sceneCmd
                k++;
                memset(temp, 0, sizeof(temp));
            }
        }
        if (i == strlen(buf) - 1)
        {
            gUartDev.wagCmd[id].CmdLen = k;
            gUartDev.wagCmd[id].AckEna = 0;
        }
    }
    return 0;
}

/********************************************************

WV_S32 TSK_UART_AnalyzeSceneCtrl(WV_S8 *buf)

*********************************************************/
WV_S32 TSK_UART_AnalyzeSceneCtrl(WV_S8 *buf)
{
    //char buf[128]="0f | F1 02 03 04 05 | 06 07 08 9f";
    int a, id, i, k = 0, j = 0;
    char temp[3];

    memset(temp, 0, sizeof(temp));
    //************get wag id****************
    for (i = 0; i < strlen(buf); i++)
    {
        if (buf[i] != ' ')
        {
            if (buf[i] == '|')
            {
                break;
            }
            temp[j] = buf[i];
            j++;
            if (j >= 3)
            {
                return -1;
            }
        }
    }
    sscanf(temp, "%x", &a);
    id = a;
    if (id >TSK_UART_SCENE_CTRLCMD_NUM)
    {
        return -1;
    }
    //*****************get wag cmd ********************
    memset(temp, 0, sizeof(temp));
    j = 0;
    for (i = i + 1; i < strlen(buf); i++)
    {
        if (buf[i] == '|')
        {
            gUartDev.sceneCtrl[id].CmdLen = k;
            gUartDev.sceneCtrl[id].AckEna = 1;
            break;
        }
        if (buf[i] != ' ' && buf[i] != 0x0a)
        {
            temp[j] = buf[i];
            j++;
            if (j == 2)
            {
                j = 0;
                sscanf(temp, "%x", &a);
                gUartDev.sceneCtrl[id].ScenceCmd[k] = (WV_U8)a;//get sceneCmd
                k++;
                memset(temp, 0, sizeof(temp));
            }
        }
        if (i == strlen(buf) - 1)
        {
            gUartDev.sceneCtrl[id].CmdLen = k;
            gUartDev.sceneCtrl[id].AckEna = 0;
        }
    }

#if 0
	for(i=0;i<TSK_UART_SCENE_CTRLCMD_NUM;i++)
	{
		if(gUartDev.sceneCtrl[i].CmdLen >0){
			WV_printf("[%d]=",i);
			for(j=0;j<gUartDev.sceneCtrl[i].CmdLen;j++){
				WV_printf("%02x ",gUartDev.sceneCtrl[i].ScenceCmd[j]);
			}
			WV_printf("\n");
		}
	}

#endif


    return 0;
}
/********************************************************

WV_S32 TSK_UART_SetSerialConf(WV_S8 * buf);

*********************************************************/ 
WV_S32 TSK_UART_SetSerialConf(WV_S8 * buf)
{
	WV_S8 name[64];
	WV_U32 data=0;
	WV_S32 i,j=0;
	memset(name,0,sizeof(name));
	for(i=0;i<64;i++)
	{
		if(buf[i] == '#')
			return 0;
		if(buf[i] == ' ')
			continue;
		if(buf[i] == '=')
			break;
		 name[j] = buf[i];
		j++;
	}
	
	i++;	
	//sscanf(&buf[i],"%d",&data);
	data = atoi(&buf[i]);
	if(strcmp(name,"Mode") == 0)
	{	
		//printf("********SetSerialConf [%s]=%d *************\n ",name,data);	
		gUartDev.port = data;
	}
	else if(strcmp(name,"BaudRate") == 0)
	{
		//printf("********SetSerialConf [%s]=%d *************\n ",name,data);
		gUartDev.baudRate = data;
	}
	else if(strcmp(name,"DataBit") == 0)
	{
		//printf("********SetSerialConf [%s]=%d *************\n ",name,data);
		gUartDev.dataBit = data;
	}
	else if(strcmp(name,"StopBit") == 0)
	{
		//printf("********SetSerialConf [%s]=%d *************\n ",name,data);
		gUartDev.stopBit = data;
	}	else if(strcmp(name,"WindowMode") == 0)
	{
		//printf("********SetSerialConf [%s]=%d *************\n ",name,data);
		gUartDev.windowMode = data;
	}	else if(strcmp(name,"OpenDev") == 0)
	{
		//printf("********SetSerialConf [%s]=%d *************\n ",name,data);
		gUartDev.openDev = data;
	}else if(strcmp(name,"OpenProjector") == 0)
	{
		//printf("********SetSerialConf [%s]=%d *************\n ",name,data);
		gUartDev.openProjector = data;
	}else if(strcmp(name,"TypeRound") == 0)
	{
		//printf("********SetSerialConf [%s]=%d *************\n ",name,data);
        if(data > 4) data = 0;
		gUartDev.typeRound = data;
	}

	printf("********SetSerialConf [%s]=%d *************\n ",name,data);
	return 0;
	
}
/********************************************************

WV_S32 TSK_UART_GetTypeRound()
查询视频循环播放，是否按类别进行循环，还是单曲循环
*********************************************************/
WV_S32 TSK_UART_GetTypeRound()
{
    return gUartDev.typeRound;
}

/********************************************************

WV_S32 TSK_UART_GetSceneCmd(TSK_UART_DEV_E  * pDev)

*********************************************************/
WV_S32 TSK_UART_GetSceneCmd(TSK_UART_DEV_E  * pDev)
{
    WV_S8 buf[1024];
    WV_S32 i, ret;

    ret = access(TSK_SCENE_CMDFILE, W_OK);
    if (ret != 0)
    {
        WV_printf("access file [%s] error\n", TSK_SCENE_CMDFILE);
        return -1;
    }
    for (i = 0; i < TSK_UART_SCENE_NUM; i++)
    {
        pDev->sceneCmd[i].SceneId = 0xFF;
    }

    FILE *fp;
    //open sceneCmd.dat
    fp = fopen(TSK_SCENE_CMDFILE, "rb+");
    if (fp == NULL)
    {
        WV_printf("fopen file [%s] error\n", TSK_SCENE_CMDFILE);
        return -1;
    }
    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] == '#')
        {
            continue;
        }
        if (buf[0] == '<')
        {
            if (strncmp(buf, "<serialconf>", 12) == 0)
            {
                while (fgets(buf, sizeof(buf), fp))
                {
                    if (buf[0] == '#')
                    {
                        continue;
                    }
                    if (buf[0] == '<')
                    {
                        break;
                    }
                    TSK_UART_SetSerialConf(buf);
                }
                break;
            }
        }
    }
    //get scene cmd 
    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] == '#')
        {
            continue;
        }
        if (buf[0] == '<')
        {
            break;
        }
        for (i = 0; i < 1024; i++)
        {
            if (buf[i] == 0x0d)
            {
                buf[i] = 0x0a;
            }
        }
        TSK_UART_AnalyzeSceneCmd(buf);
    }
    fclose(fp);
    ////////////////////////////////////////
	fp = NULL;
    fp = fopen(TSK_SCENE_CMDFILE, "rb+");
    if (fp == NULL)
    {
        WV_printf("fopen file [%s] error\n", TSK_SCENE_CMDFILE);
        return -1;
    }
    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] == '#')
        {
            continue;
        }
        if (buf[0] == '<')
        {
            if (strncmp(buf, "<wagconf>", 9) == 0)
            {
                while (fgets(buf, sizeof(buf), fp))
                {
                    if (buf[0] == '#')
                    {
                        continue;
                    }
                    if (buf[0] == '<')
                    {
                        break;
                    }
                    for (i = 0; i < 1024; i++)
                    {
                        if (buf[i] == 0x0d)
                        {
                            buf[i] = 0x0a;
                        }
                    }
                    TSK_UART_AnalyzeWagCmd(buf);
                }
                break;
            }
        }
    }
    fclose(fp);
	//获取场景控制命令/播放/暂停/上个场景/下个场景/音量+/音量-/停止/开机/待机/
    fp = NULL;
    fp = fopen(TSK_SCENE_CMDFILE, "rb+");
    if (fp == NULL)
    {
        WV_printf("fopen file [%s] error\n", TSK_SCENE_CMDFILE);
        return -1;
    }
    while (fgets(buf, sizeof(buf), fp))
    {
        if (buf[0] == '#')
        {
            continue;
        }
        if (buf[0] == '<')
        {
            if (strncmp(buf, "<sceneCtrl>", 11) == 0)
            {
                while (fgets(buf, sizeof(buf), fp))
                {
                    if (buf[0] == '#')
                    {
                        continue;
                    }
                    if (buf[0] == '<')
                    {
                        break;
                    }
                    for (i = 0; i < 1024; i++)
                    {
                        if (buf[i] == 0x0d)
                        {
                            buf[i] = 0x0a;
                        }
                    }
                    TSK_UART_AnalyzeSceneCtrl(buf);
                }
                break;
            }
        }
    }
    fclose(fp);

    return 0;
}
/********************************************************

WV_S32 TSK_UART_SetLoopConf(WV_S8 *buf)

*********************************************************/ 
WV_S32 TSK_UART_SetLoopConf(WV_S8 *buf)
{


	WV_S8 name[64];
	WV_U32 data=0;
	WV_S32 i,j=0;
	memset(name,0,sizeof(name));
	for(i=0;i<64;i++)
	{
		if(buf[i] == '#')
			return 0;
		if(buf[i] == ' ')
			continue;
		if(buf[i] == '=')
			break;
		 name[j] = buf[i];
		j++;
	}
	
	i++;	
	//sscanf(&buf[i],"%d",&data);
	data = atoi(&buf[i]);
	if(strcmp(name,"LoopScene") == 0)
	{	
		printf("********loopConf [%s]=%d *************\n ",name,data);	
		gSceneLoop.loopEna = data;
	}
	else if(strcmp(name,"LoopTime") == 0)
	{
		printf("********loopConf [%s]=%d *************\n ",name,data);
		gSceneLoop.loopTime = data;
	}

	return 0;

}
/********************************************************

WV_S32 TSK_UART_GetSceneLoop(TSK_UART_DEV_E  * pDev)

*********************************************************/ 
WV_S32 TSK_UART_GetSceneLoop()
{

	gSceneLoop.loopEna = 0 ;
	gSceneLoop.loopTime = 0;

	WV_S8 buf[1024];
	WV_S32 i,ret;
	ret = access(TSK_SCENE_CMDFILE,W_OK);

	if(ret != 0){
		WV_printf("access file [%s] error\n",TSK_SCENE_CMDFILE);
		return -1;
	}

	FILE *fp;
	//open sceneCmd.dat
	fp = fopen(TSK_SCENE_CMDFILE,"rb+");
	if(fp == NULL)
	{
		WV_printf("fopen file [%s] error\n",TSK_SCENE_CMDFILE);
		return -1;
	}
	

	while(fgets(buf,sizeof(buf),fp))
	{

		for(i=0;i<1024;i++)
		{
			if(buf[i] == 0x0d)
			{
				buf[i] = 0x0a;			
			}
		}

		if(buf[0] == '#')
			continue;
		if(buf[0] == '<')
		{
			if(strncmp(buf,"<loop>",6) == 0)
			{
				while(fgets(buf,sizeof(buf),fp))
				{
					if(buf[0] == '#')
						continue;
					if(buf[0] == '<')
						break;
					TSK_UART_SetLoopConf(buf);
				}
				break;
			} 
			
		}

	}

	fclose(fp);
	return 0;
}
/*******************************************************************
void * TSK_UART_Loop(void * prm);
*******************************************************************/
void * TSK_UART_Loop(void * prm)
{
	

	TSK_UART_LOOP_E  * pDev; 
	WV_S32 i,j;
	pDev = (TSK_UART_LOOP_E  *) prm;
	pDev-> open  = 1;
	pDev-> close  = 0;  
    WV_S32 sceneChange=1;
	while(pDev -> open == 1)
	{  
		if(gSceneLoop.loopEna == 1 && gSceneLoop.loopTime > 0)
		{
			for(i=0;i<TSK_UART_SCENE_NUM;i++)
			{
                if(sceneChange == 1)
                {
                    for(j=0;j<gSceneLoop.loopTime;j++)
                    {
                        sleep(60);
                    }
                }

                if(TSK_SCENE_Change(TSK_SCENE_TYPE_NETDATA,i) != 0){
                    sceneChange =0;
                    continue;
                }else{
                    sceneChange =1;
                }
			}
		}
		sleep(1);
		continue;
	   
    }

 pDev-> open  = 0;
 pDev-> close = 1;
 return NULL;
}
  
/****************************************************************************************

WV_S32 TSK_UART_SceneLoopOpen();

****************************************************************************************/
WV_S32 TSK_UART_SceneLoopOpen()
{ 
	WV_S32 ret = 0;  
	TSK_UART_printf("TSK_UART_SceneLoopOpen()");  

	//WV_CMD_Register("get","tty","set uart  num ",TSK_UART_GetDev); 
	memset(&gSceneLoop,0,sizeof(gSceneLoop));
	TSK_UART_GetSceneLoop();
	if(gSceneLoop.loopEna == 0){
		return WV_SOK;
	}
	WV_RET_ADD( WV_THR_Create(&gSceneLoop.thrHndl, TSK_UART_Loop , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gSceneLoop),ret);
	return WV_SOK;
}

/****************************************************************************************

WV_S32 TSK_UART_SceneLoopClose();

****************************************************************************************/
WV_S32 TSK_UART_SceneLoopClose()
{

	WV_S32 ret = 0;
   if(gSceneLoop.open == 1)
   {
   	  gSceneLoop.open = 0;
   	  while(gSceneLoop.close == 1) ;
      WV_CHECK_RET( WV_THR_Destroy(&gSceneLoop.thrHndl));	
   }
   
 return ret;  

}

/****************************************************************************************

WV_S32 TSK_UART_Open();

****************************************************************************************/
WV_S32 TSK_UART_Open()
{ 
	WV_S32 ret = 0;  
	TSK_UART_printf("TSK_UART_Open()");  
	WV_CMD_Register("set","uart","set uart  config",TSK_UART_SetDev); 
	WV_CMD_Register("get","uart","set uart  stat",TSK_UART_GetDev); 
	//WV_CMD_Register("get","tty","set uart  num ",TSK_UART_GetDev); 
	memset(&gUartDev,0,sizeof(gUartDev));
	TSK_UART_RegisterConf();
	TSK_UART_GetConf(&gUartDev);
	//get cmd conf ./env/sceneCmd.dat	
	TSK_UART_GetSceneCmd(&gUartDev);
	TSK_UART_Init(&gUartDev); 

	WV_RET_ADD( WV_THR_Create(&gUartDev.thrHndl, TSK_UART_Proc , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gUartDev),ret);
	 
	return ret;  
}
 
/****************************************************************************************

WV_S32  TSK_UART_Close();

****************************************************************************************/
WV_S32  TSK_UART_Close()
{ 

	WV_S32 ret = 0;
   if(gUartDev.open == 1)
   {
   	  gUartDev.open = 0;
   	  while(gUartDev.close == 1) ;
      WV_CHECK_RET( WV_THR_Destroy(&gUartDev.thrHndl));	
   }
   TSK_UART_DeInit(&gUartDev);  
 return ret;  
}


