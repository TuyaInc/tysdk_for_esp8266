/***********************************************************
*  File: smart_wf_frame.h 
*  Author: nzy
*  Date: 20150611
***********************************************************/
#ifndef _SMART_WF_FRAME_H
    #define _SMART_WF_FRAME_H

    #include "sys_adapter.h"
    #include "cJSON.h"
    #include "gw_intf.h"
    #include "tuya_httpc.h"
    #include "com_struct.h"
    #include "wmtime.h"
    #include "tuya_ws_db.h"
    #include "tuya_fw_ug.h"
	#include "log_seq_adpt.h"
    #include "tuya_httpc.h"
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __SMART_WF_FRAME_GLOBALS
    #define __SMART_WF_FRAME_EXT
#else
    #define __SMART_WF_FRAME_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef enum {
    LAN_CMD = 0, // command come from lan
    MQ_CMD,      // command come from mqtt
    TIMER_CMD,   // command come from timer
}SMART_CMD_E;

typedef enum {
	ACK_NORMAL = 0, //normal 
	ACK_OEM, //multi
}ACK_TYPE_E;


#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER==1)
 //low_power
typedef enum {
	GET_PASS_FAIL=0, 
	GET_PASS_OK, 
}PASS_STA_E;

#endif

// data format: {"1":100,"2":200}
typedef VOID (*SMART_FRAME_CB)(SMART_CMD_E cmd,cJSON *root);

typedef VOID (*GW_RESET_CB)(VOID);

typedef VOID (*GW_RESET_FAC_CB)(VOID);

typedef VOID (*APP_PROD_CB)(BOOL flag, CHAR rssi);

#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER==1)
//low_power
typedef VOID (*GET_PASS_RESULT_CB)(PASS_STA_E sta,BYTE *data, INT len,INT timer);

typedef VOID (*GET_MUTI_PASS_RESULT_CB)(PASS_STA_E sta,BYTE *data, INT cnt);

#endif

typedef OPERATE_RET (*IR_SPECIAL_DP_CB)(USHORT dpid,cJSON *dp_obj);

typedef enum {
    CFG_SUCCESS = 0,
    CFG_NW_NO_FIND,
    CFG_AUTH_FAIL,
}LAN_WF_CFG_RESULT_E;

typedef enum {
    SMART_CFG = 0,
    AP_CFG
}WF_CFG_MODE_E;

// complex data type descripe
typedef enum {
    CPLX_BOOL = 0,
    CPLX_VALUE,
    CPLX_STR,
    CPLX_DATE,
}CPLX_DA_T_E;

typedef union{
    BOOL bl;
    INT value;
    CHAR str[32];
    ULONG date;
}CPLX_DA_T_U;

typedef struct {
    CPLX_DA_T_E type;
    CPLX_DA_T_U data;
}CPLX_DATA_S;

// Statistics data
typedef union{
    BOOL bl;
    UINT value;
	CHAR str[0];
}DP_DATA_T_U;

typedef struct {
	UINT time;	//ͳ��ʱ��
	UINT len;   //data����
	DP_PROP_TP_E type; //dp����
    DP_DATA_T_U data;  //dp����
}STATS_DATA_S;

typedef struct {
    IR_SPECIAL_DP_CB ir_dp_cb; 
}TY_IOT_SP_DP_CBS_S;


typedef struct {
    GW_RESET_CB gw_reset_cb;
	GW_RESET_FAC_CB gw_reset_fac_cb; 
}TY_RESET_CBS;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
#if defined(ENABLE_LOW_POWER) && (ENABLE_LOW_POWER==1)

/***********************************************************
*  Function: start_get_dp_newest_state //low_power
*  Input: 
*  Output:
*  Return: 0 succ -1 fail 
*  Note:
***********************************************************/
OPERATE_RET start_get_dp_newest_state(UCHAR *dp_arr, UCHAR dp_num, NEWEST_DP_STATE_CB fun_cb);

/***********************************************************
*  Function: start_get_pass_word //low_power
*  Input: 
*  Output:
*  Return: 0 succ -1 fail 
*  Note:
***********************************************************/
VOID start_get_pass_word(VOID);

