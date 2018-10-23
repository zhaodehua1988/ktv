#include "wv_common.h"
#include "his_spi.h"
#include "wv_cmd.h"
#include "wv_thr.h"

#define FPGA_UPDATE_DATABUF_LEN 2048



typedef struct FPGA_UPDATE_DEV
{
	WV_U16  FpgaPacknum[2];//，fpga当前已经升级的包数
	WV_U16  FpgaTotalPack[2];//fpga程序总包数
	WV_U8   FpgaUpdating; //1:FPGA输入端 2: FPGA输出端
}FPGA_UPDATE_DEV;
static FPGA_UPDATE_DEV gFpgaUpdateDev;
//static WV_U16 gFpgaPackNum[2];


unsigned short const gCrcTable[256] =
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

unsigned short CAL_CRC16(const unsigned char *pChar,int len,unsigned short crc)
{
	int i = 0;
	unsigned short usTmp;
	for(i = 0;i<len;i++)
	{
	usTmp = gCrcTable[((*(pChar + i)) & 0x00ff) ^ (crc & 0x00ff)];
	crc = ((usTmp & 0xff00) | ((usTmp & 0x00ff) ^ ((crc & 0xff00) >> 8)));
	}
	return crc;
}

/*******************************************************************

WV_S32 FPGA_UPDATE_START();

*******************************************************************/
WV_S32 FPGA_UPDATE_Start(WV_U8 fpgaID)
{
	gFpgaUpdateDev.FpgaPacknum[fpgaID] = 0;
	WV_printf("updage fpga[%d] start \n",fpgaID);
	WV_S32 ret = 0;
	
	if(fpgaID == 0)
	{
		
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x50,1),ret);	
		
	}else if(fpgaID == 1){

		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x50,1),ret);			
	}
	return ret;
}

/*******************************************************************

WV_S32 FPGA_UPDATE_EraserFlash(WV_U8 fpgaID);

*******************************************************************/
WV_S32 FPGA_UPDATE_EraserFlash(WV_U8 fpgaID)
{
	WV_printf("EraserFlash fpga[%d]  start set 0x51=5...\n",fpgaID);
	WV_S32 ret = 0;
	if(fpgaID == 0 )
	{

		WV_RET_ADD(	HIS_SPI_FpgaWd(0x51,0),ret);	
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x51,0x5),ret);	
		sleep(5);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x51,0x4),ret);
		
	}else if(fpgaID == 1){

		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x51,0),ret);	
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x51,0x5),ret);	
		sleep(5);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x51,0x4),ret);
	
	}	
	return ret;
	
}

/*******************************************************************

WV_S32 FPGA_UPDATE_WriteLen(WV_U16 dataLen);

*******************************************************************/
WV_S32 FPGA_UPDATE_WriteLen(WV_U8 fpgaID,WV_U16 dataLen)
{
		
	WV_U16 data=0;	
	WV_S32 ret;
	WV_S32 i;
	if(fpgaID == 0 )
	{
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x5d,dataLen),ret);	
		do{

			//printf("write data Len = %d \n",dataLen);
			//printf("set 0x5e  to 1\n");

			HIS_SPI_FpgaWd(0x5e,1);
			usleep(100);
			WV_RET_ADD(	HIS_SPI_FpgaRd(0x5c,&data),ret);

			if(data == 1)
			{
				HIS_SPI_FpgaWd(0x5c,0);
				HIS_SPI_FpgaWd(0x5e,0);
				break;
			}
						
		
		}while(1);
	}else if(fpgaID == 1){
		WV_printf("write data Len = %d \n",dataLen);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x5d,dataLen),ret);	
		do{

			HIS_SPI_FpgaWd2(0x5e,1);
			usleep(100);
			WV_RET_ADD(	HIS_SPI_FpgaRd2(0x5c,&data),ret);

			if(data == 1)
			{
				HIS_SPI_FpgaWd2(0x5c,0);
				HIS_SPI_FpgaWd2(0x5e,0);
				break;
			}
		}while(1);
								
	}
	return ret;
}

