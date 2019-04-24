/***********************************************************
*  File: gw_intf.h 
*  Author: nzy
*  Date: 20150822
***********************************************************/
#ifndef _GW_INTF_H
    #define _GW_INTF_H

    #include "sys_adapter.h"
    #include "com_def.h"
    #include "tuya_ws_db.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __GW_INTF_GLOBALS
    #define __GW_INTF_EXT
#else
    #define __GW_INTF_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define SCHEMA_LMT 6*512

/***********************************************************
*************************variable define********************
***********************************************************/
typedef VOID(*SYN_DATA_CB)(VOID);

/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: syn_data_reg
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
VOID syn_data_reg(IN CONST SYN_DATA_CB callback);

/***********************************************************
*  Function: set_gw_mq_conn_stat
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
VOID set_gw_mq_conn_stat(IN CONST BOOL stat);

/***********************************************************
*  Function: get_gw_mq_conn_stat
*  Input: 
*  Output: 
*  Return: BOOL
***********************************************************/
__GW_INTF_EXT \
BOOL get_gw_mq_conn_stat(VOID);

/***********************************************************
*  Function: set_gw_status
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
VOID set_gw_status(IN CONST GW_STAT_E stat);

/***********************************************************
*  Function: get_gw_status
*  Input: 
*  Output: 
*  Return: GW_STAT_E
***********************************************************/
__GW_INTF_EXT \
GW_STAT_E get_gw_status(VOID);

/***********************************************************
*  Function: get_gw_cntl
*  Input: 
*  Output: 
*  Return: GW_CNTL_S
***********************************************************/
__GW_INTF_EXT \
GW_CNTL_S *get_gw_cntl(VOID);

/***********************************************************
*  Function: gw_cntl_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_cntl_init(CONST CHAR *gw_ver);

/***********************************************************
*  Function: gw_update_timezone
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_update_timezone(IN CONST CHAR *timeZone);

/***********************************************************
*  Function: gw_lc_bind_device
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_lc_bind_device(IN CONST DEV_DESC_IF_S *dev_if,\
                              IN CONST CHAR *sch_json_arr);

/***********************************************************
*  Function: gw_lc_bind_device_pk
*  Input: 
*  Output: 
*  Return: 
*  Note: use the prodect key to register
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_lc_bind_device_pk(IN CONST DEV_DESC_IF_S *dev_if);

/***********************************************************
*  Function: get_dev_cntl
*  Input: 
*  Output: 
*  Return: 
*  Note: 
***********************************************************/
__GW_INTF_EXT \
DEV_CNTL_N_S *get_dev_cntl(IN CONST CHAR *id);

/***********************************************************
*  Function: check_dev_need_update
*  Input: 
*  Output: 
*  Return: 
*  Note: user need to fill the content in the function before 
         call it
***********************************************************/
__GW_INTF_EXT \
VOID check_dev_need_update(IN CONST DEV_CNTL_N_S *dev_cntl);

/***********************************************************
*  Function: check_and_update_dev_desc_if
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
VOID check_and_update_dev_desc_if(IN CONST CHAR *id,\
                                  IN CONST CHAR *name,\
                                  IN CONST CHAR *sw_ver,\
                                  IN CONST CHAR *schema_id,\
                                  IN CONST CHAR *ui_id);

/***********************************************************
*  Function: check_all_dev_if_update
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
VOID check_all_dev_if_update(VOID);

/***********************************************************
*  Function: start_active_gateway
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
VOID start_active_gateway(VOID);

/***********************************************************
*  Function: set_gw_prodinfo
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
VOID set_gw_prodinfo(IN CONST CHAR *prod_idx,IN CONST CHAR *mac);

/***********************************************************
*  Function: set_gw_data_fac_reset
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__GW_INTF_EXT \
VOID set_gw_data_fac_reset(VOID);

/***********************************************************
*  Function: get_tysdk_ver
*  Input: none
*  Output: 
*  Return: CHAR *
***********************************************************/
__GW_INTF_EXT \
CHAR *get_tysdk_ver(VOID);

/***********************************************************
*  Function: gw_set_pcmcd_lmt
*  Input: 
*  Output: 
*  Return: none
***********************************************************/
__GW_INTF_EXT \
VOID gw_set_pcmcd_lmt(IN CONST TIME_S pr_lmt);

/***********************************************************
*  Function: gw_very_prcmd_pr
*  Input: cmd_time
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_very_prcmd_pr(IN CONST TIME_S cmd_time);

/***********************************************************
*  Function: gw_very_pcmcd_mcd
*  Input: cmd_from seq
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_very_pcmcd_mcd(IN CONST UINT cmd_from,\
                              IN CONST UINT seq);

/***********************************************************
*  Function: mq_conn_permit_by_prt
*  Input: 
*  Output: 
*  Return: none
***********************************************************/
__GW_INTF_EXT \
BOOL mq_conn_permit_by_prt(VOID);

/***********************************************************
*  Function: set_getprt_success
*  Input: 
*  Output: 
*  Return: none
***********************************************************/
__GW_INTF_EXT \
VOID set_getprt_success(VOID);
/***********************************************************
*  Function: gw_register_cbs
*  Input: cbs
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_register_cbs(IN CONST TY_IOT_CBS_S *cbs);

#ifdef __cplusplus
}
#endif
#endif

