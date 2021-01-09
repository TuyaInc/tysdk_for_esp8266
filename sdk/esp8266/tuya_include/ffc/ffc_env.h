#ifndef __TUYA_FFC_ENV_H__
#define __TUYA_FFC_ENV_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FFC_OLD_PLATFORM            0
#define FFC_NEW_PLATFORM            1
#define FFC_PLATFORM                FFC_OLD_PLATFORM

#if (FFC_NEW_PLATFORM == FFC_PLATFORM)
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "adapter_platform.h"
#include "uni_log.h"
#include "uni_time_queue.h"
#include "uni_md5.h"
#include "tuya_ws_db.h"
#include "aes_inf.h"

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
//typedef unsigned int uint32_t;

#elif (FFC_OLD_PLATFORM == FFC_PLATFORM)
#include "tuya_cloud_types.h"
#include "sys_adapter.h"
#include "uni_md5.h"
#include "flash_api.h"
#include "cJSON.h"
#include "uni_base64.h"
#include "aes_inf.h"
#include "gw_intf.h"
#include "system/sys_timer.h"
#include "ty_wifi_mgnt.h"
#include "queue.h"
#include "system/uni_semaphore.h"
#endif

OPERATE_RET ffc_env_init(void);
OPERATE_RET ffc_uuid_get(INOUT CHAR_T **uuid);

#if (FFC_OLD_PLATFORM == FFC_PLATFORM)
#define FFC_MODULE      "ffc_mod" 
#define FFC_PARTITION   "ffc_part" 

typedef BYTE_T VAR_TP_T;
#define VT_CHAR     0
#define VT_BYTE     1
#define VT_SHORT    2
#define VT_USHORT   3
#define VT_INT      4
#define VT_BOOL     5
#define VT_STRING   6
#define VT_RAW      7

typedef struct {
    CHAR_T      *key;
    VAR_TP_T     vt;
    VOID        *val;
    USHORT_T    len;
} TY_DB_RW_S;

/* tuya sdk definition of wifi function type */
typedef enum {
    WF_STATION = 0,                 // station type
    WF_AP,                          // ap type
} WF_IF_E;

/* tuya sdk definition of wifi work mode */
typedef enum {
    WWM_LOWPOWER = 0,               // wifi work in lowpower mode
    WWM_SNIFFER,                    // wifi work in sniffer mode
    WWM_STATION,                    // wifi work in station mode
    WWM_SOFTAP,                     // wifi work in ap mode
    WWM_STATIONAP,                  // wifi work in station+ap mode
} WF_WK_MD_E;

#define WIFI_SSID_LEN 32            // tuya sdk definition WIFI SSID MAX LEN
#define WIFI_PASSWD_LEN 64          // tuya sdk definition WIFI PASSWD MAX LEN
typedef struct {
    BYTE_T channel;                 // AP channel
    SCHAR_T rssi;                   // AP rssi
    BYTE_T bssid[6];                // AP bssid
    BYTE_T ssid[WIFI_SSID_LEN+1];   // AP ssid array
    BYTE_T s_len;                   // AP ssid len
} AP_IF_S;

#define SCAN_MAX_AP 64
typedef struct {
    AP_IF_S *ap_if;
    BYTE_T ap_if_nums;
    BYTE_T ap_if_count;
    SEM_HANDLE sem_handle;
} SACN_AP_RESULT_S;

OPERATE_RET wf_set_cur_channel(IN CONST BYTE_T chan);
OPERATE_RET wf_get_cur_channel(OUT BYTE_T *chan);
OPERATE_RET wf_get_mac(IN CONST WF_IF_E wf, INOUT NW_MAC_S *mac);
OPERATE_RET wf_wk_mode_set(IN CONST WF_WK_MD_E mode);
OPERATE_RET wf_wk_mode_get(OUT WF_WK_MD_E *mode);
OPERATE_RET wf_all_ap_scan(OUT AP_IF_S **ap_ary, OUT UINT_T *num);
OPERATE_RET wf_release_ap(IN AP_IF_S *ap);

OPERATE_RET wd_utils_serialize_save(IN CONST CHAR_T *key, IN CONST TY_DB_RW_S *rw,IN CONST UINT_T rw_cnt);
OPERATE_RET wd_utils_serialize_restore(IN CONST CHAR_T *key,INOUT TY_DB_RW_S *rw,IN CONST UINT_T rw_cnt);
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__TUYA_FFC_APP_H__ */

