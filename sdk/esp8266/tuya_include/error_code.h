/***********************************************************
*  File: error_code.h
*  Author: nzy
*  Date: 20150522
***********************************************************/
#ifndef _ERROR_CODE_H
    #define _ERROR_CODE_H

    #include "com_def.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __ERROR_CODE_GLOBALS
    #define __ERROR_CODE_EXT
#else
    #define __ERROR_CODE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define OPRT_OK                                   (0)    // 执行成功
#define OPRT_COM_ERROR                            (1)       // 通用错误
#define OPRT_INVALID_PARM                         (2)       // 无效的入参
#define OPRT_MALLOC_FAILED                        (3)       // 内存分配失败
#define OPRT_INIT_MUTEX_ATTR_FAILED               (4)      // 初始化同步属性失败
#define OPRT_SET_MUTEX_ATTR_FAILED                (5)      // 设置同步属性失败
#define OPRT_DESTROY_MUTEX_ATTR_FAILED            (6)      // 销毁同步属性失败
#define OPRT_INIT_MUTEX_FAILED                    (7)      // 初始化互斥量失败
#define OPRT_MUTEX_LOCK_FAILED                    (8)         // 互斥量加锁失败
#define OPRT_MUTEX_TRYLOCK_FAILED                 (9)      // 互斥量尝试加锁失败
#define OPRT_MUTEX_LOCK_BUSY                      (10)           // 互斥量忙
#define OPRT_MUTEX_UNLOCK_FAILED                  (11)       // 互斥量解锁失败
#define OPRT_MUTEX_RELEASE_FAILED                 (12)      // 互斥量释放失败
#define OPRT_INIT_SEM_FAILED                      (13)     // 初始化信号量失败
#define OPRT_WAIT_SEM_FAILED                      (14) // 等待信号量失败
#define OPRT_POST_SEM_FAILED                      (15) // 释放信号量失败
#define OPRT_THRD_STA_UNVALID                     (16)  // 线程状态非法
#define OPRT_THRD_CR_FAILED                       (17)    // 线程创建失败
#define OPRT_THRD_JOIN_FAILED                     (18)  // 线程JOIN函数调用失败
#define OPRT_THRD_SELF_CAN_NOT_JOIN               (19) // 自身线程不能调用JOIN函数
#define OPRT_TIMERID_EXIST                        (20)     // 定时器ID已存在
#define OPRT_TIMERID_NOT_FOUND                    (21) // 未找到指定定时器ID
#define OPRT_TIMERID_UNVALID                      (22) // 定时器ID非法
#define OPRT_GET_IDLE_TIMERID_ERROR               (23) // 获取空闲定时器ID错误
#define OPRT_MSG_NOT_FOUND                        (24)  // 未找到指定消息
#define OPRT_MSG_LIST_EMPTY                       (25) // 消息链空
#define OPRT_PSM_FLH_RET_ERR                      (26)
#define OPRT_PSM_FLH_TM_ERR                       (27)
#define OPRT_PSM_E_INVAL                          (28)
#define OPRT_PSM_E_IO                             (29)
#define OPRT_PSM_E_EXIST                          (30)
#define OPRT_PSM_E_NOENT                          (31)
#define OPRT_PSM_FAIL                             (32)
#define OPRT_PSM_E_NOSPC                          (33)
#define OPRT_PSM_E_METADATA_CRC                   (34)
#define OPRT_PSM_E_CRC                            (35)
#define OPRT_WIFI_SCAN_FAIL                       (36)
#define OPRT_WF_MAC_SET_FAIL                      (37)
#define OPRT_WF_CONN_FAIL                         (38)
#define OPRT_WF_NW_CFG_FAIL                       (39)
#define OPRT_SOCK_ERR                             (40) // 40
#define OPRT_SET_SOCK_ERR                         (41)
#define OPRT_SOCK_CONN_ERR                        (42)
#define OPRT_CR_MUTEX_ERR                         (43)
#define OPRT_CR_TIMER_ERR                         (44)
#define OPRT_CR_THREAD_ERR                        (45)
#define OPRT_BUF_NOT_ENOUGH                       (46)
#define OPRT_URL_PARAM_OUT_LIMIT                  (47)
#define OPRT_HTTP_OS_ERROR                        (48)
#define OPRT_HTTP_PR_REQ_ERROR                    (49)
#define OPRT_HTTP_SD_REQ_ERROR                    (50)
#define OPRT_HTTP_RD_ERROR                        (51)
#define OPRT_HTTP_AD_HD_ERROR                     (52)
#define OPRT_HTTP_GET_RESP_ERROR                  (53)
#define OPRT_HTTP_AES_INIT_ERR                    (54)
#define OPRT_HTTP_AES_OPEN_ERR                    (55)
#define OPRT_HTTP_AES_SET_KEY_ERR                 (56)
#define OPRT_HTTP_AES_ENCRYPT_ERR                 (57)
#define OPRT_TY_WS_PART_ERR                       (58)
#define OPRT_CR_CJSON_ERR                         (59)
#define OPRT_PSM_SET_ERROR                        (60)
#define OPRT_PSM_GET_ERROR                        (61)
#define OPRT_CJSON_PARSE_ERR                      (62)
#define OPRT_CJSON_GET_ERR                        (63)
#define OPRT_CR_HTTP_URL_H_ERR                    (64)
#define OPRT_HTTPS_HANDLE_FAIL                    (65)
#define OPRT_HTTPS_RESP_UNVALID                   (66)
#define OPRT_MEM_PARTITION_EMPTY                  (67)
#define OPRT_MEM_PARTITION_FULL                   (68)
#define OPRT_MEM_PARTITION_NOT_FOUND              (69)
#define OPRT_CR_QUE_ERR                           (70)
#define OPRT_SND_QUE_ERR                          (71)
#define OPRT_NOT_FOUND_DEV                        (72)
#define OPRT_NOT_FOUND_DEV_DP                     (73)
#define OPRT_DP_ATTR_ILLEGAL                      (74)
#define OPRT_DP_TYPE_PROP_ILLEGAL                 (75) // dp type property illegal
#define OPRT_DP_REPORT_CLOUD_ERR                  (76)
#define OPRT_NO_NEED_SET_PRODINFO                 (77)
#define OPRT_NW_INVALID                           (38)
#define OPRT_SELECT_ERR                           (79)
#define OPRT_SELECT_TM                            (80) // select timeout
#define OPRT_SEND_ERR                             (81)
#define OPRT_DEV_NOT_BIND                         (82)
#define OPRT_FW_UG_FAILED                         (83)
#define OPRT_VER_FMT_ERR                          (84)
#define OPRT_FW_NOT_EXIST                         (85)
#define OPRT_SEM_CR_ERR                           (86)
#define OPRT_SELECT_TIMEOUT                       (87)
#define OPRT_GW_MQ_OFFLILNE                       (88)
#define OPRT_NOT_SPT_CLX_DP                       (89)
#define OPRT_RECV_ERR                             (90)
#define OPRT_UG_PKG_NOT_ENOUGH                    (91)
#define OPRT_SCMA_INVALID                         (92)
#define OPRT_PRODECT_KEY_NULL                     (93)
#define OPRT_DEVICE_VER_NULL                      (94)
#define OPRT_MSG_OUT_OF_LMT                       (95)
#define OPRT_NOT_FOUND_AUTH_SSID                  (96)
#define OPRT_SOCKET_FAULT                         (97)
#define OPRT_MQ_PUBLISH_TIMEOUT                   (98)
#define OPRT_GW_NOT_EXIST                         (99)
#define OPRT_GW_SCHEMA_SIZE_LMT_OUT               (100)
#define OPRT_DEV_DP_CNT_INVALID                   (101)
#define OPRT_TOKEN_OVERTIME                       (102)
#define OPRT_CR_TIMER_FAILED                      (103)
#define OPRT_PUB_NO_PERMISSION                    (104)
#define OPRT_CRC32_FAILED                         (105)
#define OPRT_OPEN_FILE_FAILED                     (106)
#define OPRT_DONOT_FOUND_MODULE                   (107)
#define OPRT_PRINT_LOG_LEVEL_HIGHER               (108)
#define OPRT_FORMAT_STRING_FAILED                 (109)
#define OPRT_STRING_TOO_LONG                      (110)
#define OPRT_CMD_OUT_OF_TIME_NOT_EXEC             (111)
#define OPRT_CMD_NOT_EXEC                         (112)
#define OPRT_REPORT_OUTOF_MINUTE_RATE             (113)
#define OPRT_MCU_UART_SEND_ERROR                  (114)
#define OPRT_RECV_DA_NOT_ENOUGH                   (115)
#define OPRT_UG_FILE_HD_INVALID                   (116)
#define OPRT_UG_FILE_CRC_FAIL                     (117)
#define OPRT_HTTPS_NO_SUPPORT_RANGE               (118)
#define OPRT_GW_FW_SIZE_OVER_MAX_SIZE             (119)
#define OPRT_HMAC_DISMATCH                        (120)
#define OPRT_NOT_FOUND                            (121)
#define OPRT_UG_FLASH_BIN_CHECKSUM_ERR            (122)
#define OPRT_LOG_SEQ_LOCKED                       (123)
#define OPRT_LOG_SEQ_DONT_LOCKED                  (124)
#define OPRT_HTTP_NO_ENCRPT                       (125)
#define OPRT_HTTP_GW_RESET                        (126)
#define OPRT_HTTP_GW_FAC_RESET                    (127)
#define OPRT_FLASH_NOT_ENOUGH_PAGE				  (128)




#ifdef __cplusplus
}
#endif
#endif

