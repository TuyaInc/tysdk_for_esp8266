/**
 * @file hw_table.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define __HW_TABLE_GLOBALS
#include "hw_table.h"
#include <mem_pool.h>
#include "device.h"
#include "system/sys_timer.h"

/**
 * @brief 长按配网时间（单位ms）
 * 
 */
#define PRESS_HOLD_TIME 3000

FUNCTION_SELECT func_select =
	{
		.is_count_down = TRUE,			// 倒计时功能
		.is_save_stat = FALSE,			// 断电记忆功能
		.cd_upload_period = 30,
		.relayCounter = 0,
		.target_ch = 0,
};

/**
 * @brief 通道控制管理结构
 * 
 */
STATIC CTRL_CHANNEL channels[] =
{
	/* 第一路 */
	{
		/* 继电器 */
		.relay = {
			.type = IO_DRIVE_LEVEL_HIGH, 
			.pin = GPIO_ID_PIN(12)
			},

		/* 按键 */
		.button = {
			.type = IO_DRIVE_LEVEL_LOW, 
			.pin = GPIO_ID_PIN(13)
			},

		/* 指示灯 */
		.led = {
			.type = IO_DRIVE_LEVEL_HIGH, 
			.pin = GPIO_ID_PIN(5)
			},
		/* 控制DPID */
		.dpid = DP_SW,
		/* 通道控制倒计时DPID */
		.cd_dpid = DP_COUNT_DOWN
	}
	/* ... */
	/* 多个通道往下添加，同上结构的代码 */
};

/**
 * @brief 硬件管理结构
 * 
 */
HW_TABLE g_hw_table =
    {
        /* wifi状态指示灯 */
        .wifi_stat_led = {
            .type = IO_DRIVE_LEVEL_LOW,
            .pin = GPIO_ID_PIN(4)},
        /* 总控按键 */
        .power_button = {.type = IO_DRIVE_LEVEL_NOT_EXIST},
        /* 通道 */
        .channels = channels
	};

/**
 * @brief 硬件初始化及按键注册
 * 
 * @param hw_table 硬件管理结构
 * @param hw_key_process 按键回调
 * @return OPERATE_RET 
 */
