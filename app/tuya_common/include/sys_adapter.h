/***********************************************************
*  File: sys_adapter.h 
*  Author: nzy
*  Date: 20150526
***********************************************************/
#ifndef _SYS_ADAPTER_H
    #define _SYS_ADAPTER_H

    #define __time_t_defined // for timt_t redefined
    #include "com_def.h"
    #include "error_code.h"
    #include "esp_common.h"
    #include "mem_pool.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/semphr.h"
    #include "freertos/task.h"

	#include "system/uni_log.h"
	
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __SYS_ADAPTER_GLOBALS
    #define __SYS_ADAPTER_EXT
#else
    #define __SYS_ADAPTER_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
// rtos type,only select one as follow:
// #define LINUX
#define FREERTOS

typedef DWORD TIME_MS;
typedef DWORD TIME_S;
typedef WORD TIMER_ID;  // 定时器ID



#ifdef __cplusplus
}
#endif
#endif

