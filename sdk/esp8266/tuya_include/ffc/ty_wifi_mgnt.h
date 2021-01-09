/***********************************************************
*  File: wifi_hwl.h
*  Author: nzy
*  Date: 20170914
***********************************************************/
#ifndef _WIFI_MGNT_H
#define _WIFI_MGNT_H

#include "tuya_cloud_types.h"
#include "sys_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIFI_MGNT_GLOBAL
    #define  _WIFI_MGNT_EXT
#else
    #define  _WIFI_MGNT_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define TAG_SSID_NUMBER 0
//#define TAG_SUPPORT_RATES_NUMBER 1
#define TAG_PAYLOAD_NUMBER 221

#define PROBE_REQUEST_TYPE_SUBTYPE 0x0040
#define PROBE_REQSPONSE_TYPE_SUBTYPE 0x0050

#define PROBE_REQUEST_DURATION_ID 0x0
#define PROBE_RESPONSET_DURATION_ID 0x0


#define PROBE_REQUEST_PAYLOAD_LEN_MAX 255

#define BROADCAST_MAC_ADDR 0xFFFFFFFF


#define PROBE_SSID "tuya_smart"

#pragma pack(1)
typedef struct
{
    USHORT_T type_and_subtype;
    USHORT_T duration_id;
    UCHAR_T addr1[6];
    UCHAR_T addr2[6];
    UCHAR_T addr3[6];
    USHORT_T seq_ctrl;
} PROBE_REQUEST_PACKAGE_HEAD_S;

typedef struct
{
    UCHAR_T index;
    UCHAR_T len;
    UCHAR_T ptr[0];
} BEACON_TAG_DATA_UNIT_S;
#pragma pack()

typedef struct
{
    PROBE_REQUEST_PACKAGE_HEAD_S pack_head;
    BEACON_TAG_DATA_UNIT_S tag_ssid;
} PROBE_REQUEST_FIX_S;

typedef struct 
{
    BYTE_T mac[6]; /* mac address */
}NW_MAC_S;

/***********************************************************
*************************variable define********************
***********************************************************/
typedef void (*ty_wf_recv_mgnt_cb)(UCHAR_T *buf, INT_T buf_len, NW_MAC_S *srcmac,NW_MAC_S *dstmac);


/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: ty_wf_send_probe_request_mgnt
*  Input: in_buf in_len srcmac dstmac
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
_WIFI_MGNT_EXT \
OPERATE_RET ty_wf_send_probe_request_mgnt(IN CONST UCHAR_T *in_buf, IN CONST UINT_T in_len,NW_MAC_S *srcmac,NW_MAC_S *dstmac);

/***********************************************************
*  Function: ty_wf_register_recv_mgnt_callback
*  Input: enbale recv_cb
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
_WIFI_MGNT_EXT \
OPERATE_RET ty_wf_register_recv_mgnt_callback(BOOL_T enbale,ty_wf_recv_mgnt_cb recv_cb);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



