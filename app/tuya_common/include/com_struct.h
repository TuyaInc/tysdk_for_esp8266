/***********************************************************
*  File: com_struct.h 
*  Author: nzy
*  Date: 20150522
***********************************************************/
#ifndef _COM_STRUCT_H
    #define _COM_STRUCT_H    
    #include "com_def.h"
    #include "sys_adapter.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __COM_STRUCT_GLOBALS
    #define __COM_STRUCT_EXT
#else
    #define __COM_STRUCT_EXT extern
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/
// gw information define
#define PROD_IDX_LEN 8 // prodect index len
#define GW_ID_LEN 25 // gw id len
#define DEV_ID_LEN 25 // dev id len
#define UID_LEN 20 // user id len
#define UID_ACL_LMT 1 // user acl limit
#define SW_VER_LEN 10 // sw ver len
#define SEC_KEY_LEN 16 // securt key len
#define LOCAL_KEY_LEN 16 //securt lan dps key len
#define ACTIVE_KEY_LEN 16 // active key len
#define SCHEMA_ID_LEN 10 
#define ETAG_LEN 10
#define HTTP_URL_LMT 64
#define MQ_URL_LMT 64
#define INTR_LEN_LMT 256 //sumer timer intervals len
#define PRODUCT_KEY_LEN 16
#define TIME_ZONE_LEN  10
#define CFG_VER_LEN 10
#define MAC_ADDR_LEN 12
#define AUTH_KEY_LEN 32
#define PSK_KEY_LEN 40

//
#define WXAPPID_LEN 32
#define REGION_LEN 2
#define REGIST_KEY_LEN 4
#define SUM_TAB_LMT 6

#define LON_LAT_LMT 64
#define CH_NAME_LMT 10
#define CH_CODE_LMT 20
#define CTEI_SN_LEN 32

#if (WPS_CONFIG == 1)
#define NOT_WPS_MODE 0
#define WPS_MODE     1
#define WPS_OPEN     1
#endif


// fw upgrade status
typedef enum {
    UG_IDLE = 0,
    UG_RD,
    UPGRADING,
    UG_FIN,
    UG_EXECPTION,
}FW_UG_STAT_E;

// firmware upgrade
#define FW_URL_LEN 255
#define FW_MD5_LEN 32
#define FW_HMAC_LEN 64

// upgrade type 
typedef enum {
    GW_UPGD = 1,
    DEV_UPGD,
}UPGD_TYPE_E;

typedef struct {
    UPGD_TYPE_E tp;
    CHAR fw_url[FW_URL_LEN+1];
    CHAR fw_hmac[FW_HMAC_LEN+1];
    CHAR sw_ver[SW_VER_LEN+1];
    CHAR etag[ETAG_LEN+1];
    UINT file_size;
#if 0
    INT  auto_ug;
#endif
}FW_UG_S;

// gateway access ability
typedef INT GW_ABI;
#define GW_NO_ABI 0 // virtual gateway
#define GW_ZB_ABI (1<<0) // zigbee
#define GW_BLE_ABI (1<<1) // ble
#define GW_RF433_ABI (1<<2) // RF 315/433
#define GW_DEF_ABI GW_NO_ABI

// device ability
typedef BYTE DEV_ABI_E;
#define DEV_SINGLE 0 // 特指WIFI单品设备
#define DEV_ZB 1 // zigbee
#define DEV_BLE 2
#define DEV_RF433 3
#define DEV_WIFI 4 // 通过实体GW接入的WIFI设备

// dp type
typedef BYTE DP_TYPE_E;
#define T_OBJ 0
#define T_RAW 1
#define T_FILE 2

// dp mode
typedef BYTE DP_MODE_E;
#define M_RW 0
#define M_WR 1
#define M_RO 2

// dp schema type
typedef BYTE DP_PROP_TP_E;
#define PROP_BOOL 0
#define PROP_VALUE 1
#define PROP_STR 2
#define PROP_ENUM 3
#define PROP_BITMAP 4
#define PROP_COMPLEX 5 // add by nzy 20150810 

typedef struct {
    CHAR id[GW_ID_LEN+1];
    CHAR sw_ver[SW_VER_LEN+1]; // xx.xx
    CHAR bs_ver[SW_VER_LEN+1]; // base version xx.xx
    CHAR prtl_ver[SW_VER_LEN+1]; // protocol version
    CHAR etag[ETAG_LEN+1]; // add by nzy 20151015
    GW_ABI ability;
    BOOL sync;
}GW_DESC_IF_S;

