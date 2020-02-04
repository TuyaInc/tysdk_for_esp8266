/**
 * @file device.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _DEVICE_H
#define _DEVICE_H

#include "error_code.h"
#include "sys_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __DEVICE_GLOBALS
#define __DEVICE_EXT
#else
#define __DEVICE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
// device information define
#define SW_VER USER_SW_VER
#define PRODECT_KEY "h170dwh64shri8jq"
#define DEF_DEV_ABI DEV_SINGLE

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
__DEVICE_EXT
OPERATE_RET device_init(VOID);

#ifdef __cplusplus
}
#endif
#endif
