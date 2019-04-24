/***********************************************************
*  File: uni_msg_queue.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_MSG_QUEUE_H
#define _UNI_MSG_QUEUE_H

    #include "sys_adapter.h"
    #include "uni_pointer.h"
    
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_MSG_QUEUE_GLOBAL
    #define _UNI_MSG_QUEUE_EXT 
#else
    #define _UNI_MSG_QUEUE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID MSG_QUE_HANDLE; // 消息队列操作句柄

typedef UINT MSG_ID;            // 消息ID
typedef PVOID P_MSG_DATA;       // 消息数据
typedef UINT MSG_DATA_LEN;      // 消息数据长度

typedef UINT MSG_TYPE; // 消息类型
#define INSTANCY_MESSAGE 0  // 紧急消息(该消息为优先先执行)
#define NORMAL_MESSAGE 1    // 普通消息(该消息以先进先出方式执行)

// 消息
typedef struct
{
    MSG_ID msgID;
    P_MSG_DATA pMsgData;
    MSG_DATA_LEN msgDataLen;
}MESSAGE,*P_MESSAGE;

// 消息链
typedef struct
{
    LIST_HEAD listHead;     // 链表节点
    MESSAGE msg;
}MSG_LIST,*P_MSG_LIST;

#define USE_SEM_COUNTING 1
#if !(USE_SEM_COUNTING)
    #include "uni_system.h"
    #define PROC_MSG_DELAY 100
#endif
/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
_UNI_MSG_QUEUE_EXT \
OPERATE_RET CreateMsgQueAndInit(OUT MSG_QUE_HANDLE *pMsgQueHandle);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET AddMsgNodeToQueue(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                              IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                              IN CONST MSG_DATA_LEN msgDataLen,\
                              IN CONST MSG_TYPE msgType);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET GetMsgNodeFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                                IN CONST MSG_ID msgID,OUT P_MSG_LIST *ppMsgListNode);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET GetFirstMsgFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                                 OUT P_MSG_LIST *ppMsgListNode);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET GetMsgNodeNum(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                          OUT PINT pMsgNodeNum);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET DelAndFreeMsgNodeFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                                       IN CONST P_MSG_LIST pMsgListNode);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET ReleaseMsgQue(IN CONST MSG_QUE_HANDLE msgQueHandle);

/***********************************************************
*  Function: PostMessage 递送一个消息至模块(消息先进先执行)
*  Input: msgQueHandle->消息处理句柄
*         msgID->消息ID
*         pMsgData->消息数据
*         msgDataLen->消息数据长度
*  Output: none
*  Return: OPERATE_RET
*  Date: 20140624
***********************************************************/
_UNI_MSG_QUEUE_EXT \
OPERATE_RET PostMessage(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                        IN CONST MSG_ID msgID,\
                        IN CONST P_MSG_DATA pMsgData,\
                        IN CONST MSG_DATA_LEN msgDataLen);

/***********************************************************
*  Function: PostInstancyMsg 递送一个紧急消息至模块
*  Input: msgQueHandle->消息处理句柄
*         msgID->消息ID
*         pMsgData->消息数据
*         msgDataLen->消息数据长度
*  Output: none
*  Return: OPERATE_RET
*  Date: 20140624
***********************************************************/
_UNI_MSG_QUEUE_EXT \
OPERATE_RET PostInstancyMsg(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                            IN CONST MSG_ID msgID,\
                            IN CONST P_MSG_DATA pMsgData,\
                            IN CONST MSG_DATA_LEN msgDataLen);

/***********************************************************
*  Function: WaitMessage 等待消息 
*            WaitMessage成功需调用，消息处理完后需调用
*                       DelAndFreeMsgNodeFromQueue释放消息
*  Input: msgQueHandle->消息处理句柄
*  Output: ppMsgListNode
*  Return: OPERATE_RET
*  Date: 20140624
***********************************************************/
_UNI_MSG_QUEUE_EXT \
OPERATE_RET WaitMessage(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                        OUT P_MSG_LIST *ppMsgListNode);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
