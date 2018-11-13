#include "tsk_uart.h"
#include "sys_env.h"
#include "his_spi.h"
#include "tsk_player.h"
#include "tsk_scene.h"
#include "wv_telnet.h"
#include "his_gpio.h"
#include "his_temperature.h"
#include "his_reg.h"
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
#define TSK_UART_DEBUG_MODE 
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
//TSK_UART_SCENE_CMD_S gSceneCmd[16];
WV_U16 const gcausCrcTable[256] =
{
	0, 4489, 8978, 12955, 17956, 22445, 25910, 29887,
	35912, 40385, 44890, 48851, 51820, 56293, 59774, 63735,
	4225, 264, 13203, 8730, 22181, 18220, 30135, 25662,
	40137, 36160, 49115, 44626, 56045, 52068, 63999, 59510,
	8450, 12427, 528, 5017, 26406, 30383, 17460, 21949,
	44362, 48323, 36440, 40913, 60270, 64231, 51324, 55797,
	12675, 8202, 4753, 792, 30631, 26158, 21685, 17724,
	48587, 44098, 40665, 36688, 64495, 60006, 55549, 51572,
	16900, 21389, 24854, 28831, 1056, 5545, 10034, 14011,
	52812, 57285, 60766, 64727, 34920, 39393, 43898, 47859,
	21125, 17164, 29079, 24606, 5281, 1320, 14259, 9786,
	57037, 53060, 64991, 60502, 39145, 35168, 48123, 43634,
	25350, 29327, 16404, 20893, 9506, 13483, 1584, 6073,
	61262, 65223, 52316, 56789, 43370, 47331, 35448, 39921,
	29575, 25102, 20629, 16668, 13731, 9258, 5809, 1848,
	65487, 60998, 56541, 52564, 47595, 43106, 39673, 35696,
	33800, 38273, 42778, 46739, 49708, 54181, 57662, 61623,
	2112, 6601, 11090, 15067, 20068, 24557, 28022, 31999,
	38025, 34048, 47003, 42514, 53933, 49956, 61887, 57398,
	6337, 2376, 15315, 10842, 24293, 20332, 32247, 27774,
	42250, 46211, 34328, 38801, 58158, 62119, 49212, 53685,
	10562, 14539, 2640, 7129, 28518, 32495, 19572, 24061,
	46475, 41986, 38553, 34576, 62383, 57894, 53437, 49460,
	14787, 10314, 6865, 2904, 32743, 28270, 23797, 19836,
	50700, 55173, 58654, 62615, 32808, 37281, 41786, 45747,
	19012, 23501, 26966, 30943, 3168, 7657, 12146, 16123,
	54925, 50948, 62879, 58390, 37033, 33056, 46011, 41522,
	23237, 19276, 31191, 26718, 7393, 3432, 16371, 11898,
	59150, 63111, 50204, 54677, 41258, 45219, 33336, 37809,
	27462, 31439, 18516, 23005, 11618, 15595, 3696, 8185,
	63375, 58886, 54429, 50452, 45483, 40994, 37561, 33584,
	31687, 27214, 22741, 18780, 15843, 11370, 7921, 3960
};

static TSK_UART_DEV_E gUartDev;
static WV_S32 gWagShow = -1;
/*******************************************************************

WV_U16 TSK_UART_CRC16(const WV_U8 *pChar,WV_S32 len,WV_U16 crc);

*******************************************************************/
WV_U16 TSK_UART_CRC16(const WV_U8 *pChar,WV_S32 len,WV_U16 crc)
{
	WV_S32 i = 0;
	WV_U16 usTmp;
	for(i = 0;i<len;i++)
	{
		usTmp = gcausCrcTable[((*(pChar + i)) & 0x00ff) ^ (crc & 0x00ff)];
		crc = ((usTmp & 0xff00) | ((usTmp & 0x00ff) ^ ((crc & 0xff00) >> 8)));
	}

	return crc;
}

