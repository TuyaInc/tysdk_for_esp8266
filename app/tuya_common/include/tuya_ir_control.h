/*!
\file tuya_ir_control.h
Copyright(C),2017, 涂鸦科技 www.tuya.comm
*/
#ifndef _TUYA_IR_CONTROL_H
#define _TUYA_IR_CONTROL_H

#include "com_def.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TY_SYSTEM_DP_IR_CTL     	201
#define TY_SYSTEM_DP_STUDY_REPORT 	202

typedef enum {
    TY_IR_STUDY_START = 0,     
    TY_IR_STUDY_EXIT = 1,
    
}TY_IR_STUDY_CTL_E;

typedef struct {
    // UINT      *code;
    USHORT    *code;
    USHORT    code_len;
    BYTE      send_count;     //repeat send count, don't need delay
    UINT      feq;
    USHORT    delay;          //ms
    
}TY_IR_CODE_S;

typedef VOID (*TY_IR_STUDY_CTL_CB)(TY_IR_STUDY_CTL_E mode);
typedef VOID (*TY_IR_SEND_CB)(TY_IR_CODE_S *ir_code, UCHAR code_num);

typedef struct {
    TY_IR_STUDY_CTL_CB  study_ctl_cb;
    TY_IR_SEND_CB       send_cb;
    
}TY_IR_CONTROL_CBS_S;

OPERATE_RET tuya_ir_control_register(IN CONST TY_IR_CONTROL_CBS_S *p_ir_cbs);
OPERATE_RET tuya_ir_study_code_report(IN CONST USHORT *p_code, IN CONST UINT code_len);
OPERATE_RET tuya_ir_study_error_report();
OPERATE_RET tuya_ir_code_free(IN       TY_IR_CODE_S *ir_code, IN UCHAR code_num);
OPERATE_RET tuya_ir_study_stat_report(IN TY_IR_STUDY_CTL_E mode);

#ifdef __cplusplus
}
#endif

#endif  

