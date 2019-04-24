/***********************************************************
*  File: tuya_led_api.h 
*  Author: anby
*  Date: 20160629
***********************************************************/
#ifndef _TUYA_LED_API_H
#define _TUYA_LED_API_H

#include "led_indicator.h"

/*************************************************************************************
函数功能: 创建LED控制句柄
输入参数: gpio_no GPIO序号
输出参数: handle  LED句柄
返 回 值: 参照返回值列表
备    注: 无
*************************************************************************************/
OPERATE_RET tuya_create_led_handle(IN CONST INT gpio_no,OUT LED_HANDLE *handle);

/*************************************************************************************
函数功能: LED控制
输入参数: handle  LED句柄
          type    控制类型
          <1> OL_LOW    LED低电平
          <2> OL_HIGH   LED高电平
          <3> OL_FLASH_LOW  LED低电平闪烁
          <4> OL_FLASH_HIGH LED高电平闪烁
          flh_mstime 闪烁间隔时间
输出参数: 无
返 回 值: 参照返回值列表
备    注: 无
*************************************************************************************/
VOID tuya_set_led_type(IN CONST LED_HANDLE handle,IN CONST LED_LT_E type,IN CONST USHORT flh_mstime);

#endif

