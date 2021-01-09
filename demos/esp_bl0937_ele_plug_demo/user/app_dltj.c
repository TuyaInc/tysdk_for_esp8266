/**
 * @File: app_dltj.c 
 * @Author: caojq 
 * @Last Modified time: 2020-03-05 
 * @Description: 电量统计上层应用。
 * 支持dp查询，支持tuya_common_user编译。
 * 
 */
#define _APP_DLTJ_GLOBAL
#include "app_dltj.h"
#include "cJSON.h"
#include "tuya_ws_db.h"
#include "system/uni_time_queue.h"
#include "system/uni_thread.h"
#include "system/uni_semaphore.h"
#include "mem_pool.h"
#include "com_def.h"
#include "smart_wf_frame.h"
#include "device.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#if _APP_DLTJ_DEBUG
#define APP_DLTJ_DEBUG  PR_DEBUG
#else
#define APP_DLTJ_DEBUG(...)
#endif

#define DEVICE_MOD "device_mod"
#define DEVICE_PART "device_part"
#define TEM_ELE_SAVE_KEY "tem_ele_save_key"
#define ELE_SAVE_KEY "ele_save_key"
#define TEM_ELE_SAVE_KEY "tem_ele_save_key"

#define TIME_POSIX_2016 1451577600

#define ADD_ELE_THRE 100//临时电量累加处理阈值
#define ADD_TIME_MIN_THRE 60//电量处理最小时间间隔
#define ADD_TIME_MAX_THRE 1800//电量处理最大时间间隔，单位S
#define PVI_REPORT_BIG_THRE  30 //电流电压功率数据上报大间隔
#define PVI_REPORT_SMALL_THRE 5 //电流电压功率数据上报小间隔
#define PVI_REPORT_MAX_THRE 3600 //电流电压功率数据上报最长间隔
#define PVI_REPORT_BIG_THRE_DP_QUERY  1800 //存在dp查询时的电流电压功率数据上报大间隔
#define PVI_REPORT_SMALL_THRE_DP_QUERY 600 //存在dp查询时的电流电压功率数据上报小间隔

#define REPT_THRE_CURR 10//电流上报变化阈值，单位%
#define REPT_THRE_VOL 2//电压上报变化阈值，单位%
#define REPT_THRE_PWR 30//功率上报变化阈值，单位%
#define ELE_SLEEP_TIME 1//电量线程休眠间隔

#define _IS_OVER_PROTECT 1//是否存在过流保护功能
#define LIMIT_CURRENT 17000//过流保护限值，单位1mA
#define _IF_REPT_COE   TRUE     //是否上报电量校准系数
#define _DLTJ_REPT_TEST 0      //电量统计数据上报调试用，打开后自动上报数据
#define SMALLEST_POWER 30   //功率小于此值时，屏蔽电流和功率,单位0.1W
#define CPREEPING_POWER_THRE 1 /* 潜动电量阈值，有些电量芯片如bl0937可能存在空载30分钟测到0.001度电的情况，此时将这部分电量舍弃 */
#define MAX_POWER_ABS 50
/***********************************************************
*************************variable define********************
***********************************************************/

typedef struct 
{
    UINT unix_time;
    UINT ele_energy;
}LOC_DATA;

typedef struct 
{
    UINT add_ele;//下层电量统计驱动中累加的电量
    UINT cur_posix;//当前时间戳
    UINT loc_data_cnt;//本地电量数据组的元素个数
    UINT cur_current;
    UINT cur_vol;
    UINT cur_power;
    UINT last_current;
    UINT last_vol;
    UINT last_power;
    UINT tem_ele_val;//存放已经累加的但还未上报或存储到local_data中的电量
    THREAD report_thread;
    LOC_DATA ele_sav_data[10];//本地电量数据组
}POWER_CNT;
STATIC POWER_CNT power_cnt;

typedef enum{
    ELE_NOT_ACTIVE,//上电后未配网激活
    ELE_NORMAL,//已配网成功且当前mqtt连接成功
    ELE_UNCONNECT,//已配网成功当当前mqtt连接失败
    ELE_SYS_OTA//系统正在进行在线升级
}ELE_THREAD_STATE;

typedef struct{
    TIMER_ID                    app_dltj_timer_id;
    THREAD                      app_dltj_thread;
    SEM_HANDLE                  app_dltj_sem;
}AppDltj_S,*pAppDltj_S;
AppDltj_S mAppDltj;

typedef struct {
    BOOL is_dp_query;
    UINT pvi_small_thre;
    UINT pvi_big_thre;
} handle_time_t;

STATIC handle_time_t handle_time = { 
    .is_dp_query = FALSE,
    .pvi_big_thre = PVI_REPORT_BIG_THRE,
    .pvi_small_thre = PVI_REPORT_SMALL_THRE,
};

STATIC ELE_THREAD_STATE ele_state;
STATIC UINT ele_handle_time = 0;//距离上一次电量上报之后的时间，单位比1S略大一些
STATIC UINT pvi_handle_time = 0;
STATIC UINT pvi_report_min_time = PVI_REPORT_BIG_THRE;//当前上报间隔，可调

