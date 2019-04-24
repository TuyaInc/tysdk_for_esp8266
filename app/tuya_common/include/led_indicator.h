/***********************************************************
*  File: led_indicator.h 
*  Author: nzy
*  Date: 20150909
***********************************************************/
#ifndef _LED_INDICATOR_H
    #define _LED_INDICATOR_H

    #include "com_def.h"
    #include "sys_adapter.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __LED_INDICATOR_GLOBALS
    #define __LED_INDICATOR_EXT
#else
    #define __LED_INDICATOR_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID LED_HANDLE; // led handle

typedef enum {
    OL_LOW = 0,    // output level low
    OL_HIGH,       // output level high
    OL_FLASH_LOW,  // when led flash,the level output low first.
                   // when led flash end,the the level output low.
    OL_FLASH_HIGH, // when led flash,the level output high first.
                   // when led flash end,the the level output high.
}LED_LT_E;

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: create_led_handle
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__LED_INDICATOR_EXT \
OPERATE_RET create_led_handle(IN CONST INT gpio_no,OUT LED_HANDLE *handle);

/***********************************************************
*  Function: set_led_light_type
*  Input: 
*  Output: 
*  Return: OPERATE_RET
*  note: if(OL_FLASH == type) then flh_mstime is valid
***********************************************************/
__LED_INDICATOR_EXT \
VOID set_led_light_type(IN CONST LED_HANDLE handle,IN CONST LED_LT_E type,IN CONST USHORT flh_mstime);

/***********************************************************
*  Function: set_led_light_new,new add flash sum time.
*  Input: 
*  Output: 
*  Return: OPERATE_RET
*  note: if(OL_FLASH == type) then flh_mstime and flh_ms_sumtime is valid
***********************************************************/
__LED_INDICATOR_EXT \
VOID set_led_light_type_new(IN CONST LED_HANDLE handle,IN CONST LED_LT_E type,\
                            IN CONST USHORT flh_mstime,IN CONST flh_ms_sumtime);

#ifdef __cplusplus
}
#endif
#endif

