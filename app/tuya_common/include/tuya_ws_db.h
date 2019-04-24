/***********************************************************
*  File: tuya_ws_db.h 
*  Author: nzy
*  Date: 20150601
***********************************************************/
#ifndef _TUYA_WS_DB_H
    #define _TUYA_WS_DB_H

    #include "sys_adapter.h"
    #include "mem_pool.h"
    #include "com_struct.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __TUYA_WS_DB_GLOBALS
    #define __TUYA_WS_DB_EXT
#else
    #define __TUYA_WS_DB_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct {
    INT time;   //last fetchtime
    INT count;   //record count
    BOOL is_whole; //record is whole
}LOCAL_TIMER_S;

typedef enum {
    NW_SMART_CFG = 0,
    NW_AP_CFG,
}NW_CFG_MODE_E;

typedef enum {
	TYPE_EASYCONNECT = 0,
	TYPE_AIRKISS,
	TYPE_EASYCONNECT_V3,
} SMARTLINK_TYPE;

typedef enum {
	PATH_NONE = 0,
	PATH_MULTICAST,
	PATH_BROADCAST,
	PATH_ALL,
} CONFIG_PATH_E;

typedef enum {
	SOURCE_NONE = 0,
	SOURCE_STATION,
	SOURCE_AP,
	SOURCE_ALL,
} CONFIG_SOURCE_E;

typedef enum {
	NOT_SAFE_FIRM = 0,
	SAFE_FIRM,
}FIRM_TYPE;

typedef BYTE WF_WK_MODE;
#define WWM_NORMAL 0 //  normal mode 
#define WWM_LOW_POWER 1 // low power work mode
#define WWM_SPECIAL_NWCM 2 // work in network config.
//                         // when do not config network info over five minutes,then use old network info to connect 

#define WF_SSID_LEN 32+1
#define WF_PASSWD_LEN 64+1

typedef struct {
    WF_WK_MODE wk_mode;
    NW_CFG_MODE_E mode;
    CHAR ssid[WF_SSID_LEN+1];
	CHAR passwd[WF_PASSWD_LEN+1];
	SMARTLINK_TYPE link_type;
	//
	CONFIG_SOURCE_E source;
	CONFIG_PATH_E path;
	uint8_t config_time;
	//
	uint8_t airkiss_random;
}WF_NW_REC_S;

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: ws_db_init
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_init(VOID);

/***********************************************************
*  Function: ws_exist_auzkey
*  Input: 
*  Output: 
*  Return: BOOL
***********************************************************/
BOOL ws_exist_auzkey(VOID);

/***********************************************************
*  Function: ws_db_set_prod_if
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_set_prod_if(IN CONST PROD_IF_REC_S *prod_if);

/***********************************************************
*  Function: ws_db_get_prod_if
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_get_prod_if(OUT PROD_IF_REC_S *prod_if);

/***********************************************************
*  Function: ws_db_set_gw_actv
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_set_gw_actv(IN CONST GW_ACTV_IF_S *gw_actv);

/***********************************************************
*  Function: ws_db_get_gw_actv
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_get_gw_actv(OUT GW_ACTV_IF_S *gw_actv);

/***********************************************************
*  Function: ws_db_set_dev_if
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_set_dev_if(IN CONST DEV_DESC_IF_S *dev_if);

/***********************************************************
*  Function: ws_db_get_dev_if
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_get_dev_if(OUT DEV_DESC_IF_S *dev_if);

/***********************************************************
*  Function: ws_db_set_wf_nw
*  Input: 
*  Output: 
*  Return: OPERATE_RET
*  Note: 
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_set_wf_nw(IN CONST WF_NW_REC_S *wf_nw_rec);

/***********************************************************
*  Function: ws_db_get_wf_nw
*  Input: 
*  Output: 
*  Return: OPERATE_RET
*  Note: 
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_get_wf_nw(OUT WF_NW_REC_S *wf_nw_rec);

/***********************************************************
*  Function: ws_db_set_ap_ssid
*  Input: ssid
*  Output: 
*  Return: OPERATE_RET
*  Note: 
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_set_ap_ssid(IN CONST CHAR *ssid, IN CONST CHAR *pwd);

/***********************************************************
*  Function: ws_db_get_ap_ssid
*  Input: ssid_buf_len
*  Output: ssid
*  Return: OPERATE_RET
*  Note: 
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_get_ap_ssid(OUT CHAR *ssid,OUT CHAR *pwd, OUT BOOL *is_pwd);

/***********************************************************
*  Function: ws_db_set_dev_schema
*  Input: schema
*  Output: 
*  Return: OPERATE_RET
*  Note: 
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_set_dev_schema(IN CONST CHAR *schema);

/***********************************************************
*  Function: ws_db_get_dev_schema
*  Input: schema_buf_len
*  Output: schema
*  Return: OPERATE_RET
*  Note: 
***********************************************************/
__TUYA_WS_DB_EXT \
OPERATE_RET ws_db_get_dev_schema(OUT CHAR *schema,IN CONST UINT schema_buf_len);

/***********************************************************
*  Function: ws_db_set_timer_posix
*  Input: local_timer
*  Output: 
*  Return: OPERATE_RET
*  Note: 
***********************************************************/
OPERATE_RET ws_db_set_timer_posix(IN CONST LOCAL_TIMER_S *local_timer);

/***********************************************************
*  Function: ws_db_get_timer_posix
*  Input:
*  Output: local_timer
*  Return: OPERATE_RET
*  Note: 
***********************************************************/
OPERATE_RET ws_db_get_timer_posix(OUT LOCAL_TIMER_S *local_timer);

/***********************************************************
*  Function: ws_db_reset
*  Input: 
*  Output: 
*  Return: OPERATE_RET
*  Note: only reset gw reset info and device bind info
***********************************************************/
__TUYA_WS_DB_EXT \
VOID ws_db_reset(VOID);

VOID get_muid_string(CHAR *muid_str);

/***********************************************************
*  Function: clear_prod_test_mode
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__TUYA_WS_DB_EXT \
VOID clear_prod_test_mode(VOID);

/***********************************************************
*  Function: ws_db_set_user_param
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_set_user_param(IN CHAR *data,IN CONST UINT len);

/***********************************************************
*  Function: ws_db_get_user_param
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_get_user_param(OUT CHAR **pp_data,OUT UINT *len);

/***********************************************************
*  Function: ws_db_set_sumer_table
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_set_sumer_table(IN CONST CHAR *in_tab);

/***********************************************************
*  Function: ws_db_get_sumer_table
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_get_sumer_table(OUT CHAR *out_tab, IN CONST INT out_lmt);

/***********************************************************
*  Function: ws_db_set_ctei_info
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_set_ctei_info(IN CONST CHAR* in_tab);

/***********************************************************
*  Function: ws_db_get_ctei_info
*  Input: 
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_get_ctei_info(OUT CHAR* out_tab, IN CONST UINT out_lmt);

#ifdef __cplusplus
}
#endif
#endif