OPERATE_RET init_hw(HW_TABLE *hw_table, KEY_CALLBACK hw_key_process) {
	INT i;
	OPERATE_RET op_ret = OPRT_OK;  // 返回状态
	PR_DEBUG("initialize hardware...");

	// key process init
	op_ret = tuya_kb_init();
	if (OPRT_OK != op_ret) {
		return op_ret;
	}

	hw_table->channel_num = CNTSOF(channels);
	// 总控按键注册
	if (hw_table->power_button.type != IO_DRIVE_LEVEL_NOT_EXIST) {
		op_ret = tuya_kb_reg_proc(hw_table->power_button.pin, PRESS_HOLD_TIME, hw_key_process);
		if (OPRT_OK != op_ret) {
			return op_ret;
		}
	}

	// WIFI指示灯注册
	if (hw_table->wifi_stat_led.type != IO_DRIVE_LEVEL_NOT_EXIST) {
		op_ret = tuya_create_led_handle(hw_table->wifi_stat_led.pin, &hw_table->wifi_stat_led_handle);
		if (OPRT_OK != op_ret) {
			return op_ret;
		}
		/* 设置WIFI指示灯默认状态 */
		tuya_set_led_type(hw_table->wifi_stat_led_handle, (hw_table->wifi_stat_led.type == IO_DRIVE_LEVEL_HIGH) ? OL_LOW : OL_HIGH, 0);
	}

	for (i = 0; i < hw_table->channel_num; ++i) {
		hw_table->channels[i].stat = FALSE;  // 初始状态
		// 1.初始化通道中的继电器 如不存在则跳过 *!* 继电器不存在目前不能想象 但仍支持不存在
		if (hw_table->channels[i].relay.type != IO_DRIVE_LEVEL_NOT_EXIST) {
			op_ret = create_led_handle(hw_table->channels[i].relay.pin, &hw_table->channels[i].relay_handle);
			if (OPRT_OK != op_ret) {
				return op_ret;
			}
		}
		// 2.初始化通道中的按钮 如不存在则跳过
		if (hw_table->channels[i].button.type != IO_DRIVE_LEVEL_NOT_EXIST) {
			op_ret = tuya_kb_reg_proc(hw_table->channels[i].button.pin, PRESS_HOLD_TIME, hw_key_process);
			if (OPRT_OK != op_ret) {
				return op_ret;
			}
		}
		// 3.初始化通道中的状态指示灯 如不存在则跳过
		if (hw_table->channels[i].led.type != IO_DRIVE_LEVEL_NOT_EXIST) {
			op_ret = create_led_handle(hw_table->channels[i].led.pin, &hw_table->channels[i].led_handle);
			if (OPRT_OK != op_ret) {
				return op_ret;
			}
			/* 设置WIFI指示灯默认状态 */
			tuya_set_led_type(hw_table->channels[i].led_handle, (hw_table->channels[i].led.type == IO_DRIVE_LEVEL_HIGH) ? OL_LOW : OL_HIGH, 0);
		}
		/* 上电默认关闭通道 */
		hw_set_channel(&g_hw_table, i, FALSE);
		/* 初始化定时器为停止 */
		hw_table->channels[i].cd_sec = -1;
	}
	return op_ret;
}

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
VOID hw_set_channel(HW_TABLE *hw_table, INT channel_no, BOOL is_active) {
	if (channel_no < 0 || channel_no >= hw_table->channel_num) {
		PR_ERR("channel_no error: %d", channel_no);
		return;
	}

	if (is_active) {
		// 通道有效
		PR_DEBUG("channel: %d true", channel_no);
		hw_table->channels[channel_no].stat = TRUE;
		// 根据继电器 有效电平定义 控制继电器
		switch (hw_table->channels[channel_no].relay.type) {
			case IO_DRIVE_LEVEL_HIGH:
				// 高电平有效 置高
				tuya_set_led_type(hw_table->channels[channel_no].relay_handle, OL_HIGH, 0);
				break;
			case IO_DRIVE_LEVEL_LOW:
				// 低电平有效 置低
				tuya_set_led_type(hw_table->channels[channel_no].relay_handle, OL_LOW, 0);
				break;
			case IO_DRIVE_LEVEL_NOT_EXIST:
				// IO不存在 跳过
				break;
			default:
				break;
		}

		// 根据状态指示灯 有效电平定义 控制状态指示灯
		switch (hw_table->channels[channel_no].led.type) {
			case IO_DRIVE_LEVEL_HIGH:
				// 高电平有效 置高
				tuya_set_led_type(hw_table->channels[channel_no].led_handle, OL_HIGH, 0);
				break;
			case IO_DRIVE_LEVEL_LOW:
				// 低电平有效 置低
				tuya_set_led_type(hw_table->channels[channel_no].led_handle, OL_LOW, 0);
				break;
			case IO_DRIVE_LEVEL_NOT_EXIST:
				// IO不存在 跳过
				break;
			default:
				break;
		}
	} else {
		// 通道无效
		PR_DEBUG("channel: %d false", channel_no);
		hw_table->channels[channel_no].stat = FALSE;
		// 根据继电器 无效电平定义 控制继电器
		switch (hw_table->channels[channel_no].relay.type) {
			case IO_DRIVE_LEVEL_HIGH:
				// 高电平有效 置低
				tuya_set_led_type(hw_table->channels[channel_no].relay_handle, OL_LOW, 0);
				break;
			case IO_DRIVE_LEVEL_LOW:
				// 低电平有效 置高
				tuya_set_led_type(hw_table->channels[channel_no].relay_handle, OL_HIGH, 0);
				break;
			case IO_DRIVE_LEVEL_NOT_EXIST:
				// IO不存在 跳过
				break;
			default:
				break;
		}

		// 根据状态指示灯 无效电平定义 控制继电器
		switch (hw_table->channels[channel_no].led.type) {
			case IO_DRIVE_LEVEL_HIGH:
				// 高电平有效 置低
				tuya_set_led_type(hw_table->channels[channel_no].led_handle, OL_LOW, 0);
				break;
			case IO_DRIVE_LEVEL_LOW:
				// 低电平有效 置高
				tuya_set_led_type(hw_table->channels[channel_no].led_handle, OL_HIGH, 0);
				break;
			case IO_DRIVE_LEVEL_NOT_EXIST:
				// IO不存在 跳过
				break;
			default:
				break;
		}
	}
}

