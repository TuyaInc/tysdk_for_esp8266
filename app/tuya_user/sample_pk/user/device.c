/***********************************************************
*  File: device.c 
*  Author: nzy
*  Date: 20150605
***********************************************************/
#define __DEVICE_GLOBALS
#include "device.h"
#include "tuya_smart_api.h"
#include "wf_sdk_adpt.h"


/***********************************************************
*************************micro define***********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
STATIC OPERATE_RET device_differ_init(VOID);
STATIC VOID key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt);

/***********************************************************
*************************variable define********************
***********************************************************/
#define WF_RESET_KEY GPIO_ID_PIN(0)
#define WF_DIR_LED GPIO_ID_PIN(14)

LED_HANDLE wf_light = NULL;
/***********************************************************
*************************function define********************
***********************************************************/
STATIC VOID wfl_timer_cb(UINT timerID,PVOID pTimerArg);

// call back function
VOID set_firmware_tp(IN OUT CHAR *firm_name, IN OUT CHAR *firm_ver)
{
	strcpy(firm_name,APP_BIN_NAME);
	strcpy(firm_ver,USER_SW_VER);
	return;
}

BOOL gpio_func_test(VOID)
{
	return TRUE;
}

VOID pre_app_init(VOID)
{
    return;
}

VOID prod_test(BOOL flag,CHAR rssi)
{
     if(flag == FALSE){
         PR_ERR("no auth \r\n");
         return;
     }
     PR_NOTICE("product test mode \r\n");
}

VOID app_init(VOID)
{
    set_console(FALSE);
    app_cfg_set(WCM_LOW_POWER,prod_test);
}

VOID device_cb(SMART_CMD_E cmd,cJSON *root)
{
    CHAR *buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("malloc error");
        return;
    }

    PR_DEBUG("root cmd:%s",buf);
    
    OPERATE_RET op_ret = tuya_obj_dp_report(buf);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_obj_dp_report err:%d",op_ret);
        PR_DEBUG_RAW("%s\r\n",buf);
        Free(buf);
        return;
    }
    Free(buf);
}

//查询回调函数，推送设备当前状态
STATIC VOID dp_qeury_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{

}

/***********************************************************
*  Function: device_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
OPERATE_RET device_init(VOID)
{
    PR_NOTICE("fireware info name:%s version:%s",APP_BIN_NAME,USER_SW_VER);
  
    OPERATE_RET op_ret;

    op_ret = tuya_device_init(PRODECT_KEY,device_cb,USER_SW_VER);
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    TY_IOT_CBS_S wf_cbs = {
        .dev_dp_query_cb = dp_qeury_cb,
        .ug_reset_inform_cb = NULL,
    };
    gw_register_cbs(&wf_cbs);

    op_ret = device_differ_init();
    if(op_ret != OPRT_OK) {
        return op_ret;
    }
    
    return op_ret;
}

STATIC VOID wifi_rssi_scan(OPERATE_RET *ret,CHAR *rssi)
{
	signed char result = *rssi;
	if(*ret != OPRT_OK) {
		PR_DEBUG("NO SSID");
	} else {
		PR_DEBUG("RSSI:%d",result);
	}
}

STATIC VOID key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt)
{
    PR_DEBUG("gpio_no: %d",gpio_no);
    PR_DEBUG("type: %d",type);
    PR_DEBUG("cnt: %d",cnt);

    if(WF_RESET_KEY == gpio_no) {
        if(LONG_KEY == type) {
            tuya_dev_reset_factory();
        }else if(SEQ_KEY == type && cnt == 2) {
            ShowSysMemPoolInfo();
        }else if(NORMAL_KEY == type) {
            PR_NOTICE("remain size:%d",system_get_free_heap_size());
        }
    }
}

STATIC OPERATE_RET device_differ_init(VOID)
{
    OPERATE_RET op_ret;
    
    // key process init
    op_ret = tuya_kb_init();
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }

    // register key to process
    op_ret = tuya_kb_reg_proc(WF_RESET_KEY,3000,key_process);
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }

    // create led handle
    op_ret = tuya_create_led_handle(WF_DIR_LED,&wf_light);
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }

    TIMER_ID timer;
    op_ret = sys_add_timer(wfl_timer_cb,NULL,&timer);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }else {
        sys_start_timer(timer,300,TIMER_CYCLE);
    }

    return OPRT_OK;
}

STATIC VOID wfl_timer_cb(UINT timerID,PVOID pTimerArg)
{
    STATIC UINT last_wf_stat = 0xffffffff;
    GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
    
    if(last_wf_stat != wf_stat) {
        PR_DEBUG("wf_stat:%d",wf_stat);
        switch(wf_stat) {
            case STAT_UNPROVISION: {
				PR_DEBUG("STAT_UNPROVISION");
                tuya_set_led_type(wf_light,OL_FLASH_HIGH,250);
            }
            break;
            
            case STAT_AP_STA_UNCONN:
            case STAT_AP_STA_CONN: {
				PR_DEBUG("STAT_AP_STA_UNCONN");
                tuya_set_led_type(wf_light,OL_FLASH_HIGH,1500);
            }
            break;

			case STAT_LOW_POWER:
            case STAT_STA_UNCONN: {
				PR_DEBUG("STAT_STA_UNCONN");
                tuya_set_led_type(wf_light,OL_LOW,0);
            }
            break;
            
            case STAT_STA_CONN: {
				PR_DEBUG("STAT_STA_CONN");
                tuya_set_led_type(wf_light,OL_HIGH,0);
            }
            break;
        }

        last_wf_stat = wf_stat;
    }
}



