/***********************************************************
*  File: uni_log.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_LOG_H
#define _UNI_LOG_H


#include "com_def.h"
#include "error_code.h"
#include "sys_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_LOG_GLOBAL
    #define _UNI_LOG_EXT
#else
    #define _UNI_LOG_EXT extern
#endif


#if DEBUG
#define PR_DEBUG(_fmt_, ...) \
        os_printf("[D]%s:%d "_fmt_"\n", __FILE__,__LINE__,##__VA_ARGS__)
#define PR_DEBUG_RAW(_fmt_, ...) \
        os_printf(_fmt_,##__VA_ARGS__)
#else
#define PR_DEBUG(...)
#define PR_DEBUG_RAW(_fmt_, ...)
#endif

#define PR_NOTICE(_fmt_, ...) \
        os_printf("[N]%s:%d "_fmt_"\n", __FILE__,__LINE__,##__VA_ARGS__)
#define PR_ERR(_fmt_, ...) \
        os_printf("[ERR]%s:%d "_fmt_"\n", __FILE__,__LINE__,##__VA_ARGS__)


#define PrintLogRaw os_printf

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