/**
 * @brief 翻转通道状态
 * 
 * @param hw_table 硬件管理结构
 * @param channel_no 通道号
 * @return VOID 
 */
VOID hw_trig_channel(HW_TABLE *hw_table, INT channel_no) {
	// 测试下标是否越界 下标范围[0, 通道数-1]
	if (channel_no < 0 || channel_no >= hw_table->channel_num) {
		PR_ERR("channel_no error: %d", channel_no);
		return;
	}

	// 按目前通道状态反置通道状态
	if (hw_table->channels[channel_no].stat == TRUE) {
		hw_set_channel(hw_table, channel_no, FALSE);
	} else {
		hw_set_channel(hw_table, channel_no, TRUE);
	}
}

/**
 * @brief 设置wifi状态指示灯状态
 * 
 * @param hw_table 硬件管理结构
 * @param wifi_stat wifi 状态
 * @return VOID 
 */
VOID hw_set_wifi_led_stat(HW_TABLE *hw_table, GW_WIFI_STAT_E wifi_stat) {
	INT i;
	BOOL is_any_active = FALSE;
	switch (wifi_stat) {
		/* EZ模式 */
		case STAT_UNPROVISION: {
			tuya_set_led_type(hw_table->wifi_stat_led_handle, OL_FLASH_HIGH, 250);
		} break;

		/* AP模式 */
		case STAT_AP_STA_UNCONN:
		case STAT_AP_STA_CONN: {
			tuya_set_led_type(hw_table->wifi_stat_led_handle, OL_FLASH_HIGH, 1500);
		} break;

		/* 低功耗模式 */
		case STAT_LOW_POWER:

		break;

		/* 未联网：根据具体需求修改指示灯状态（此代码默认灭灯） */
		case STAT_STA_UNCONN: {
			switch (hw_table->wifi_stat_led.type) {
				case IO_DRIVE_LEVEL_HIGH:
					// 高电平有效 置低
					tuya_set_led_type(hw_table->wifi_stat_led_handle, OL_LOW, 0);
					break;
				case IO_DRIVE_LEVEL_LOW:
					// 低电平有效 置高
					tuya_set_led_type(hw_table->wifi_stat_led_handle, OL_HIGH, 0);
					break;
				case IO_DRIVE_LEVEL_NOT_EXIST:
					// IO不存在 跳过
					break;
				default:
					break;
			}
		} break;

		/* 已联网：根据具体需求修改指示灯状态（此代码默认亮灯）*/
		case STAT_STA_CONN: {
			switch (hw_table->wifi_stat_led.type) {
				case IO_DRIVE_LEVEL_HIGH:
					// 高电平有效 置高
					tuya_set_led_type(hw_table->wifi_stat_led_handle, OL_HIGH, 0);
					break;
				case IO_DRIVE_LEVEL_LOW:
					// 低电平有效 置低
					tuya_set_led_type(hw_table->wifi_stat_led_handle, OL_LOW, 0);
					break;
				case IO_DRIVE_LEVEL_NOT_EXIST:
					// IO不存在 跳过
					break;
				default:
					break;
			}
		} break;
	}
}

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
INT hw_set_channel_by_dpid(HW_TABLE *hw_table, INT dpid, BOOL is_active) {
	INT i;
	// 遍历通道列表
	for (i = 0; i < hw_table->channel_num; ++i) {
		// 判断dpid是否一致
		if (dpid == hw_table->channels[i].dpid) {
			hw_set_channel(hw_table, i, is_active);
			return i;
		}
	}
	// 至此未返回说明未找到通道
	return -1;
}

/**
 * @brief 通过DPID 查找通道
 * 
 * @param hw_table 硬件管理结构
 * @param dpid DPID
 * @return INT 
 */
INT hw_find_channel_by_cd_dpid(HW_TABLE *hw_table, INT dpid) {
	INT i;
	// 遍历通道列表
	for (i = 0; i < hw_table->channel_num; ++i) {
		// 判断dpid是否一致
		if (dpid == hw_table->channels[i].cd_dpid) {
			return i;
		}
	}
	return -1;
}
