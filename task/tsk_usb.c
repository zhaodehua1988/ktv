

#include"tsk_usb.h"
#include "tsk_usbShow.h"
#include "tsk_usbImport.h"
#define  TSK_USB_PATH            "/usb"
#define  TSK_USB_UPDATE_PATH     "/update"
#define  TSK_USB_MAIN_NAME       "hello"
//#define  TSK_USB_DEV             "/dev/sda1"

char TSK_USB_DEV[10];

typedef struct TSK_USB_DEV_E 
{
    WV_THR_HNDL_T thrHndl;
    WV_U32      open;
    WV_U32      close;
    WV_U32      plug;
    WV_U32      mount;
    WV_U32      plugCnt;
}TSK_USB_DEV_E;

static TSK_USB_DEV_E gUsbDev;
/********************************************
WV_S32 TSK_GET_USB_DEV()
查找usb设备文件
*********************************************/
WV_S32 TSK_GET_USB_DEV()
{
    WV_S32 dsk = 0;
    WV_S32 i = 0 ;
    WV_S8 dev[10];
    if(access("/dev/usb/by-devid/_/",F_OK) != WV_SOK)
    {
        //printf("usb disk is not here!\n");
        return WV_EFAIL;
    }
    else
    {
        if(WV_FILE_Access("/dev/sda") == WV_SOK)
        {
            //printf("usb disk a is here!\n");
            dsk = 1;
        }
        else if(WV_FILE_Access("/dev/sdb") == WV_SOK)
        {
            //printf("usb disk b is here!\n");
            dsk = 2;
        }
        else if(WV_FILE_Access("/dev/sdc") == WV_SOK)
        {
            //printf("usb disk c is here!\n");
            dsk = 3;
        }

        switch (dsk) {
        case 1:
            sprintf(dev,"/dev/sda");
            break;
        case 2:
            sprintf(dev,"/dev/sdb");
            break;
        case 3:
            sprintf(dev,"/dev/sdc");
            break;
        default:
            break;
        }
        for(i=0 ; i<10; i++)
        {
            WV_S8 num[2];
            sprintf(num,"%d",i);
            dev[8] = num[0];
            dev[9] = '\0';
            if(WV_FILE_Access(dev) != WV_SOK)
            {
                //printf("usb disk %d is not here!\n",i);
            }
            else {
               // printf("the usb disk is %s\n",dev);
                break;
            }
        }

        //printf("the last get is %s\n",dev);
    }
    strcpy(TSK_USB_DEV,dev);
    return WV_SOK;
}

/******************************************************************************

WV_S32 TSK_USB_Mount(TSK_USB_DEV_E  * pDev);

******************************************************************************/
WV_S32 TSK_USB_Mount(TSK_USB_DEV_E  * pDev)
{ 
    WV_S32  ret;

    //ret = WV_FILE_Access(TSK_USB_DEV);

    ret = TSK_GET_USB_DEV();
    if(ret == WV_SOK)
    {
        if(pDev->plug == 0)
        {
            pDev -> plug = 1;
            WV_printf("usb disk plug in\r\n");
            ret = mount(TSK_USB_DEV, TSK_USB_PATH,"vfat",0,"codepage=936,iocharset=gb2312");
            if(ret == 0)
            {
                pDev -> mount = 1;
                pDev -> plugCnt++;
                WV_printf("usb disk mount ok\r\n");

            }
            else
            {
                ret = mount(TSK_USB_DEV,TSK_USB_PATH,"vfat",MS_REMOUNT,"codepage=936,iocharset=gb2312");

                if(ret == 0)
                {
                    pDev -> mount = 1;
                    pDev -> plugCnt++;
                    WV_printf("usb disk re mount ok\r\n");
                }
                else
                {
                    WV_printf("usb disk mount fail\r\n");
                }
            }
            if(pDev->mount == 1){
                //update

                TSK_UP_Update();
                if(TSK_USBIMPROT_Proc() == 0 )
                {
                    //WV_CMD_GetReset();
                }
                TSK_USB_SetShowEna();
            }
        }
    }
    else
    {
        if(pDev -> mount == 1)
        {
            //TSK_USB_SetShowEna();
            pDev -> plug = 0;
            WV_printf("usb disk plug out\r\n");
            if((TSK_Mobile_GetShowFlag() & 0x1) == 0)
            {
                TSK_GO_DecClose(0);
            }
            if((TSK_Mobile_GetShowFlag() & 0x2) == 0){
                TSK_PLAYER_setWin(2,0,0,0,0);
                TSK_PLAYER_Destory(2);
            }
            ret = umount2(TSK_USB_PATH,0);
            if(ret == 0)
            {
                pDev -> mount = 0;
                WV_printf("usb disk unmount ok\r\n");
                //sleep(1);
                TSK_USB_SetShowEna();
            }
            else
            {
                WV_printf("%d\r\n",errno);
                WV_printf("usb disk unmount fail\r\n");
            }


        }
    }

    return WV_SOK;
}