/***********************************************************
*  Function: get_muti_pass_word_sta //low_power
*  Input: 
*  Output:
*  Return: 0 succ -1 fail 
*  Note:
***********************************************************/
VOID get_muti_pass_word_sta(PASS_STA_E sta,BYTE *data, INT cnt, BOOL is_schedule);

/***********************************************************
*  Function: start_get_muti_pass_word //low_power
*  Input: 
*  Output:
*  Return: 0 succ -1 fail 
*  Note:
***********************************************************/
VOID start_get_muti_pass_word(VOID);

/***********************************************************
*  Function: start_get_muti_pass_word_schedule //low_power
*  Input: 
*  Output:
*  Return: 0 succ -1 fail 
*  Note:
***********************************************************/
VOID start_get_muti_pass_word_schedule(VOID);

/***********************************************************
*  Function: lock_pass_word_init //low_power
*  Input: 
*  Output:
*  Return: 0 succ -1 fail 
*  Note:
***********************************************************/
 __SMART_WF_FRAME_EXT \
OPERATE_RET lock_pass_word_init(GET_PASS_RESULT_CB cb, GET_MUTI_PASS_RESULT_CB muti_cb,
								      GET_MUTI_PASS_RESULT_CB muti_schedule_cb);
									  
/***********************************************************
*  Function: common_send_http_msg
*  Input:    
*  Output:
*  Return: none
*  Note: none
***********************************************************/
OPERATE_RET common_send_http_msg(CHAR *api, CHAR *ver, CHAR *data, COMM_PROC_HTTP_RET_CB pfun_cb);

/***********************************************************
*  Function: 默认低功耗模式下，当设备被移除后，是否进入低功耗模式
*  Input:
*  Output:
*  Return: none
*  Note:   none
***********************************************************/
VOID set_lowpower_after_remote(BOOL flag);

#endif
/***********************************************************
*  Function: get_local_time
*  Input: 
*  Output: st_time ����ʱ��
*  Return: 
*  Note:��ȡ����ʱ��,������ʱ��+/-ʱ��
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET get_local_time(OUT struct tm *st_time);

/***********************************************************
*  Function: get_fix_wmtime
*  Input: 
*  Output: fixed wmtime
*  Return: 0 succ -1 fail 
*  Note:
***********************************************************/
int get_fix_wmtime(struct tm *tm);

/***********************************************************
*  Function: check_gw_exist
*  Input: 
*  Output: 
*  Return: 
*  Note:
***********************************************************/
__SMART_WF_FRAME_EXT \
VOID check_gw_exist(VOID);

/***********************************************************
*  Function: is_exist_localKey
*  Input: 
*  Output: 
*  Return: 
*  Note:
***********************************************************/
__SMART_WF_FRAME_EXT \
BOOL is_exist_localKey(VOID);

/***********************************************************
*  Function: lan_data_encrypt
*  Input: plian
*  Output: cipher
*  Return: 
*  Note:
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET lan_data_encrypt(IN CONST CHAR *plian, OUT CHAR **cipher);

/***********************************************************
*  Function: lan_data_decrypt
*  Input: cipher
*  Output: plian
*  Return: 
*  Note:
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET lan_data_decrypt(IN CONST CHAR *cipher, INOUT CHAR *plian);

/***********************************************************
*  Function: set_acktive_type
*  Input: type: ACK_NORMAL ʹ��productid(��Ʒid)����
*				ACK_OEM    ʹ��productkey(�̼�key)����
*  Output: 
*  Return: 
*  Note: default use ACK_NORMAL, 
*        must be called in app_init
*        ע��:�̼�key��key��ͷ��,�� "key......"����Ҫ���
***********************************************************/
__SMART_WF_FRAME_EXT \
VOID set_acktive_type(ACK_TYPE_E type);

/***********************************************************
*  Function: system_init
*  Input: 
*  Output: 
*  Return: 
*  Note:
***********************************************************/
OPERATE_RET system_init(VOID);

/*****************exit******************************************
*  Function: smart_frame_init
*  Input: 
*  Output: 
*  Return: 
*  Note:
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET smart_frame_init(IN CONST SMART_FRAME_CB cb,CONST CHAR *sw_ver);

/***********************************************************
*  Function: get_product_key
*  Input: 
*  Output: 
*  Return: 
*  Note: get product key
***********************************************************/
OPERATE_RET get_product_key(CHAR *prdkey);

