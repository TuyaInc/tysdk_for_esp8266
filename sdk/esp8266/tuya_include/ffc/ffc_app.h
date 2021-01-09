#ifndef __TUYA_FFC_APP_H__
#define __TUYA_FFC_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ffc_env.h"
#include "tuya_iot_wifi_ffc.h"

typedef enum {
    FFC_GROUP_0 = 0x00,
    FFC_GROUP_1,
    FFC_GROUP_2,
    FFC_GROUP_3,
} ffc_group_t;

typedef enum {
    FFC_CHANNEL_SCAN_FLAG = 0x01,
    FFC_GROUP_SET_FLAG    = 0x02,
} ffc_flag_t;

typedef FFC_CB_STATE_E ffc_cb_state_t;

typedef struct {
    uint8_t    *uuid;
    uint8_t     auzkey[32 + 1];
    uint8_t     mac[6];
    uint8_t     channel[13];         
    uint8_t     channel_count;     
    uint32_t    flag;
} ffc_cfg_t;

typedef struct {
    int     (*channel_set)(uint8_t new_channel);
    uint8_t (*channel_get)(void);
    int     (*send_frame)(uint8_t *frame, uint16_t frame_len, uint8_t src_mac[6], uint8_t dst_mac[6]);
} ffc_ops_t;

typedef struct {
    FFC_STATUS_CB status_cb;
    FFC_RECV_CB   recv_cb;
} ffc_cb_t;


#define ffc_init        tuya_iot_wifi_ffc_init
#define ffc_bind        tuya_iot_wifi_ffc_bind
#define ffc_unbind      tuya_iot_wifi_ffc_unbind
#define ffc_send        tuya_iot_wifi_ffc_send
#define ffc_control     tuya_iot_wifi_ffc_control


#ifdef __cplusplus
}
#endif

#endif  /*__TUYA_FFC_APP_H__ */

