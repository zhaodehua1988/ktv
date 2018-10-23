#include "svr_control_leishi.h"

//#define SVR_CONTROL_LEISHI_DB_FILE_PATH      "./env/leishi.db"
//#define SVR_CONTROL_LEISHI_CMD_MAX_NUM 16
//#define SVR_CONTROL_LEISHI_DB_TABLE_NAME     "SongClass"
//#define SVR_CONTROL_LEISHI_DB_SONGCLASS_NAME       "Class"
//#define SVR_CONTROL_LEISHI_CMD_MAX_LEN  64
///********************control cmd ***************************/
//#define 	SVR_CONTROL_LEISHI_STARTING_CMD           0x00        //设备开机
//#define 	SVR_CONTROL_LEISHI_STANDBY_CMD            0x01        //设备待机
//#define 	SVR_CONTROL_LEISHI_OPEN_PROJECTOR_CMD     0x02        //投影仪开
//#define 	SVR_CONTROL_LEISHI_CLOSE_PROJECTOR_CMD    0x03        //投影仪关
//#define 	SVR_CONTROL_LEISHI_GET_MAC_CMD            0x06        //获取MAC地址
///***********************************************************/

//typedef struct SVR_CONTROL_DEVICE_CMD
//{
//	WV_U8 CmdLen;
//	WV_U8 cmd[SVR_CONTROL_LEISHI_CMD_MAX_LEN];
//	WV_U8 AckLen;
//	WV_U8 ack[SVR_CONTROL_LEISHI_CMD_MAX_LEN]; //可以是回复的命令，也可以是控制投影的命令，
//}SVR_CONTROL_LEISHI_DEVICE_CMD;

//typedef struct SVR_CONTROL_LEISHI_DEV
//{
//	WV_U8 movChangeByID;
//	SVR_CONTROL_LEISHI_DEVICE_CMD controlDev[SVR_CONTROL_LEISHI_CMD_MAX_NUM];
//}SVR_CONTROL_LEISHI_DEV;

//SVR_CONTROL_LEISHI_DEV gSvrLeishiDev;

/********************************************************
*
*WV_S32 SVR_CONTROL_Leishi_ControlCmd(WV_S8 *buf)
*解析网络待机命令
*
*********************************************************/ 
WV_S32 SVR_CONTROL_Leishi_ControlCmd(WV_S8 *buf)
{
    //命令格式如下：
    //cmd like :"00 |5a 5a 5a a5 00|"; #网络待机命令
    //cmd like :"01 |5a 5a 5a a5 01|"; #网络开机命令
    //cmd like :"02 |5a 5a 5a a5 02|7e 30 30 30 30 20 31 0d|"; #网络开投影
    //cmd like :"03 |5a 5a 5a a5 03|7e 30 30 30 30 20 30 0d|"; #网络关投影
    //cmd like :"04 |5a 5a 5a a5 04|"; #查询mac地址

    int a,id,i,k=0,j=0;
    char temp[3];
    memset(temp,0,sizeof(temp));
    //************get control device cmd id ****************
    for(i=0;i<strlen(buf);i++)
    {
        if(buf[i] != ' ')
        {
            if(buf[i] == '|') break;

            temp[j]=buf[i];
            j++;
            if(j>=3) return -1;
        }
    }
    sscanf(temp,"%x",&a);
    id = a;
    if(id >= SVR_CONTROL_LEISHI_CMD_MAX_NUM)
    {
        //printf("get cmd error \n");
        return -1;
    }

    //*****************get control device cmd  ********************


    memset(temp,0,sizeof(temp));

    j=0;
    for(i=i+1;i<strlen(buf);i++)
    {
        if(buf[i] == '#')
        {
            //gControlDev.controlDev[id].CmdLen = k;
            break;
        }

        if(buf[i] == '|')
        {
            gSvrLeishiDev.controlDev[id].CmdLen = k;
            /*
                        k = 0;
                        if(id == 2 || id == 3)//id为2，开投影，id为3关投影，需要后面跟开关命令
                        {
                                for(i=i+1;i<strlen(buf);i++)
                                {
                                        if(buf[i] == '#')
                                        {
                                                //gControlDev.controlDev[id].AckLen = k;
                                                break;
                                        }
                                        if(buf[i] == '|')
                                        {
                                                gSvrLeishiDev.controlDev[id].AckLen = k;
                                                break;
                                        }

                                        if(buf[i] != ' ' && buf[i] != 0x0a )
                                        {
                                                temp[j]=buf[i];
                                                j++;
                                                if(j==2){
                                                        j=0;
                                                        sscanf(temp,"%x",&a);
                                                        gSvrLeishiDev.controlDev[id].ack[k] = (WV_U8)a;//get swCmd
                                                        k++;
                                                        memset(temp,0,sizeof(temp));
                                                }
                                        }
                                }
                        }//end if(id == 2 || id == 3)
                        */
            break;
        }

        if(buf[i] != ' ' && buf[i] != 0x0a )
        {
            temp[j]=buf[i];
            j++;
            if(j==2){
                j=0;
                sscanf(temp,"%x",&a);
                gSvrLeishiDev.controlDev[id].cmd[k] = (WV_U8)a;//get swCmd
                k++;
                memset(temp,0,sizeof(temp));
            }
        }

        if(i==strlen(buf)-1)
        {
            gSvrLeishiDev.controlDev[id].CmdLen = k;
        }

    }

    return WV_SOK;
}