/***********************************************************
*  Function: single_wf_device_init_pk
*  Input: 
*  Output: 
*  Return: 
*  Note: if can not read dev_if in the flash ,so use the def_dev_if
*        def_dev_if->id == gwid
*        use product key 
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET single_wf_device_init_pk(INOUT DEV_DESC_IF_S *def_dev_if);

/***********************************************************
*  Function: get_single_wf_dev
*  Input:  
*  Output: 
*  Return: 
*  Note: DEV_CNTL_N_S *
***********************************************************/
__SMART_WF_FRAME_EXT \
DEV_CNTL_N_S *get_single_wf_dev(VOID);

/***********************************************************
*  Function: sw_ver_change
*  Input:  
*  Output: 
*  Return: 
*  Note:   
***********************************************************/
OPERATE_RET sw_ver_change(IN CONST CHAR *ver,OUT UINT *change);

/***********************************************************
*  Function: mcu_ug_init
*  Input:  
*  Output: 
*  Return: 
*  Note:   
***********************************************************/
OPERATE_RET mcu_ug_init(IN CONST FILE_HTTPC_CB cb);

VOID send_ug_pro_msg(VOID);

/***********************************************************
*  Function: send_mcu_ug_start
*  Input:  
*  Output: 
*  Return: 
*  Note:   
***********************************************************/
VOID send_mcu_ug_start(VOID);

/***********************************************************
*  Function: send_syn_data_start
*  Input:  
*  Output: 
*  Return: 
*  Note:   
***********************************************************/
VOID send_syn_data_start(VOID);

/***********************************************************
*  Function: sf_obj_dp_report ,obj dp report for user
*  Input: data->the format is :{"1":1,"2":true}
*  Output: 
*  Return: 
*  Note:
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET sf_obj_dp_report(IN CONST CHAR *id,IN CONST CHAR *data);
/***********************************************************
*  Function: sf_obj_dp_trans_report ,obj dp transprent report for user
*  Input: data->the format is :{"1":1,"2":true}/{"devid":"xx","dps":{"1",1}}
*  Output: 
*  Return: 
*  Note:
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET sf_obj_dp_trans_report(IN CONST CHAR *id,IN CONST CHAR *data);

/***********************************************************
*  Function: sf_obj_dp_sync_rtc:obj dp sync report to cloud server
*  Input: id->device id
*         data->{"1":1,"2":true}
*         time->{"1":dp1_time,"2":dp2_time},dp1_time == json int
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET sf_obj_dp_sync_rtc(IN CONST CHAR *id,IN CONST CHAR *data,IN CONST CHAR *time);

/***********************************************************
*  Function: sf_raw_dp_report ,raw dp report for user
*  Input: data->raw 
*  Output: 
*  Return: 
*  Note:
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET sf_raw_dp_report(IN CONST CHAR *id,IN CONST BYTE dpid,\
                             IN CONST BYTE *data, IN CONST UINT len);

/***********************************************************
*  Function: sf_get_dp_cntl
*  Input: id or dev_cntl use to get dev_cntl
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
DP_CNTL_S *sf_get_dp_cntl(IN CONST CHAR *id,\
                          IN CONST DEV_CNTL_N_S *dev_cntl,\
                          IN CONST BYTE dpid);

/***********************************************************
*  Function: get_fw_ug_stat
*  Input: none
*  Output: 
*  Return: FW_UG_STAT_E
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
FW_UG_STAT_E get_fw_ug_stat(VOID);

/***********************************************************
*  Function: sf_fw_ug_msg_infm
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET sf_fw_ug_msg_infm(VOID);

/***********************************************************
*  Function: sf_load_sumer_table
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
VOID sf_load_sumer_table(VOID);


/***********************************************************
*  Function: tuya_get_wf_cfg_mode
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
WF_CFG_MODE_E tuya_get_wf_cfg_mode(VOID);

/***********************************************************
*  Function: direct_single_dev_reset
*  Input: none
*  Output:
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET direct_single_dev_reset(VOID);

/***********************************************************
*  Function: single_dev_reset_factory
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
void single_dev_reset_factory(void);

/***********************************************************
*  Function: single_dev_reset_factory_select
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
void single_dev_reset_factory_select(NW_CFG_MODE_E cfg_mode);

/***********************************************************
*  Function: single_dev_ug_timer
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
void single_dev_ug_timer(void);

/***********************************************************
*  Function: single_dev_query_timer
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
void single_dev_query_timer(void);

/***********************************************************
*  Function: select_ap_cfg_wf
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
void select_ap_cfg_wf(void);

/***********************************************************
*  Function: select_smart_cfg_wf
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
void select_smart_cfg_wf(void);

/***********************************************************
*  Function: auto_select_wf_cfg
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
void auto_select_wf_cfg(void);

/***********************************************************
*  Function: get_fw_size
*  Input: none
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
UINT get_fw_size(VOID);

/***********************************************************
*  Function: TYWE1S_gpio_test
*  Input: 
*  Output: 
*  Return: TRUE or FALSE
*  Note: gpio test func
***********************************************************/
__SMART_WF_FRAME_EXT \
BOOL TYWE1S_gpio_test(VOID);