/*******************************************************************

WV_S32 FPGA_UPDATE_WriteData(WV_U8 fpgaID,WV_U16 data);

*******************************************************************/
WV_S32 FPGA_UPDATE_WriteData(WV_U8 fpgaID,WV_U16 data)
{
	WV_S32 ret = 0;
	WV_U16 reg;
	if(fpgaID == 0)
	{

		WV_RET_ADD(	HIS_SPI_FpgaWd(0x53,data),ret);	
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x58,1),ret);
		while(1)
		{
			WV_RET_ADD(	HIS_SPI_FpgaRd(0x57,&reg),ret);	
			if(reg == 1 )
			{
				WV_RET_ADD(	HIS_SPI_FpgaWd(0x57,0),ret);

				break;	
			}
			usleep(1);		
		}
	}else if(fpgaID == 1){
		//WV_printf("0x%04x ",data);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x53,data),ret);	
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x58,1),ret);
		while(1)
		{
			WV_RET_ADD(	HIS_SPI_FpgaRd2(0x57,&reg),ret);	
			if(reg == 1 )
			{
				WV_RET_ADD(	HIS_SPI_FpgaWd2(0x57,0),ret);

				break;	
			}
			usleep(1);		
		}		
	}

	return ret;
}

/*******************************************************************

WV_S32 FPGA_UPDATE_CheckCRC(WV_U8 fpgaID,WV_U16 crc);

*******************************************************************/
WV_S32 FPGA_UPDATE_CheckCRC(WV_U8 fpgaID,WV_U16 crc)
{

	//WV_printf("check fpga updage crc [%d]  \n",fpgaID);
	WV_S32 ret = 0;
	WV_U16 reg;
	if(fpgaID == 0)
	{
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x59,crc),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x5b,1),ret);
		
		while(1)
		{
				WV_RET_ADD(	HIS_SPI_FpgaRd(0x5a,&reg),ret);	
				if(reg == 1 )
				{
					WV_printf("fpga[%d] crc is right crc=%04x \n",fpgaID,crc);
					ret = 0;
					break;	
				}else if(reg == 2)
				{
					WV_printf("fpga[%d] crc is error crc=%04x\n",fpgaID,crc);
					ret = -1;
					break;
				}
				usleep(1);	
		}
	}else if(fpgaID == 1){
		
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x59,crc),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x5b,1),ret);
		
		while(1)
		{
				WV_RET_ADD(	HIS_SPI_FpgaRd2(0x5a,&reg),ret);	
				if(reg == 1 )
				{
					WV_printf("fpga[%d] crc is right crc=%04x \n",fpgaID,crc);
					ret = 0;
					break;	
				}else if(reg == 2)
				{
					WV_printf("fpga[%d] crc is error crc=%04x\n",fpgaID,crc);
					ret = -1;
					break;
				}
				usleep(1);	
		}
	}
	return ret;
}

/*******************************************************************

WV_S32 FPGA_UPDATE_SetPackNum(WV_U8 fpgaID,WV_U16 num);

*******************************************************************/
WV_S32 FPGA_UPDATE_SetPackNum(WV_U8 fpgaID,WV_U16 num)
{
	WV_S32 ret=0;
	if(fpgaID == 0 )
	{	

		WV_RET_ADD(	HIS_SPI_FpgaWd(0x56,num),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd(0x5a,0),ret);

	}else if(fpgaID == 1){

		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x56,num),ret);
		WV_RET_ADD(	HIS_SPI_FpgaWd2(0x5a,0),ret);

	}
		WV_printf("fpga[%d] ,pack num = %d \n",fpgaID,num); 
	return ret;
}

