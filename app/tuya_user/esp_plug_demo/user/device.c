/**
 * @file device.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#define __DEVICE_GLOBALS
#include "device.h"
#include "tuya_smart_api.h"
#include "wf_sdk_adpt.h"
#include "hw_table.h"
/***********************************************************
*************************micro define***********************
***********************************************************/
#define DEVICE_MOD "device_mod"
#define DEVICE_PART "device_part"

/***********************************************************
*************************function define********************
***********************************************************/
STATIC OPERATE_RET device_differ_init(VOID);
STATIC VOID key_process(INT gpio_no, PUSH_KEY_TYPE_E type, INT cnt);

/***********************************************************
*************************variable define********************
***********************************************************/
#define PT_RELAY_TIME (500)			/* 产测继电器动作时长单位ms */
/***********************************************************
*************************function define********************
***********************************************************/
STATIC VOID wfl_timer_cb(UINT timerID, PVOID pTimerArg);
STATIC VOID cd_timer_cb(UINT timerID, PVOID pTimerArg);
STATIC VOID all_channel_stat_upload(VOID);
// call back function
VOID set_firmware_tp(IN OUT CHAR *firm_name, IN OUT CHAR *firm_ver) {
	strcpy(firm_name, APP_BIN_NAME);
	strcpy(firm_ver, USER_SW_VER);
	return;
}

/**
 * @brief gpio 测试
 * 
 * @return BOOL 
 */
BOOL gpio_func_test(VOID) {
	return TRUE;
}

/**
 * @brief pre_app_init 初始化
 *        比app_init 提前调用
 * 
 * @return VOID 
 */
VOID pre_app_init(VOID) {
	//需要设置为输出的引脚 放置在数组中
	uint8 gpio_out_config[] = {12, 5};

	//调用set_gpio_out函数  第一个传参为数组指针，第二个参数为设置的io个数。
	//数组中的io设置为输出，其余为输入。
	//此函数只设置为输出状态，不设置电平状态。
	set_gpio_out(gpio_out_config, CNTSOF(gpio_out_config));
	return;
}

/**
 * @brief 继电器动作定时器回调
 * 
 * @param timerID 定时器ID
 * @param pTimerArg 
 * @return STATIC relay_tm_cb 
 */
STATIC VOID relay_tm_cb(UINT timerID, PVOID pTimerArg)
{
    PR_DEBUG("relay timer callback");

    if (func_select.target_ch < g_hw_table.channel_num) {
        hw_trig_channel(&g_hw_table, func_select.target_ch);
        ++func_select.relayCounter;
    }

    // 如果执行少于三次则再次启动定时器
    if ((func_select.relayCounter < 3) && (func_select.target_ch < g_hw_table.channel_num)) {
        sys_start_timer(func_select.relay_tm, PT_RELAY_TIME, TIMER_ONCE);
    } else if (func_select.target_ch < g_hw_table.channel_num) {
        func_select.relayCounter = 0;
        ++func_select.target_ch;
        sys_start_timer(func_select.relay_tm, PT_RELAY_TIME, TIMER_ONCE);
    }
}

/**
 * @brief 产测按键回调
 * 
 * @param gpio_no 
 * @param type 
 * @param cnt 
 * @return STATIC pt_key_process 
 */
STATIC VOID pt_key_process(INT gpio_no, PUSH_KEY_TYPE_E type, INT cnt)
{
    PR_DEBUG("gpio_no: %d", gpio_no);
    PR_DEBUG("type: %d", type);
    PR_DEBUG("cnt: %d", cnt);

    INT i;  
	//通道下标 通道号为通道下标
    //总控通道遍历设置
    func_select.target_ch = 0;
    // 启动操作链
    func_select.relayCounter = 0;

    for (i = 0; i < g_hw_table.channel_num;i++) {
        if (gpio_no == g_hw_table.channels[i].button.pin) {
            if (NORMAL_KEY == type) {
                if (IsThisSysTimerRun(func_select.relay_tm) == FALSE) {
                    sys_start_timer(func_select.relay_tm, PT_RELAY_TIME, TIMER_ONCE);
                }
            }
        }
	}
    return;
}