/****************************************************************************

WV_S32 SVR_CONTROL_LeiShi_controlDev(WV_S8 *pData,WV_S32 len)
//控制本机和外部设备(投影仪)命令格式(16进制)：5a 5a 5a a5 00

****************************************************************************/
WV_S32 SVR_CONTROL_LeiShi_controlDev(WV_S8 *pData,WV_S32 len)
{

    WV_S32 i,j;
    WV_U8 mac[6];
    WV_S32 ret = -1;
    for(i=SVR_CONTROL_LEISHI_CMD_MAX_NUM;i>=0;i--)
    {
        //printf("*************i = %d *************\n",i);
        if(gSvrLeishiDev.controlDev[i].CmdLen == len)
        {
            for(j=0;j<len;j++)
            {
                if(gSvrLeishiDev.controlDev[i].cmd[j] != pData[j])
                {
                    //WV_printf("\nget scene cmd \n");
                    break;
                }
                //WV_printf("i=%d,j=%d[%02x] \n",i,j,pData[j]);
            }

            if(j==len )
            {
                //printf("*************i = %d *************\n",i);
                switch(i)
                {
                case SVR_CONTROL_LEISHI_STARTING_CMD:
                    SVR_CONTROL_printf("**********get cmd: starting up dev ***************\n");
                    TSK_SCENE_StartingUP();
                    ret = 0;
                    break;
                case SVR_CONTROL_LEISHI_STANDBY_CMD:
                    SVR_CONTROL_printf("**********get cmd: standby dev***************\n");
                    TSK_SCENE_Standby();
                    ret = 0;
                    break;
                case SVR_CONTROL_LEISHI_OPEN_PROJECTOR_CMD:
                    NET_UART_ProjectorCmd(1);
                    SVR_CONTROL_printf("**********get cmd: open external  projector 1***************\n");
                    ret = 0;
                    break;
                case SVR_CONTROL_LEISHI_CLOSE_PROJECTOR_CMD:
                    NET_UART_ProjectorCmd(0);
                    SVR_CONTROL_printf("**********get cmd: close external  projector 1 ***************\n");
                    ret = 0;
                    break;
                case SVR_CONTROL_LEISHI_GET_MAC_CMD:
                    SYS_IP_getMacInt(mac);
                    SVR_CONTROL_Send(mac,sizeof(mac));
                    SVR_CONTROL_printf("**********get cmd: get mac addr ***************\n");
                    ret = 0;
                default:
                    SVR_CONTROL_printf("**********get cmd: error ***************\n");
                    break;

                }
            }//endif


        }

    }

    return ret;
}