/*******************************************************************

WV_S32 FPGA_UPDATE_getFpgaData(WV_U8 fpgaID,WV_U8 *pData,WV_U16 len);

*******************************************************************/
WV_S32 FPGA_UPDATE_getFpgaData(WV_U8 fpgaID,WV_U16 *pData,WV_U16 len)
{
	WV_printf("set fpga data ,len=%d\n",len);
	if(fpgaID != 0  && fpgaID != 1)
	{
		WV_printf("update fpga is error ,fpgaID=[%d] \n",fpgaID);
		return WV_EFAIL;
	}
	WV_S32 i,ret=0;
	WV_U16 data=0,crc=0xffff;
	//
	ret = FPGA_UPDATE_WriteLen(fpgaID,len);
	
	for(i=0;i<len;i++)
	{

		data = pData[i];
		WV_printf("fpga[%d]write data[%d]=0x%04x\n",fpgaID,i,data);
		FPGA_UPDATE_WriteData(fpgaID,data);

	}

	crc = CAL_CRC16((const unsigned char *)pData,len*2,crc);
		

	WV_printf("fpga write data ok,start crc check..crc=0x%04x\n",crc);	
	ret = FPGA_UPDATE_CheckCRC(fpgaID,crc);
	if(ret == 0 )
	{
		gFpgaUpdateDev.FpgaPacknum[fpgaID] = gFpgaUpdateDev.FpgaPacknum[fpgaID]+1;
		FPGA_UPDATE_SetPackNum(fpgaID,gFpgaUpdateDev.FpgaPacknum[fpgaID]);	
	}

	return ret;
}

/*******************************************************************

WV_S32 FPGA_UPDATE_getFpgaBin(WV_U8 fpgaID);

*******************************************************************/
WV_S32 FPGA_UPDATE_getFpgaBin(WV_U8 fpgaID)
{

	WV_S32 i;
	FILE *fp;
	WV_U8 fpgaBinFile[32];
	WV_U16 buf[FPGA_UPDATE_DATABUF_LEN];
	WV_S32 readLen=0;	
	WV_S32 ret;
	sprintf(fpgaBinFile,"./fpga%d.bin",fpgaID);
	
	fp = fopen(fpgaBinFile,"rb");
	if(fp == NULL)
	{
		WV_printf("file %s is not exit \n",fpgaBinFile);
		return WV_EFAIL;
	}
	printf("upfpga[%d] get bin start ..\n",fpgaID);
	while(1)
	{
		memset(buf,0xff,sizeof(buf));
		readLen= fread(buf,2,FPGA_UPDATE_DATABUF_LEN,fp);
		if(readLen <= 0)
		{
			break;
		}
		
		while(1)
		{
			
			if(FPGA_UPDATE_getFpgaData(fpgaID,buf,readLen) == 0 ) break;
			usleep(1);
		}
		usleep(1);		
	}
	
	fclose(fp);
	return WV_SOK;
	
}

/****************************************************************************

void *TSK_UPDATE_update();

****************************************************************************/
void *TSK_UPDATE_update(void *prm)
{
	WV_S32 ret=0;	
	pthread_detach(pthread_self());
	WV_U8 *pID = (WV_U8 *)prm;
	WV_U8 fpgaID = *pID;

	FPGA_UPDATE_Start(fpgaID);
	FPGA_UPDATE_EraserFlash(fpgaID);
	FPGA_UPDATE_getFpgaBin(fpgaID);

	WV_printf("***********fpga[%d] update ok*********\n",fpgaID);

}

/***********************************************************************

WV_S32  TSK_UPDATE_FPGAGetUpdateNum(WV_U16 u16UpdatedPack,WV_U16 u16TotalPack);
函数功能::查询当前升级进度
u16UpdatedPack:已经升级的包数
u16TotalPack： 总包数
***********************************************************************/
WV_S32  TSK_UPDATE_FPGAGetUpdateNum(WV_U16 *pTotalPack,WV_U16 *pUpdatedPack)
{
	if(gFpgaUpdateDev.FpgaUpdating != 1 && gFpgaUpdateDev.FpgaUpdating != 2) 
	return WV_EFAIL;
#if 1
	*pUpdatedPack = gFpgaUpdateDev.FpgaPacknum[gFpgaUpdateDev.FpgaUpdating-1];
	*pTotalPack = gFpgaUpdateDev.FpgaTotalPack[gFpgaUpdateDev.FpgaUpdating-1];
#else


#endif
	return WV_SOK;
}