/*******************************************************************

WV_U16 TSK_UART_PackData(const WV_U8 *pSrc,WV_U32 len,WV_U8 *pDes);

*******************************************************************/
WV_U16 TSK_UART_PackData(const WV_S8 *pSrc,WV_U32 len,WV_S8 *pDes)
{
	WV_U16 crc;
	WV_U16 desLen = 0;
	WV_U8 *p;
	WV_S32 i;
	p = pDes;
	p[0] = 0x7e;
	p++;
	desLen++;
	for(i=0;i<len;i++){
		if(pSrc[i]==0x7e){
			p[0] = 0x7d;
			p[1] = 0x5e;
			p+=2;
			desLen+=2;
		}else if(pSrc[i] == 0x7d){
			p[0] = 0x7d;
			p[1] = 0x5d;
			p+=2;
			desLen+=2;
		}else{
			p[0] = pSrc[i];
			p++;
			desLen++;
		}
	}

	crc = TSK_UART_CRC16(pSrc,desLen-1,0xffff);
	crc^=0xffff;

	p[0] = crc & 0xff;
	if(p[0] == 0x7e){
		p[0] = 0x7d;
		p[1] = 0x5e;
		p+=2;
		desLen+=2;
	}else if(p[0] == 0x7d){
		p[0] = 0x7d;
		p[1] = 0x5d;
		p+=2;
		desLen+=2;
	}else{
		p++;
		desLen++;
	}
	
	p[0] = (crc>>8) & 0xff;
	if(p[0] == 0x7e){
		p[0] = 0x7d;
		p[1] = 0x5e;
		p+=2;
		desLen+=2;
	}else if(p[0] == 0x7d){
		p[0] = 0x7d;
		p[1] = 0x5d;
		p+=2;
		desLen+=2;
	}else{
		p++;
		desLen++;
	}
	p[0] = 0x7e;
	desLen++;
	//TSK_UART_printf("desLen [%d]\n",desLen);
	return desLen;
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

WV_S32 TSK_UART_RecvCheck();

****************************************************************************************/

WV_S32 TSK_UART_RecvCheck(WV_U8 *pBuf,WV_U32 len)
{
	WV_S32 i;
	WV_U16 crc;

	//printf("recv:\n");

	for(i=0;i<len;i++){
		TSK_UART_printf("Buf[%d]=[%02x]\n",i,pBuf[i]);
	}

	if(pBuf[0] != 0x7E){
		return WV_EFAIL;
	}

	if(pBuf[len-1] != 0x7E){
			return WV_EFAIL;
	}
	
	crc = TSK_UART_CRC16(&(pBuf[1]),len-2,0xffff);
	//TSK_UART_printf("crc = [%04x]",crc);
	if(crc !=  0xf0b8)
	{
		return WV_EFAIL;
	}

	if((pBuf[1] != 0x00) && (pBuf[1] != 0x09))
	{
		return WV_EFAIL;
	}
		return WV_SOK;
}

/****************************************************************************************

WV_S32 TSK_UART_RecvCmd();

****************************************************************************************/
WV_S32 TSK_UART_RecvCmd(TSK_UART_DEV_E * pDev,WV_U8  *pBuf,WV_S32 *pLen)
{ 

		printf("TSK_UART_RecvCmd\n");
		WV_S32  recvLen,len,i;
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

WV_S32 TSK_UART_Send(WV_U8  *pBuf ,WV_U32  len)
{ 

	 WV_S32  ret,i;
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

WV_S32 TSK_UART_RecvCheck();

****************************************************************************************/
WV_S32 TSK_UART_AckOk(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret;
	WV_U32  len;
	WV_S8 src[30];
	WV_S8	des[30];
	src[0] = 0x90;
	src[1] = 0x00;

	len = 2;

	len = TSK_UART_PackData(src,len,des);

	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART resetACK  send errro !!!");  
	}
	return ret;  
}
/****************************************************************************************

WV_S32 TSK_UART_AckSceneCmd();

****************************************************************************************/
WV_S32 TSK_UART_AckSceneCmd(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret;
	WV_U32  len;
	WV_S8 buf[30];
	buf[0] = 0x7e;
	buf[1] = 0x09;
	buf[2] = 0x90;
	buf[3] = 0x90;
	buf[4] = 0x00;
	buf[5] = 0x99;
	buf[6] = 0x95;
	buf[7] = 0x7e;
	len = 8;
	ret  = write(pDev->fd,buf,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART resetACK  send errro !!!");  
	}
	return ret;  
}




/****************************************************************************************

WV_S32 TSK_UART_AckOk();

****************************************************************************************/

WV_S32 TSK_UART_AckReset(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret;
	WV_U32  len;
	WV_S8 src[30];
	WV_S8	des[30];
	src[0] = 0x00;
	src[1] = 0x81;
	src[2] = 0x90;
	src[3] = 0x00;
	len = 4;
	len = TSK_UART_PackData(src,len,des);

	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART resetACK  send errro !!!");  
	}
	return ret;  
}
/****************************************************************************************

WV_S32 TSK_UART_AckAlam();

****************************************************************************************/
WV_S32 TSK_UART_AckAlam(TSK_UART_DEV_E * pDev)
{
 

	WV_S32  ret,len; 
	float tmp;	
	WV_S8  src[150];
	WV_S8  tempInfo[100];
	WV_S8  des[150];
	WV_S8  *pData;
	WV_U16 t;
	pData = tempInfo;
	ret = 0;
	//fpga1(outPort) temp
	HIS_SPI_FpgaRd(0x30,&t);
	tmp = (t*503.975)/4096-273.15;
	if(tmp >50){
		ret+=1;
	}
	sprintf(pData,"fpgaOut temp[%.2f]  ",tmp);
	len = strlen(tempInfo);
	pData += len;
	
	//fpga2(inPort) temp
	HIS_SPI_FpgaRd2(0x34,&t);
	tmp = (t*503.975)/4096-273.15;
	sprintf(pData,"fpgaIn temp[%.2f]  ",tmp);
	len=strlen(tempInfo)-len;
	pData+=len;
	if(tmp >50){
		ret+=1;
	}
	//cpu temp
	HIS_TEMP_ReadTemp(&tmp);
	sprintf(pData,"cpu temp[%.2f]",tmp);
	if(tmp >50){
		ret+=1;
	}
	// no alam 
	if(ret == 0){
	
		src[0] = 0x00;
		src[1] = 0x84;
		src[2] = 0x90;
		src[3] = 0x00;
		len = 4;
		len=TSK_UART_PackData(src,len,des);
		ret  = write(pDev->fd,des,len); 
		if(ret != len)
		{
			TSK_UART_printf("TSK_UART resetACK  send errro !!!");  
		}
		
	}else{
		//temp Alam 

		len = strlen(tempInfo);
		src[0] = 0x00;
		src[1] = 0x84;
		src[2] = 0x01;
		src[len+3] = 0x61;
		src[len+4] = 0x00;

		memcpy(src+3,tempInfo,len); 
		len = TSK_UART_PackData(src,len,des);
		len+=4;
		ret  = write(pDev->fd,src,len+6); 
		if(ret != len)
		{
	       	 	TSK_UART_printf("TSK_UART ack temp  send info !!!");  
		}
	
	}
	return ret;
}

/****************************************************************************************

WV_S32 TSK_UART_AckTime();

****************************************************************************************/

WV_S32 TSK_UART_AckTime(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret,len;
	struct  timeval    tv;
	struct  timezone   tz;
	gettimeofday(&tv,&tz);


	WV_S8  src[30],des[30],*pTime;	 
	pTime =(WV_S8 *) (&tv.tv_sec);
	src[0] = 0x00;
	src[1] = 0x82;
	src[2] = pTime[3]; 
	src[3] = pTime[2]; 
	src[4] = pTime[1]; 
	src[5] = pTime[0]; 
	src[6] = 0x90;
	src[7] = 0x00;
	len = 8;
	len = TSK_UART_PackData(src,len,des);
	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack time   send errro !!!");  
	}
	return ret;  
}

