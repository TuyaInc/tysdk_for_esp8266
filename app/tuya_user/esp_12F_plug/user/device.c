/***********************************************************
*  File: device.c  
*  Author: husai
*  Date: 20151215
***********************************************************/
#define __DEVICE_GLOBALS
#include "device.h"
#include "tuya_smart_api.h"
#include "wf_sdk_adpt.h"


/***********************************************************
*************************micro define***********************
***********************************************************/
#define SYN_TIME  2  //同步开关状态
#define ONE_SEC   1
#define WM_SUCCESS 0
#define WM_FAIL 1

#define DEVICE_MOD "device_mod"
#define DEVICE_PART "device_part"
#define POWER_STAT_KEY "power_stat_key"

#define TIME_POSIX_2016 1451577600 //2016年时间戳

// reset key define
#define WF_RESET_KEY GPIO_ID_PIN(0)  //WIFI复位按键
// wifi direct led
#define WF_DIR_LED GPIO_ID_PIN(16)   //WiFi指示灯

// power contrl
#define POWER_CTRL GPIO_ID_PIN(12)  //继电器控制
#define POWER_LED  GPIO_ID_PIN(5)   //电源开关指示灯

typedef enum
{
	DP_SWITCH = 1,  //开关
	DP_COUNT_DOWN = 2,  //倒计时
}DP_ID;

typedef struct 
{
    INT power;
    INT appt_sec;
    TIMER_ID syn_timer;
    TIMER_ID count_timer;
    TIMER_ID save_timer;
    THREAD msg_thread;
    THREAD sec_thread;   
    SEM_HANDLE press_key_sem;
    SEM_HANDLE sec_up_sem; 
}TY_MSG_S;

/***********************************************************
*************************variable define********************
***********************************************************/
STATIC TY_MSG_S ty_msg;
STATIC LED_HANDLE wf_light = NULL;
STATIC LED_HANDLE power_ctrl = NULL;
STATIC LED_HANDLE power_led = NULL;

/***********************************************************
*************************function define***********************
***********************************************************/
STATIC VOID key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt);
STATIC VOID wfl_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC OPERATE_RET device_differ_init(VOID);
STATIC OPERATE_RET set_power_stat(INT state);
STATIC OPERATE_RET get_power_stat(INT *state);

STATIC KEY_ENTITY_S key_tbl[] = {
    {WF_RESET_KEY,3000,key_process,0,0,0,0},
};

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
    //需要设置为输出的引脚 放置在数组中
    uint8 gpio_out_config[]={12,5,16};

    //调用set_gpio_out函数  第一个传参为数组指针，第二个参数为设置的io个数。
    //数组中的io设置为输出，其余为输入。
    //此函数只设置为输出状态，不设置电平状态。
    set_gpio_out(gpio_out_config,CNTSOF(gpio_out_config));
}

VOID app_init(VOID)
{
   app_cfg_set(WCM_OLD,NULL);
}

STATIC INT msg_upload_proc(INT state)
{
    GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
    if((wf_stat < STAT_STA_CONN) || \
       (tuya_get_gw_status() != STAT_WORK)) {
        return WM_FAIL;
    }

    cJSON *root = cJSON_CreateObject();
    if(NULL == root) {
        return WM_FAIL;
    }
    
    cJSON_AddBoolToObject(root,"1",state);
    if( ty_msg.appt_sec == 0 ) {
        cJSON_AddNumberToObject(root, "2", 0);
    }
    
    CHAR *out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("cJSON_PrintUnformatted err:");
        return WM_FAIL;
    }
    
//    PR_DEBUG("out[%s]", out);
    OPERATE_RET op_ret = tuya_obj_dp_report(out);
    Free(out);
    if( OPRT_OK == op_ret ) {
        return WM_SUCCESS;
    }else {
        return WM_FAIL;
    }
}

