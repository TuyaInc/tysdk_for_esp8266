/***********************************************************
*  File: uni_semaphore.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_SEMAPHORE_H
#define _UNI_SEMAPHORE_H

    #include "sys_adapter.h"
    
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_SEMAPHORE_GLOBAL
    #define _UNI_SEMAPHORE_EXT 
#else
    #define _UNI_SEMAPHORE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID SEM_HANDLE;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
_UNI_SEMAPHORE_EXT \
SEM_HANDLE CreateSemaphore(VOID);

#if defined(LINUX)
_UNI_SEMAPHORE_EXT \
OPERATE_RET InitSemaphore(IN CONST SEM_HANDLE semHandle,IN CONST UINT semCnt);
#elif defined(FREERTOS)
_UNI_SEMAPHORE_EXT \
OPERATE_RET InitSemaphore(IN CONST SEM_HANDLE semHandle,IN CONST UINT semCnt,\
                          IN CONST UINT sem_max);
#endif

_UNI_SEMAPHORE_EXT \
OPERATE_RET WaitSemaphore(IN CONST SEM_HANDLE semHandle);

_UNI_SEMAPHORE_EXT \
OPERATE_RET PostSemaphore(IN CONST SEM_HANDLE semHandle);

_UNI_SEMAPHORE_EXT \
OPERATE_RET ReleaseSemaphore(IN CONST SEM_HANDLE semHandle);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