/****************************************************************************************

WV_S32 TSK_UART_AckHeartBeat(TSK_UART_DEV_E * pDev);

****************************************************************************************/

WV_S32 TSK_UART_AckHeartBeat(TSK_UART_DEV_E * pDev)
{ 

	WV_S8  src[30],des[30];
	WV_U32 len;	 
	WV_S32 ret;
	src[0] = 0x00;
	src[1] = 0x83;
	src[2] = 0x90; 
	src[3] = 0x00; 
	len = 4;
	len = TSK_UART_PackData(src,len,des);
	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack time   send errro !!!");  
	}
	return ret;  
}



/****************************************************************************************

WV_S32 TSK_UART_AckId();

****************************************************************************************/

WV_S32 TSK_UART_AckId(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret,len; 
	WV_S8  src[30]; 
	WV_S8  des[30];
	src[0] = 0x00;
	src[1] = 0x85;
	src[18] = 0x90;
	src[19] = 0x00;
	memcpy(src+2,(WV_S8 *)(&pDev->id),16); 
	len = 20;
	
	len = TSK_UART_PackData(src,len,des);
	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack Id  send errro !!!");  
	}
	return ret;  
}


/****************************************************************************************

WV_S32 TSK_UART_AckInfo();

****************************************************************************************/