#define DLTJ_INIT_CONFIG_DEFAULT()              \
    {                                           \
        .if_have = true,                        \
        .over_curr = LIMIT_CURRENT,             \
        .dp_info = {                            \
            .edpid = DP_ELE,                    \
            .idpid = DP_CURRENT,                \
            .pdpid = DP_POWER,                  \
            .vdpid = DP_VOLTAGE,                \
            .fault_dpid = DP_FAULT,             \
        },                                      \
        .drv_cfg = {                            \
            .epin = 4,                          \
            .ivpin = 5,                         \
            .ivcpin.pin = 12,                   \
            .ivcpin.type = IO_DRIVE_LEVEL_HIGH, \
            .v_ref = V_COE_BL,                  \
            .i_ref = I_COE_BL,                  \
            .p_ref = P_COE_BL,                  \
            .e_ref = E_COE_BL,                  \
            .v_def = V_DEF_220,                 \
            .i_def = I_DEF_220,                 \
            .p_def = P_DEF_220,                 \
            .dp_pt_rslt = DP_PTRSLT,            \
            .dp_vcoe = DP_VREF,                 \
            .dp_icoe = DP_IREF,                 \
            .dp_pcoe = DP_PREF,                 \
            .dp_ecoe = DP_EREF,                 \
        },                                      \
    }

APP_DLTJ_CFG g_dltj = {
    .if_have = true,
    .over_curr = LIMIT_CURRENT,
    .dp_info = {
        .edpid = DP_ELE,
        .idpid = DP_CURRENT,
        .pdpid = DP_POWER,
        .vdpid = DP_VOLTAGE,
        .fault_dpid = DP_FAULT,
    },
    .drv_cfg = {
        .epin = 4,
        .ivpin = 5,
        .ivcpin.pin = 12,
        .ivcpin.type = IO_DRIVE_LEVEL_HIGH,
        .v_ref = V_COE_BL,
        .i_ref = I_COE_BL,
        .p_ref = P_COE_BL,
        .e_ref = E_COE_BL,
        .v_def = V_DEF_220,
        .i_def = I_DEF_220,
        .p_def = P_DEF_220,
        .dp_pt_rslt = DP_PTRSLT,
        .dp_vcoe = DP_VREF,
        .dp_icoe = DP_IREF,
        .dp_pcoe = DP_PREF,
        .dp_ecoe = DP_EREF,
    },
};

/***********************************************************
*************************function define********************
***********************************************************/
STATIC OPERATE_RET report_pvi_data(VOID);
STATIC OPERATE_RET report_fault_warn(IN UCHAR dpid_fault,IN INT fault_word);
STATIC VOID addto_local_data(IN UINT time,IN UINT ele);
STATIC VOID report_local_data(VOID);
STATIC VOID add_ele_handle(VOID);
STATIC BOOL value_range_judge(IN UINT JudgedValue,IN UINT TargetValue,IN UINT range);
STATIC BOOL same_day_judge(IN UINT u_time1,IN UINT u_time2);
STATIC BOOL if_pvi_need_report(VOID);
VOID over_protect(VOID);

