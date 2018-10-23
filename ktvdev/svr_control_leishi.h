#ifndef  _SVR_CONTROL_LEISHI_H_H_
#define  _SVR_CONTROL_LEISHI_H_H_
#include "wv_common.h"  
#include "tsk_conf.h"
#include "tsk_scene.h"
#include "tsk_uart.h"
#include "sys_ip.h"
#include "wv_sqlite3.h"
#include "svr_control_cmd.h"
#include "net_uart.h"


#define SVR_CONTROL_LEISHI_DB_FILE_PATH      "./env/leishi.db"
#define SVR_CONTROL_LEISHI_CMD_MAX_NUM 16
#define SVR_CONTROL_LEISHI_DB_TABLE_NAME     "SongClass"
#define SVR_CONTROL_LEISHI_DB_SONGCLASS_NAME       "Class"
#define SVR_CONTROL_LEISHI_CMD_MAX_LEN  64
/********************control cmd ***************************/
#define 	SVR_CONTROL_LEISHI_STARTING_CMD           0x00        //设备开机
#define 	SVR_CONTROL_LEISHI_STANDBY_CMD            0x01        //设备待机
#define 	SVR_CONTROL_LEISHI_OPEN_PROJECTOR_CMD     0x02        //投影仪开
#define 	SVR_CONTROL_LEISHI_CLOSE_PROJECTOR_CMD    0x03        //投影仪关
#define 	SVR_CONTROL_LEISHI_GET_MAC_CMD            0x06        //获取MAC地址
/***********************************************************/

typedef struct SVR_CONTROL_DEVICE_CMD
{
        WV_U8 CmdLen;
        WV_U8 cmd[SVR_CONTROL_LEISHI_CMD_MAX_LEN];
        WV_U8 AckLen;
        WV_U8 ack[SVR_CONTROL_LEISHI_CMD_MAX_LEN]; //可以是回复的命令，也可以是控制投影的命令，
}SVR_CONTROL_LEISHI_DEVICE_CMD;

typedef struct SVR_CONTROL_LEISHI_DEV
{
        WV_U8 movChangeByID;
        SVR_CONTROL_LEISHI_DEVICE_CMD controlDev[SVR_CONTROL_LEISHI_CMD_MAX_NUM];
}SVR_CONTROL_LEISHI_DEV;

SVR_CONTROL_LEISHI_DEV gSvrLeishiDev;

WV_S32 SVR_CONTROL_Leishi_ControlCmd(WV_S8 *buf);
WV_S32 SVR_CONTROL_LeiShi_controlDev(WV_S8 *pData,WV_S32 len);
WV_S32 SVR_CONTROL_Leishi_MapSongType(WV_U8 *pName,WV_U8 *pOut);
WV_S32 SVR_CONTROL_Leishi_callback_DB(void* data, int ncols, char** values, char** headers);
WV_S32 SVR_CONTROL_LeiShi_ChangeMovByCmd(WV_S8 *pID,WV_S8 *pName);
WV_S32 SVR_CONTROL_LeiShi(WV_S8 *pData,WV_S32 len);
WV_S32 SVR_CONTROL_LeiShi_Init();
#endif


