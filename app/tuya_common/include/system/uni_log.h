/***********************************************************
*  File: uni_log.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_LOG_H
#define _UNI_LOG_H


#include "com_def.h"
#include "error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_LOG_GLOBAL
    #define _UNI_LOG_EXT
#else
    #define _UNI_LOG_EXT extern
#endif





#define HTTP_LOG_TIMER  (15*60*1000)

// 日志输出等级
typedef INT LOG_LEVEL;
#define LOG_LEVEL_ERR       0  // 错误信息，程序正常运行不应发生的信息
#define LOG_LEVEL_NOTICE    2  // 需要注意的信息
#define LOG_LEVEL_DEBUG     4  // 程序运行调试信息

typedef OPERATE_RET (*http_log_func)(IN CONST CHAR *log, IN CONST INT len);
OPERATE_RET CreateLogManageAndInit(http_log_func log_func);
OPERATE_RET SetLogManageLogLevel(IN CONST LOG_LEVEL curLogLevel);
OPERATE_RET ReleaseLogManage(VOID);
OPERATE_RET PrintHttpLog(IN CONST LOG_LEVEL logLevel, IN CONST INT line, IN CONST CHAR *pFunc, IN CONST INT value, IN CONST CHAR * log);

#define HTTP_LOG(value,log)



#if DEBUG
#define PR_DEBUG(_fmt_, ...) \
        os_printf("[dbg]%s:%d "_fmt_"\n\r", __FILE__,__LINE__,##__VA_ARGS__)
#define PR_DEBUG_RAW(_fmt_, ...) \
        os_printf(_fmt_,##__VA_ARGS__)
#else
#define PR_DEBUG(...)
#define PR_DEBUG_RAW(_fmt_, ...)
#endif

#define PR_NOTICE(_fmt_, ...) \
        os_printf("[notice]%s:%d "_fmt_"\n\r", __FILE__,__LINE__,##__VA_ARGS__)
#define PR_ERR(_fmt_, ...) \
        os_printf("[err]%s:%d "_fmt_"\n\r", __FILE__,__LINE__,##__VA_ARGS__)


#define PrintLogRaw os_printf

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


