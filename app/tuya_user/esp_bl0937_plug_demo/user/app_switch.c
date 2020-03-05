/**
 * @File: app_switch.c
 * @Author: caojq
 * @Last Modified time: 2020-03-05
 * @Description: 电量统计开关操作
 */
 
#include <mem_pool.h>// 编译器非标准，在使用下面方法初始化结构体时依赖此头文件
#include "smart_wf_frame.h"
//#include "led_indicator.h"
#include "system/sys_timer.h"
#include "system/uni_thread.h"
#include "system/uni_mutex.h"
#include "system/uni_semaphore.h"
#include "wf_sdk_adpt.h"
#include "app_switch.h"
#include "device.h"
#include "tuya_smart_api.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define _APP_SWITCH_GLOBAL
#define DEFAULT_KEY_RST_TIME_S 3
#define COUNTDOWN_REPT_TIME 30
#define DEVICE_MOD "device_mod"
#define POWER_STAT_KEY "relay_stat_key"
#define PT_END_KEY "pt_end_key"

/***********************************************************
*************************variable define********************
************************************************************/
STATIC BOOL wf_dir_rl_en = FALSE;     //允许wifi灯指示继电器
STATIC BOOL pt_key_en = TRUE;      //允许产测按键使用
STATIC uint8 pt_curr_ch = 0;
STATIC CTRL_SW pt_ch_deal = CTRL_SW_OPEN;

STATIC TIMER_ID countdown_timer;
STATIC TIMER_ID save_stat_timer;
STATIC TIMER_ID switch_wfl_timer;
STATIC TIMER_ID enter_pt_timer;
STATIC TIMER_ID end_pt_timer;
STATIC TIMER_ID pt_total_bt_timer;

CTRL_CHANNEL_CONFIG ctrl_channels[] = {
    { .led = { .type = IO_DRIVE_LEVEL_NOT_EXIST },
        .relay = { .type = IO_DRIVE_LEVEL_HIGH, .pin = 14 },
        .button = { .type = IO_DRIVE_LEVEL_NOT_EXIST },
        .dpid = 1,
        .cd_dpid = 9,
        .init_ch_stat = INIT_CH_CLOSE,
        .cd_sec = 0 }
};

HW_TABLE g_hw_table = {
    .channels = ctrl_channels,
    .wf_led = {
        .wfl_io = { .type = IO_DRIVE_LEVEL_LOW, .pin = 13 },
        .press_time = DEFAULT_KEY_RST_TIME_S,
        .wfl_cs = WFL_DIR_RL, /* wifi灯连接时指示状态 */
        .wfl_ucs = WFL_DIR_RL,
        .wcm_mode = WCM_LOW_POWER,
    },
    .tch = {
        .tled = { .type = IO_DRIVE_LEVEL_NOT_EXIST },
        .tbt = { .type = IO_DRIVE_LEVEL_LOW, .pin = 3 },
    }
};

/***********************************************************
*************************function define********************
***********************************************************/
STATIC OPERATE_RET hw_switch_init(IN APP_SW_MODE mode);
STATIC OPERATE_RET read_saved_stat(VOID);
STATIC OPERATE_RET upload_switch_state(IN UCHAR channel_num);
STATIC OPERATE_RET upload_switch_countdown(IN UCHAR channel_num);
STATIC VOID set_hw_relay(IN UCHAR channel_num,IN BOOL state);
STATIC VOID set_hw_led(IN UCHAR channel_num,IN BOOL state);
STATIC VOID set_hw_wifi_led(IN BOOL state);
STATIC VOID set_hw_total_led(IN BOOL state);
STATIC VOID init_switch_state(VOID);
STATIC VOID countdown_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID enter_pt_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID end_pt_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID pt_total_bt_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID save_stat_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID switch_wfl_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID wfl_direct_relay(VOID);
STATIC VOID hw_key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt);
STATIC VOID pt_hw_key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt);
STATIC VOID key_short_press_deal(IN INT key_num,IN KEY_TYPE key_type);
STATIC VOID channel_event_deal(IN UCHAR channel_num,IN CHAN_EVENT_TYPE event_type);

//判断有任何开关处于开启/关闭状态，如果是返回true。
BOOL judge_any_sw(IN BOOL on_or_off)
{
    BOOL ch_state;
    UCHAR i;
    if(on_or_off){
        ch_state = FALSE;
        for(i = 0; i<g_hw_table.channel_num; i++){
            ch_state = ch_state || g_hw_table.channels[i].stat;
        }
        return ch_state;
    }else{
        ch_state = TRUE;
        for(i = 0; i < g_hw_table.channel_num; i++){
            ch_state = ch_state && g_hw_table.channels[i].stat;
        }
        return !ch_state;
    }
}