/*********************************************************************************
 * FUNCTION:       get_time_posix
 * DESCRIPTION:    获取当前时间戳
 * INPUT:          none
 * OUTPUT:         curPosix:当前时间戳
 * RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
 * OTHERS:         单位是毫秒
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC OPERATE_RET get_time_posix(OUT UINT *curPosix)
{
    UINT tmp_posix;
    tmp_posix = wmtime_time_get_posix();
    if(tmp_posix < TIME_POSIX_2016) {
        APP_DLTJ_DEBUG("get curPosix err!");
        return OPRT_INVALID_PARM;
    }
    *curPosix = tmp_posix;
    return OPRT_OK;
}

/*********************************************************************************
 * FUNCTION:       get_tem_ele_val
 * DESCRIPTION:    获取falsh中的临时电量值
 * INPUT:          none
 * OUTPUT:         val:临时电量值
 * RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
 * OTHERS:         flash中未配网电量存储的格式:
     {“dps”:{“dp_ele”:ele_vlaue},”dpsTime”:{“dp_ele”:s_timer_posix},”ts”:ms_time_posix}
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC OPERATE_RET get_tem_ele_val(OUT INT *val)
{
    OPERATE_RET op_ret;
    cJSON *root = NULL, *json = NULL;
    UCHAR *buf;

    buf = (UCHAR *)Malloc(256);
    if(NULL == buf) {
        PR_ERR("malloc error");
        *val = 0;
        return OPRT_MALLOC_FAILED;
    }

    op_ret = tuya_psm_get_single(DEVICE_MOD,TEM_ELE_SAVE_KEY,buf,256);
    if(OPRT_OK != op_ret) {
        APP_DLTJ_DEBUG("tuya_psm_get_single err:%d",op_ret);
        Free(buf);
        *val = 0;
        return op_ret;
    }
    APP_DLTJ_DEBUG("Get tem ele val:%s",buf);
    root = cJSON_Parse(buf);
    if(NULL == root) {
        PR_ERR("cjson parse");
        goto JSON_PARSE_ERR;
    }

    json = cJSON_GetObjectItem(root,"tem_ele");
    if(NULL == json) {
        PR_ERR("cjson get ");
        goto JSON_PARSE_ERR;
    }

    *val = json->valueint;
    cJSON_Delete(root);
    Free(buf);
    return OPRT_OK;

JSON_PARSE_ERR:
    cJSON_Delete(root);
    Free(buf);
    *val = 0;
    return OPRT_COM_ERROR;
}

/*********************************************************************************
 * FUNCTION:       set_tem_ele_val
 * DESCRIPTION:    在falsh中存储临时电量值
 * INPUT:          none
 * OUTPUT:         val:临时电量值
 * RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
 * OTHERS:         flash中未配网电量存储的格式:
     {“dps”:{“dp_ele”:ele_vlaue},”dpsTime”:{“dp_ele”:s_timer_posix},”ts”:ms_time_posix}
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC OPERATE_RET set_tem_ele_val(IN INT val)
{
    OPERATE_RET op_ret;
    cJSON *root = NULL;
    UCHAR *buf = NULL;

    root = cJSON_CreateObject();
    if(NULL == root) {
        PR_ERR("cJSON_CreateObject error");
        return OPRT_CJSON_GET_ERR;
    }
    
    cJSON_AddNumberToObject(root, "tem_ele", val);
    buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("buf is NULL");
        cJSON_Delete(root);
        return OPRT_COM_ERROR;
    }    
    cJSON_Delete(root);
    APP_DLTJ_DEBUG("to set tem ele val:%s",buf);
    op_ret = tuya_psm_set_single(DEVICE_MOD,TEM_ELE_SAVE_KEY,buf);
    if(OPRT_OK != op_ret) {
    APP_DLTJ_DEBUG("tuya_psm_set_single err:%d",op_ret);
        Free(buf);
        return op_ret;
    }

    Free(buf);
    return OPRT_OK;    
}

/*********************************************************************************
 * FUNCTION:       get_ele_data
 * DESCRIPTION:    获取falsh中的断网电量值
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
 * OTHERS:         flash中电量存储的格式:
     {“ele_save_key”:[{time:time_posix1,ele:ele_value1},{time:time_posix2,ele:ele_value2}...{time:time_posix10,ele:ele_value10}]}
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC OPERATE_RET get_ele_data(VOID)
{
    OPERATE_RET op_ret;
    cJSON *arrayItem = NULL, *array = NULL, *json = NULL;
    UCHAR *buf=NULL;
    INT i;

    buf = (UCHAR *)Malloc(1024);
    if(NULL == buf) {
        PR_ERR("malloc error");
        power_cnt.loc_data_cnt = 0;
        return OPRT_MALLOC_FAILED;  
    }
    op_ret = tuya_psm_get_single(DEVICE_MOD,ELE_SAVE_KEY,buf,1024);
    if(OPRT_OK != op_ret) {
        APP_DLTJ_DEBUG("msf_get_ele_data err:%d",op_ret);
        power_cnt.loc_data_cnt = 0;
        Free(buf);
        return op_ret;
    }
    APP_DLTJ_DEBUG("get ele data buf:%s",buf);
    array = cJSON_Parse(buf);
    if(NULL == array) {
        PR_ERR("cjson parse err");
        power_cnt.loc_data_cnt = 0;
        Free(buf);
        return OPRT_COM_ERROR;
    }

    Free(buf);
    power_cnt.loc_data_cnt = cJSON_GetArraySize(array);

    for(i = 0;i < power_cnt.loc_data_cnt; i++){
        arrayItem = cJSON_GetArrayItem(array, i);
        if(arrayItem){
            json = cJSON_GetObjectItem(arrayItem,"time");
            if(NULL == json){
                cJSON_Delete(array);
                return OPRT_COM_ERROR;
            }
            power_cnt.ele_sav_data[i].unix_time = json->valueint;
            json = cJSON_GetObjectItem(arrayItem,"ele");
            if(NULL == json) {
                cJSON_Delete(array);
                return OPRT_COM_ERROR;
            }
            power_cnt.ele_sav_data[i].ele_energy = json->valueint;
            APP_DLTJ_DEBUG("i:%d,time:%d,ele:%d",i,power_cnt.ele_sav_data[i].unix_time,\
            power_cnt.ele_sav_data[i].ele_energy);
        }
    }
    cJSON_Delete(array);
    return OPRT_OK;
}

/*********************************************************************************
 * FUNCTION:       set_ele_data
 * DESCRIPTION:    在falsh中存储的断网电量值
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
 * OTHERS:         flash中电量存储的格式:
     {“ele_save_key”:[{time:time_posix1,ele:ele_value1},{time:time_posix2,ele:ele_value2}...{time:time_posix10,ele:ele_value10}]}
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC OPERATE_RET set_ele_data(VOID)
{
    OPERATE_RET op_ret;
    cJSON *arrayItem = NULL, *array = NULL;
    UCHAR *buf = NULL;
    INT i;

    array = cJSON_CreateArray();
    if(NULL == array) {
        PR_ERR("cJSON_CreateArray err");
        return OPRT_CJSON_GET_ERR;    
    }

    for(i = 0; i < power_cnt.loc_data_cnt; i++){
        arrayItem = cJSON_CreateObject();
        if(NULL == arrayItem) {
            cJSON_Delete(array);
            PR_ERR("cJSON_CreateObject error");
            return OPRT_CJSON_GET_ERR;
        }
        cJSON_AddNumberToObject(arrayItem, "time", power_cnt.ele_sav_data[i].unix_time);
        cJSON_AddNumberToObject(arrayItem, "ele", power_cnt.ele_sav_data[i].ele_energy);
        cJSON_AddItemToArray(array,arrayItem);
    }
    buf = cJSON_PrintUnformatted(array);
    if(NULL == buf) {
        PR_ERR("buf is NULL");
        cJSON_Delete(array);
        return OPRT_COM_ERROR;
    }    
    APP_DLTJ_DEBUG("set ele data buf:%s",buf);

    op_ret = tuya_psm_set_single(DEVICE_MOD,ELE_SAVE_KEY,buf);
    if(OPRT_OK != op_ret) {
        APP_DLTJ_DEBUG("msf_set_ele data err:%d",op_ret);
        cJSON_Delete(array);
        Free(buf);
        return op_ret;
    }

    cJSON_Delete(array);
    Free(buf);
    return OPRT_OK;    
}

/*********************************************************************************
 * FUNCTION:       dltj_driver_init
 * DESCRIPTION:    bl0937芯片硬件模式引脚初始化/电量脉冲计数初始化和硬件采样配置
 * INPUT:          mode芯片计数模式：正常模式和产测模式
 * OUTPUT:         none
 * RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
 * OTHERS:         bl0937计量芯片配置
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC OPERATE_RET dltj_driver_init(IN APP_DLTJ_MODE mode)
{
    OPERATE_RET op_ret = OPRT_OK;
    bl0937_init(&(g_dltj.drv_cfg));
    if(mode == APP_DLTJ_NORMAL){
        op_ret = ele_cnt_init(D_NORMAL_MODE);
    }else{
        op_ret = ele_cnt_init(D_CAL_START_MODE);
    }
    return op_ret;
}

/*********************************************************************************
 * FUNCTION:       ele_par_filter
 * DESCRIPTION:    过滤电流和功率事件
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         当前功率小于一定值，或者所有通道关闭时过滤清零电流功率
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC VOID ele_par_filter(VOID)
{
    if( power_cnt.cur_power <= SMALLEST_POWER\
        || !judge_any_sw(true)) {
        power_cnt.cur_current = 0;
        power_cnt.cur_power = 0;
    }
}

/*********************************************************************************
 * FUNCTION:       pvi_data_handle
 * DESCRIPTION:    电量参数事件
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         获取电量实时参数和对应过载事件;
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC VOID pvi_data_handle(VOID)
{
#if _DLTJ_REPT_TEST
    static UINT p,v,i;
    p+=10;
    v++;
    i += 10;
    if(i > 30000){
        i = 0;
        p = 0;
        v = 0;
    }
    power_cnt.cur_current = i;
    power_cnt.cur_power = p;
    power_cnt.cur_vol = v;
#else
    get_ele_par(&power_cnt.cur_power,&power_cnt.cur_vol,&power_cnt.cur_current);
#endif
    ele_par_filter();
    APP_DLTJ_DEBUG("cur:%d power:%d vol:%d",power_cnt.cur_current,\
    power_cnt.cur_power,power_cnt.cur_vol);
    if(g_dltj.over_curr){
        if(power_cnt.cur_current >= g_dltj.over_curr){
            over_protect(); //过流事件
        }
    }
}

/*********************************************************************************
* FUNCTION:       update_ele_data
* DESCRIPTION:    上报增加电量值
* INPUT:          time：时间戳；ele_value：增加电量值
* OUTPUT:         none
* RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
* OTHERS:         电量上报接口函数
* HISTORY:        2020-03-04
*******************************************************************************/
STATIC OPERATE_RET update_ele_data(IN UINT time,IN UINT ele_value)
{
    OPERATE_RET op_ret;
    STATS_DATA_S *stat_data;
    INT size = 0;
    size = sizeof(STATS_DATA_S) + 4;
    APP_DLTJ_DEBUG("size:%d",size);
    stat_data = (STATS_DATA_S *)Malloc(size);
    memset(stat_data, 0, size);

    stat_data->time = time;
    stat_data->type = PROP_VALUE;
    stat_data->len  = 4;
    stat_data->data.value = ele_value;
    APP_DLTJ_DEBUG("Upload tem ele time:%d, value:%d!!!!!",time,ele_value);
    op_ret = mq_client_publish_obj_data(g_dltj.dp_info.edpid, stat_data);
    Free(stat_data);
    if(OPRT_OK != op_ret){
        PR_ERR("Ele report fail %d!!!", op_ret);
        return OPRT_COM_ERROR;
    }
    
    APP_DLTJ_DEBUG("Ele report success...");
    return OPRT_OK;
}

