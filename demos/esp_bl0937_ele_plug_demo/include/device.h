/**
 * @File: device.h 
 * @Author: caojq 
 * @Last Modified time: 2020-03-05 
 * @Description: 
 */
#ifndef _DEVICE_H
    #define _DEVICE_H

    #include "sys_adapter.h"
    #include "error_code.h"
    
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __DEVICE_GLOBALS
    #define __DEVICE_EXT
#else
    #define __DEVICE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
// device information define
#define SW_VER USER_SW_VER
#define DEF_DEV_ABI DEV_SINGLE
#define PRODUCT_KEY "b8yrxe9cjcfdzbeu"
/***********************************************************
*************************variable define********************
***********************************************************/
    
/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: device_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__DEVICE_EXT \
OPERATE_RET device_init(VOID);



#ifdef __cplusplus
}
#endif
#endif

