/***********************************************************
*  File: uni_mutex.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_MUTEX_H
#define _UNI_MUTEX_H

    #include "sys_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_MUTEX_GLOBAL
    #define _UNI_MUTEX_EXT
#else
    #define _UNI_MUTEX_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
//#define CREATE_RECURSION_MUTEX // 是否初始化成一个递归锁，默认非递归

// 定义互斥量管理句柄
typedef PVOID MUTEX_HANDLE;
/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
_UNI_MUTEX_EXT \
OPERATE_RET CreateMutexAndInit(OUT MUTEX_HANDLE *pMutexHandle);

_UNI_MUTEX_EXT \
OPERATE_RET MutexLock(IN CONST MUTEX_HANDLE mutexHandle);

_UNI_MUTEX_EXT \
OPERATE_RET MutexTryLock(IN CONST MUTEX_HANDLE mutexHandle);

_UNI_MUTEX_EXT \
OPERATE_RET MutexUnLock(IN CONST MUTEX_HANDLE mutexHandle);

_UNI_MUTEX_EXT \
OPERATE_RET ReleaseMutex(IN CONST MUTEX_HANDLE mutexHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