//按键短按处理，三种按键事件（优先全开，优先全关，单通道取反）
STATIC VOID key_short_press_deal(IN INT key_num,IN KEY_TYPE key_type)
{
    UCHAR i;
    if(key_type == KEY_ALL_PRIOR_OPEN){
        BOOL ch_state = judge_any_sw(FALSE);//若有一个通道关闭，则全开
        total_channel_event_deal(ch_state);
    }else if(key_type == KEY_ALL_PRIOR_CLOSE){
        BOOL ch_state = judge_any_sw(TRUE);//若有一个通道开通，则全关
        total_channel_event_deal(!ch_state);
    }else{
        for(i = 0;i < g_hw_table.channel_num; i++){
            if(key_num == g_hw_table.channels[i].button.pin){
                channel_event_deal(i,CHAN_EVENT_TOGGLE);
                return;
            }
        }
    }
    return;
}

//app短按开关处理
VOID app_send_press_deal(IN       UINT ch_dp_id,IN BOOL ch_state)
{
    UCHAR i;
    PR_DEBUG("app press deal...");
    for(i = 0; i<g_hw_table.channel_num; i++) {
        if(ch_dp_id == g_hw_table.channels[i].dpid) {
            if(ch_state != g_hw_table.channels[i].stat) {
                channel_event_deal(i,ch_state ? CHAN_EVENT_OPEN : CHAN_EVENT_CLOSE);
            }else {
                upload_switch_state(i);
            }
            return;
        }
    }
    return;
}

//app倒计时下发处理
VOID app_send_countdown_deal(IN UINT cd_dp_id,IN INT cd_secs)
{
    UCHAR i;
    for(i = 0; i<g_hw_table.channel_num; i++){
        if(cd_dp_id == g_hw_table.channels[i].cd_dpid){
            g_hw_table.channels[i].cd_sec = cd_secs;
            upload_switch_countdown(i);//如果不上传倒计时时间APP是否会开始计数？
            sys_start_timer(countdown_timer, 1000, TIMER_CYCLE);
            return;
        }
    }
    return;
}

//对通道继电器的纯硬件操作
STATIC VOID set_hw_relay(IN UCHAR channel_num,IN BOOL state)
{
    if(g_hw_table.channels[channel_num].relay.type == IO_DRIVE_LEVEL_HIGH){
        tuya_set_led_type(g_hw_table.channels[channel_num].relay_handle,\
            (state ? OL_HIGH :OL_LOW), 0);
    }else if(g_hw_table.channels[channel_num].relay.type == IO_DRIVE_LEVEL_LOW){
        tuya_set_led_type(g_hw_table.channels[channel_num].relay_handle,\
            (state ? OL_LOW :OL_HIGH), 0);
    }else if(g_hw_table.channels[channel_num].relay.type == IO_DRIVE_LEVEL_NOT_EXIST){
        //若通道继电器不由模块GPIO控制，在此处控制
    }
    wfl_direct_relay();
    BOOL if_on = judge_any_sw(TRUE);
    set_hw_total_led(if_on);
}

//对通道指示灯的纯硬件操作
STATIC VOID set_hw_led(IN UCHAR channel_num,IN BOOL state)
{
    if(g_hw_table.channels[channel_num].led.type == IO_DRIVE_LEVEL_HIGH){
        tuya_set_led_type(g_hw_table.channels[channel_num].led_handle,\
            (state ? OL_HIGH :OL_LOW), 0);
    }else if(g_hw_table.channels[channel_num].led.type == IO_DRIVE_LEVEL_LOW){
        tuya_set_led_type(g_hw_table.channels[channel_num].led_handle,\
            (state ? OL_LOW :OL_HIGH), 0);
    }else if(g_hw_table.channels[channel_num].led.type == IO_DRIVE_LEVEL_NOT_EXIST){
        //若通道LED不由模块GPIO控制，在此处控制
    }
}

//对wifi灯的纯硬件操作
STATIC VOID set_hw_wifi_led(IN BOOL state)
{
    if(g_hw_table.wf_led.wfl_io.type == IO_DRIVE_LEVEL_HIGH){
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
            (state ? OL_HIGH :OL_LOW), 0);
    }else if(g_hw_table.wf_led.wfl_io.type == IO_DRIVE_LEVEL_LOW){
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
            (state ? OL_LOW :OL_HIGH), 0);
    }else if(g_hw_table.wf_led.wfl_io.type == IO_DRIVE_LEVEL_NOT_EXIST){
        //若wifiLED不由模块GPIO控制，在此处控制
    }
}