/**
 * @brief 产测接口函数
 * 
 * @param flag 产测标志
 * @param rssi 当前产测路由 RSSI值
 * @return VOID 
 */
VOID prod_test(BOOL flag, CHAR rssi) {
    // TODO: 信号值测试
    if (flag == FALSE) {
        PR_ERR("no auth");
        return;
    }

    PR_NOTICE("product test mode");
    OPERATE_RET op_ret;  // 注册操作的返回值

    /* 硬件配置初始化 配置完后所有通道默认无效 */
    op_ret = init_hw(&g_hw_table, pt_key_process);
    if (OPRT_OK != op_ret) {
        PR_ERR("op_ret:%d", op_ret);
        return;
    }

    /* 注册继电器回调 */
    op_ret = sys_add_timer(relay_tm_cb, NULL, &func_select.relay_tm);
    if (OPRT_OK != op_ret) {
        PR_ERR("add relay_tm err");
        return;
    }

	/* 产测标识 3s 闪烁 */
    tuya_set_led_type(g_hw_table.wifi_stat_led_handle, OL_FLASH_HIGH, 3000);
    return;
}



/**
 * @brief app_init 初始化
 * 
 * @return VOID 
 */
VOID app_init(VOID) {
    //set_console(FALSE); /* 设置日志打印IO 为 TX --- 注释此行则打印口 IO2 */

	/* 设置WIFI 模式 */
    app_cfg_set(WCM_LOW_POWER, prod_test);
}

/**
 * @brief 解析控制子函数
 * 
 * @param dpid DPID
 * @param value 控制值
 * @return  
 */
STATIC VOID deal_dps_proc(UCHAR dpid, UINT value) {
	INT ch_index = 0;
	switch (dpid) {
		case DP_SW: {
			ch_index = hw_set_channel_by_dpid(&g_hw_table, dpid, value);
			if (ch_index >= 0) {
				if (g_hw_table.channels[ch_index].cd_dpid != DPID_NOT_EXIST) {
					g_hw_table.channels[ch_index].cd_sec = -1;
				}
				if (func_select.is_save_stat) {
					sys_start_timer(func_select.save_stat_timer, 5000, TIMER_ONCE);
				}
			}
		} break;

		case DP_COUNT_DOWN: {
			ch_index = hw_find_channel_by_cd_dpid(&g_hw_table, dpid);
			if (ch_index >= 0) {
				PR_DEBUG("cd[%d]: %d", ch_index, value);
				if (value != 0) {
					g_hw_table.channels[ch_index].cd_sec = value;
				} else {
					g_hw_table.channels[ch_index].cd_sec = -1;
				}
			}
		} break;

		default:
			break;
	}

	return;
}

/**
 * @brief device_cb 接收回调
 * 
 * @param cmd 
 * @param root 接受的cjson 格式数据 
 * @return VOID 
 */
VOID device_cb(SMART_CMD_E cmd, cJSON *root) {
	PR_DEBUG("cmd:%d", cmd);
	cJSON *nxt = root->child;
	UCHAR dpid = 0;
	UINT dps = 0;

	while (nxt) {
		/* 解析DPID */
		dpid = atoi(nxt->string);

		/* bool 类型值 */
		if (nxt->type == cJSON_False) {
			dps = 0;
		} else if (nxt->type == cJSON_True) {
			dps = 1;
		} else if (nxt->type == cJSON_Number) {
			dps = nxt->valueint;
		}

		PR_DEBUG("dpid:%d, dps:%d", dpid, dps);
		deal_dps_proc(dpid, dps);
		nxt = nxt->next;
	}

	/* 所有状态上报 */
	all_channel_stat_upload();
}

