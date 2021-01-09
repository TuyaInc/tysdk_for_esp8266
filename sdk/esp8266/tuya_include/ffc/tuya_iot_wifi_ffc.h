/*!
\file tuya_iot_wifi_ffc.h
Copyright(C),2017, 涂鸦科技 www.tuya.comm
*/

#ifndef _TUYA_IOT_WIFI_FFC_H
#define _TUYA_IOT_WIFI_FFC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"

#define FFC_MASTER                    0x01
#define FFC_SLAVER                    0x02

typedef enum {
    FFC_INIT_STATE = 0,
    FFC_CONTROL_STATE,
    FFC_BINDING_BEGIN_STATE,
    FFC_BINDING_SUCCESS_STATE,
    FFC_BINDING_FINSH_STATE,
} FFC_CB_STATE_E;

typedef INT_T (*FFC_STATUS_CB)(FFC_CB_STATE_E state);
typedef INT_T (*FFC_RECV_CB)(BYTE_T *data_cmd, SHORT_T data_len);

OPERATE_RET tuya_iot_wifi_ffc_init(BYTE_T role, FFC_STATUS_CB status_cb, FFC_RECV_CB recv_cb);
OPERATE_RET tuya_iot_wifi_ffc_bind(SHORT_T timeout_s);
OPERATE_RET tuya_iot_wifi_ffc_unbind(SHORT_T timeout_s);
OPERATE_RET tuya_iot_wifi_ffc_send(BYTE_T *data, SHORT_T len);
OPERATE_RET tuya_iot_wifi_ffc_control(INT_T flag, VOID_T *data);


#ifdef __cplusplus
}
#endif

#endif  /*_TUYA_IOT_WIFI_FFC_H*/