//对总led灯的纯硬件操作
STATIC VOID set_hw_total_led(IN BOOL state)
{
    if(g_hw_table.tch.tled.type == IO_DRIVE_LEVEL_HIGH){
        tuya_set_led_type(g_hw_table.tch.tled_handle,\
            (state ? OL_HIGH :OL_LOW), 0);
    }else if(g_hw_table.tch.tled.type == IO_DRIVE_LEVEL_LOW){
        tuya_set_led_type(g_hw_table.tch.tled_handle,\
            (state ? OL_LOW :OL_HIGH), 0);
    }else if(g_hw_table.tch.tled.type == IO_DRIVE_LEVEL_NOT_EXIST){
        //若总LED不由模块GPIO控制，在此处控制
    }
}

//控制某一个通道的 （状态+继电器+指示灯），三者绑定。
VOID ctrl_switch_state(IN UCHAR channel_num,IN CTRL_SW ctrl_sw)
{
    if(channel_num < 0 || channel_num >= g_hw_table.channel_num){
        PR_ERR("input channel number error: %d", channel_num);
        return;
    }
    switch(ctrl_sw){
    case CTRL_SW_OPEN:
        g_hw_table.channels[channel_num].stat = TRUE;
    break;
    case CTRL_SW_CLOSE:
        g_hw_table.channels[channel_num].stat = FALSE;
    break;
    case CTRL_SW_TOGGLE:
        g_hw_table.channels[channel_num].stat = !g_hw_table.channels[channel_num].stat;
    break;
    case CTRL_SW_STAT:
    break;
    default:
    break;
    }
    set_hw_relay(channel_num,g_hw_table.channels[channel_num].stat);
    set_hw_led(channel_num,g_hw_table.channels[channel_num].stat);
}

//倒计时定时器回调，在有任何一个通道需要倒计时时开启，都结束后关闭定时器
STATIC VOID countdown_timer_cb(UINT timerID,PVOID pTimerArg)
{
    UCHAR i;
    BOOL countdown_state = FALSE;
    //PR_DEBUG("countdown -1s... ");
    for(i = 0; i<g_hw_table.channel_num; i++){
        if(g_hw_table.channels[i].cd_dpid > 0){//若存在倒计时
            if(g_hw_table.channels[i].cd_sec > 0){
                g_hw_table.channels[i].cd_sec--;
                if(g_hw_table.channels[i].cd_sec == 0){
                    channel_event_deal(i, CHAN_EVENT_TOGGLE);
                }
                if(g_hw_table.channels[i].cd_sec % COUNTDOWN_REPT_TIME == 0){
                    upload_switch_countdown(i);
                }
                countdown_state = countdown_state || g_hw_table.channels[i].cd_sec;
            }else{
                g_hw_table.channels[i].cd_sec = 0;
            }
            PR_DEBUG("ch[%d] sec:%d",i,g_hw_table.channels[i].cd_sec);
        }
    }
    if(!countdown_state){
        if(IsThisSysTimerRun(countdown_timer)){
            sys_stop_timer(countdown_timer);
        }
    }
}

//单通道事件,在正常工作时调用，会触发上报及断电记忆储存
STATIC VOID channel_event_deal(IN UCHAR channel_num,IN CHAN_EVENT_TYPE event_type)
{
    if(channel_num < 0 || channel_num >= g_hw_table.channel_num){
        PR_ERR("input channel number error: %d", channel_num);
        return;
    }

    switch(event_type){
    case CHAN_EVENT_OPEN:
        ctrl_switch_state(channel_num,CTRL_SW_OPEN);
        break;
    case CHAN_EVENT_CLOSE:
        ctrl_switch_state(channel_num,CTRL_SW_CLOSE);
        break;
    case CHAN_EVENT_TOGGLE:
        ctrl_switch_state(channel_num,CTRL_SW_TOGGLE);
        break;
    default:
        break;
    }
    upload_switch_state(channel_num);
    if(g_hw_table.channels[channel_num].init_ch_stat == INIT_CH_MEM){
        sys_start_timer(save_stat_timer, 5000, TIMER_ONCE);
    }
}

//全通道事件，在正常工作时调用，会触发上报及断电记忆储存
VOID total_channel_event_deal(IN BOOL on_or_off)
{
    UCHAR i;
    BOOL exist_pwr_mem = FALSE;
    for(i = 0; i<g_hw_table.channel_num; i++){
        ctrl_switch_state(i,on_or_off? CTRL_SW_OPEN : CTRL_SW_CLOSE);
        if(g_hw_table.channels[i].init_ch_stat == INIT_CH_MEM){
            exist_pwr_mem = TRUE;
        }
    }
    if(exist_pwr_mem){
        sys_start_timer(save_stat_timer, 5000, TIMER_ONCE);
    }
    if(OPRT_OK != upload_all_switch_state()){
        PR_DEBUG("upload all switch data err");
    }
}

