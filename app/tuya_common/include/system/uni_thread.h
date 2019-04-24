/***********************************************************
*  File: uni_thread.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_THREAD_H
#define _UNI_THREAD_H

    #include "sys_adapter.h"
    
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_THREAD_GLOBAL
    #define _UNI_THREAD_EXT 
#else
    #define _UNI_THREAD_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#if defined(LINUX)
typedef pthread_t THREAD;
#elif defined(FREERTOS)
typedef xTaskHandle THREAD;
typedef unsigned short STACK_SIZE;
typedef unsigned portBASE_TYPE TRD_PRI;
#define TRD_PRIO_0     9	 /** High **/
#define TRD_PRIO_1     8
#define TRD_PRIO_2     7
#define TRD_PRIO_3     6
#define TRD_PRIO_4     5	 
#define TRD_PRIO_5     4	 
#define TRD_PRIO_6     3	 
#define TRD_PRIO_7     2
#define TRD_PRIO_8     1	 
	 
#endif

// 线程执行状态
typedef INT THRD_STA;
#define STATE_EMPTY 0 
#define STATE_DETACH_RUNNING 1
#define STATE_RUNNING 2
#define STATE_STOP 3

#if defined(LINUX)
typedef PVOID (*P_THRD_FUNC)(PVOID pArg);
#elif defined(FREERTOS)
typedef VOID (*P_THRD_FUNC)(PVOID pArg);
#endif

// 定义线程管理句柄
typedef PVOID THRD_HANDLE;
/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
#if defined(LINUX)
_UNI_THREAD_EXT \
OPERATE_RET CreateAndStartDetach(OUT THRD_HANDLE *pThrdHandle,\
                                 IN CONST P_THRD_FUNC pThrdFunc,\
                                 IN CONST PVOID pThrdFuncArg);

_UNI_THREAD_EXT \
OPERATE_RET CreateAndStart(OUT THRD_HANDLE *pThrdHandle,\
                           IN CONST P_THRD_FUNC pThrdFunc,\
                           IN CONST PVOID pThrdFuncArg);

_UNI_THREAD_EXT \
THRD_STA GetThrdSta(IN CONST THRD_HANDLE thrdHandle);

_UNI_THREAD_EXT \
THREAD GetSelfId(VOID);

_UNI_THREAD_EXT \
OPERATE_RET ThrdJoin(IN CONST THRD_HANDLE thrdHandle,OUT VOID **ppThrdRet);

#elif defined(FREERTOS)
_UNI_THREAD_EXT \
OPERATE_RET CreateAndStart(OUT THRD_HANDLE *pThrdHandle,\
                           IN CONST P_THRD_FUNC pThrdFunc,\
                           IN CONST PVOID pThrdFuncArg,\
                           IN CONST STACK_SIZE stack_size,\
                           IN CONST TRD_PRI pri,\
                           IN CONST CHAR *thrd_name);

_UNI_THREAD_EXT \
OPERATE_RET ThrdJoin(IN CONST THRD_HANDLE thrdHandle,OUT VOID **ppThrdRet);
#endif

/***********************************************************
*  Function: DeleteThrdHandle
*  Input: thrdHandle->线程管理模块句柄
*  Output: none
*  Return: none
***********************************************************/
_UNI_THREAD_EXT \
VOID DeleteThrdHandle(IN CONST THRD_HANDLE thrdHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

