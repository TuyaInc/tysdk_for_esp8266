/**
 * @file hw_table.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef HW_TABLE_H
#define HW_TABLE_H
#include <com_def.h>
#include <key.h>
#include <led_indicator.h>
#include "wf_sdk_adpt.h"

#ifdef __HW_TABLE_GLOBALS
#define __HW_TABLE_EXT
#else
#define __HW_TABLE_EXT extern
#endif

/**
 * @brief DPID 不存在
 * 
 */
#define DPID_NOT_EXIST (-1)
/**
 * @brief DPID
 * 
 */
typedef enum {
	DP_SW = 1,
	DP_COUNT_DOWN = 9,
} DP_ID_E;

/**
 * @brief IO有效电平
 * 
 */
typedef enum {
	IO_DRIVE_LEVEL_HIGH,			// 高有效
	IO_DRIVE_LEVEL_LOW,				// 低有效
	IO_DRIVE_LEVEL_NOT_EXIST		// IO未使用
} IO_DRIVE_TYPE;

typedef struct
{
	IO_DRIVE_TYPE type;
	UCHAR pin;
} IO_CONFIG;

typedef struct
{
	IO_CONFIG relay;
	LED_HANDLE relay_handle;
	IO_CONFIG button;
	IO_CONFIG led;
	LED_HANDLE led_handle;
	INT dpid;
	INT cd_dpid;
	INT cd_sec;
	BOOL stat;
} CTRL_CHANNEL;

typedef struct
{
	IO_CONFIG wifi_stat_led;
	LED_HANDLE wifi_stat_led_handle;
	// 电源总开关 存在通道有效时按下关闭 否则按下全部开启
	IO_CONFIG power_button;
	INT channel_num;
	CTRL_CHANNEL *channels;
} HW_TABLE;

typedef struct {
	BOOL is_count_down;        // 倒计时开关
	BOOL is_save_stat;         // 断电记忆开关
	TIMER_ID wfl_timer;        //wifi状态定时器
	TIMER_ID save_stat_timer;  //断电记忆
	TIMER_ID cd_timer;         // 倒计时定时器
	INT cd_upload_period;      // 倒计时状态上报周期 单位:秒
	
	/**************************产测管理代码************************************/
	INT relayCounter;   // 继电器动作次数计数器
	INT target_ch;      //通道计数器
	TIMER_ID relay_tm;  // 继电器动作定时器句柄
} FUNCTION_SELECT;

/**
 * @brief 硬件初始化及按键注册
 * 
 * @param hw_table 硬件管理结构
 * @param hw_key_process 按键回调
 * @return OPERATE_RET 
 */
OPERATE_RET init_hw(HW_TABLE *hw_table, KEY_CALLBACK hw_key_process);
/**
 * @brief 设置通道状态
 * 
 * @param hw_table 硬件管理结构
 * @param channel_no 通道号
 * @param is_active 控制状态：
 *        [TRUE]  --- 开启
 *        [FALSE] --- 关闭
 * @return VOID 
 */
VOID hw_set_channel(HW_TABLE *hw_table, INT channel_no, BOOL is_active);
/**
 * @brief 翻转通道状态
 * 
 * @param hw_table 硬件管理结构
 * @param channel_no 通道号
 * @return VOID 
 */
VOID hw_trig_channel(HW_TABLE *hw_table, INT channel_no);
/**
 * @brief 设置wifi状态指示灯状态
 * 
 * @param hw_table 硬件管理结构
 * @param wifi_stat wifi 状态
 * @return VOID 
 */
VOID hw_set_wifi_led_stat(HW_TABLE *hw_table, GW_WIFI_STAT_E wifi_stat);

/**
 * @brief 通过DPID 查找通道并设置对应状态
 * 
 * @param hw_table 硬件管理结构
 * @param dpid DPID
 * @param is_active 控制状态：
 *        [TRUE]  --- 开启
 *        [FALSE] --- 关闭
 * @return INT 
 */
INT hw_set_channel_by_dpid(HW_TABLE *hw_table, INT dpid, BOOL is_active);

/**
 * @brief 通过DPID 查找通道
 * 
 * @param hw_table 硬件管理结构
 * @param dpid DPID
 * @return INT 
 */
INT hw_find_channel_by_cd_dpid(HW_TABLE *hw_table, INT dpid);

// ================================================================

// ===========================================================
extern HW_TABLE g_hw_table;

extern FUNCTION_SELECT func_select;
// =======================================================
#endif