STATIC VOID init_switch_state(VOID)
{
    UCHAR i;
    BOOL exist_pwr_mem = FALSE;
    for(i = 0; i < g_hw_table.channel_num; i++){
        switch(g_hw_table.channels[i].init_ch_stat){
        case INIT_CH_CLOSE:
            ctrl_switch_state(i,CTRL_SW_CLOSE);
            break;
        case INIT_CH_OPEN:
            ctrl_switch_state(i,CTRL_SW_OPEN);
            break;
        case INIT_CH_MEM:
            exist_pwr_mem = TRUE;
            break;
        default:
            break;
        }
    }
    if(exist_pwr_mem){//若存在需要断电记忆的通道
        read_saved_stat();
    }
}

STATIC OPERATE_RET hw_switch_init(IN APP_SW_MODE mode)
{
    PR_DEBUG("initialize sw hardware...");
    OPERATE_RET op_ret = OPRT_OK;
    tuya_set_kb_seq_enable(FALSE);
    op_ret = tuya_kb_init();
    if(OPRT_OK != op_ret) {
        return op_ret;
    }

    g_hw_table.channel_num = SIZEOF(ctrl_channels) / SIZEOF(CTRL_CHANNEL_CONFIG);
    PR_DEBUG("number:%d", g_hw_table.channel_num);

    if(g_hw_table.channel_num <= 0){
        PR_ERR("Error number of channel: %d", g_hw_table.channel_num);
        return OPRT_INVALID_PARM;
    }

    // 初始化wifi状态指示灯
    if(g_hw_table.wf_led.wfl_io.type != IO_DRIVE_LEVEL_NOT_EXIST){
        op_ret = tuya_create_led_handle(g_hw_table.wf_led.wfl_io.pin ,\
        &g_hw_table.wf_led.wfl_handle);
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
        g_hw_table.wf_led.wfl_io.type ==\
        IO_DRIVE_LEVEL_HIGH ? OL_LOW : OL_HIGH, 0);
        if(mode == APP_SW_MODE_NORMAL) {
            op_ret = sys_add_timer(switch_wfl_timer_cb,NULL,&switch_wfl_timer);
            if(OPRT_OK != op_ret) {
                return op_ret;
            }else {
                sys_start_timer(switch_wfl_timer,300,TIMER_CYCLE);
            }
        }
        op_ret = sys_add_timer(enter_pt_timer_cb,NULL,&enter_pt_timer);
        if(OPRT_OK != op_ret) {
            return op_ret;
        }
        op_ret = sys_add_timer(end_pt_timer_cb,NULL,&end_pt_timer);
        if(OPRT_OK != op_ret) {
            return op_ret;
        }
    }
    UCHAR i = 0;
    // 初始化控制通道
    for(i = 0; i < g_hw_table.channel_num; i++){
        g_hw_table.channels[i].stat = FALSE; // 初始状态
        g_hw_table.channels[i].cd_sec = 0;
        if(g_hw_table.channels[i].relay.type != IO_DRIVE_LEVEL_NOT_EXIST){
            op_ret = tuya_create_led_handle(g_hw_table.channels[i].relay.pin ,\
            &g_hw_table.channels[i].relay_handle);
            tuya_set_led_type(g_hw_table.channels[i].relay_handle, \
            g_hw_table.channels[i].relay.type ==\
            IO_DRIVE_LEVEL_HIGH ? OL_LOW : OL_HIGH, 0);
        }else {
            //如继电器非由模块GPIO直接控制，在此处初始化
        }
        if(g_hw_table.channels[i].button.type != IO_DRIVE_LEVEL_NOT_EXIST){
            if(g_hw_table.channels[i].button.type == IO_DRIVE_LEVEL_HIGH){
                set_key_detect_high_valid(TRUE);
            }
            tuya_kb_reg_proc(g_hw_table.channels[i].button.pin,\
            g_hw_table.wf_led.press_time * 1000,\
            mode == APP_SW_MODE_NORMAL ? hw_key_process : pt_hw_key_process);
        }else {
            //如按键非由模块GPIO直接控制，在此处初始化
        }
        if(g_hw_table.channels[i].led.type != IO_DRIVE_LEVEL_NOT_EXIST){
            op_ret = tuya_create_led_handle(g_hw_table.channels[i].led.pin ,\
            &g_hw_table.channels[i].led_handle);
            tuya_set_led_type(g_hw_table.channels[i].led_handle, \
            g_hw_table.channels[i].led.type ==\
            IO_DRIVE_LEVEL_HIGH ? OL_LOW : OL_HIGH, 0);
        }else {
            //如通道LED非由模块GPIO直接控制，在此处初始化
        }
    }

    if(g_hw_table.tch.tbt.type != IO_DRIVE_LEVEL_NOT_EXIST){
        if(g_hw_table.tch.tbt.type == IO_DRIVE_LEVEL_HIGH){
            tuya_set_kb_detect_high_valid(TRUE);
        }
        tuya_kb_reg_proc(g_hw_table.tch.tbt.pin, g_hw_table.wf_led.press_time* 1000,\
        mode == APP_SW_MODE_NORMAL ? hw_key_process : pt_hw_key_process);
        PR_DEBUG("There exist totoal button:%d.",g_hw_table.tch.tbt.pin);

        if(APP_SW_MODE_PRODTEST == mode){
            op_ret = sys_add_timer(pt_total_bt_timer_cb,NULL,&pt_total_bt_timer);
            if(OPRT_OK != op_ret) {
                return op_ret;
            }
        }
    }else{
        PR_DEBUG("There not exist totol button.");
        //如按键非由模块GPIO直接控制，在此处初始化
    }

    if(g_hw_table.tch.tled.type != IO_DRIVE_LEVEL_NOT_EXIST){
        op_ret = tuya_create_led_handle(g_hw_table.tch.tled.pin ,\
        &g_hw_table.tch.tled_handle);
        tuya_set_led_type(g_hw_table.tch.tled_handle,\
        g_hw_table.tch.tled.type ==\
        IO_DRIVE_LEVEL_HIGH ? OL_LOW : OL_HIGH, 0);
        PR_DEBUG("There exist totol relay led:%d.",g_hw_table.tch.tled.pin);
    }else{
        PR_DEBUG("There not exist totol relay led.");
        //如总继电器led非由模块GPIO直接控制，在此处初始化
    }

    return op_ret;
}