/**
 * @brief 查询回调函数，推送设备当前状态
 * 
 */
STATIC VOID dp_qeury_cb(IN CONST TY_DP_QUERY_S *dp_qry) 
{
	all_channel_stat_upload();
}

/**
 * @brief device_init 设备初始化
 * 
 * @return OPERATE_RET 
 */
OPERATE_RET device_init(VOID) {
	PR_NOTICE("fireware info name:%s version:%s", APP_BIN_NAME, USER_SW_VER);
	OPERATE_RET op_ret;

	op_ret = tuya_device_init(PRODECT_KEY, device_cb, USER_SW_VER);
	if (op_ret != OPRT_OK) {
		return op_ret;
	}

	TY_IOT_CBS_S wf_cbs = {
		.dev_dp_query_cb = dp_qeury_cb,
		.ug_reset_inform_cb = NULL,
	};
	gw_register_cbs(&wf_cbs);

	op_ret = tuya_psm_register_module(DEVICE_MOD, DEVICE_PART);
	if (op_ret != OPRT_OK && op_ret != OPRT_PSM_E_EXIST) {
		PR_ERR("tuya_psm_register_module error:%d", op_ret);
		return op_ret;
	}

	op_ret = init_hw(&g_hw_table, key_process);
	if (op_ret != OPRT_OK) {
		return op_ret;
	}

	op_ret = device_differ_init();
	if (op_ret != OPRT_OK) {
		return op_ret;
	}

    /* wifi状态监测定时器 wifi状态改变时触发相关操作 */
    op_ret = sys_add_timer(wfl_timer_cb, NULL, &func_select.wfl_timer);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }else {
        sys_start_timer(func_select.wfl_timer, 300, TIMER_CYCLE);
    }

    if (func_select.is_count_down) {
        /* 倒计时 */
        op_ret = sys_add_timer(cd_timer_cb, NULL, &func_select.cd_timer);
        if (OPRT_OK != op_ret) {
            return op_ret;
        } else {
            sys_start_timer(func_select.cd_timer, 1000, TIMER_CYCLE);
        }
    }

	return op_ret;
}

/**
 * @brief 按键回调
 * 
 * @param gpio_no 按键触发引脚号
 * @param type 按下类型（长按、单击）
 * @param cnt 连续操作计数值
 * @return STATIC key_process 
 */
STATIC VOID key_process(INT gpio_no, PUSH_KEY_TYPE_E type, INT cnt) 
{
	INT i = 0;
	PR_DEBUG("gpio_no: %d", gpio_no);
	PR_DEBUG("type: %d", type);
	PR_DEBUG("cnt: %d", cnt);

    for (i = 0; i < g_hw_table.channel_num;i++) {
        if (gpio_no == g_hw_table.channels[i].button.pin) {
            if (LONG_KEY == type) {
                tuya_dev_reset_factory();
            } else if (NORMAL_KEY == type) {
                /* 查看系统剩余内存 */
                PR_NOTICE("remain size:%d", system_get_free_heap_size());
                /* 单击触发 */
                hw_trig_channel(&g_hw_table, i);

				/* 清空该通道的倒计时 */
                if (g_hw_table.channels[i].cd_dpid != DPID_NOT_EXIST) {
                    g_hw_table.channels[i].cd_sec = -1;
				}				
                all_channel_stat_upload();
            }
        }
	}
}

/**
 * @brief 设备其他状态初始化
 * 
 * @return STATIC device_differ_init 
 */
STATIC OPERATE_RET device_differ_init(VOID) {
	OPERATE_RET op_ret;

	return OPRT_OK;
}

/**
 * @brief 倒计时回调
 * 
 * @param timerID 
 * @param pTimerArg 
 * @return STATIC cd_timer_cb 
 */