/*********************************************************************************
* FUNCTION:       report_local_data
* DESCRIPTION:    上报本地存储电量值
* INPUT:          none
* OUTPUT:         none
* RETURN:         none
* OTHERS:         上报电量值和对应时间戳
* HISTORY:        2020-03-04
*******************************************************************************/
STATIC VOID report_local_data(VOID)
{
    INT i,j;
    UINT val_size = power_cnt.loc_data_cnt;
    INT cha_flag = 0;
    if(0 == val_size)
        return ;
    for(i = 0;i < val_size; i++){
        if(update_ele_data(power_cnt.ele_sav_data[0].unix_time,power_cnt.ele_sav_data[0].ele_energy) != OPRT_OK){
            break;
        }
        APP_DLTJ_DEBUG("Upload cnt:%d,curr_i:%d,unix_time:%d,ele_energy:%d success...",power_cnt.loc_data_cnt,i,\
                power_cnt.ele_sav_data[0].unix_time,power_cnt.ele_sav_data[0].ele_energy);
        cha_flag = 1;
        for(j=0; j<val_size-1; j++){
            power_cnt.ele_sav_data[j].unix_time = power_cnt.ele_sav_data[j+1].unix_time;
            power_cnt.ele_sav_data[j].ele_energy = power_cnt.ele_sav_data[j+1].ele_energy;
        }
        power_cnt.loc_data_cnt -= 1;
    }

    if(1 == cha_flag){
        if(OPRT_OK != set_ele_data()){
            PR_ERR("set ele to flash err ...");
        }
    }
}