/******************************************************************************

WV_S32 SVR_CONTROL_Leishi_MapSongType_DB() ;

男女对唱              浪漫
欢乐庆典              梦幻
民歌传唱              中国风    
经典老歌              梦幻
校园民谣              教室
影视金曲              舒缓
戏曲片段              京剧
友谊万岁              浪漫
网络歌曲              梦幻
纯音乐                浪漫
心碎伤感              雪景
经典翻唱              梦幻
甜蜜幸福              浪漫
寂寞空灵              梦幻
异国风情              宫殿教堂
高清歌曲              梦幻
流行歌曲              梦幻
外国歌曲              梦幻
生日快乐              生日
大海                  海景
草原歌曲              草原歌曲
军旅歌曲              军旅歌曲
儿童歌曲              儿童歌曲
歌颂祖国              歌颂祖国
disco                 disco   
******************************************************************************/  
WV_S32 SVR_CONTROL_Leishi_MapSongType(WV_U8 *pName,WV_U8 *pOut)  
{
    //printf("goto SVR_CONTROL_Leishi_MapSongType\n");
    WV_S32 ret;

    WV_U8 nameToUtf8[16]={0};
    WV_U8 nameMap[16]={0};
    WV_U32 len=strlen(pName);
    //if(len > 8) return WV_EFAIL;
    //printf("the len is %d\n",len);
    if(len > 12)
        return WV_EFAIL;
    //判断是否是utf8
    char watch[16];
    //char *watch;
    int watchNum;
    strcpy(watch,pName);
    watchNum = is_utf8_string(watch);
    if(watchNum)
    {
        //printf("the pName is utf8\n");
        strcpy(nameToUtf8,pName);
    }
    else
    {
        //printf("the pName is not utf8\n");
        ret = gb2312toutf8(pName,len,nameToUtf8,sizeof(nameToUtf8));
        if(ret != 0 )
        {
            WV_ERROR(" Map song type is error \n");
            return WV_EFAIL;
        }
    }
    //由于不确定http发来的编码类型是否是gb3212还是utf-8,所以将len的长度控制在12,
    //如果gb2312toutf8执行错误，也不返回，仅仅打印出错，并将nameToUtf8 自动等于pName。
    if(strcmp(nameToUtf8,"男女对唱") == 0){
        sprintf(nameMap,"浪漫");
    }else if(strcmp(nameToUtf8,"欢乐庆典") == 0){
        sprintf(nameMap,"梦幻");
    }else if(strcmp(nameToUtf8,"民歌传唱") == 0){
        sprintf(nameMap,"中国风");
    }else if(strcmp(nameToUtf8,"经典老歌") == 0){
        sprintf(nameMap,"梦幻");
    }else if(strcmp(nameToUtf8,"校园民谣") == 0){
        sprintf(nameMap,"教室");
    }else if(strcmp(nameToUtf8,"影视金曲") == 0){
        sprintf(nameMap,"舒缓");
    }else if(strcmp(nameToUtf8,"戏曲片段") == 0){
        sprintf(nameMap,"京剧");
    }else if(strcmp(nameToUtf8,"友谊万岁") == 0){
        sprintf(nameMap,"浪漫");
    }else if(strcmp(nameToUtf8,"网络歌曲") == 0){
        sprintf(nameMap,"梦幻");
    }else if(strcmp(nameToUtf8,"纯音乐") == 0){
        sprintf(nameMap,"浪漫");
    }else if(strcmp(nameToUtf8,"心碎伤感") == 0){
        sprintf(nameMap,"雪景");
    }else if(strcmp(nameToUtf8,"经典翻唱") == 0){
        sprintf(nameMap,"梦幻");
    }else if(strcmp(nameToUtf8,"甜蜜幸福") == 0){
        sprintf(nameMap,"浪漫");
    }else if(strcmp(nameToUtf8,"寂寞空灵") == 0){
        sprintf(nameMap,"梦幻");
    }else if(strcmp(nameToUtf8,"异国风情") == 0){
        sprintf(nameMap,"宫殿教堂");
    }else if(strcmp(nameToUtf8,"高清歌曲") == 0){
        sprintf(nameMap,"梦幻");
    }else if(strcmp(nameToUtf8,"流行歌曲") == 0){
        sprintf(nameMap,"梦幻");
    }else if(strcmp(nameToUtf8,"外国歌曲") == 0){
        sprintf(nameMap,"梦幻");
    }else if(strcmp(nameToUtf8,"生日快乐") == 0){
        sprintf(nameMap,"生日");
    }else if(strcmp(nameToUtf8,"大海") == 0){
        sprintf(nameMap,"海景");
    }else if(strcmp(nameToUtf8,"草原歌曲") == 0){
        sprintf(nameMap,"草原歌曲");
    }else if(strcmp(nameToUtf8,"军旅歌曲") == 0){
        sprintf(nameMap,"军旅歌曲");
    }else if(strcmp(nameToUtf8,"儿童歌曲") == 0){
        sprintf(nameMap,"儿童歌曲");
    }else if(strcmp(nameToUtf8,"歌颂祖国") == 0){
        sprintf(nameMap,"歌颂祖国");
    }else if(strcmp(nameToUtf8,"disco") == 0){
        sprintf(nameMap,"disco");
    }else{
        return WV_EFAIL;
    }
    if(utf8togb2312(nameMap,strlen(nameMap),pOut,8) != 0)
    {
        WV_ERROR(" Map song type is error \n");
        return WV_EFAIL;
    }

    return WV_SOK;
}