OPERATE_RET app_switch_init(IN APP_SW_MODE mode)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = hw_switch_init(mode);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }
    init_switch_state();
    if(APP_SW_MODE_NORMAL == mode){
        op_ret = sys_add_timer(countdown_timer_cb, NULL, &countdown_timer);
        if(OPRT_OK != op_ret) {
            return op_ret;
        }
        
        op_ret = sys_add_timer(save_stat_timer_cb, NULL, &save_stat_timer);
        if(OPRT_OK != op_ret) {
            return op_ret;
        }
    }else{

    }
    PR_DEBUG("app switch init success");
    return op_ret;
}

//正常工作模式下按键回调
STATIC VOID hw_key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt)
{
    PR_DEBUG("gpio_no: %d, type: %d, cnt: %d",gpio_no,type,cnt);
    if(LONG_KEY == type) {
        tuya_dev_reset_factory();
    }else if(NORMAL_KEY == type) {
        if(IO_DRIVE_LEVEL_NOT_EXIST != g_hw_table.tch.tbt.type && \
        gpio_no == g_hw_table.tch.tbt.pin){
            key_short_press_deal(gpio_no,KEY_ALL_PRIOR_CLOSE);
        }else{
            key_short_press_deal(gpio_no,KEY_SINGLE_TOGGLE);
        }
    }
}
//产测按键回调函数
STATIC VOID pt_hw_key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt)
{
    UCHAR i;
    PR_DEBUG("gpio_no: %d, type: %d, cnt: %d",gpio_no,type,cnt);
/*    if(g_hw_table.wf_led.wfl_cs == WFL_DIR_RL || \
    g_hw_table.wf_led.wfl_ucs == WFL_DIR_RL){
        set_wfl_state(WFL_DIR_RL);
    }else{
        set_wfl_state(WFL_OFF);
    }*/
    if(pt_key_en && (NORMAL_KEY == type)) {
        if(IO_DRIVE_LEVEL_NOT_EXIST != g_hw_table.tch.tbt.type && \
        gpio_no == g_hw_table.tch.tbt.pin){
            if(!IsThisSysTimerRun(pt_total_bt_timer)){
                if(judge_any_sw(TRUE) && judge_any_sw(FALSE)){//如果存在半开半关，先全关
                    for(i = 0; i<g_hw_table.channel_num; i++){
                        ctrl_switch_state(i, CTRL_SW_CLOSE);
                    }
                }
                BOOL ch_state = judge_any_sw(TRUE);
                pt_curr_ch = 0;
                pt_ch_deal = ch_state ? CTRL_SW_CLOSE: CTRL_SW_OPEN;
                sys_start_timer(pt_total_bt_timer,500,TIMER_CYCLE);
            }
        }else{
            for(i = 0; i<g_hw_table.channel_num; i++){
                if(gpio_no == g_hw_table.channels[i].button.pin){
                    ctrl_switch_state(i, CTRL_SW_TOGGLE);
                    return;
                }
            }
        }
    }else if(pt_key_en && LONG_KEY == type){
        return;//先按老版本出
        OPERATE_RET op_ret = save_pt_end_flag(1);
        if(OPRT_OK == op_ret){
            PR_DEBUG("save pt end key success!!!");
        }else{
            PR_ERR("save pt end key fail!!!", op_ret);
        }
        set_wfl_state(WFL_END_PT);
    }
}