/******************************************************************************

void *TSK_USB_Proc(Void * prm);
******************************************************************************/
void * TSK_USB_Proc ( void * prm)
{
    TSK_USB_DEV_E  * pDev;
    pDev = (TSK_USB_DEV_E  *) prm;

    pDev-> open  = 1;
    pDev-> close  = 0;
    //sleep(5);
    while(pDev -> open == 1)
    {
        TSK_USB_Mount(pDev);
        sleep(1);
    }
    
    pDev-> open  = 0;
    pDev-> close = 1;
    return NULL;

} 



/****************************************************************************************

WV_S32 TSK_USB_lsDir(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************************/
WV_S32 TSK_USB_GetUsb(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{ 

    DIR    *dir;
    WV_S8  dirPath[30];
    WV_S8 filePath[128];
    struct    dirent    *ptr;
    struct stat statBuf;
    WV_S32 ret;
    if(argc <=  0)
    {
        prfBuff += sprintf(prfBuff,"get usb <cmd> \r\n");
        prfBuff += sprintf(prfBuff,"get usb ls  : get all file on udb disk \r\n");
        return WV_SOK;
    }

    // cmd ls
    if(0 == strcmp(argv[0],"ls"))

    {
        prfBuff += sprintf(prfBuff,"plug   =  %d\r\n",gUsbDev.plug);
        prfBuff += sprintf(prfBuff,"mount  =  %d\r\n",gUsbDev.mount);
        
        if(gUsbDev.plug == 0)
        {
            prfBuff += sprintf(prfBuff,"usb not plug\r\n");
            return 0;
        }
        if(gUsbDev.mount == 0)
        {
            prfBuff += sprintf(prfBuff,"usb not mount\r\n");
            return 0;
        }

        if(argc < 1)
        {
            sprintf(dirPath,"%s",TSK_USB_PATH);
        }
        else
        {
            sprintf(dirPath,"%s/%s",TSK_USB_PATH,argv[1]);
        }
        dir = opendir(dirPath);
        if(dir == NULL)
        {
            prfBuff += sprintf(prfBuff," %s not exist!!!\r\n",  dirPath);
            return WV_SOK;
        }
        prfBuff += sprintf(prfBuff,"FileName\r\n");
        while((ptr = readdir(dir)) != NULL)
        {

            prfBuff += sprintf(prfBuff,"********************\r\n");
            prfBuff += sprintf(prfBuff,"name     = %s\r\n", ptr->d_name);
            prfBuff += sprintf(prfBuff,"node num = %d\r\n", ptr->d_ino);
            prfBuff += sprintf(prfBuff,"node off = %d \r\n",ptr->d_off);
            prfBuff += sprintf(prfBuff,"type     = %d \r\n",ptr->d_type);
            prfBuff += sprintf(prfBuff,"len      = %d \r\n",ptr->d_reclen );
            sprintf(filePath,"%s/%s",dirPath,ptr->d_name);
            ret = stat(filePath,&statBuf);
            if(ret != 0)
            {
                prfBuff += sprintf(prfBuff,"    get %s stat erro !! \r\n",ptr->d_name);
                continue;
            }
            prfBuff += sprintf(prfBuff,"size   = %d \r\n",statBuf.st_size );

        }
        closedir(dir);
    }
    //
    
    return 0;

}




/****************************************************************************************

WV_S32 TSK_USB_GetLs(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************************/
WV_S32 TSK_USB_GetLs(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{ 
    DIR    *dir;
    WV_S8  dirPath[30];
    WV_S8 filePath[128];
    struct    dirent    *ptr;
    struct stat statBuf;
    WV_S32 ret;
    sprintf(dirPath,"%s",TSK_USB_PATH);

    if(argc > 0)
    {
        sprintf(dirPath,"%s",argv[0]);
    }
    // cmd ls
    if(0 == strcmp(dirPath,TSK_USB_PATH))
    {
        prfBuff += sprintf(prfBuff,"plug   =  %d\r\n",gUsbDev.plug);
        prfBuff += sprintf(prfBuff,"mount  =  %d\r\n",gUsbDev.mount);
        
        if(gUsbDev.plug == 0)
        {
            prfBuff += sprintf(prfBuff,"usb not plug\r\n");
            return 0;
        }
        if(gUsbDev.mount == 0)
        {
            prfBuff += sprintf(prfBuff,"usb not mount\r\n");
            return 0;
        }
    }

    dir = opendir(dirPath);
    if(dir == NULL)
    {
        prfBuff += sprintf(prfBuff," %s not exist!!!\r\n",  dirPath);
        return WV_SOK;
    }
    prfBuff += sprintf(prfBuff,"File Info:\r\n");
    while((ptr = readdir(dir)) != NULL)
    {

        sprintf(filePath,"%s/%s",dirPath,ptr->d_name);
        ret = stat(filePath,&statBuf);
        if(ret != 0)
        {
            prfBuff += sprintf(prfBuff,"    get %s stat erro !! \r\n",ptr->d_name);
            continue;
        }
        prfBuff += sprintf(prfBuff,"%40s %2d %8x %8d\r\n",ptr->d_name,ptr->d_type,statBuf.st_mode,statBuf.st_size );

    }
    closedir(dir);
    //
    
    return 0;

}




/****************************************************************************************

WV_S32 TSK_USB_GetStat(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************************/
WV_S32 TSK_USB_GetStat(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{ 
    
    WV_S8  dirPath[30];
    struct statfs sfs;
    WV_S32 ret,lenM;
    sprintf(dirPath,"%s", TSK_USB_PATH );
    
    if(argc > 0 )
    {
        sprintf(dirPath,"%s", argv[0] );
    }
    prfBuff += sprintf(prfBuff,"get %s stat:\r\n",dirPath);
    ret =  statfs(dirPath, &sfs);
    if(ret != 0)
    {
        prfBuff += sprintf(prfBuff,"get %s stat erro !!!\r\n",dirPath);
        return 0;
    }

    prfBuff += sprintf(prfBuff,"block size : %d\r\n \r\n",sfs.f_bsize);
    prfBuff += sprintf(prfBuff,"block all  : %d\r\n \r\n",sfs.f_blocks);
    prfBuff += sprintf(prfBuff,"block free : %d\r\n \r\n",sfs.f_bfree);
    prfBuff += sprintf(prfBuff,"block avail : %d\r\n \r\n",sfs.f_bavail);

    WV_FILE_GetAvail(dirPath,&lenM);

    prfBuff += sprintf(prfBuff,"avail %d MB\r\n \r\n",lenM);

    return 0;

}



/****************************************************************************************

WV_S32 TSK_USB_GetCpy(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************************/
WV_S32 TSK_USB_GetCpy(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{  
    WV_S32 ret = 0;
    if(argc <  2)
    {
        prfBuff += sprintf(prfBuff,"usb cpy <pathScr> <pathDst> \r\n");
        return WV_SOK;
    }

    ret = WV_FILE_Cpy(argv[0],argv[1]);
    if(ret == WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"cpy %s to  %s ok \r\n",argv[0],argv[1]);
    }
    else
    {
        prfBuff += sprintf(prfBuff,"cpy %s to  %s erro!!!! \r\n",argv[0],argv[1]);
    }
    return WV_SOK;

}

/****************************************************************************************

WV_S32 TSK_USB_GetCpy(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************************/
WV_S32 TSK_USB_GetDel(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{  
    WV_S32 ret = 0;
    if(argc <  1)
    {
        prfBuff += sprintf(prfBuff,"usb del  <file> \r\n");
        return WV_SOK;
    }
    ret = WV_FILE_Access(argv[0]);
    if(ret != WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"%s not exist !!! \r\n",argv[0]);
        return  WV_SOK;
    }
    
    ret = WV_FILE_Remove(argv[0]);
    if(ret == WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"del %s ok \r\n",argv[0]);
    }
    else
    {
        prfBuff += sprintf(prfBuff,"del %s erro!!! \r\n",argv[0]);
    }
    return 0;

}



/****************************************************************************************

WV_S32 TSK_USB_GetUpdate(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************************/
WV_S32 TSK_USB_GetUpdate(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{  
    WV_S32 ret = 0;
    WV_S8 scrFile[64];
    WV_S8 desFile[64];
    WV_S8 backFile[64];
    
    sprintf(scrFile,"%s%s/%s",TSK_USB_PATH,TSK_USB_UPDATE_PATH,TSK_USB_MAIN_NAME);
    sprintf(backFile,"./%s_b",TSK_USB_MAIN_NAME);
    sprintf(desFile,"./%s",TSK_USB_MAIN_NAME);
    
    if(argc > 1)
    {
        sprintf(scrFile,"%s%s%s",TSK_USB_PATH,TSK_USB_UPDATE_PATH,argv[0]);
    }
    ret = WV_FILE_Access(scrFile);
    if(ret != WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"%s file not exist !!! \r\n",scrFile);
        return  WV_SOK;
    }
    
    ret = WV_FILE_Access(desFile);
    if(ret != WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"%s file not exist !!! \r\n",desFile);
        return  WV_SOK;
    }

    // back fiele
    ret = WV_FILE_Cpy(desFile,backFile);
    if(ret == WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"cpy %s to  %s ok \r\n",desFile,backFile);
    }
    else
    {
        prfBuff += sprintf(prfBuff,"cpy %s to  %s erro !!!\r\n",desFile,backFile);
        return  WV_SOK;
    }

    //update file
    ret = WV_FILE_Cpy(scrFile,desFile);
    if(ret == WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"cpy %s to  %s ok \r\n",scrFile,desFile);
    }
    else
    {
        prfBuff += sprintf(prfBuff,"cpy %s to  %s erro !!!\r\n",desFile,backFile);
    }
    
    //
    if(ret == WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"update ok\r\n");
        return WV_SOK;
    }

    //recover old file
    ret = WV_FILE_Cpy(backFile,desFile);
    if(ret == WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"cpy %s to  %s ok \r\n",backFile,desFile);
    }
    else
    {
        prfBuff += sprintf(prfBuff,"cpy %s to  %s erro !!!\r\n",backFile,desFile);

    }
    prfBuff += sprintf(prfBuff,"recovery erro !!!\r\n");
    return 0;

}