STATIC INT msg_upload_sec(INT sec)
{
    GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
    if((wf_stat < STAT_STA_CONN) || \
       (tuya_get_gw_status() != STAT_WORK)) {
        return WM_FAIL;
    }

    cJSON *root = cJSON_CreateObject();
    if(NULL == root) {
        return WM_FAIL;
    }
    
    cJSON_AddNumberToObject(root, "2", sec);
    CHAR *out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("cJSON_PrintUnformatted err:");
        return WM_FAIL;
    }

    OPERATE_RET op_ret;
    op_ret = tuya_obj_dp_report(out);
    Free(out);
    if( OPRT_OK == op_ret ) {
        return WM_SUCCESS;
    }else {
        return WM_FAIL;
    }
}

STATIC VOID msg_upload_all_stat(VOID)
{
    GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
    if((wf_stat < STAT_STA_CONN) || \
       (tuya_get_gw_status() != STAT_WORK)) {
        return;
    }

    cJSON *root = cJSON_CreateObject();
    if(NULL == root) {
        return;
    }

    CHAR dp_buf[4] = {0};
    sprintf(dp_buf,"%d",DP_SWITCH);
    if(0 == ty_msg.power) {
        cJSON_AddFalseToObject(root,dp_buf);
    }else {
        cJSON_AddTrueToObject(root,dp_buf);
    }

    sprintf(dp_buf,"%d",DP_COUNT_DOWN);
    UINT remain_sec = 0;
    UINT cur_posix = wmtime_time_get_posix();
    if(ty_msg.appt_sec > cur_posix) {
        remain_sec = ty_msg.appt_sec - cur_posix;
    }
    cJSON_AddNumberToObject(root, dp_buf, remain_sec);
    CHAR *out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("malloc err");
        return;
    }

    tuya_obj_dp_report(out);
    Free(out);
    return;
}

STATIC VOID msg_send(INT cmd)
{
    if(cmd == ty_msg.power) {
        msg_upload_proc(cmd);
    }else {
        PostSemaphore(ty_msg.press_key_sem);
    }
}

STATIC VOID ctrl_power_led(INT state)
{
    if( state ) {
        tuya_set_led_type(power_ctrl,OL_HIGH,0);
        tuya_set_led_type(power_led,OL_HIGH,0);
    }else {
        tuya_set_led_type(power_ctrl,OL_LOW,0);
        tuya_set_led_type(power_led,OL_LOW,0);
    }
}

STATIC VOID msg_proc(PVOID pArg)
{
    while( 1 ) {
        WaitSemaphore(ty_msg.press_key_sem);
        if(ty_msg.power != 0) {
            ty_msg.power = 0;
        }else {
            ty_msg.power = 1;
        }
        
        //根据power状态控制继电器和电源指示灯
        ctrl_power_led(ty_msg.power);
        if( ty_msg.appt_sec > 0 ) {
            ty_msg.appt_sec = 0;
        }

        sys_start_timer(ty_msg.save_timer,5*1000,TIMER_ONCE);
        msg_upload_proc(ty_msg.power);
        SystemSleep(50);
    }
}

STATIC VOID sec_proc(PVOID pArg)
{
    while( 1 ) {
        WaitSemaphore(ty_msg.sec_up_sem);
        INT sec = wmtime_time_get_posix();
        if( ty_msg.appt_sec == 0 ) {
            msg_upload_sec(0);
        }else if( ty_msg.appt_sec > sec ) {
            msg_upload_sec(ty_msg.appt_sec - sec);
        }else {
            msg_upload_sec(0);
        }
        SystemSleep(50);
    }
}

/***********************************************************
*  Function: fish_dps_handle
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
STATIC VOID deal_dps_handle(UCHAR dps,cJSON *nxt)
{
    switch(dps)
    {
        case DP_SWITCH: {
            if(nxt->type == cJSON_False) {
                msg_send(0);
            }else if(nxt->type == cJSON_True) {
                msg_send(1);
            }
        }
        break;

        case DP_COUNT_DOWN: {
            if(nxt->type == cJSON_Number) {
                if( nxt->valueint == 0 ) { //停止定时
                    ty_msg.appt_sec = 0;
                    msg_upload_sec(0);
                }else {
                    ty_msg.appt_sec = wmtime_time_get_posix() + nxt->valueint;
                    msg_upload_sec(nxt->valueint);
                }
            }
        }
        break;

        default:
        break;
    }
}

/***********************************************************
*************************function define********************
***********************************************************/
STATIC VOID device_cb(SMART_CMD_E cmd,cJSON *root)
{  
//    PR_DEBUG("cmd:%d",cmd);
    cJSON *nxt = root->child; 
    UCHAR dp_id = 0;
    while(nxt) {
        dp_id = atoi(nxt->string);
        deal_dps_handle(dp_id,nxt);
        nxt = nxt->next; 
        SystemSleep(50);
    }
}