STATIC VOID cd_timer_cb(UINT timerID, PVOID pTimerArg)
{
    INT i = 0;  // 通道号
    CHAR buff[30];

	/* 遍历所有通道 */
    for (i = 0; i < g_hw_table.channel_num; ++i) {
        if (g_hw_table.channels[i].cd_sec < 0) {
            continue;  // 通道计时关闭
        } else {
            --g_hw_table.channels[i].cd_sec;
            if (g_hw_table.channels[i].cd_sec <= 0) {  // 计时到达
                hw_trig_channel(&g_hw_table, i);
                g_hw_table.channels[i].cd_sec = -1;  // 关闭通道定时

                if (g_hw_table.channels[i].stat) {
                    sprintf(buff, "{\"%d\":true,\"%d\":0}", g_hw_table.channels[i].dpid, g_hw_table.channels[i].cd_dpid);
                } else {
                    sprintf(buff, "{\"%d\":false,\"%d\":0}", g_hw_table.channels[i].dpid, g_hw_table.channels[i].cd_dpid);
                }
                tuya_obj_dp_report(buff);
            } else {
                if (g_hw_table.channels[i].cd_sec % 30 == 0) {
                    sprintf(buff, "{\"%d\":%d}", g_hw_table.channels[i].cd_dpid, g_hw_table.channels[i].cd_sec);
                    tuya_obj_dp_report(buff);
                }
            }
        }
    }
}

/**
 * @brief wifi状态监控定时回调
 * 
 * @param timerID 定时器ID
 * @param pTimerArg 
 * @return STATIC wfl_timer_cb 
 */
STATIC VOID wfl_timer_cb(UINT timerID, PVOID pTimerArg) {
	STATIC UINT last_wf_stat = 0xffffffff;
	STATIC BOOL is_syn = FALSE;

	GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
	if (last_wf_stat != wf_stat) {
		PR_DEBUG("wf_stat:%d", wf_stat);
		hw_set_wifi_led_stat(&g_hw_table, wf_stat);
		last_wf_stat = wf_stat;
		
		/* 设备离线/同步标志清除 */
		if (wf_stat == STAT_STA_UNCONN && is_syn) {
			is_syn = FALSE;
		}
	}

	/* 等待系统正常工作/同步所有状态 */
	if (is_syn == FALSE && (STAT_WORK == tuya_get_gw_status())) {
		all_channel_stat_upload();
		is_syn = TRUE;
	}
}

/**
 * @brief 系统所有状态上报
 * 
 * @return 
 */
STATIC VOID all_channel_stat_upload(VOID) 
{
	INT i;
	INT remain_sec = 0;
	CHAR buff[4];
	cJSON *root = NULL;
	root = cJSON_CreateObject();
	if (NULL == root) {
		PR_ERR("C_JSON_ERROR");
		return;
	}

	for (i = 0; i < g_hw_table.channel_num; ++i) {
		/* 通道状态 */
		sprintf(buff, "%d", g_hw_table.channels[i].dpid);
		if (g_hw_table.channels[i].stat) {
			cJSON_AddTrueToObject(root, buff);
		} else {
			cJSON_AddFalseToObject(root, buff);
		}

		if (g_hw_table.channels[i].cd_dpid != DPID_NOT_EXIST) {
			/* 倒计时上报 */
			if (g_hw_table.channels[i].cd_sec > 0) {
				remain_sec = g_hw_table.channels[i].cd_sec;
			} else {
				remain_sec = 0;
			}
			sprintf(buff, "%d", g_hw_table.channels[i].cd_dpid);
			cJSON_AddNumberToObject(root, buff, remain_sec);
		}
	}
	CHAR *out = cJSON_PrintUnformatted(root);
	if (NULL == out) {
		PR_ERR("OUT = NULL");
		cJSON_Delete(root);
		return;
	}
	/* 查看上报信息 */
	PR_DEBUG("OUT = %s ", out);

	tuya_obj_dp_report(out);
	Free(out);
	cJSON_Delete(root);
	
	return;
}
