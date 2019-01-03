#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdlib.h>

#include "wv_common.h"
#include "tsk_conf.h"
#include "tsk_scene.h"
#include "tsk_uart.h"
#include "sys_ip.h"
#include "wv_sqlite3.h"
#include "svr_control_cmd.h"
#include "net_uart.h"

#include "cJSON.h"

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"


#define SERVER_PORT 4322

cJSON *json_get = 0;
pthread_t newthread;
//Switch song
int SWITCH_SONG_YTPE = 0;                      //切换歌曲标志位
typedef struct ORDER_POST_HTTP
{
    cJSON *command;
    cJSON *className;
    cJSON *lightCode;
    cJSON *serialNO;

}ORDER_POST_HTTP;

typedef struct THE_ORDER_USE
{
    char *the_order_get;
    char *the_song_category;
    int   the_light_code;
    char *the_serial_num;
    //char  pid[10];
}THE_ORDER_USE;

ORDER_POST_HTTP JsonOrder;
THE_ORDER_USE UseOrder;


//the 200
void KTV_LEISHI_RecvOk(int client)
{
    char buf[1024];
    cJSON * usr_send;
    usr_send=cJSON_CreateObject();   //创建根数据对象
    cJSON_AddNumberToObject(usr_send,"code",200);  //加整数
    cJSON_AddStringToObject(usr_send,"msg","OK");  //加入键值，加字符串
    char *out = cJSON_Print(usr_send);   //将json形式打印成正常字符串形式
    int length = strlen(out);
    char leng[4];
    sprintf(leng,"%d",length);

    /* 200 页面 */
    sprintf(buf, "HTTP/1.1 200 OK\r\n");
    strcat(buf, "Server: Apache-Coyote/1.1\r\n");
    strcat(buf, "Content-Type:application/json\r\n");
    strcat(buf, "Content-Length: ");
    strcat(buf, leng);
    strcat(buf, "\r\n\r\n");
    strcat(buf,out);
    send(client, buf, strlen(buf), 0);

    //send(client, out, strlen(out), 0);
    // 释放内存
    cJSON_Delete(usr_send);
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void KTV_LEISHI_RECV_Nofound(int client)
{
    char buf[1024];
    cJSON * usr_send;

    usr_send=cJSON_CreateObject();   //创建根数据对象
    cJSON_AddNumberToObject(usr_send,"code",404);  //加整数
    cJSON_AddStringToObject(usr_send,"msg","NOT FOUND");  //加入键值，加字符串
    char *out = cJSON_Print(usr_send);   //将json形式打印成正常字符串形式

    int length = strlen(out);
    char leng[4];
    sprintf(leng,"%d",length);

    /* 404 页面 */
    sprintf(buf, "HTTP/1.1 404 NOT FOUND\r\n");
    strcat(buf, "Server: Apache-Coyote/1.1\r\n");
    strcat(buf, "Content-Type:application/json\r\n");
    strcat(buf, "Content-Length: ");
    strcat(buf, leng);
    strcat(buf, "\r\n\r\n");
    strcat(buf,out);
    send(client, buf, strlen(buf), 0);

    //send(client, out, strlen(out), 0);
    // 释放内存
    cJSON_Delete(usr_send);
}


/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void KTV_LEISHI_RECV_Unimplemented(int client)
{
    char buf[1024];

    cJSON * usr_send;
    usr_send=cJSON_CreateObject();   //创建根数据对象
    cJSON_AddNumberToObject(usr_send,"code",501);  //加整数
    cJSON_AddStringToObject(usr_send,"msg","Method Not Implemented");  //加入键值，加字符串
    char *out = cJSON_Print(usr_send);   //将json形式打印成正常字符串形式

    int length = strlen(out);
    char leng[4];
    sprintf(leng,"%d",length);

    /* 501 页面 */
    sprintf(buf, "HTTP/1.1 501 Method Not Implemented\r\n");
    strcat(buf, "Server: Apache-Coyote/1.1\r\n");
    strcat(buf, "Content-Type:application/json\r\n");
    strcat(buf, "Content-Length: ");
    strcat(buf, leng);
    strcat(buf, "\r\n\r\n");
    strcat(buf,out);
    send(client, buf, strlen(buf), 0);
    // send(client, out, strlen(out), 0);
    // 释放内存
    cJSON_Delete(usr_send);
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void KTV_LEISHI_RECV_BadRequest(int client)
{
    char buf[1024];
    cJSON * usr_send;

    usr_send=cJSON_CreateObject();   //创建根数据对象
    cJSON_AddNumberToObject(usr_send,"code",400);  //加整数
    cJSON_AddStringToObject(usr_send,"msg","BAD REQUEST");  //加入键值，加字符串
    char *out = cJSON_Print(usr_send);   //将json形式打印成正常字符串形式
    send(client, out, strlen(out), 0);

    int length = strlen(out);
    char leng[4];
    sprintf(leng,"%d",length);

    sprintf(buf, "HTTP/1.1 400 BAD REQUEST\r\n");
    strcat(buf, "Server: Apache-Coyote/1.1\r\n");
    strcat(buf, "Content-Type:application/json\r\n");
    strcat(buf, "Content-Length: ");
    strcat(buf, leng);
    strcat(buf, "\r\n\r\n");
    strcat(buf,out);
    send(client, buf, strlen(buf), 0);
    // 释放内存
    cJSON_Delete(usr_send);
}


/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void KTV_LEISHI_RECV_CannotExecute(int client)
{

    char buf[1024];
    cJSON * usr_send;

    usr_send=cJSON_CreateObject();   //创建根数据对象
    cJSON_AddNumberToObject(usr_send,"code",500);  //加整数
    cJSON_AddStringToObject(usr_send,"msg","Internal Server Error");  //加入键值，加字符串
    char *out = cJSON_Print(usr_send);   //将json形式打印成正常字符串形式

    int length = strlen(out);
    char leng[4];
    sprintf(leng,"%d",length);

    /*  页面 */
    sprintf(buf, "HTTP/1.1 500 Internal Server Error\r\n");
    strcat(buf, "Server: Apache-Coyote/1.1\r\n");
    strcat(buf, "Content-Type:application/json\r\n");
    strcat(buf, "Content-Length: ");
    strcat(buf, leng);
    strcat(buf, "\r\n\r\n");
    strcat(buf,out);
    send(client, buf, strlen(buf), 0);
    // 释放内存
    cJSON_Delete(usr_send);

    /* 回应客户端 cgi 无法执行*/
}


/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, lightCode indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
void error_die(const char *sc)
{
    /*出错信息处理 */
    perror(sc);
    exit(1);
}

/*****************************************************/
//解析命令
/*****************************************************/
int KTV_LEISHI_ParseCommand(int client)
{
    JsonOrder.command = cJSON_GetObjectItem(json_get,"command");
    JsonOrder.className = cJSON_GetObjectItem(json_get,"className");
    JsonOrder.lightCode = cJSON_GetObjectItem(json_get,"lightCode");
    JsonOrder.serialNO = cJSON_GetObjectItem(json_get,"serialNO");

    if(JsonOrder.command)
    {
        UseOrder.the_order_get = JsonOrder.command->valuestring;
        if(UseOrder.the_order_get)
            printf("the order is ******* %s\n",UseOrder.the_order_get);
        else
        {
            UseOrder.the_order_get = (char *)malloc(16);
            strcpy(UseOrder.the_order_get,"");
        }
    }


    if(JsonOrder.className)
    {
        UseOrder.the_song_category= JsonOrder.className->valuestring;
        if(UseOrder.the_song_category)
        {
            printf("the song category is ******* %s\n",UseOrder.the_song_category);
            printf("the length of song cate is %d\n",strlen(UseOrder.the_song_category));
        }
        else {
            printf("no the_song_category\n");
            UseOrder.the_song_category = (char *)malloc(16);
            strcpy(UseOrder.the_song_category,"");
            printf("get the song class to %s\n",UseOrder.the_song_category);
        }
    }

    if(JsonOrder.serialNO)
    {

        UseOrder.the_serial_num = JsonOrder.serialNO->valuestring;
        if(UseOrder.the_serial_num)
        {
            printf("the num is ******* %s\n",JsonOrder.serialNO->valuestring);
            printf("the serial num is ******* %s\n",UseOrder.the_serial_num);
        }
        else
        {
            UseOrder.the_serial_num = (char *)malloc(16);
            strcpy(UseOrder.the_serial_num,"");
        }
    }

    if(JsonOrder.lightCode)
    {
        UseOrder.the_light_code = JsonOrder.lightCode->valueint;
        if(UseOrder.the_light_code)
            printf("the light is ******* %d\n",UseOrder.the_light_code);
        else
        {
            UseOrder.the_light_code = (int)malloc(16);
            UseOrder.the_light_code = -1;
        }
    }


    return WV_SOK;
}


/***********************************************************************/
//executive command 执行命令
/***********************************************************************/
int KTV_LEISHI_ExecutiveCommand(int client)
{
    if(strcmp(UseOrder.the_order_get,"OPEN") == 0)
    {
        //因为open设备函数，最终调用了“reboot”系统命令，所以，应该先开投影仪，再open设备
        if(((NET_UART_ProjectorCmd(1) ==0) && (TSK_SCENE_StartingUP(TSK_SCENE_TYPE_NETDATA) == 0)))
            KTV_LEISHI_RecvOk(client);
        else
        {
            KTV_LEISHI_RECV_BadRequest(client);
            return WV_EFAIL;
        }
    }
    else if(strcmp(UseOrder.the_order_get,"CLOSE") == 0)
    {
        if((TSK_SCENE_Standby(TSK_SCENE_TYPE_NETDATA) == 0) && (NET_UART_ProjectorCmd(0) == 0))
            KTV_LEISHI_RecvOk(client);
        else
        {
            KTV_LEISHI_RECV_BadRequest(client);
            return WV_EFAIL;
        }
    }
    else if(strcmp(UseOrder.the_order_get,"PAUSE") == 0)
    {
        return WV_SOK;
    }
    else if(strcmp(UseOrder.the_order_get,"RESUME") == 0)
    {
        return WV_SOK;
    }
    else if (strcmp(UseOrder.the_order_get,"PLAYSONG") == 0) {
        if(SVR_CONTROL_LeiShi_ChangeMovByCmd(UseOrder.the_serial_num,UseOrder.the_song_category) == 0)
            KTV_LEISHI_RecvOk(client);
        else
        {
            KTV_LEISHI_RECV_Nofound(client);
            return WV_EFAIL;
        }
    }
    else if (strcmp(UseOrder.the_order_get,"RANDOM") == 0) {
        if(SVR_CONTROL_LeiShi_ChangeMovByCmd(UseOrder.the_serial_num,UseOrder.the_song_category) == 0)
            KTV_LEISHI_RecvOk(client);
        else
        {
            KTV_LEISHI_RECV_Nofound(client);
            return WV_EFAIL;
        }
    }else
    {
        KTV_LEISHI_RECV_BadRequest(client);
        return WV_EFAIL;
    }
    return WV_SOK;
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null)
 *解析包头
*/
/**********************************************************************/
int KTV_LEISHI_AnalyseLine(int sock, char *buf, int size)
{ int i = 0;
    char c = '\0';
    int n;

    /*把终止条件统一为 \n 换行符，标准化 buf 数组*/
    while ((i < size - 1) && (c != '\n'))
    {
        /*一次仅接收一个字节*/
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            /*收到 \r 则继续接收下个字节，因为换行符可能是 \r\n */
            if (c == '\r')
            {
                /*使用 MSG_PEEK 标志使下一次读取依然可以得到这次读取的内容，可认为接收窗口不滑动*/
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                /*但如果是换行符则把它吸收掉*/
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            /*存到缓冲区*/
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';
    //printf("------%d\n",i);
    /*返回 buf 数组大小*/
    return(i);
}

/*********************************************************************
//这个是解析收到的json数据。
int KTV_LEISHI_AnalyseJson(int sock, char *buf, int size)；
**********************************************************************/
int KTV_LEISHI_AnalyseJson(int sock, char *buf, int size)
{
    //printf("go to get json\n");
    char c ;
    int n , i = 0;

    while ((i < size -1) && (c != '}')) {
        n = recv(sock, &c, 1, 0);
        buf[i] = c;
        i++;
        // printf("get i is %d\n",i);
    }
    buf[i++] = '}';
    printf("the json get is %s\n",buf);
    //json是json对象指针,json_name是 name对象的指针,json_age是age对象的指针

    //把收到的json字符串打包成json
    json_get = cJSON_Parse(buf);

    //判断是否打包成功
    if(json_get == NULL)
    {
        printf("json pack into cjson error…\n");
    }
    else{//打包成功调用cJSON_Print打印输出
        //cJSON_Print(json_get);
        //char *out = cJSON_Print(json_get);
        //printf("mmmmmmmmmmm %s\n",out);
    }
    return(i);
}
/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
void KTV_LEISHI_ExecuteCgi(int client, const char *path, const char *method, const char *query_string)
{
    char buf[1024];

    int numchars = 1;
    int content_length = -1;

    buf[0] = 'A'; buf[1] = '\0';
    /* POST */

    /* 对 POST 的 HTTP 请求中找出 content_length */
    numchars = KTV_LEISHI_AnalyseLine(client, buf, sizeof(buf));
    // printf("+++++%s\n",buf);
    while ((numchars > 0) && strcmp("\n", buf))
    {
        /*利用 \0 进行分隔 */
        buf[15] = '\0';
        /* HTTP 请求的特点*/
        if (strcasecmp(buf, "Content-Length:") == 0)
            content_length = atoi(&(buf[16]));
        // printf("****contenr_lenth is : %d\n",content_length);
        numchars = KTV_LEISHI_AnalyseLine(client, buf, sizeof(buf));            //这行代码读出了空行，空行一下就是json命令了
        // printf("xxxxx numchars is %d\n",numchars);
    }

    /*没有找到 content_length */
    if (content_length == -1) {
        /*错误请求*/
        printf("no Content-Length!\n");
        KTV_LEISHI_RECV_BadRequest(client);
        return ;
    }

    int orderfd;
    //将接受到的json字符串打包道json结构体里面去。
    orderfd = KTV_LEISHI_AnalyseJson(client, buf, content_length);
    printf("the orderfd is %d\n",orderfd);

    //发送正确命令
    //    KTV_LEISHI_RecvOk(client);
    int temp = 0;
    temp = KTV_LEISHI_ParseCommand(client);
    if(temp != WV_EFAIL)
        KTV_LEISHI_ExecutiveCommand(client);
    else
        return  ;

}





/********************************************************

WV_S32 KTV_LEISHI_Init(u_short *port);

*********************************************************/ 
int KTV_LEISHI_Init(u_short *port)
{
    int httpd = 0;
    struct sockaddr_in name;

    /*建立 socket */
    httpd = socket(PF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
        error_die("socket");
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    //name.sin_port = htons(*port);
    name.sin_port = htons(SERVER_PORT);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
        error_die("bind");
    /*如果当前指定端口是 0，则动态随机分配一个端口*/
    if (*port == 0)  /* if dynamically allocating a port */
    {
        int namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name, (socklen_t *)&namelen) == -1)
            error_die("getsockname");
        *port = ntohs(name.sin_port);
    }
    /*开始监听*/
    if (listen(httpd, 5) < 0)
        error_die("listen");
    /*返回 socket id */
    return(httpd);
}

/********************************************************

void * KTV_LEISHI_Proc(int client);

*********************************************************/ 
void * KTV_LEISHI_Proc(void * client)
{
    WV_S32 *clientInt;
    clientInt = (WV_S32 *)client;
    printf("go to accept_request!\n");
    char buf[1024];
    int numchars;
    char method[255];
    char url[255];
    char path[512];
    size_t i, j;
    //struct stat st;
    int cgi = 0;      /* becomes true if server decides this is a CGI program */
    char *query_string = NULL;

    /*得到请求的第一行*/
    numchars = KTV_LEISHI_AnalyseLine(*clientInt, buf, sizeof(buf));
    //    printf("*****%s",buf);   //解析出来了：*****POST /thunder/command HTTP/1.1
    i = 0; j = 0;
    /*把客户端的请求方法存到 method 数组*/
    while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
    {
        method[i] = buf[j];
        i++; j++;
    }
    method[i] = '\0';
    //    printf("method is %s\n",method);    //解析出来了：method is POST
    /*如果既不是 GET 又不是 POST 则无法处理 */
    if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
    {
        KTV_LEISHI_RECV_Unimplemented(*clientInt);
        return NULL;
    }

    /* POST 的时候开启 cgi */
    if (strcasecmp(method, "POST") == 0)
        cgi = 1;

    /*读取 url 地址*/
    i = 0;
    while (ISspace(buf[j]) && (j < sizeof(buf)))
        j++;
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
    {
        /*存下 url */
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';
    //printf("url is %s\n",url);  //解析出来了：url is /thunder/command

    /*格式化 url 到 path 数组，html 文件都在 htdocs 中*/
    //sprintf(path, "htdocs%s", url);
    strcpy(path,url);
    //printf("path is %s\n",path);
    /*默认情况为 index.html */
    if (path[strlen(path) - 1] == '/')
        ;
    //strcat(path, "index.html");
    else
    {
        path[strlen(path)] = '/';
        path[strlen(path)] = '\0';
        //strcat(path, "index.html");
    }
    // printf("the path under is: %s\n",path);

    KTV_LEISHI_ExecuteCgi(*clientInt, path, method, query_string);

    //发送正确命令
    //KTV_LEISHI_RecvOk(client);
    //    KTV_LEISHI_ParseCommand();
    //    KTV_LEISHI_ExecutiveCommand(client);

    /*断开与客户端的连接（HTTP 特点：无连接）*/
    close(*clientInt);
    return NULL;
}

/********************************************************

WV_S32 KTV_LEISHI_Open();

*********************************************************/ 
int KTV_LEISHI_Open()
{
    int server_sock = -1;
    u_short port = 0;
    int client_sock = -1;
    struct sockaddr_in client_name;
    int client_name_len = sizeof(client_name);

    //在配置文件里增加雷石新版本标志为4,建立线程前，先判断是否是该版本
    if(SVR_CONTROL_GetKtvDev() != 4)
    {
        printf("this is not new ktv_leishi dev\n");
        return WV_EFAIL;
    }

    /*在对应端口建立 httpd 服务*/
    server_sock = KTV_LEISHI_Init(&port);
    printf("httpd running on port %d\n", port);

    while (1)
    {
        /*套接字收到客户端连接请求*/
        client_sock = accept(server_sock,(struct sockaddr *)&client_name,(socklen_t *)&client_name_len);
        if (client_sock == -1)
            error_die("accept");
        /*派生新线程用 accept_request 函数处理新请求*/
        /* accept_request(client_sock); */
        if (pthread_create(&newthread , NULL,  KTV_LEISHI_Proc, &client_sock) != 0)
            perror("pthread_create");
    }

    close(server_sock);
    return WV_SOK;
}
/********************************************************

WV_S32 KTV_LEISHI_Close();

*********************************************************/ 
int KTV_LEISHI_Close()
{
    //在配置文件里增加雷石新版本标志为4,建立线程前，先判断是否是该版本
    if(SVR_CONTROL_GetKtvDev() != 4)
    {
        printf("this is not new ktv_leishi dev\n");
        return WV_EFAIL;
    }

    //释放内存
    cJSON_Delete(json_get);
    WV_S32 ret;
    void *returnVal;
    if(&newthread != NULL)
    {
        ret = pthread_cancel(newthread); 
        ret = pthread_join(newthread, &returnVal); 
    }  
    //WV_THR_Destroy(&(newthread));
    return WV_SOK;
}