/***********************************************************************

WV_S32  TSK_UPDATE_FPGA_RecvBin(WV_U8 fpgaPort,WV_U32 len,WV_U8 *pData);
函数功能 ::   通过网络接受fpga升级包， 并且保存到本地
fpgaPort::  选择升级哪个fpga,1:输入 2:输出
len     ::  fpga升级包大小(字节)
pData   ::  fpga升级包数据
***********************************************************************/
WV_S32  TSK_UPDATE_FPGA_RecvBin(WV_U8 fpgaPort,WV_U32 len,WV_U8 *pData)
{
	if(fpgaPort != 1 && fpgaPort != 2) return WV_EFAIL;

	FILE *fp= NULL;
	WV_U8 fpgaName[16]={0};
	WV_S32 writeLen=0;
	sprintf(fpgaName,"./fpga%d.bin",fpgaPort-1);
	fp = fopen(fpgaName,"wb");
	if(fp == NULL) return WV_EFAIL;

	writeLen = fwrite(pData,1,len,fp);
	if(writeLen != len) 
	{

		fclose(fp);
		remove(fpgaName);
		return WV_EFAIL;
	}

	fclose(fp);
	WV_U16 packNum =  len/2048;
	if(len/2048) packNum+=1; 	
	gFpgaUpdateDev.FpgaUpdating = fpgaPort;
	gFpgaUpdateDev.FpgaTotalPack[fpgaPort-1] = 	packNum ;
	gFpgaUpdateDev.FpgaPacknum[fpgaPort-1] = 	0 ;
	return WV_SOK;
}

/****************************************************************************

WV_S32 TSK_UPDATE_CMDStart(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_UPDATE_CMDStart(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{


   WV_U16 data,addr;
   WV_U32  temp;
   WV_S32 id,ret = 0; 
   
   if(argc < 1)
	{
         prfBuff += sprintf(prfBuff,"upFpga start <fpgaId> \r\n");
         return WV_SOK;  
	}

	ret =WV_STR_S2v(argv[0],&id);
	if(id != 0 && id != 1 )
	{
		  prfBuff += sprintf(prfBuff,"get fpga id error :id=%d .\r\n",id);
		return WV_SOK;
	}
	WV_U8 fpgaID = id;
	WV_THR_HNDL_T thrHndl;	
	WV_CHECK_RET( WV_THR_Create(&thrHndl,TSK_UPDATE_update, WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, (void *)&fpgaID));

  return WV_SOK;
}



/****************************************************************************

WV_S32 TSK_UPDATE_CMDFpgaPackAdd(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************/
WV_S32 TSK_UPDATE_CMDFpgaPackAdd(WV_S32 argc, WV_S8 **argv, WV_S8 * prfBuff)
{


   WV_U16 data,addr;
   WV_U32  temp;
   WV_U32 id,pack,ret = 0; 
   
   if(argc < 2)
	{
         prfBuff += sprintf(prfBuff,"upFpga pack <fpgaId> <AddpackNum>\r\n");
         return WV_SOK;  
	}

	ret =WV_STR_S2v(argv[0],&id);
	if(id != 0 && id != 1 ) return WV_EFAIL;

	ret =WV_STR_S2v(argv[1],&pack);	
	gFpgaUpdateDev.FpgaPacknum[id]+=pack;
	if(gFpgaUpdateDev.FpgaPacknum[id] > gFpgaUpdateDev.FpgaTotalPack[id]) 

		gFpgaUpdateDev.FpgaPacknum[id] = gFpgaUpdateDev.FpgaTotalPack[id];

	WV_printf("update fpga LocalPack[%d]  TotalPack[%d] \n",gFpgaUpdateDev.FpgaPacknum[id],gFpgaUpdateDev.FpgaTotalPack[id]);

  return WV_SOK;
}



/***********************************************************************

WV_S32  TSK_UPDATE_FPGA_Open();

***********************************************************************/
WV_S32  TSK_UPDATE_FPGA_Open()
{ 	
	memset(&gFpgaUpdateDev,0,sizeof(gFpgaUpdateDev));
    WV_CMD_Register("upFpga",NULL,"upFpga comand",NULL);
    WV_CMD_Register("upFpga","start","upFpga comand start",TSK_UPDATE_CMDStart);
    WV_CMD_Register("upFpga","pack","upFpga comand pack add",TSK_UPDATE_CMDFpgaPackAdd);


 return WV_SOK;
}