/*********************************************************************************
* FUNCTION:       same_day_judge
* DESCRIPTION:    时间戳判断
* INPUT:          u_time1：时间戳1；u_time2：时间戳2
* OUTPUT:         none
* RETURN:         BOOL：FLASE不是同一天，TRUE同一天
* OTHERS:         判断两个时间戳是否为同一天
* HISTORY:        2020-03-04
*******************************************************************************/
STATIC BOOL same_day_judge(UINT u_time1, UINT u_time2)
{
    return (u_time1+28800)/86400 == (u_time2+28800)/86400 ? TRUE: FALSE;
}

/*********************************************************************************
* FUNCTION:       addto_local_data
* DESCRIPTION:    本地电量处理
* INPUT:          time：时间戳，ele:电量值
* OUTPUT:         none
* RETURN:         none
* OTHERS:         最多存储10天的电量值，存储格式为：{"ele_time":time,"ele_value":xxx}
* HISTORY:        2020-03-04
*******************************************************************************/
STATIC VOID addto_local_data(UINT time, UINT ele)
{
    INT i;
    APP_DLTJ_DEBUG("try to add to local data time:%d,ele:%d",time,ele);

    if(power_cnt.loc_data_cnt != 0){
        if(time == 0){
            power_cnt.ele_sav_data[power_cnt.loc_data_cnt-1].ele_energy += ele;
            return;
        }
        //电量按天存在flash中
        if(same_day_judge(power_cnt.ele_sav_data[power_cnt.loc_data_cnt-1].unix_time,time)){
            power_cnt.ele_sav_data[power_cnt.loc_data_cnt-1].unix_time = time;
            power_cnt.ele_sav_data[power_cnt.loc_data_cnt-1].ele_energy += ele;
            APP_DLTJ_DEBUG("Same day ...%d %d",power_cnt.ele_sav_data[power_cnt.loc_data_cnt-1].ele_energy,ele);
        }else{
            if(power_cnt.loc_data_cnt == 10){
                //for(i=0; i<9; i++){
                //	power_cnt.ele_sav_data[i].unix_time = power_cnt.ele_sav_data[i+1].unix_time;
                //	power_cnt.ele_sav_data[i].ele_energy = power_cnt.ele_sav_data[i+1].ele_energy;
                //}
                //power_cnt.ele_sav_data[power_cnt.loc_data_cnt-1].unix_time = time;
                power_cnt.ele_sav_data[power_cnt.loc_data_cnt-1].ele_energy += ele;//达到最大存储数目之后，将后续电量存在最后一天
            }else{
                power_cnt.ele_sav_data[power_cnt.loc_data_cnt].unix_time = time;
                power_cnt.ele_sav_data[power_cnt.loc_data_cnt].ele_energy = ele;
                power_cnt.loc_data_cnt++;
            }

        }
    }else{
        power_cnt.ele_sav_data[0].unix_time = time;
        power_cnt.ele_sav_data[0].ele_energy = ele;
        power_cnt.loc_data_cnt++;
    }
    APP_DLTJ_DEBUG("set loc_data_cnt : %d",power_cnt.loc_data_cnt);
    if(OPRT_OK != set_ele_data()){
        APP_DLTJ_DEBUG("set ele to flash err ...");
    }
}