/****************************************************************************************

WV_S32 TSK_USB_GetMov(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************************/
WV_S32 TSK_USB_GetMov(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{  
    WV_S32 ret = 0;
    WV_S8 scrFile[64];
    WV_S8 desFile[64];
    WV_S8 scrPath[64];
    WV_S32  i;


    sprintf(scrPath,"%s%s/mov",TSK_USB_PATH,TSK_USB_UPDATE_PATH);
    if(argc > 1)
    {
        sprintf(scrPath,"TSK_USB_PATH/%s",argv[0]);
    }
    ret = WV_FILE_Access(scrPath);
    if(ret != WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"%s file not exist !!! \r\n",scrPath);
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
            prfBuff += sprintf(prfBuff,"%s file not exist !!! \r\n",scrFile);
            continue;
        }
        ret = WV_FILE_Cpy(scrFile,desFile);

        if(ret == WV_SOK)
        {
            prfBuff += sprintf(prfBuff,"cpy %s to  %s ok \r\n",scrFile,desFile);
        }
        else
        {
            prfBuff += sprintf(prfBuff,"cpy %s to  %s erro !!!\r\n",scrFile,desFile);
        }
    }
    
    return 0;

}


/****************************************************************************************

WV_S32 TSK_USB_GetAni(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)

****************************************************************************************/
WV_S32 TSK_USB_GetAni(WV_S32 argc, WV_S8 **argv,WV_S8 *prfBuff)
{  
    WV_S32 ret = 0;
    WV_S8 scrFile[64];
    WV_S8 desFile[64];
    WV_S8 scrPath[64];
    WV_S8 desPath[64];
    WV_S32  i,j;

    sprintf(scrPath,"%s%s/png",TSK_USB_PATH,TSK_USB_UPDATE_PATH);
    if(argc > 1)
    {
        sprintf(scrPath,"TSK_USB_PATH/%s",argv[0]);
    }

    ret = WV_FILE_Access(scrPath);
    if(ret != WV_SOK)
    {
        prfBuff += sprintf(prfBuff,"%s file not exist !!! \r\n",scrPath);
        return WV_SOK;
    }
    ret = WV_FILE_Access("./png");
    if(ret != WV_SOK)
    {
        mkdir("./png",0755);
    }

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
        while(1)
        {
            sprintf(scrFile,"%s/png%d/%d.png",scrPath,i,j);
            sprintf(desFile,"./png/png%d/%d.png",i,j);
            ret = WV_FILE_Access(scrFile);
            if(ret != WV_SOK)
            {
                prfBuff += sprintf(prfBuff,"%s file not exist !!! \r\n",scrFile);
                break;
            }

            ret = WV_FILE_Cpy(scrFile,desFile);
            if(ret == WV_SOK)
            {
                j++;
                //prfBuff += sprintf(prfBuff,"cpy %s to  %s ok \r\n",scrFile,desFile);
            }
            else
            {
                // prfBuff += sprintf(prfBuff,"cpy %s to  %s erro !!!\r\n",scrFile,desFile);
                break;
            }
        }

        prfBuff += sprintf(prfBuff,"cpy ani  %d fiel  %d \r\n",i,j);
    }
    
    return 0;
}