WV_S32 TSK_UART_AckInfo(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret,len; 
	WV_S8  src[30]; 
	WV_S8 des[30];
	len = strlen(pDev->info);
	src[0] = 0x00;
	src[1] = 0x86;
	src[len+2] = 0x90;
	src[len+3] = 0x00;

	memcpy(src+2,pDev -> info,len); 
	len += 4;
		
	len = TSK_UART_PackData(src,len,des);

	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack Id  send info !!!");  
	}
	return ret;  
}
/****************************************************************************************

WV_S32 TSK_UART_AckKey(TSK_UART_DEV_E * pDev);

****************************************************************************************/

WV_S32 TSK_UART_AckKey(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret,len; 
	WV_S8  src[20]; 
	WV_S8  des[20];
	src[0] = 0x00;
	src[1] = 0xf0;
	src[2] = 0x62;
	src[3] = 0x00;
	len = 4;
	len = TSK_UART_PackData(src,len,des);
	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack Id  send info !!!");  
	}
	return ret;  
}
/****************************************************************************************

WV_S32 TSK_UART_AckRandomNum(TSK_UART_DEV_E * pDev);

****************************************************************************************/

WV_S32 TSK_UART_AckRandomNum(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret,len; 
	WV_S8  src[20]; 
	WV_S8  des[20];
	src[0] = 0x00;
	src[1] = 0xf1;
	src[2] = 0x62;
	src[3] = 0x00;
	len = 4;
	len = TSK_UART_PackData(src,len,des);

	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack Id  send info !!!");  
	}
	return ret;  
}
/****************************************************************************************

WV_S32 TSK_UART_AckStartNormal(TSK_UART_DEV_E * pDev);

****************************************************************************************/

WV_S32 TSK_UART_AckStartNormal(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret,len; 
	WV_S8  src[20]; 
	WV_S8  des[20];
	src[0] = 0x00;
	src[1] = 0xf2;
	src[2] = 0x62;
	src[3] = 0x00;
	len = 4;
	len = TSK_UART_PackData(src,len,des);
	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack Id  send info !!!");  
	}
	return ret;  
}
/****************************************************************************************

WV_S32 TSK_UART_AckTemp();

****************************************************************************************/

WV_S32 TSK_UART_AckTemp(TSK_UART_DEV_E * pDev)
{ 

	WV_S32  ret,i,len; 
	float tmp;	
	WV_S8  src[150],des[150];
	WV_S8  tempInfo[100];
	WV_S8  *pData;
	WV_U16 t;
	pData = tempInfo;
	//fpga1(outPort) temp
	HIS_SPI_FpgaRd(0x30,&t);
	tmp = (t*503.975)/4096-273.15;
	sprintf(pData,"fpgaOut temp[%.2f]  ",tmp);
	len = strlen(tempInfo);
	pData += len;

	//fpga2(inPort) temp
	HIS_SPI_FpgaRd2(0x34,&t);
	tmp = (t*503.975)/4096-273.15;
	sprintf(pData,"fpgaIn temp[%.2f]  ",tmp);
	len=strlen(tempInfo)-len;
	pData+=len;

	//cpu temp
	HIS_TEMP_ReadTemp(&tmp);
	sprintf(pData,"cpu temp[%.2f]",tmp);
	
	//
	 len = strlen(tempInfo);
	 src[0] = 0x00;
	 src[1] = 0x88;
	 memcpy(src+2,tempInfo,len);
	 
	len+=2;
	len = TSK_UART_PackData(src,len,des);
	ret  = write(pDev->fd,des,len); 

	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack temp  send info !!!");  
	}
	return ret;  
}
/****************************************************************************************

WV_S32 TSK_UART_AckPort(TSK_UART_DEV_E * pDev);

****************************************************************************************/

