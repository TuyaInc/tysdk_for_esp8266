/***********************************************************
*  File: uni_time_queue.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_TIME_QUEUE_H
#define _UNI_TIME_QUEUE_H

    #include "sys_adapter.h"
    
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_TIME_QUEUE_GLOBAL
    #define _UNI_TIME_QUEUE_EXT 
#else
    #define _UNI_TIME_QUEUE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID TIMER_QUE_HANDLE; // 定时器队列操作句柄

// 设置的定时器处理函数
typedef VOID (* P_TIMER_FUNC)(UINT timerID,PVOID pTimerArg);

typedef enum {
    TIMER_ONCE = 0,
    TIMER_CYCLE,
}TIMER_TYPE;
/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
_UNI_TIME_QUEUE_EXT \
OPERATE_RET CreateTimerQueAndInit(OUT TIMER_QUE_HANDLE *pTimerQueHandle);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET AddTimerToQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                            IN CONST TIME_MS timeCycle,\
                            IN CONST P_TIMER_FUNC pTimerFunc,\
                            IN CONST TIMER_ID timerID,\
                            IN CONST PVOID pTimerArg);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET DeleteTimerFromQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                                 IN CONST TIMER_ID timerID);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET StartTimerFromQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                                IN CONST TIMER_ID timerID,\
                                IN CONST TIME_MS timeCycle,\
                                IN CONST TIMER_TYPE timer_type);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET StopTimerFromQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                               IN CONST TIMER_ID timerID);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET DispatchTimerFromQuene(IN CONST TIMER_QUE_HANDLE timerQueHandle);

_UNI_TIME_QUEUE_EXT
OPERATE_RET GetTimerNodeNum(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                            OUT PINT pTimerNodeNum);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET ReleaseTimerQue(IN CONST TIMER_QUE_HANDLE timerQueHandle);

/***********************************************************
*  Function: GetIdleTimerIdRapid 快速获取空闲的timer_id
*  Input: pMsgQueHandle->消息管理结构句柄
*  Output: p_timer_id
*  Return: OPERATE_RET
*  Date: 20140625
***********************************************************/
_UNI_TIME_QUEUE_EXT \
TIMER_ID GetIdleTimerIdRapid(IN CONST TIMER_QUE_HANDLE timerQueHandle);

/***********************************************************
*  Function: GetIdleTimerId 获取空闲的timer_id
*  Input: pMsgQueHandle->消息管理结构句柄
*  Output: p_timer_id
*  Return: OPERATE_RET
*  Date: 20140625
***********************************************************/
_UNI_TIME_QUEUE_EXT \
OPERATE_RET GetIdleTimerId(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                           OUT TIMER_ID *p_timer_id);

/***********************************************************
*  Function: IsThisTimerRun 确定该定时器是否运行
*  Input: pMsgQueHandle->消息管理结构句柄
*         p_timer_id
*  Output: none
*  Return: OPERATE_RET
*  Date: 20140625
***********************************************************/
_UNI_TIME_QUEUE_EXT \
BOOL IsThisTimerRun(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                    IN CONST TIMER_ID timer_id);

/***********************************************************
*  Function: ShowTimerInfo
*  Input: timerQueHandle
*  Output: none
*  Return: OPERATE_RET
*  Date: 20180330
***********************************************************/
_UNI_TIME_QUEUE_EXT \
OPERATE_RET ShowTimerInfo(IN CONST TIMER_QUE_HANDLE timerQueHandle);

/***********************************************************
*  Function: CheckTimerExcept
*  Input: timerQueHandle
*         timerID
*  Output: none
*  Return: OPERATE_RET
*  Date: 20180330
***********************************************************/
_UNI_TIME_QUEUE_EXT \
OPERATE_RET CheckTimerExcept(IN CONST TIMER_QUE_HANDLE timerQueHandle,IN CONST TIMER_ID timerID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