/*联网激活前，所有新增加的电量存储在power_cnt.tem_ele_val中，并存储在TEM_ELE_SAVE_KEY区，
 激活之后，将power_cnt.tem_ele_val上报并清空TEM_ELE_SAVE_KEY，此后对于新增加的电量，先尝试
 上报，上报失败则存在ele_sav_data数组中，联网状态下电量线程每次循环会尝试上报ele_sav_data数组*/
 /*********************************************************************************
 * FUNCTION:       add_ele_handle
 * DESCRIPTION:    增加电量值参数事件
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         增量电量值事件处理;
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC VOID add_ele_handle(VOID)
{
    get_ele(&power_cnt.add_ele);
    get_time_posix(&power_cnt.cur_posix);
#if _DLTJ_REPT_TEST
    power_cnt.add_ele = 10;
#endif
    power_cnt.tem_ele_val += power_cnt.add_ele;
    APP_DLTJ_DEBUG("add_ele = %d,tem_ele_val = %d",power_cnt.add_ele,power_cnt.tem_ele_val);
    power_cnt.add_ele = 0;
    //电量处理事件触发
    if((power_cnt.tem_ele_val >= ADD_ELE_THRE && ele_handle_time >= ADD_TIME_MIN_THRE )\
    || ele_handle_time >= ADD_TIME_MAX_THRE){
        ele_handle_time = 0;
        if (power_cnt.tem_ele_val > CPREEPING_POWER_THRE) {
            if(ELE_NOT_ACTIVE == ele_state){
                set_tem_ele_val(power_cnt.tem_ele_val);
                APP_DLTJ_DEBUG("set tem ele val :%d success...",power_cnt.tem_ele_val);
            }else if(ELE_NORMAL == ele_state){
                if(OPRT_OK != update_ele_data(power_cnt.cur_posix, power_cnt.tem_ele_val)){
                    addto_local_data(power_cnt.cur_posix, power_cnt.tem_ele_val);
                }
                power_cnt.tem_ele_val = 0;
            }else if(ELE_UNCONNECT == ele_state || ELE_SYS_OTA == ele_state){
                addto_local_data(power_cnt.cur_posix, power_cnt.tem_ele_val);
                power_cnt.tem_ele_val = 0;
            }
        } else {
            power_cnt.tem_ele_val = 0;
        }
    }
}

/*********************************************************************************
 * FUNCTION:       small_power_report
 * DESCRIPTION:    小功率上报
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         BOOL:FALSE(不上报)，TRUE(上报)
 * OTHERS:         小功率是否上报；
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC BOOL small_power_report(VOID)
{
    if(abs(power_cnt.cur_power - power_cnt.last_power) > MAX_POWER_ABS) {
        return TRUE;
    }
    return FALSE;
}

/*********************************************************************************
 * FUNCTION:       value_range_judge
 * DESCRIPTION:    变化率判断函数
 * INPUT:          JudgedValue：判断值；TargetValue：标准值；range：变化率
 * OUTPUT:         none
 * RETURN:         BOOL:FALSE(不上报)，TRUE(上报)
 * OTHERS:         小功率是否上报；
 * HISTORY:        2020-03-04
 *******************************************************************************/
//判断JudgedValue是否在TargetValue的正负range%范围之内，如果是返回1，如果否返回0
//JudgedValue为被判定的值，TargetValue为被判定值的目标值，range为浮动范围0-100(%).
STATIC BOOL value_range_judge(IN UINT JudgedValue,IN UINT TargetValue,IN UINT range)
{
    if((JudgedValue * 100 >= TargetValue * (100 - range)) && \
        (JudgedValue * 100 <= TargetValue * (100 + range))){
        return TRUE;
    }else{
        return FALSE;
    }
}