WV_S32 TSK_UART_AckPort(TSK_UART_DEV_E * pDev)
{

	WV_S8  src[30],des[30];
	WV_U32 len;	 
	WV_S32 ret;
	src[0] = 0x00;
	src[1] = 0x89;
	src[2] = 0x90; 
	src[3] = 0x00; 
	len = 4;
	len = TSK_UART_PackData(src,len,des);
	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack time   send errro !!!");  
	}
	return ret;  
}
/****************************************************************************************

WV_S32 TSK_UART_AckIP();

****************************************************************************************/

WV_S32 TSK_UART_AckIP(TSK_UART_DEV_E * pDev)
{ 
 
	WV_S32  ret,i,len; 
	WV_S8  src[150],des[150];
	WV_S8  *pData;
	WV_S8  ip[20];
	WV_S8  mask[20];
	WV_S8  gw[20]; 
        WV_S8  port[20];
        WV_S8  ipInfo[100];
	SYS_ENV_Get("IP_Ip",ip);
	SYS_ENV_Get("IP_Mask",mask);
	SYS_ENV_Get("IP_Gatway",gw);
	SYS_ENV_Get("SVR_PORT",port);
	 
   
	pData=ipInfo;
	//ip
	strcpy(pData,"ip:");
	pData+=3;
        len=strlen(ip);
	strncpy(pData,ip,len);
	pData+=len;
	strcpy(pData," ");
	pData+=1;
	//mask
	strcpy(pData,"mask:");
	pData+=5;
        len=strlen(mask);
	strncpy(pData,mask,len);
	pData+=len;
	strcpy(pData," ");
	pData+=1;
	
	//gw
	strcpy(pData,"gw:");
	pData+=3;
        len=strlen(gw);
	strncpy(pData,gw,len);
	pData+=len;
	strcpy(pData," ");
	pData+=1;
	
	//port
	strcpy(pData,"port:");
	pData+=5;
        len=strlen(port);
	strncpy(pData,port,len);
	pData+=len;
	strcpy(pData," ");
	pData+=1;
	//
	len = strlen(ipInfo);
	src[0] = 00;
	src[1] = 0x87;

	memcpy(src+2,ipInfo,len); 
	 
	len+=2;
	len = TSK_UART_PackData(src,len,des);
	ret  = write(pDev->fd,des,len); 
	if(ret != len)
	{
		TSK_UART_printf("TSK_UART ack temp  send info !!!");  
	}
	return ret;  
}