STATIC VOID save_stat_timer_cb(UINT timerID,PVOID pTimerArg)
{
    OPERATE_RET op_ret = OPRT_OK;
    cJSON *root = NULL;
    UCHAR *buf = NULL;
    UCHAR i = 0;
    root = cJSON_CreateObject();
    if(NULL == root) {
        PR_ERR("cJSON_CreateObject error");
        return;
    }
    CHAR dpid_str[8];
    for(i = 0;i < g_hw_table.channel_num;i++){
        if(g_hw_table.channels[i].init_ch_stat == INIT_CH_MEM){
            sprintf(dpid_str, "%d", i);
            cJSON_AddBoolToObject(root,dpid_str,g_hw_table.channels[i].stat);
        }
    }
    buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("buf is NULL");
        cJSON_Delete(root);
        return;
    }    
    PR_DEBUG("msf_set ch state:%s",buf);
    cJSON_Delete(root);

    op_ret = tuya_psm_set_single(DEVICE_MOD,POWER_STAT_KEY,buf);
    if(OPRT_OK != op_ret) {
        PR_DEBUG("tuya_psm_set_single err:%d",op_ret);
        Free(buf);
        return;
    }
    Free(buf);
    return;
}

STATIC OPERATE_RET read_saved_stat(VOID)
{
    OPERATE_RET op_ret;
    cJSON *root = NULL, *json = NULL;
    UCHAR *buf;
    buf = (UCHAR *)Malloc(256);
    if(NULL == buf) {
        PR_ERR("malloc error");
        return OPRT_MALLOC_FAILED;
    }
    op_ret = tuya_psm_get_single(DEVICE_MOD,POWER_STAT_KEY,buf,256);
    if(OPRT_OK != op_ret) {
        PR_DEBUG("msf_get ch state err:%d",op_ret);
        Free(buf);
        return op_ret;
    }
    PR_DEBUG("read ch state:%s",buf);
    root = cJSON_Parse(buf);
    if(NULL == root) {
        PR_NOTICE("read save cjson parse");
        goto JSON_PARSE_ERR;
    }
    UCHAR i;
    CHAR dpid_str[8];
    for(i = 0;i < g_hw_table.channel_num;i++){
        if(g_hw_table.channels[i].init_ch_stat == INIT_CH_MEM){
            sprintf(dpid_str, "%d", i);
            json = cJSON_GetObjectItem(root,dpid_str);
            if(NULL == json) {
                PR_DEBUG("cjson %d ch stat not found", i);
                ctrl_switch_state(i, CTRL_SW_CLOSE);
            }else{
                if(json->type == cJSON_True){
                    ctrl_switch_state(i, CTRL_SW_OPEN);
                }else{
                    ctrl_switch_state(i, CTRL_SW_CLOSE);
                }
            }
        }
    }

    cJSON_Delete(root);
    Free(buf);
    return OPRT_OK;

JSON_PARSE_ERR:
    cJSON_Delete(root);
    Free(buf);
    return OPRT_COM_ERROR;
}

STATIC OPERATE_RET upload_switch_countdown(IN UCHAR channel_num)
{
    if(g_hw_table.channels[channel_num].cd_dpid < 0){
        return OPRT_OK;
    }
    GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
    if(STAT_UNPROVISION == wf_stat || \
       (get_gw_status() != STAT_WORK)) {
        return OPRT_DP_REPORT_CLOUD_ERR;
    }
    cJSON *root = NULL;
    root = cJSON_CreateObject();
    if(NULL == root) {
        return OPRT_CR_CJSON_ERR;
    }
    CHAR dpid_str[8];
    sprintf(dpid_str, "%d", g_hw_table.channels[channel_num].cd_dpid);
    cJSON_AddNumberToObject(root,dpid_str,g_hw_table.channels[channel_num].cd_sec);

    CHAR *out;
    out=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("cJSON_PrintUnformatted err:");
        return OPRT_MALLOC_FAILED;
    }
    
    PR_DEBUG("out:%s",out);
    OPERATE_RET ret = tuya_obj_dp_report(out);
    Free(out);
    return ret;
}

/**
 * @brief 
 * 
 * @param channel_num 
 * @return OPERATE_RET 
 * @TODO 拆开倒计时清除逻辑和上报逻辑
 */
