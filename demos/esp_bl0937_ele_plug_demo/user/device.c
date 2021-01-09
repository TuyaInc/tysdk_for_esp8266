/**
 * @File: device.c 
 * @Author: caojq 
 * @Last Modified time: 2020-03-05 
 * @Description: 电量统计单插demo方案
 *
 */
#define __DEVICE_GLOBALS
#include "device.h"
#include "mem_pool.h"
#include "smart_wf_frame.h"
#include "key.h"
#include "led_indicator.h"
#include "system/sys_timer.h"
#include "system/uni_thread.h"
#include "system/uni_semaphore.h"
#include "system/uni_mutex.h"
#include "freertos/semphr.h"
#include "uart.h"
#include "wf_sdk_adpt.h"
#include "app_dltj.h"
#include "app_switch.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define SYN_TIME  1  //同步开关状态

#define DEVICE_MOD "device_mod"
#define DEVICE_PART "device_part"

//根据使用的IO口调整，若使用UART0作为日志口持续输出，TXD和RXD都将不可用
#define PRINT_LOG_PORT UART1

/***********************************************************
*************************variable define********************
************************************************************/
STATIC TIMER_ID dv_syn_timer;
extern HW_TABLE g_hw_table;
extern APP_DLTJ_CFG g_dltj;

/***********************************************************
*************************function define********************
***********************************************************/
STATIC VOID syn_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC OPERATE_RET init_hw_tables(VOID);

/***********************************************************
*************************function define********************
***********************************************************/
STATIC VOID deal_dps_handle(IN UCHAR dps,IN cJSON *nxt)
{
    if(nxt->type == cJSON_False) {
        app_send_press_deal(dps, FALSE);
    }else if(nxt->type == cJSON_True) {
        app_send_press_deal(dps, TRUE);
    }else if(nxt->type == cJSON_Number){
        app_send_countdown_deal(dps,nxt->valueint);
    }
}

VOID device_cb(SMART_CMD_E cmd,cJSON *root)
{
    cJSON *nxt = root->child; 
    UCHAR dp_id = 0;
    while(nxt) {
        dp_id = atoi(nxt->string);
        deal_dps_handle(dp_id,nxt);
        nxt = nxt->next;
    }
}

BOOL gpio_func_test(VOID)
{
    return TRUE;
}

VOID prod_test(BOOL flag, CHAR rssi)
{
    OPERATE_RET op_ret;
    op_ret = app_switch_init(APP_SW_MODE_PRODTEST);
    if(op_ret != OPRT_OK) {
        return;
    }
    PR_NOTICE("have scaned the ssid, and enter product test.rssi value:%d",rssi);
    if(rssi < -60) {
        set_wfl_state(WFL_FLASH_VERY_FAST);
        PR_ERR("no authorization, product test err!");
        return;
    }
    if(FALSE == flag) {
        set_wfl_state(WFL_FLASH_VERY_FAST);
        PR_ERR("current rssi value is less than -60 dB, product test err!");
        return;
    }
    if(g_dltj.if_have){
        set_wfl_state(WFL_FLASH_SLOW);
        set_pt_key_en(FALSE);
        UCHAR i;
        for(i = 0;i < g_hw_table.channel_num;i++){
            ctrl_switch_state(i, CTRL_SW_OPEN);
        }
        SystemSleep(1500);
        op_ret = app_dltj_init(APP_DLTJ_PRODTEST);
        for(i = 0;i < g_hw_table.channel_num;i++){
            ctrl_switch_state(i, CTRL_SW_CLOSE);
        }
        set_pt_key_en(TRUE);
        if(op_ret != OPRT_OK) {
            set_wfl_state(WFL_OFF);
        }else{
            set_wfl_state(WFL_ON);
        }
    }
}

STATIC VOID syn_timer_cb(UINT timerID,PVOID pTimerArg)
{
    if(!get_gw_mq_conn_stat()) {
        return;
    }
    PR_DEBUG("enter data syn...");
    if(OPRT_OK!= upload_all_switch_state()){
        PR_DEBUG("syn switch data fail!!!");
    }else{
        sys_stop_timer(dv_syn_timer);
        PR_DEBUG("syn timer cb end...");
    }
}

VOID reset_power_default(VOID)
{
    reset_power_stat();
    reset_clear_temp_ele();
    reset_clear_ele();
}

VOID app_init(VOID)
{
    if(UART1 == PRINT_LOG_PORT) {
        print_port_init(UART1);
    }

    OPERATE_RET op_ret;
    op_ret = tuya_psm_register_module(DEVICE_MOD,DEVICE_PART);
    if(op_ret != OPRT_OK && op_ret != OPRT_PSM_E_EXIST) {
        PR_ERR("msf_register_module error:%d",op_ret);
        return;
    }

    app_cfg_set(g_hw_table.wf_led.wcm_mode,prod_test);
    PR_DEBUG("wifi_mode:%d",g_hw_table.wf_led.wcm_mode);
    INT pt_end;
    op_ret = get_prod_test_data(&pt_end);
    if((OPRT_OK == op_ret) && (1 == pt_end)){
        set_prod_ssid("tuya_mdev_test2");
        PR_NOTICE("need scan ssid: tuya_mdev_test2 to enter product test repeatedly.");
    } else {
        PR_NOTICE("need scan ssid: tuya_mdev_test1 to enter product test.");
    }
}

STATIC VOID local_dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    PR_NOTICE("Recv DP Query Cmd");
    if (NULL == dp_qry) {
        PR_ERR("dp_query input param is null!");
        return;
    }
    TY_DP_QUERY_S *d = (TY_DP_QUERY_S *)dp_qry;
    if (NULL == d->cid) {
        PR_ERR("dev id query, return");
        return;
    }
    PR_NOTICE("cid:%s, cnt:%d", dp_qry->cid, dp_qry->cnt);    
    if (0 == d->cnt) {
        PR_NOTICE("dp count = 0! ready to report all dp");
    } else {
        switch_ele_dp_query(d);
    }
}

OPERATE_RET device_init(VOID)
{
    OPERATE_RET op_ret;
    op_ret = tuya_device_init(PRODUCT_KEY,device_cb,USER_SW_VER);
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    TY_IOT_CBS_S wf_cbs = {
        .dev_dp_query_cb = local_dev_dp_query_cb,
        .ug_reset_inform_cb = NULL,
    };
    op_ret = gw_register_cbs(&wf_cbs);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    op_ret = sys_add_timer(syn_timer_cb,NULL,&dv_syn_timer);
    if(OPRT_OK != op_ret) {
        PR_ERR("add syn_timer err");
        return op_ret;
    }else {
        sys_start_timer(dv_syn_timer,SYN_TIME*1000,TIMER_CYCLE);
    }
    
    op_ret= app_dltj_init(APP_DLTJ_NORMAL);
    if(OPRT_OK != op_ret) {
        PR_ERR("dltj init err!");
        return op_ret;
    }

    op_ret = app_switch_init(APP_SW_MODE_NORMAL);
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    return op_ret;
}

VOID pre_app_init(VOID)
{
    return;
}

VOID set_firmware_tp(IN OUT CHAR *firm_name, IN OUT CHAR *firm_ver)
{
	strcpy(firm_name,APP_BIN_NAME);
	strcpy(firm_ver,USER_SW_VER);
	return;
}

