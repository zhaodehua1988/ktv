#ifndef _SVR_CMD_DEFINE_H_
#define _SVR_CMD_DEFINE_H_
#include"wv_common.h"

#define SVR_CMD_HANDSHARK       0x01
#define SVR_CMD_MAP             0x02
#define SVR_CMD_OUTLINE         0x03
#define SVR_CMD_SPLIT           0x04 
#define SVR_CMD_OUTRESOLUTION   0x05
#define SVR_CMD_SPLITCONF       0x06 
#define SVR_CMD_WINDOWCONF      0x07
#define SVR_CMD_MOVCONF         0x08
#define SVR_CMD_GIFCONF         0x09 
#define SVR_CMD_SCENECONF       0x0A 
#define SVR_CMD_IPCONF          0x0B
#define SVR_CMD_DRAWLINECONF    0x0C
#define SVR_CMD_LIGHTCONF       0x0D 
#define SVR_CMD_COLOR           0x0E  
#define SVR_CMD_FPGAUPDATE      0x0F
#define SVR_CMD_GETTEMP         0x10
#define SVR_CMD_GETRESOLUTION   0x11
#define SVR_CMD_GETSPLITCONF    0x12
#define SVR_CMD_GETMOVINFO      0x13
#define SVR_CMD_UPDATEMOV       0x14
#define SVR_CMD_DELETMOV        0x15
#define SVR_CMD_CHANGEMOV	    0x16
#define SVR_CMD_RENAMEMOV	    0x17
#define SVR_CMD_GETSCENEINFO    0x18

#define SVR_CMD_GETMOVURL       0x1a
#define SVR_CMD_GETPIC          0x1b
#define SVR_CMD_GET_CONTROL_PROTOCOL     0x1c
#define SVR_CMD_GET_ID          0x1d
#define SVR_CMD_SET_LICENCES_CMD 0x1e
#define SVR_CMD_GET_DEVINFO     0x1f
#define SVR_CMD_SET_GIF         0x20
#define SVR_CMD_SET_RESET       0x21
#define SVR_CMD_GET_MAPLINE     0x22
#define SVR_CMD_SET_CASCADING        0x23
#define SVR_CMD_GET_CASCADING        0x24

#define SVR_CMD_SET_PLAYMODE        0x25
#define SVR_CMD_SET_PLAYWINDOW      0x26   
#define SVR_CMD_SAVE_ANGLE          0x27
#define SVR_CMD_GET_ANGLE           0x28   
#define SVR_CMD_SET_VOLUME          0x29 

#define SVR_CMD_SAVE_MOV_TYPE        0x2a
#define SVR_CMD_SAVE_SCENE_TYPE        0x2b
#define SVR_CMD_SET_TEXT         0x2c    //text
#define SVR_CMD_CONF_TEXTSHOW         0x2d   //conf
#define SVR_CMD_CONF_NETUART         0x2e   
#define SVR_CMD_CONF_DEV_ON_OFF      0x2f
#define SVR_CMD_CUSTOM_SCENE         0x30   //自定义场景
#define SVR_CMD_FADE_INOUT         0x31   //淡入淡出

//#define SVR_SYNC_WORD         0x5a5a5aa5  
#endif