STATIC VOID syn_timer_cb(UINT timerID,PVOID pTimerArg)
{
    if( FALSE == tuya_get_cloud_stat() ) {
        return;
    }
    
    INT ret = msg_upload_proc(ty_msg.power);
    if( WM_SUCCESS == ret ) {
        if( ty_msg.appt_sec == 0 ) {
            ret = msg_upload_sec(0);       
            if( WM_SUCCESS == ret ) {            
                sys_stop_timer(ty_msg.syn_timer);
            }
        }else {
            sys_stop_timer(ty_msg.syn_timer);
        }
    }
    PR_DEBUG("syn timer cb ...");
}

STATIC VOID count_timer_cb(UINT timerID,PVOID pTimerArg)
{
    if( ty_msg.appt_sec == 0 ) {
        return;
    }

    INT cur_posix = wmtime_time_get_posix();
    if( cur_posix < ty_msg.appt_sec){
        if( (ty_msg.appt_sec - cur_posix)%30 == 0 ) {
            PostSemaphore(ty_msg.sec_up_sem);//30秒更新一次
        }
    }else {
        ty_msg.appt_sec = 0;
        PostSemaphore(ty_msg.press_key_sem);
        PostSemaphore(ty_msg.sec_up_sem);
        PR_DEBUG("count down...");
    }
}

STATIC VOID save_timer_cb(UINT timerID,PVOID pTimerArg)
{
    set_power_stat(ty_msg.power);
}

STATIC OPERATE_RET set_power_stat(INT state)
{
    cJSON *root = cJSON_CreateObject();
    if(NULL == root) {
		PR_ERR("cJSON_CreateObject error");
		return OPRT_CJSON_GET_ERR;
	}
    
    cJSON_AddNumberToObject(root, "power", state);
    UCHAR *buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("buf is NULL");
        cJSON_Delete(root);
        return OPRT_MALLOC_FAILED;
    }
    cJSON_Delete(root);
    
	OPERATE_RET op_ret = tuya_psm_set_single(DEVICE_MOD,POWER_STAT_KEY,buf);
	Free(buf);
	if(OPRT_OK != op_ret) {
		PR_ERR("tuya_psm_set_single op_ret:%d",op_ret);
		return op_ret;
	}
    
    return OPRT_OK;
}

STATIC OPERATE_RET get_power_stat(INT *state)
{
	OPERATE_RET op_ret;
    cJSON *root = NULL, *json = NULL;

    UCHAR *buf = (UCHAR *)Malloc(128);
	if(NULL == buf) {
		PR_ERR("malloc error");
		return OPRT_MALLOC_FAILED;
	}

	op_ret = tuya_psm_get_single(DEVICE_MOD,POWER_STAT_KEY,buf,128);
	if(OPRT_OK != op_ret) {
		PR_ERR("tuya_psm_get_single op_ret:%d",op_ret);
        Free(buf);
		return op_ret;
	}

	root = cJSON_Parse(buf);
	if(NULL == root) {
		PR_ERR("cjson parse");
        goto JSON_PARSE_ERR;
	}

    json = cJSON_GetObjectItem(root,"power");
    if(NULL == json) {
        PR_ERR("cjson get power");
        goto JSON_PARSE_ERR;
	}

    *state = json->valueint;
    cJSON_Delete(root);
    Free(buf);
    return OPRT_OK;

JSON_PARSE_ERR:
    cJSON_Delete(root);
    Free(buf);
    return OPRT_COM_ERROR;
}

