/***********************************************************
*  File: wf_sdk_adpt.h 
*  Author: nzy
*  Date: 20150831
***********************************************************/
#ifndef _WF_SDK_ADPT_H
    #define _WF_SDK_ADPT_H
    
    #include "com_def.h"
    #include "com_struct.h"
    #include "mem_pool.h"
    #include "sys_adapter.h"
    #include "tuya_ws_db.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __WF_SDK_ADPT_GLOBALS
    #define __WF_SDK_ADPT_EXT
#else
    #define __WF_SDK_ADPT_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef BYTE WF_CFG_MTHD_SEL; // wifi config method select
#define WCM_OLD 0 // do not have low power mode
#define WCM_LOW_POWER 1 // with low power mode
#define WCM_SPCL_MODE 2 // special with low power mode
#define WCM_OLD_CPT   3 // old mode scan
#define WCM_LOW_POWER_AUTOCFG 4 // with low power mode && auto cfg
#define WCM_SPCL_AUTOCFG 5 // special with low power mode && auto cfg


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: wifi_country_code_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
VOID wifi_country_code_init(VOID);

/***********************************************************
*  Function: wifi_nw_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
OPERATE_RET wifi_nw_init(VOID);


#if (WPS_CONFIG == 1) 
/***********************************************************
*  Function: set_smart_only
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID set_smart_only(VOID);
#endif

/***********************************************************
*  Function: set_ap_only
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID set_ap_only(VOID);

/***********************************************************
*  Function: set_ap_offline
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID set_ap_offline(VOID);

/***********************************************************
*  Function: set_ap_ssid
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID set_ap_ssid(IN CONST CHAR *ssid);

/***********************************************************
*  Function: set_all_ap_ssid
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID set_all_ap_ssid(IN CONST CHAR *ssid);

/***********************************************************
*  Function: get_link_type
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
SMARTLINK_TYPE get_link_type(VOID);

/***********************************************************
*  Function: get_wf_gw_status
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
GW_WIFI_STAT_E get_wf_gw_status(VOID);

/***********************************************************
*  Function: get_gw_cur_ip
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
CHAR *get_gw_cur_ip(IN CONST GW_WIFI_STAT_E wf_stat);

/***********************************************************
*  Function: wifi_scan
*  Input: 
*  Output: pp_wf_lst->need to Free after use
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
OPERATE_RET wifi_scan(OUT CHAR **pp_wf_lst);

/***********************************************************
*  Function: lan_set_net_work
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
OPERATE_RET lan_set_net_work(IN CONST CHAR *ssid,IN CONST CHAR *passphrase);

/***********************************************************
*  Function: wf_sdk_adpt_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
OPERATE_RET wf_sdk_adpt_init(VOID);

/***********************************************************
*  Function: get_wf_nw_rec
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
WF_NW_REC_S *get_wf_nw_rec(VOID);

/***********************************************************
*  Function: wifi_func_verify_pt
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__WF_SDK_ADPT_EXT \
OPERATE_RET wifi_func_verify_pt(CHAR *rssi);

/***********************************************************
*  Function: set_wf_cfg_method
*  Input: mthd
*  Output: none
*  Return: none
*  Note: default set as WCM_OLD mode.if u want to set other
*        mode,u must call this function before "smart_frame_init"
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID set_wf_cfg_method(IN CONST WF_CFG_MTHD_SEL mthd);

/***********************************************************
*  Function: get_wf_cfg_method
*  Input: none
*  Output: 
*  Return: WF_CFG_MTHD_SEL
***********************************************************/
__WF_SDK_ADPT_EXT \
WF_CFG_MTHD_SEL get_wf_cfg_method(VOID);

/***********************************************************
*  Function: wf_end_smt_cfg_manual
*  Input: none
*  Output: none
*  Return: none
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID wf_end_smt_cfg_manual(VOID);

/***********************************************************
*  Function: wifi_param_init
*  Input: none
*  Output: none
*  Return: none
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID wifi_param_init(WF_CFG_MTHD_SEL metd);

/***********************************************************
* 设置低功耗模式/spcl模式下超时时间
* 须在app_init中app_cfg_set之前调用
* 入参单位为秒，最小为60s，默认3分钟
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID set_wf_cfg_timeout(INT timeout);


/***********************************************************
*  Function: wifi_param_init
*  Input: WIFI_EVENT_CB cb
*  Return: OPERATE_RET
***********************************************************/
__WF_SDK_ADPT_EXT \
OPERATE_RET register_wifi_event_cb(wifi_event_handler_cb_t cb);

/***********************************************************
*  Function: nw_cfg_stop_cb
*  Input: void
*  Return: void
***********************************************************/
__WF_SDK_ADPT_EXT \
VOID nw_cfg_stop_cb(VOID);

/***********************************************************
*	Function: select_cfg_mode_fac
*	Output:
*	Return: none
*	Note: none
***********************************************************/
__WF_SDK_ADPT_EXT \
OPERATE_RET select_cfg_mode_fac(void);

/***********************************************************
*  Function: ty_wifi_get_mac_addr
*  Input: void
*  Return: void
***********************************************************/
__WF_SDK_ADPT_EXT \
OPERATE_RET ty_wifi_get_chip_mac_addr(CHAR* mac_addr,CHAR len);


#ifdef __cplusplus
}
#endif
#endif

