/***********************************************************
*  File: deepsleep.h
*  Author: lt
*  Date: 170503
***********************************************************/
#ifndef  __DEEPSLEEP_H__
#define  __DEEPSLEEP_H__

	#include "com_def.h"
	#include "sys_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DEEPSLEEP_GLOBAL
    #define _DEEPSLEEP_EXT
#else
    #define _DEEPSLEEP_EXT extern
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/
#define DEEP_SLEEP_DATA_MEM_ADDR 120
#define INIT_MAGIC 0x7e7e55aa
#define MAX_DS_CNT_NUM	1000

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/

_DEEPSLEEP_EXT \
OPERATE_RET init_deep_sleep(VOID);

_DEEPSLEEP_EXT \
OPERATE_RET control_system_start(IN VOID (*sys_first_power_on_cb)(VOID),
								  IN VOID(*sys_not_first_power_on_cb)(VOID));

_DEEPSLEEP_EXT \
OPERATE_RET enter_deep_sleep_mode(VOID);


_DEEPSLEEP_EXT \
OPERATE_RET get_time_start2now_us(OUT PUINT start2now_time);






#endif