STATIC VOID init_power_stat(VOID)
{
    ty_msg.power = 0;
    ty_msg.appt_sec = 0;

    get_power_stat(&ty_msg.power);

    //恢复默认状态
    ctrl_power_led(ty_msg.power);
}

//查询回调函数，推送设备当前状态
STATIC VOID dp_qeury_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    return msg_upload_all_stat();
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

    op_ret = tuya_psm_register_module(DEVICE_MOD, DEVICE_PART);
    if(op_ret != OPRT_OK && op_ret != OPRT_PSM_E_EXIST) {
        PR_ERR("tuya_psm_register_module error:%d",op_ret);
        return op_ret;
    }

    op_ret = tuya_create_led_handle(POWER_CTRL,&power_ctrl);
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }

    op_ret = tuya_create_led_handle(POWER_LED,&power_led);
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }

    init_power_stat();

    ty_msg.press_key_sem = CreateSemaphore();
    if( NULL == ty_msg.press_key_sem ) {
        return OPRT_MALLOC_FAILED;
    }

    op_ret = InitSemaphore(ty_msg.press_key_sem,0,1);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }

    ty_msg.sec_up_sem = CreateSemaphore();
    if( NULL == ty_msg.sec_up_sem ) {
        return OPRT_MALLOC_FAILED;
    }
    
    op_ret = InitSemaphore(ty_msg.sec_up_sem,0,1);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }
    
    op_ret = CreateAndStart(&ty_msg.msg_thread,msg_proc,NULL,1024,TRD_PRIO_2,"ty_task");
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    op_ret = CreateAndStart(&ty_msg.sec_thread,sec_proc,NULL,1024,TRD_PRIO_2,"sec_task");
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    op_ret = sys_add_timer(syn_timer_cb,NULL,&ty_msg.syn_timer);
    if(OPRT_OK != op_ret) {
		PR_ERR("add syn_timer err");
    	return op_ret;
    }else {
    	sys_start_timer(ty_msg.syn_timer,SYN_TIME*1000,TIMER_CYCLE);
    }

    op_ret = sys_add_timer(count_timer_cb,NULL,&ty_msg.count_timer);
    if(OPRT_OK != op_ret) {
        PR_ERR("add syn_timer err");
        return op_ret;
    }else {
        sys_start_timer(ty_msg.count_timer,ONE_SEC*1000,TIMER_CYCLE);
    }

    op_ret = sys_add_timer(save_timer_cb,NULL,&ty_msg.save_timer);
    if(OPRT_OK != op_ret) {
        PR_ERR("add save_timer err");
        return op_ret;
    }

    op_ret = device_differ_init();
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    return op_ret;
}


STATIC VOID key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt)
{
    PR_DEBUG("gpio_no: %d",gpio_no);
    PR_DEBUG("type: %d",type);
    PR_DEBUG("cnt: %d",cnt);
    PR_DEBUG("remain memory:%d",system_get_free_heap_size());

    if(WF_RESET_KEY == gpio_no) {
        if(LONG_KEY == type) {
            tuya_dev_reset_factory();
        }else if(NORMAL_KEY == type) {
            PostSemaphore(ty_msg.press_key_sem);
        }
    }
}

STATIC OPERATE_RET device_differ_init(VOID)
{
    OPERATE_RET op_ret;

    // 取消连击，需在按键初始化前设置
    set_key_detect_time(0);
    
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
                tuya_set_led_type(wf_light,OL_FLASH_HIGH,250);
            }
            break;
            
            case STAT_AP_STA_UNCONN:
            case STAT_AP_STA_CONN: {
                tuya_set_led_type(wf_light,OL_FLASH_HIGH,1500);
            }
            break;

            case STAT_LOW_POWER:
            case STAT_STA_UNCONN: {
                tuya_set_led_type(wf_light,OL_HIGH,0);
            }
            break;
            
            case STAT_STA_CONN: {
                tuya_set_led_type(wf_light,OL_LOW,0);
            }
            break;
        }

        last_wf_stat = wf_stat;
    }
}