/***************************************************************

WV_S32 TSK_UART_Cmd(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf);

***************************************************************/
WV_S32 TSK_UART_Cmd(TSK_UART_DEV_E  *pDev,WV_U8 *pBuf)
{
		WV_S32 ret = 0;
		switch(pBuf[2])
		{
			case  0x01://reset 
			   TSK_UART_printf("recev CMD  reset"); 
			   TSK_UART_AckReset(pDev);
			   sleep(1); 
			   WV_CMD_GetReset(); 
			    break;
			case  0x02://work time 
			    TSK_UART_printf("recev CMD  run time");
			    TSK_UART_AckTime(pDev); 
			    break;
			case  0x03://hand
			   TSK_UART_printf("recev CMD  hand");
			   TSK_UART_AckHeartBeat(pDev);
			    break;
			case  0x04://query alam
			   TSK_UART_printf("recev CMD  query");
			   TSK_UART_AckAlam(pDev);
			   break;
			case  0x05: //read id
			    TSK_UART_printf("recev CMD  read id");
			    TSK_UART_AckId(pDev);
			     break;
			case  0x06: //read info
			   TSK_UART_printf("recev CMD  read info");
			   TSK_UART_AckInfo(pDev); 
			   break;
			case 0x07://read ip
			   TSK_UART_printf("recev CMD  read ip");
			   TSK_UART_AckIP(pDev); 
			   break;
			case 0x08://read temp
			   TSK_UART_printf("recev CMD  read temp");
			   TSK_UART_AckTemp(pDev); 
			   break;
			case 0x09://change uart mode
			   TSK_UART_printf("recev CMD change uart mode ");
				TSK_UART_AckPort(pDev);
				sleep(1);
			   TSK_UART_SetPort(0);
			   TSK_UART_SaveConf(pDev);
				break;
			case 0x10://change scene
			     TSK_UART_printf("recev CMD  scene change[%d]\n",pBuf[3]);
					if(pBuf[1]==0x09){
			     		ret = TSK_SCENE_Change(TSK_SCENE_TYPE_NETDATA, pBuf[3]);
			     		TSK_UART_AckSceneCmd(pDev); 
					}
			     break; 
			case 0x70://
					TSK_UART_printf("recev CMD  ack key\n");
				   TSK_UART_AckKey(pDev);
					break;
			case 0x71://
					TSK_UART_AckRandomNum(pDev);
				   TSK_UART_printf("recev CMD  ack random num\n");
					break;
			case 0x72://
					TSK_UART_AckStartNormal(pDev);
				   TSK_UART_printf("recev CMD  ack start normal\n");
					break;				    
			default:
			   pDev -> procEroNum ++; 
			  TSK_UART_printf("recev CMD   not list"); 
		} 

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

	//memset(gUartDev.sceneCmd,0,sizeof(gUartDev.sceneCmd));
	//TSK_UART_AnalyzeCmd(pData);

	return 0;	
	
}

/****************************************************************************************

WV_S32 TSK_UART_SceneAck();

****************************************************************************************/
WV_S32 TSK_UART_SceneAck(TSK_UART_DEV_E * pDev,WV_S32 id)
{
	WV_S32 ret,i;
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
				
		ret  = TSK_UART_RecvCmd(pDev, recvBuf,&cmdLen);
		//printf("uart recv cmd \n");
		ret = TSK_UART_SceneCheck(pDev,recvBuf,cmdLen);
		ret = TSK_UART_SceneCmd(pDev,recvBuf,cmdLen);
        ret = TSK_UART_WagCheck(pDev, recvBuf, cmdLen);
        ret = TSK_UART_WagCmd(pDev, recvBuf, cmdLen);
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

WV_S32 TSK_UART_SetSerialConf(WV_S8 * buf);

*********************************************************/ 
WV_S32 TSK_UART_SetSerialConf(WV_S8 * buf)
{
	WV_S8 name[64];
	WV_U32 data=0;
	WV_S32 i,j=0,len;
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
	}

	printf("********SetSerialConf [%s]=%d *************\n ",name,data);
	return 0;
	
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
    return 0;
}
/********************************************************

WV_S32 TSK_UART_SetLoopConf(WV_S8 *buf)

*********************************************************/ 
WV_S32 TSK_UART_SetLoopConf(WV_S8 *buf)
{


	WV_S8 name[64];
	WV_U32 data=0;
	WV_S32 i,j=0,len;
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
	WV_S32 i,j,ret;
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
	WV_S32 ret=-1,i,j;
	pDev = (TSK_UART_LOOP_E  *) prm;
	pDev-> open  = 1;
	pDev-> close  = 0;  
	while(pDev -> open == 1)
	{  
		if(gSceneLoop.loopEna == 1 && gSceneLoop.loopTime > 0)
		{
			for(i=0;i<TSK_UART_SCENE_NUM;i++)
			{
				for(j=0;j<gSceneLoop.loopTime;j++)
				{
					sleep(60);
				}

				while(TSK_SCENE_Change(TSK_SCENE_TYPE_NETDATA, i) != 0 )
				{
					i++;
					if(i>=TSK_UART_SCENE_NUM)
						break;
				}
			
			}
		}
		sleep(1);
		continue;
	   
    }

 pDev-> open  = 0;
 pDev-> close = 1;
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