STATIC OPERATE_RET upload_switch_state(IN UCHAR channel_num)
{
    GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
    if(STAT_UNPROVISION == wf_stat || \
       (get_gw_status() != STAT_WORK)) {
        return OPRT_DP_REPORT_CLOUD_ERR;
    }

    cJSON *root = NULL;
    root = cJSON_CreateObject();
    if(NULL == root) {
        return OPRT_CR_CJSON_ERR;
    }
    CHAR dpid_str[8];
    sprintf(dpid_str, "%d", g_hw_table.channels[channel_num].dpid);
    cJSON_AddBoolToObject(root,dpid_str,g_hw_table.channels[channel_num].stat);

    if(g_hw_table.channels[channel_num].cd_sec){
        g_hw_table.channels[channel_num].cd_sec = 0;
        sprintf(dpid_str, "%d", g_hw_table.channels[channel_num].cd_dpid);
        cJSON_AddNumberToObject(root,dpid_str,g_hw_table.channels[channel_num].cd_sec);
    }
    CHAR *out;
    out=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("cJSON_PrintUnformatted err:");
        return OPRT_MALLOC_FAILED;
    }
    
    PR_DEBUG("out:%s",out);
    OPERATE_RET ret = tuya_obj_dp_report(out);
    Free(out);
    return ret;
}

//调用此函数会清零所有倒计时，并上报所有开关状态+倒计时。
OPERATE_RET upload_all_switch_state(VOID)
{
    GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
    if(STAT_UNPROVISION == wf_stat || \
       STAT_STA_UNCONN == wf_stat || \
       (get_gw_status() != STAT_WORK)) {
        return OPRT_DP_REPORT_CLOUD_ERR;
    }

    cJSON *root = NULL;
    root = cJSON_CreateObject();
    if(NULL == root) {
        return OPRT_CR_CJSON_ERR;
    }
    UCHAR i;
    CHAR dpid_str[8];
    for(i = 0;i < g_hw_table.channel_num;i++){
        sprintf(dpid_str, "%d", g_hw_table.channels[i].dpid);
        cJSON_AddBoolToObject(root,dpid_str,g_hw_table.channels[i].stat);
        if(g_hw_table.channels[i].cd_sec){
            PR_DEBUG("!!!ch[%d] countdown sec:%d",i,g_hw_table.channels[i].cd_sec);
            g_hw_table.channels[i].cd_sec = 0;
        }
        sprintf(dpid_str, "%d", g_hw_table.channels[i].cd_dpid);
        cJSON_AddNumberToObject(root,dpid_str,g_hw_table.channels[i].cd_sec);
    }

    CHAR *out;
    out=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("cJSON_PrintUnformatted err:");
        return OPRT_MALLOC_FAILED;
    }
    
    PR_DEBUG("upload:%s",out);
    OPERATE_RET ret = tuya_obj_dp_report(out);
    Free(out);
    return ret;
}

//开关wifi灯定时器
STATIC VOID switch_wfl_timer_cb(UINT timerID,PVOID pTimerArg)
{
    STATIC UINT last_wf_stat = 0xffffffff;
    GW_WIFI_STAT_E wf_stat = tuya_get_wf_status();
    if(last_wf_stat != wf_stat ) {
        PR_DEBUG("wf_stat:%d",wf_stat);
        switch(wf_stat) {
            case STAT_UNPROVISION: 
                set_wfl_state(WFL_FLASH_FAST);
            break;
            case STAT_AP_STA_UNCONN:
            case STAT_AP_STA_CONN: {
                set_wfl_state(WFL_FLASH_SLOW);
            }
            break;
            case STAT_STA_UNCONN:
                set_wfl_state(g_hw_table.wf_led.wfl_ucs);
            break;
            case STAT_LOW_POWER:
                set_wfl_state(g_hw_table.wf_led.wfl_ucs);
            break;
            case STAT_STA_CONN: 
                set_wfl_state(g_hw_table.wf_led.wfl_cs);
            break;
            default:
            break;
        }
        last_wf_stat = wf_stat;
    }
}
//进入产测指示灯定时器
STATIC VOID enter_pt_timer_cb(UINT timerID,PVOID pTimerArg)
{
    STATIC BOOL chg_flag = FALSE;
    if(chg_flag){
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
        OL_FLASH_LOW, 100);
    }else{
        set_hw_wifi_led(TRUE);
    }
    chg_flag = !chg_flag;
}
//结束产测指示灯定时器
STATIC VOID end_pt_timer_cb(UINT timerID,PVOID pTimerArg)
{
    set_wfl_state(WFL_OFF);
}

