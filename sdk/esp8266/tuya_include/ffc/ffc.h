#ifndef __TUYA_FFC_H__
#define __TUYA_FFC_H__

//! FFC ->  flexible fast control, alias -> tuya wifi mesh

#ifdef __cplusplus
extern "C" {
#endif

#include "ffc_app.h"

#define FFC_MAGIC                  "FFC"
#define FFC_VER1                    0x01
#define FFC_VER2                    0x02
#define FFC_BINDING_SEQ             0x01
#define FFC_BINDING_HOP             0x01
#define FFC_BEACON_HOP              0x01
#define FFC_CONTROL_HOP             0x03
#define FFC_MAX_DATA_LEN            0x100

#define FFC_KEY_XOR                 0x01
#define FFC_KEY_MD5_XOR             0x02
#define FFC_AUTH_CRC                0x03    
#define FFC_AUTH_MD5                0x04

#define FFC_KEY_TYPE                FFC_KEY_MD5_XOR
#define FFC_AUTH_TYPE               FFC_AUTH_CRC

#define FFC_AUTH_SUCCEED            0x01
#define FFC_AUTH_EXIST              0x02
#define FFC_AUTH_FAILED             0x03

#define FFC_CONTROL_CMD_COUNT       0x08
#define FFC_BEACON_CMD_COUNT        0x03
#define FFC_BINDING_CMD_COUNT       0x03

#define FFC_GROUP_NUM               0x04

#define FFC_BINDING_KEY             "6xg0Qo9Cxi85oooA"

typedef enum {
    FFC_BIND_PROBE_CMD  = 0x01,
    //! slaver cmd
    FFC_BIND_REQ_CMD,
    FFC_JOIN_REQ_CMD,
    FFC_AUTH_REQ_CMD,
    //! master cmd
    FFC_BIND_RSP_CMD,
    FFC_JOIN_RSP_CMD,
    FFC_AUTH_RSP_CMD,
    FFC_AUTH_FAST_CMD,
    FFC_BEACON_CMD,
    FFC_CTRL_CMD,
    FFC_MAX_CMD,
} ffc_cmd_t;

#pragma pack(1)
typedef struct {
    uint8_t             magic[3];        
    uint8_t             version;        
    uint8_t             hop;
    uint8_t             cmd;
    uint16_t            sequence;
    uint16_t            data_len;
    uint8_t             data[0];
} ffc_head_t;

typedef struct {
    uint32_t            hash;          
} ffc_tail_t;

typedef struct {
    uint16_t            ctrl_seq;
    uint8_t             mac[6];
} ffc_item_t;

typedef struct {
    uint8_t             uuid_md5[16];
    uint8_t             num;
    uint8_t             item[0];
} ffc_bind_req_t;

typedef struct {
    uint16_t            ctrl_seq;
    uint8_t             group;
} ffc_bind_probe_t;

typedef struct {
    uint8_t             channel;
    uint8_t             group;
} ffc_beacon_t;

typedef struct {
    uint16_t            ctrl_seq;
    uint8_t             random_a[16];
} ffc_bind_rsp_t;

typedef struct {
    uint8_t             random_b[16];
}  ffc_join_req_t;

typedef struct {
    uint8_t             type;           
    uint8_t             ctrl_key[16];
} ffc_join_rsp_t;

typedef struct {
    uint8_t             type;           //! md5 or crc
    uint8_t             text[16];       //! uuid + ra + rb + key -> md5 or crc
} ffc_auth_req_t;

typedef struct {
    uint8_t             result;
    uint8_t             group;
} ffc_auth_rsp_t;

typedef struct {
    uint16_t            ctrl_seq;      
    uint8_t             group;   
    uint8_t             data[0];
} ffc_ctrl_t;
#pragma pack()


extern void ffc_frame_printf(uint8_t *frame);
extern int ffc_frame_verify(uint8_t *frame, uint16_t frame_len);
extern uint8_t *ffc_frame_send_buffer_get(void);
extern void ffc_frame_head_set(uint8_t *frame, uint8_t hip, uint8_t cmd, uint16_t sequence);
extern void ffc_frame_version_set(uint8_t *frame, uint8_t version);
extern void ffc_frame_data_set(uint8_t *frame, uint8_t *data, uint16_t data_len);
extern void ffc_frame_hash_set(uint8_t *frame);
extern uint16_t ffc_frame_current_len_get(uint8_t *frame);
extern void ffc_frame_data_len_set(ffc_head_t *head, uint16_t data_len);
extern void ffc_frame_seq_set(uint8_t *frame,  uint16_t sequence);


extern void ffc_seed(uint32_t seed);
extern uint32_t ffc_random(void);
extern void ffc_random_data(uint8_t *data, uint16_t data_len);
extern uint32_t ffc_random_range(uint32_t min,  uint32_t max);


#define FFC_MAX_SLAVER_COUNT                    31
#define FFC_CHANGE_CHANNEL_TIME                 120

typedef enum {
    FFC_MASTER_GROUP_CMD,
} ffc_master_cmd_t;

typedef struct {
    uint8_t             state; 
    uint8_t             mac[6];
    uint8_t             uuid_md5[16];
    uint8_t             random_a[16];
    uint8_t             random_b[16];
} ffc_slaver_list_t;

typedef struct {
    uint8_t             status;
    uint8_t             channel;
    uint16_t            ctrl_seq;
    uint16_t            packet_seq;
} ffc_slaver_group_t;


typedef struct {
    ffc_slaver_list_t   *slaver;
    ffc_slaver_list_t   slaver_list[FFC_MAX_SLAVER_COUNT];
    ffc_cfg_t           *cfg;
    ffc_ops_t           *ops;
    ffc_cb_t            *cb;
    uint8_t             scan;
    uint8_t             beacon;
    uint8_t             channel_index;
    uint8_t             slaver_count;
    uint8_t             bind_key[16];   
    uint8_t             ctrl_key[16];
    TIMER_ID            binding_timer;
    TIMER_ID            channel_timer;
    uint8_t             group_index;
    ffc_slaver_group_t  groups[FFC_GROUP_NUM];
    uint16_t            packet_ack;
} ffc_master_t;

extern ffc_master_t *ffc_master_malloc(void);
extern int ffc_master_init(ffc_master_t *master, ffc_cfg_t *cfg, ffc_ops_t *ops, ffc_cb_t *cb);
extern int ffc_master_control_cmd_send(ffc_master_t *master, uint8_t *data, uint16_t data_len);
extern int ffc_master_binding_slaver(ffc_master_t *master, uint8_t binding_time);
extern int ffc_master_unbinding_slaver(ffc_master_t *master, uint8_t binding_time);
extern int ffc_master_group_set(ffc_master_t *master, uint8_t group);
extern int ffc_master_frame_parser(ffc_master_t *master, 
                                   uint8_t      *frame, 
                                   uint32_t     frame_len, 
                                   uint8_t      src_mac[6], 
                                   uint8_t      dst_mac[6]);

#define FFC_MAX_MASTER_COUNT                    5
#define FFC_SLAVER_MAX_FORWARD_BEACON_MSG       5
#define FFC_SLAVER_MAX_PKT_SEQ_NUM              10

#define FFC_FORWARD_BEACON_RANDOM_ENABLE        0   
#define FFC_SLAVER_BEACON_RANDOM_MIN_MS         10
#define FFC_SLAVER_BEACON_RANDOM_MAX_MS         30

#define FFC_SLAVER_BINDING_MIN_MS               80
#define FFC_SLAVER_BINDING_MAX_MS               110

#define FFC_FORWARD_CMD_RANDOM_ENABLE           1
#define FFC_SLAVER_MAX_FORWARD_CMD_MSG          3
#define FFC_SLAVER_CMD_RANDOM_MIN_MS            10
#define FFC_SLAVER_CMD_RANDOM_MAX_MS            50


typedef struct {
    uint16_t            data[FFC_SLAVER_MAX_PKT_SEQ_NUM];
    uint16_t            index;
} ffc_seq_t;;

typedef struct {
    uint8_t             state;
    uint16_t            ctrl_seq;
} ffc_master_group_t;

typedef struct {
    ffc_seq_t           cmd_seq;
    uint8_t             mac[6];
    uint8_t             ctrl_key[16];
    uint32_t            capacity;
    ffc_master_group_t  groups[FFC_GROUP_NUM];
} ffc_master_list_t;

typedef struct {
    uint8_t             mac[6];
    uint8_t             random_a[16];
    uint8_t             random_b[16];
    uint8_t             ctrl_key[16];
    uint8_t             state;
    uint16_t            ctrl_seq;
} ffc_master_item_t;

typedef struct {
    uint8_t             status;
    ffc_master_item_t   master;
    ffc_master_list_t   master_list[FFC_MAX_MASTER_COUNT];
    ffc_cfg_t           *cfg;
    ffc_ops_t           *ops;
    ffc_cb_t            *cb;
    uint8_t             uuid_md5[16];
    uint8_t             bind_key[16];   
    uint8_t             master_index;
    uint8_t             master_num;
    uint8_t             master_ver;
    uint8_t             group_index;
    uint8_t             master_lock;
    uint32_t            master_capacity;
    TIMER_ID            binding_timer;
    TIMER_ID            packet_timer;
    uint8_t             packet_reset;
    uint16_t            packet_seq;
    uint8_t             packet_cmd;
    ffc_seq_t           beacon_seq;
#if FFC_FORWARD_CMD_RANDOM_ENABLE || FFC_FORWARD_BEACON_RANDOM_ENABLE
    void                *forward_queue;
    TIMER_ID            forward_timer;
#endif
} ffc_slaver_t;

extern ffc_slaver_t *ffc_slaver_malloc(void);
extern int ffc_slaver_binding_master(ffc_slaver_t *slaver, uint8_t binding_time);
extern int ffc_slaver_init(ffc_slaver_t *slaver, ffc_cfg_t *cfg, ffc_ops_t *ops, ffc_cb_t *cb);
extern int ffc_slaver_frame_parser(ffc_slaver_t *slaver, 
                                  uint8_t     *frame, 
                                  uint32_t     frame_len, 
                                  uint8_t      src_mac[6], 
                                  uint8_t      dst_mac[6]);
#ifdef __cplusplus
}
#endif

#endif  