/******************************************************************************

WV_S32 SVR_CONTROL_Leishi_callback_DB(void* data, int ncols, char** values, char** headers) ;

******************************************************************************/  
WV_S32 SVR_CONTROL_Leishi_callback_DB(void* data, int ncols, char** values, char** headers)  
{
    //printf("goto SVR_CONTROL_Leishi_callback_DB\n");
    WV_S32 i,j;
    WV_S32 len =0;
    WV_S32 ll=0;
    SVR_CONTROL_KTV_CHANGEMODE_E ChangeMode = SVR_CONTROL_CHANGE_NULL;
    WV_U8 destbuf[16];
    WV_U8 srcbuf[16];
    memset(destbuf,0,sizeof(destbuf));
    memset(srcbuf,0,sizeof(srcbuf));
    for(i=0; i < ncols; i++)
    {
        if(strlen(headers[i])>len)
            len = strlen(headers[i]);
    }
    for(i=0; i < ncols; i++)
    {
        ll = len-strlen(headers[i]);
        while(ll)
        {
            SVR_CONTROL_printf(" \n");
            --ll;
        }
        //WV_printf("[%d]%s: %s\n",i, headers[i], values[i]);
        memcpy(srcbuf,values[i],strlen(values[i]));

        if(utf8togb2312(srcbuf,strlen(srcbuf),destbuf,sizeof(destbuf)) != 0)
        {
            return WV_EFAIL;
        }
        //SVR_CONTROL_printf("[%d]%s: %s\n",i, headers[i], values[i]);
        //根据歌曲类别查找视频
        ChangeMode = SVR_CONTROL_GetKtvChangeMode();
        if(SVR_CONTROL_CHANGE_MOV == ChangeMode )     //1:背景视频随动
        {
            TSK_CONF_changeMovByType(destbuf);
        }else if(ChangeMode == SVR_CONTROL_CHANGE_SCENE )//2:场景随动
        {
            TSK_CONF_changeSceneByType(destbuf);
        }
        gSvrLeishiDev.movChangeByID = 1;
        return WV_SOK;
    }
    //SVR_CONTROL_printf("\n");
    return WV_EFAIL;
}  

