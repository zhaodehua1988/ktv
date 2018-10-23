#include "fpga_conf.h"

/*************************************************************************************

WV_S32  HIS_FPGA_Init();

*************************************************************************************/
WV_S32 HIS_FPGA_Init(){
	
	WV_S32 chl;

	FPGA_CONF_GetVersion();

	chl = FPGA_CONF_GetOutChl_Num();
	if( chl < 0){
		
		WV_printf("error:get out channel num is [%d]\n",chl);
		return WV_EFAIL;
	}
	WV_printf(" \nget out channel num is [%d]\n",chl);

	return WV_SOK;
}
/*************************************************************************************

WV_S32 HIS_FPGA_DeInit();

*************************************************************************************/
WV_S32 HIS_FPGA_DeInit(){

	return WV_SOK;
}