STATIC VOID pt_total_bt_timer_cb(UINT timerID,PVOID pTimerArg)
{
    ctrl_switch_state(pt_curr_ch, pt_ch_deal);
    pt_curr_ch++;
    if(pt_curr_ch >= g_hw_table.channel_num){
        sys_stop_timer(pt_total_bt_timer);
    }
}
//wifi灯直接指示继电器
STATIC VOID wfl_direct_relay(VOID)
{
    if(wf_dir_rl_en){
        BOOL if_on = judge_any_sw(TRUE);
        set_hw_wifi_led(if_on);
    }
}
//设定WIFI灯状态
VOID set_wfl_state(IN WFL_STAT wfl_stat)
{
    if(g_hw_table.wf_led.wfl_io.type == \
    IO_DRIVE_LEVEL_NOT_EXIST){
        return;
    }
    if(wfl_stat == WFL_DIR_RL){
        wf_dir_rl_en = TRUE;
    }else{
        wf_dir_rl_en = FALSE;
    }
    if(wfl_stat != WFL_ENTER_PT){
        if(IsThisSysTimerRun(enter_pt_timer)){
            sys_stop_timer(enter_pt_timer);
        }
    }
    PR_DEBUG("wfl stat:%d", wfl_stat);
    switch(wfl_stat){
    case WFL_ON:
        set_hw_wifi_led(TRUE);
        break;
    case WFL_OFF:
        set_hw_wifi_led(FALSE);
        break;
    case WFL_DIR_RL:
        wfl_direct_relay();
        break;
    case WFL_FLASH_VERY_FAST:
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
        OL_FLASH_LOW, 100);
        break;
    case WFL_FLASH_FAST:
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
        OL_FLASH_LOW, 250);
        break;
    case WFL_FLASH_SLOW:
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
        OL_FLASH_LOW, 1500);
        break;
    case WFL_ENTER_PT:
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
        OL_FLASH_LOW, 100);
        sys_start_timer(enter_pt_timer,500,TIMER_CYCLE);
        break;
    case WFL_END_PT:
        tuya_set_led_type(g_hw_table.wf_led.wfl_handle,\
        OL_FLASH_LOW, 250);
        sys_start_timer(end_pt_timer,2500,TIMER_ONCE);
        break;
    }
}
//过流保护事件
VOID over_protect(VOID)
{
    PR_DEBUG("over protect!!!");
    BOOL if_on = judge_any_sw(TRUE);
    if(if_on){
        PR_DEBUG("There exist at least one switch ON");
        total_channel_event_deal(FALSE);
        report_over_curr();
    }
}

VOID set_pt_key_en(IN BOOL if_en)
{
    pt_key_en = if_en;
}

//保存产测结束标志位
OPERATE_RET save_pt_end_flag(IN INT state)
{
    OPERATE_RET op_ret;
    cJSON *root = NULL;
    UCHAR *buf = NULL;
    root = cJSON_CreateObject();
    if(NULL == root) {
        PR_ERR("cJSON_CreateObject error");
        return OPRT_CJSON_GET_ERR;
    }
    cJSON_AddNumberToObject(root, "pt_end", state);
    buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("buf is NULL");
        cJSON_Delete(root);
        return OPRT_COM_ERROR;
    }    
    cJSON_Delete(root);
    PR_DEBUG("set buf:%s",buf);
    
    op_ret = tuya_psm_set_single(DEVICE_MOD,PT_END_KEY,buf);
    if(OPRT_OK != op_ret) {
        PR_DEBUG("tuya_psm_set_single err:%d",op_ret);
        Free(buf);
        return op_ret;
    }
    
    Free(buf);
    return OPRT_OK;    
}
//获取产测结束标志位
OPERATE_RET get_pt_end_flag(OUT INT *state)
{
    OPERATE_RET op_ret;
    cJSON *root = NULL, *json = NULL;
    UCHAR *buf;

    buf = (UCHAR *)Malloc(16);
    if(NULL == buf) {
        PR_ERR("malloc error");
        return OPRT_MALLOC_FAILED;
    }

    op_ret = tuya_psm_get_single(DEVICE_MOD,PT_END_KEY,buf,16);
    if(OPRT_OK != op_ret) {
        PR_DEBUG("msf_get_pt_end_flag err:%d",op_ret);
        Free(buf);
        return op_ret;
    }
    PR_DEBUG("get buf:%s",buf);
    root = cJSON_Parse(buf);
    if(NULL == root) {
        PR_ERR("cjson parse");
        goto JSON_PARSE_ERR;
    }

    json = cJSON_GetObjectItem(root,"pt_end");
    if(NULL == json) {
        PR_ERR("cjson get ");
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

VOID reset_power_stat(VOID)
{
    CHAR buff[4] = {0};
    OPERATE_RET op_ret = tuya_psm_set_single(DEVICE_MOD,POWER_STAT_KEY,buff);
    PR_NOTICE("tuya_psm_set_single op_ret:%d",op_ret);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_psm_set_single clear power_stat error");
        return;
    }
    PR_DEBUG("reset power stat default");
}