typedef struct {
    CHAR id[DEV_ID_LEN+1];
    CHAR sw_ver[SW_VER_LEN+1];
    CHAR schema_id[SCHEMA_ID_LEN+1];
    CHAR etag[ETAG_LEN+1]; // add by nzy 20151015
    CHAR product_key[PRODUCT_KEY_LEN+1]; // add by nzy 20151016,for support product key to activate device
    DEV_ABI_E ability;
    BOOL bind;
    BOOL sync;
}DEV_DESC_IF_S;

typedef struct {
    INT httploglevel;
    INT silentUgprade;
}GW_RUNTIME_COFIG_IF_S;


// dp prop 
typedef struct {
    INT min;
    INT max;
    SHORT step;
    USHORT scale; // 描述value型DP的10的指数
    INT value;
}DP_PROP_VAL_S;

typedef struct {
    INT cnt;
    CHAR **pp_enum;
    INT value;
}DP_PROP_ENUM_S;

typedef struct {
    INT max_len;
    CHAR *value;
}DP_PROP_STR_S;

typedef struct {
    BOOL value;
}DP_BOOL_S;

typedef struct {
    UINT max_len;
    UINT value;
}DP_PROP_BITMAP;

// 用于描述DP的附加属性，日期类型
typedef struct {
    ULONG value; 
}DP_DATE_S;

typedef union {
    DP_PROP_VAL_S prop_value;
    DP_PROP_ENUM_S prop_enum;
    DP_PROP_STR_S prop_str;
    DP_BOOL_S prop_bool;
    DP_PROP_BITMAP prop_bitmap;
}DP_PROP_VALUE_U;

#pragma pack(1)
typedef struct {
    CHAR *cid; // if(NULL == cid) then then the cid represents gwid
    UINT cnt; // dpid cnt if(0 == cnt) then query all object dp
    BYTE dpid[0]; // dpid
}TY_DP_QUERY_S;
#pragma pack()

typedef BYTE DP_TRIG_T_E;
#define TRIG_PULSE 0
#define TRIG_DIRECT 1

/*
标识某DP状态是否为主动上报
DP SCHEMA中记录值为bool型
*/
typedef BYTE DP_PSV_E;
#define PSV_FALSE 0
#define PSV_TRUE 1
#define PSV_F_ONCE 2

// dp statistics type 
typedef BYTE DP_STAT_TP_T;
#define DST_NONE 0 // no need statistics 
#define DST_INC 1 // dp statistics increase
#define DST_TOTAL 2 // dp statistics total

typedef struct {
    USHORT cir_time; // circle time unit:second
    USHORT cir_rept_item_lmt; // admitting report to cloud items in circle(tiem == time_lmt)
    USHORT cir_rept_items; // report items in one circle
    TIME_S fir_rept_timestamp; // the first report success timestamp.
}DP_REPT_CNTL_S;

typedef struct {
    BYTE dp_id;
    DP_MODE_E mode;
    DP_PSV_E passive;
    DP_TYPE_E type;
    DP_PROP_TP_E prop_tp; // type == obj时有效
    DP_TRIG_T_E trig_t; // 联动触发类型
    DP_STAT_TP_T stat;
}DP_DESC_IF_S;

typedef BYTE DP_PV_STAT_E;
#define INVALID 0 // 数据无效
#define VALID_LC 1 // 本地有效数据
#define VALID_ULING 2 // 数据云端上传中 add by nzy 20150808
#define VALID_CLOUD 3 // 本地有效数据与服务端一致


typedef BYTE HTTP_UP_OP;
#define CTCC_JT_UP 0    //电信集团数据上报
#define CTCC_NJ_UP 1    //电信南京数据上报
#define CUCC_JT_UP 2    //联通集团数据上报  

typedef struct {
    DP_DESC_IF_S dp_desc;
    DP_PROP_VALUE_U prop;
    DP_PV_STAT_E pv_stat;
    DP_REPT_CNTL_S rept_cntl;
}DP_CNTL_S;

typedef struct dev_cntl_n_s {
    struct dev_cntl_n_s *next;
    DEV_DESC_IF_S dev_if;
    BOOL online;
    BOOL preprocess; // 指示该设备是否预处理
    BYTE dp_num;
    DP_CNTL_S dp[0];
}DEV_CNTL_N_S;