/*********************************************************************************
 * FUNCTION:       if_pvi_need_report
 * DESCRIPTION:    电量实时数据是否需要上报
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         BOOL:FALSE(不需要上报)，TRUE(需要上报)
 * OTHERS:         功率/电压变化率判断是否上报实时电量数据
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC BOOL if_pvi_need_report(VOID)
{
    if((!value_range_judge(power_cnt.cur_power,power_cnt.last_power,REPT_THRE_PWR) &&small_power_report())\
        ||!value_range_judge(power_cnt.cur_vol,power_cnt.last_vol,REPT_THRE_VOL)){
        pvi_report_min_time = handle_time.pvi_small_thre;
        return TRUE;
    }else{
        pvi_report_min_time = handle_time.pvi_big_thre;
        return FALSE;
    }
}

/*********************************************************************************
 * FUNCTION:       report_pvi_data
 * DESCRIPTION:    上报电量实时数据
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         OPERATE_RET：返回值结果
 * OTHERS:         电流/电压功率当前值
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC OPERATE_RET report_pvi_data(VOID)
{
    cJSON *root = NULL;
    root = cJSON_CreateObject();
    if(NULL == root) {
        return OPRT_CR_CJSON_ERR;
    }
    CHAR dpid_str[8];
    sprintf(dpid_str, "%d", g_dltj.dp_info.idpid);
    cJSON_AddNumberToObject(root,dpid_str,power_cnt.cur_current);
    sprintf(dpid_str, "%d", g_dltj.dp_info.pdpid);
    cJSON_AddNumberToObject(root,dpid_str,power_cnt.cur_power);
    sprintf(dpid_str, "%d", g_dltj.dp_info.vdpid);
    cJSON_AddNumberToObject(root,dpid_str,power_cnt.cur_vol); 
    CHAR *out;
    out=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("cJSON_PrintUnformatted err:");
        return OPRT_CJSON_GET_ERR;
    }

    APP_DLTJ_DEBUG("try to report:[%s]", out);
    OPERATE_RET op_ret = tuya_obj_dp_report(out);
    Free(out);
    if(OPRT_OK != op_ret) {
        PR_ERR("report ele par err,op_ret:%d",op_ret);
        return OPRT_DP_REPORT_CLOUD_ERR;
    }
    return OPRT_OK;
}

/*********************************************************************************
 * FUNCTION:       save_reported_pvi_data
 * DESCRIPTION:    更新电流实时参数
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         电量统计参数全局变量更新；
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC VOID save_reported_pvi_data(VOID)
{
    power_cnt.last_current = power_cnt.cur_current;
    power_cnt.last_vol = power_cnt.cur_vol;
    power_cnt.last_power = power_cnt.cur_power;
}

/*********************************************************************************
 * FUNCTION:       app_dltj_proc
 * DESCRIPTION:    电量统计数据上报线程
 * INPUT:          pArg：线程参数
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         处理电量参数：电实时参数上报(电流电压功率)及增加电量值存储和上报；
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC VOID app_dltj_proc(PVOID pArg)
{
    ele_state = ELE_NOT_ACTIVE;
    while(1){
        switch(ele_state){
        case ELE_NOT_ACTIVE:
            if(get_gw_mq_conn_stat() && OPRT_OK == get_time_posix(&power_cnt.cur_posix)){
                ele_state = ELE_NORMAL;//首次联网激活，将临时电量上报并清零临时电量内存
                pvi_handle_time = pvi_report_min_time;
                ele_handle_time = ADD_TIME_MAX_THRE;
                set_tem_ele_val(0);
                #if _IF_REPT_COE
                if(OPRT_OK != report_coe_data()){
                    PR_DEBUG("report coe data fail!!!");
                }
                #endif
            }
            break;
        case ELE_NORMAL:
        case ELE_UNCONNECT:
            if(UPGRADING == get_fw_ug_stat()){
                ele_state = ELE_SYS_OTA;
                break;
            }
            if(get_gw_mq_conn_stat() != TRUE){
                ele_state = ELE_UNCONNECT;
            }else{
                ele_state = ELE_NORMAL;
            }
            break;
        case ELE_SYS_OTA://OTA结束成功将重启,失败将继续工作
            if(UG_EXECPTION == get_fw_ug_stat()||UG_FIN == get_fw_ug_stat()){
                ele_state = ELE_NORMAL;
            }
            break;
        }
        pvi_data_handle();
        add_ele_handle();
        if(ele_state == ELE_NORMAL){
            if((pvi_handle_time >= pvi_report_min_time && if_pvi_need_report())\
            || pvi_handle_time >= PVI_REPORT_MAX_THRE){
                pvi_handle_time = 0;
                if(OPRT_OK == report_pvi_data()){
                    save_reported_pvi_data();
                }
            }
            pvi_handle_time += ELE_SLEEP_TIME;
            if(power_cnt.loc_data_cnt != 0){
                report_local_data();
            }
        }
        PR_DEBUG("remain size:%d",system_get_free_heap_size());
        SystemSleep(ELE_SLEEP_TIME *1000);
        ele_handle_time += ELE_SLEEP_TIME;
        APP_DLTJ_DEBUG(" ele state :%d,pvi time:%d, ele time:%d,cur_posix :%d",\
        ele_state,pvi_handle_time,ele_handle_time,power_cnt.cur_posix);
    }
}

/*********************************************************************************
 * FUNCTION:       clear_ele_data
 * DESCRIPTION:    清空flash中的增加电量值
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         清除断网电量值和未配网电量值
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC VOID clear_ele_data(VOID)
{
    INT tem_ele = 0;
    if(OPRT_OK == get_tem_ele_val(&tem_ele) && tem_ele != 0) {
        set_tem_ele_val(0);
    }
    if(OPRT_OK == get_ele_data() && power_cnt.loc_data_cnt != 0) {
        power_cnt.loc_data_cnt = 0;
        set_ele_data();
    }
}

/*********************************************************************************
 * FUNCTION:       app_dltj_init
 * DESCRIPTION:    电量统计初始化
 * INPUT:          mode：电量统计设备模式：正常模式和产测模式
 * OUTPUT:         none
 * RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
 * OTHERS:         bl0937芯片初始化/电量统计参数上报线程
 * HISTORY:        2020-03-04
 *******************************************************************************/
OPERATE_RET app_dltj_init(IN APP_DLTJ_MODE mode)
{
    if(g_dltj.if_have){
        OPERATE_RET op_ret;
        if(APP_DLTJ_NORMAL == mode){
            op_ret = dltj_driver_init(mode);
            if(OPRT_OK != op_ret){
                PR_ERR("mode:%d,dltj driver init err!!!",mode);
                return op_ret;
            }
            if(OPRT_OK != get_ele_data()){
                APP_DLTJ_DEBUG("get ele data err...");
            }
            if(OPRT_OK != get_tem_ele_val(&power_cnt.tem_ele_val)){
                APP_DLTJ_DEBUG("get tem ele data err...");
            }
            op_ret = CreateAndStart(&mAppDltj.app_dltj_thread,app_dltj_proc,\
            NULL,1024+512,TRD_PRIO_2,"app_dltj_task");
            if(op_ret != OPRT_OK) {
                PR_ERR("creat ele thread err...");
                return op_ret;
            }
        }else{
            clear_ele_data();
            op_ret = dltj_driver_init(mode);
            if(OPRT_OK != op_ret){
                PR_ERR("dltj cal err! product test err!");
                return op_ret;
            }
        }
    }else{
        PR_DEBUG("There not exist dltj");
    }
    return OPRT_OK;
}

