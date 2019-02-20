#ifndef  _TSK_UART_RS232_H_
#define  _TSK_UART_RS232_H_
#include "wv_common.h" 
WV_S32  TSK_UART_Open();
WV_S32  TSK_UART_Close();
WV_S32  TSK_UART_SetPort(WV_U32 port);
WV_S32 TSK_UART_GetPort(WV_U32 * pPort);
WV_S32 TSK_UART_SetSpeed(WV_U32 baud);
WV_S32 TSK_UART_GetSpeed(WV_U32 * pBaud);

WV_S32 TSK_UART_SetUartSceneCmd(WV_U8 *pData,WV_S32 len);
WV_S32 TSK_UART_AnalyzeCmd(WV_S8 *buf);
WV_S32 TSK_UART_SceneLoopOpen();
WV_S32 TSK_UART_SceneLoopClose();
WV_S32 TSK_UART_GetWindowMode();
WV_S32 TSK_UART_GetOpenDevMode();
WV_S32 TSK_UART_Send(WV_S8  *pBuf ,WV_U32  len);
WV_S32 TSK_UART_GetTypeRound();
WV_S32 TSK_UART_GetOpenprojectorMode();
WV_S32 TSK_UART_QueGet(WV_U8 *pBufOut, WV_U32 *pLenOut);
#endif