typedef struct {
    CHAR token[SEC_KEY_LEN+1];
    CHAR key[SEC_KEY_LEN+1];
    CHAR local_key[LOCAL_KEY_LEN+1];
    CHAR http_url[HTTP_URL_LMT+1];
    CHAR httpsPsk_url[HTTP_URL_LMT+1];
    CHAR mq_url[MQ_URL_LMT+1];
    CHAR mq_tls_url[MQ_URL_LMT+1];
	CHAR time_zone[TIME_ZONE_LEN+1];
    INT uid_cnt;
    CHAR uid_acl[UID_ACL_LMT][UID_LEN+1];
	CHAR region[REGION_LEN + 1];
	CHAR regist_key[REGIST_KEY_LEN + 1];
    CHAR wx_app_id[WXAPPID_LEN+1];
    CHAR ch_name[CH_NAME_LMT+1];
    CHAR compy_name[CH_NAME_LMT+1];
    #if (WPS_CONFIG == 1)  
    INT  mqtt_alive_tim;
    INT  wps_falg;
    #endif

}GW_ACTV_IF_S;

typedef struct {
    CHAR prod_idx[PROD_IDX_LEN+1];
    CHAR mac[MAC_ADDR_LEN+1];
	CHAR psk_key[PSK_KEY_LEN+1];
    CHAR auz_key[AUTH_KEY_LEN+1]; //authorize key
    BOOL prod_test; // production test
}PROD_IF_REC_S;

typedef struct{
    CHAR ch_name[CH_NAME_LMT+1];
    CHAR ch_code[CH_CODE_LMT+1];
    CHAR ch_sn[CTEI_SN_LEN+1];
}CH_CODE_ST;

typedef BYTE GW_STAT_E;
#define UN_INIT 0 // 未初始化，比如生产信息未写入
#define PROD_TEST 1 // 产品产测模式
#define UN_ACTIVE 2 // 未激活
#define ACTIVE_RD 3 // 激活就绪态
#define STAT_WORK 4 // 正常工作态

typedef BYTE GW_WIFI_STAT_E;
#define STAT_LOW_POWER 0    // low power 
#define STAT_UNPROVISION 1 
#define STAT_AP_STA_UNCONN 2 // ap WIFI not config
#define STAT_AP_STA_CFG_UNC 3 // ap WIFI already config,station disconnect
#define STAT_AP_STA_CONN 4
#define STAT_STA_UNCONN 5
#define STAT_STA_CONN 6
/*! 
\typedef typedef VOID (*DEV_DP_QUERY_CB)(IN DP_CMD_TYPE_E cmd_tp, IN CONST CHAR_T *p_dev_id, IN CONST BYTE_T *p_dp_arr, IN CONST UINT_T size);
\brief 子设备dp数据查询回调函数定义。

\param dp_qry [in] 设备数据查询通道,仅支持object类型dp查询
\return VOID
*/
typedef VOID (*DEV_DP_QUERY_CB)(IN CONST TY_DP_QUERY_S *dp_qry);

/**
 * 需在smart_frame_init()之前注册。
 * 固件升级成功后,每次重新上电时，若还未成功上报当前固件版本到云端，则会调用此回调。
 * priv_data  reserved, not used now.
 */
typedef VOID (*UPGRADE_RESET_INFORM_CB)(IN CONST VOID* priv_data);

typedef struct {
    /* To add other cbs */
    DEV_DP_QUERY_CB dev_dp_query_cb; /*!< 设备指令查询接口，可以为NULL*/
    UPGRADE_RESET_INFORM_CB ug_reset_inform_cb; /*!< 重启提示设备升级成功，可以为NULL*/
}TY_IOT_CBS_S;


typedef struct {
    UINT cmd_from;
    UINT seq;
}MCD_S;

// down multi channel data arrive and prevent replay
typedef struct {
    BOOL get_prt; // is get prevent replay time
    TIME_S pr_lmt; // prevent replay second time limit 
    #define MCD_SIZE 5
    MCD_S mcd[MCD_SIZE]; // multi channel data 
    BYTE mcd_cnt;
    BYTE mcd_in;
}PR_MCD_S;

typedef struct {
    GW_DESC_IF_S gw;
    GW_ACTV_IF_S active;
    PROD_IF_REC_S prod_if; //new add by liukang 20180607
    GW_STAT_E stat;
    PR_MCD_S *prmcd;    
    BOOL mq_conn_stat;
    INT dev_num;
    DEV_CNTL_N_S *dev;
    TY_IOT_CBS_S cbs;
}GW_CNTL_S;


/***********************************************************
*************************variable define********************
***********************************************************/
#define BS_VER "5.56"
#define PT_VER "2.2"
#define LAN_PRO_VER "3.3"
#define CD_VER "1.0.0"

//devAttribute(bitmap)
#define DEVATTR_THINGCONFIG        (1 << 0)
#define DEVATRR_DPQUERY            (1 << 1)

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: 
*  Input: 
*  Output: 
*  Return: 
***********************************************************/

#ifdef __cplusplus
}
#endif
#endif