/*********************************************************************************
 * FUNCTION:       report_fault_warn
 * DESCRIPTION:    故障事件上报
 * INPUT:          dpid_fault：故障dp;fault_word:故障值
 * OUTPUT:         none
 * RETURN:         OPERATE_RET:初始化状态，返回数值型状态值
 * OTHERS:         故障事件上报
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC OPERATE_RET report_fault_warn(IN UCHAR dpid_fault,IN INT fault_word)
{
    cJSON *root = NULL;
    root = cJSON_CreateObject();
    if(NULL == root) {
        return OPRT_CR_CJSON_ERR;
    }
    CHAR dpid_str[8];

    sprintf(dpid_str, "%d", dpid_fault);
    cJSON_AddNumberToObject(root,dpid_str,fault_word);
    
    CHAR *out;
    out=cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("cJSON_PrintUnformatted err:");
        return OPRT_CJSON_GET_ERR;
    }

    APP_DLTJ_DEBUG("try to report:[%s]", out);
    OPERATE_RET op_ret = tuya_obj_dp_report(out);
    Free(out);
    if(OPRT_OK != op_ret) {
        
        PR_ERR("report fault notice err,op_ret:%d",op_ret);
        return OPRT_DP_REPORT_CLOUD_ERR;
    }
    return OPRT_OK;
}

/*********************************************************************************
 * FUNCTION:       report_over_curr
 * DESCRIPTION:    上报过流事件
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         过流数据上报
 * HISTORY:        2020-03-04
 *******************************************************************************/
VOID report_over_curr(VOID)
{
    if(g_dltj.dp_info.fault_dpid){
        report_fault_warn(g_dltj.dp_info.fault_dpid,0x01);
    }else{
        PR_DEBUG("old oem dltj ,no need to report fault");
    }
}

VOID reset_clear_temp_ele(VOID)
{
    CHAR buff[4] = {0};
    OPERATE_RET op_ret = tuya_psm_set_single(DEVICE_MOD,TEM_ELE_SAVE_KEY,buff);
    PR_NOTICE("tuya_psm_set_single op_ret:%d",op_ret);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_psm_set_single clear ele_value error");
        return;
    }
    PR_DEBUG("clear ele_value default");
}

VOID reset_clear_ele(VOID)
{
    CHAR buff[4] = {0};
    OPERATE_RET op_ret = tuya_psm_set_single(DEVICE_MOD,ELE_SAVE_KEY,buff);
    PR_NOTICE("tuya_psm_set_single op_ret:%d",op_ret);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_psm_set_single clear ele_value error");
        return;
    }
    PR_DEBUG("clear ele_value default");
}

#define DP_STR_LEN 8
/*********************************************************************************
 * FUNCTION:       switch_ele_dp_query
 * DESCRIPTION:    dp_query电量参数
 * INPUT:          dp_qry:app面板查询结构体（设备ID ,查询个数和查询dp点）
 * OUTPUT:         none
 * RETURN:         OPERATE_RET:返回数值型状态值
 * OTHERS:         NULL
 * HISTORY:        2020-03-04
*******************************************************************************/
OPERATE_RET switch_ele_dp_query(IN CONST TY_DP_QUERY_S *dp_qry)
{
    if (dp_qry == NULL || dp_qry->cnt == 0) {
        return OPRT_INVALID_PARM;
    }

    cJSON *root = NULL;
    root = cJSON_CreateObject();
    if(NULL == root) {
        return OPRT_CR_CJSON_ERR;
    }

    if (FALSE == handle_time.is_dp_query) {
        handle_time.is_dp_query = TRUE;
        handle_time.pvi_small_thre = PVI_REPORT_SMALL_THRE_DP_QUERY;
        handle_time.pvi_big_thre = PVI_REPORT_BIG_THRE_DP_QUERY;
        PR_NOTICE("support ele dp query, not report actively from now.");
    }

    CHAR dpid_str[DP_STR_LEN] = {0};
    INT i = 0;
    BOOL have_ele = FALSE;
    for (i=0; i<dp_qry->cnt; i++) {
        PR_DEBUG("query dp:%d", dp_qry->dpid[i]);
        if (dp_qry->dpid[i] == g_dltj.dp_info.idpid) {
            have_ele = TRUE;
            snprintf(dpid_str, DP_STR_LEN, "%d", g_dltj.dp_info.idpid);
            cJSON_AddNumberToObject(root,dpid_str,power_cnt.cur_current);
        } else if (dp_qry->dpid[i] == g_dltj.dp_info.pdpid) {
            have_ele = TRUE;
            snprintf(dpid_str, DP_STR_LEN, "%d", g_dltj.dp_info.pdpid);
            cJSON_AddNumberToObject(root,dpid_str,power_cnt.cur_power);
        } else if (dp_qry->dpid[i] == g_dltj.dp_info.vdpid) {
            have_ele = TRUE;
            snprintf(dpid_str, DP_STR_LEN, "%d", g_dltj.dp_info.vdpid);
            cJSON_AddNumberToObject(root,dpid_str,power_cnt.cur_vol);
        }else {
            return OPRT_INVALID_PARM;
        }
    }

    if(FALSE == have_ele) {
        PR_DEBUG("not query ele");
        cJSON_Delete(root);
        return OPRT_OK;
    }
    CHAR *out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if(NULL == out) {
        PR_ERR("cJSON_PrintUnformatted err:");
        return OPRT_CJSON_GET_ERR;
    }
    APP_DLTJ_DEBUG("try to report:[%s]", out);

    OPERATE_RET op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,out);
    Free(out);
    if(OPRT_OK != op_ret) {
        PR_ERR("report ele par err,op_ret:%d",op_ret);
        return OPRT_DP_REPORT_CLOUD_ERR;
    }
    
    return OPRT_OK;
}