/***********************************************************
*  Function: lan_set_network_and_active
*  Input: 
*  Output: 
*  Return: 
*  Note: 
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET lan_set_network_and_active(IN CONST CHAR *ssid,IN CONST CHAR *passwd,IN CONST CHAR *token);

/***********************************************************
*  Function: set_console
*  Input: TRUE uart1, FALSE uart0
*  Output: 
*  Return: 
*  Note: 
***********************************************************/
__SMART_WF_FRAME_EXT \
void set_console(BOOL uart1);

/***********************************************************
*  Function: set_prod_ssid
*  Input: product test ssid
*  Output: 
*  Return: 
*  Note: default use ssid tuya_mdev_test
***********************************************************/
__SMART_WF_FRAME_EXT \
void set_prod_ssid(CHAR *ssid);

/***********************************************************
*  Function: astro_send_request_msg
*  Input:    plon  经度
             plat  纬度
             pfun_cb 接收处理函数
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET astro_send_request_msg(IN CONST CHAR *plon,IN CONST CHAR *plat,
                                         IN CONST ASTRO_TIMER_CB pfun_cb);


/***********************************************************
*	 Function: smart_frame_send_ap_udp_msg
*	 Input:   data_code  上报错误码
*	 Output: 
*	 Return: none
*	 Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET smart_frame_send_ap_udp_msg(IN CONST BYTE data_code);

/***********************************************************
*	 Function: smart_frame_send_ap_log_msg
*	 Input:   type  上报类型
*	 Output: 
*	 Return: none
*	 Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET smart_frame_send_ap_log_msg(IN CONST LOG_SEQ_LOG_FILE_RP*rp_log);

/***********************************************************
*	 Function: set_multi_firm_key
*	 Input:   firmkey
*	 Return: none
*	 Note: must befor single_wf_device_init_pk
***********************************************************/
__SMART_WF_FRAME_EXT \
VOID set_multi_firm_key(UCHAR* firm_key);

/***********************************************************
*	 Function: get_multi_firm_key
*	 Input:   firmkey
*	 Return: none
*	 Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
UCHAR* get_multi_firm_key(VOID);
/***********************************************************
*	 Function: smart_frame_send_rp_timer_info_msg
*	 Input:   info  上报信息
*	 Output: 
*	 Return: none
*	 Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET smart_frame_send_rp_timer_info_msg(CHAR*info);

/***********************************************************
*  Function: report_mcu_sn_msg
*  Input:    sn mcu sn
          pfun_cb 接收处理函数
*  Output:
*  Return: none
*  Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
OPERATE_RET report_mcu_sn_msg(UCHAR *data, UCHAR len, REPORT_MCU_SN fun_cb);

/***********************************************************
*	 Function: start_test_close_timer
*	 Input:   void
*	 Return: none
*	 Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
VOID start_test_close_timer(VOID);

/***********************************************************
*	 Function: set_gw_reset_cb
*	 Input:   void
*	 Return: none
*	 Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
VOID set_gw_reset_cb(GW_RESET_CB cb);

/***********************************************************
*	 Function: set_gw_reset_fac_cb
*	 Input:   void
*	 Return: none
*	 Note: none
***********************************************************/
__SMART_WF_FRAME_EXT \
VOID set_gw_reset_fac_cb(GW_RESET_FAC_CB cb);



#ifdef __cplusplus
}
#endif
#endif