/****************************************************************************************

WV_S32 TSK_USB_Open();

****************************************************************************************/

WV_S32 TSK_USB_Open()
{ 
    WV_S32 ret = 0;
    WV_CMD_Register("get","usb","get usb info",TSK_USB_GetUsb);
    WV_CMD_Register("usb","ls","list dir file",TSK_USB_GetLs);
    WV_CMD_Register("usb","cpy","cpy file",TSK_USB_GetCpy);
    WV_CMD_Register("usb","del","remove file",TSK_USB_GetDel);
    WV_CMD_Register("usb","update","update file",TSK_USB_GetUpdate);
    WV_CMD_Register("usb","stat","get  fs stat ",TSK_USB_GetStat);
    WV_CMD_Register("usb","mov","get  mov to local ",TSK_USB_GetMov);
    WV_CMD_Register("usb","ani","get  ani to local ",TSK_USB_GetAni);

    memset(&gUsbDev,0,sizeof(gUsbDev));
    ret = WV_FILE_Access(TSK_USB_PATH);
    if(ret != WV_SOK)
    {

        mkdir(TSK_USB_PATH,0755);
        WV_printf("creat usb dir\r\n");
    }
    TSK_USB_Mount(&gUsbDev);
    WV_RET_ADD( WV_THR_Create(&gUsbDev.thrHndl, TSK_USB_Proc , WV_THR_PRI_DEFAULT, WV_THR_STACK_SIZE_DEFAULT, &gUsbDev),ret);

    return ret;
}

/****************************************************************************************

WV_S32  TSK_USB_Close();

****************************************************************************************/

WV_S32  TSK_USB_Close()
{ 

    WV_S32 ret = 0;
    if(gUsbDev.open == 1)
    {
        gUsbDev.open = 0;
        while(gUsbDev.close == 1) ;
        WV_CHECK_RET( WV_THR_Destroy(&gUsbDev.thrHndl));
    }
    return ret;
}