/****************************************************************************

WV_S32 SVR_CONTROL_LeiShi_ChangeMovByCmd(WV_S8 *pID,WV_S8 *pName)

功能：根据id或者name切换对应歌曲类型的视频
参数：pID是歌曲id(固定7个字节)，pName(最多8个字节)
返回值：成功返回0，不成功返回-1
****************************************************************************/ 
WV_S32 SVR_CONTROL_LeiShi_ChangeMovByCmd(WV_S8 *pID,WV_S8 *pName)
{
    //printf("goto SVR_CONTROL_LeiShi_ChangeMovByCmd\n");
    SVR_CONTROL_KTV_CHANGEMODE_E movChangeMode = SVR_CONTROL_CHANGE_NULL;
    movChangeMode = SVR_CONTROL_GetKtvChangeMode();
    //if id exist ,use id
    if(movChangeMode == SVR_CONTROL_CHANGE_NULL )
    {
        return WV_EFAIL;
    }

    //根据歌曲ID切换对应类别的视频,查询数据库
    WV_S8 sql[128];
    memset(sql,0,sizeof(sql));
    sprintf(sql,"select %s from %s where ID = '%s' ",SVR_CONTROL_LEISHI_DB_SONGCLASS_NAME,SVR_CONTROL_LEISHI_DB_TABLE_NAME,pID);
    //printf("\nsql:");
    //printf("%s\n",sql);
    gSvrLeishiDev.movChangeByID = 0;
    WV_SQLITE_SearchVal(SVR_CONTROL_LEISHI_DB_FILE_PATH ,sql,SVR_CONTROL_Leishi_callback_DB);
    if(gSvrLeishiDev.movChangeByID == 1)
        return WV_SOK; //movChangeByID == 1 ,说明mov已经changeByID，

    //map 雷石歌曲类别到背景视频类别
    //WV_U8 mapName[9]={0};
    WV_U8 mapName[9]={0};
    printf("no change move by id \n");
    if(SVR_CONTROL_Leishi_MapSongType(pName,mapName) != 0 )
    {
        printf("MapSongType error!!!\n");
        return WV_EFAIL;
    }

    //根据歌曲类别查找视频
    if(movChangeMode == SVR_CONTROL_CHANGE_MOV )     //1:背景视频随动 ;
    {
        TSK_CONF_changeMovByType(mapName);
    }else if(movChangeMode == SVR_CONTROL_CHANGE_SCENE )//2:场景随动
    {
        TSK_CONF_changeSceneByType(mapName);
    }

    return WV_SOK;
}
/****************************************************************************

WV_S32 SVR_CONTROL_LeiShi(WV_S8 *pData,WV_S32 len)
//雷石ktv网络数据*pData [2:Serialno:%s ClassName:%s LightIndex:%d]

****************************************************************************/
WV_S32 SVR_CONTROL_LeiShi(WV_S8 *pData,WV_S32 len)
{

    if(SVR_CONTROL_LeiShi_controlDev(pData,len) == 0 )
    {
        return WV_SOK;
    }

    if(len < 20)
    {
        return WV_SOK;
    }


    WV_S32 ret;
    WV_U8 id[16];
    WV_U8 name[8+1];
    WV_U32 lightIndex;
    WV_S32 i=0,j=0;
    WV_S8 *p;
    p=pData;
    WV_U8 num=0;

    memset(id,0,sizeof(id));
    memset(name,0,sizeof(name));

    for(i=0;i<4;i++)
    {
        while(1)
        {
            if(p[0] == ':')
            {

                num++;
                if(num >=len)
                {
                    return WV_EFAIL;
                }
                p++;
                break;
            }
            num++;
            if(num >=len)
            {
                return WV_EFAIL;
            }
            p++;
        }

        switch(i)
        {
        case 0:
            break;
        case 1:
            j=0;

            while(p[0] != ' ')
            {
                id[j]=p[0];
                num++;
                if(num >=len)
                {
                    return WV_EFAIL;
                }
                p++;
                j++;
            }
            break;
        case 2:
            j=0;
            while(p[0] != ' ')
            {
                name[j]=p[0];
                num++;
                if(num >=len)
                {
                    return WV_EFAIL;
                }
                p++;
                j++;
            }
            break;
        case 3:
            lightIndex = atoi(p);
            break;
        default:
            break;
        }
        p++;
    }

    WV_printf("id=%s,name=%s,lightIndex=%d\n",id,name,lightIndex);

    ret = SVR_CONTROL_LeiShi_ChangeMovByCmd(id,name);
    return ret;

}

/****************************************************************************

WV_S32 SVR_CONTROL_LeiShi_Init()

****************************************************************************/
WV_S32 SVR_CONTROL_LeiShi_Init()
{
    memset(&gSvrLeishiDev,0,sizeof(gSvrLeishiDev));
    return WV_SOK;
}

