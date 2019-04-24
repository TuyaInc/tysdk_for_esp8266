/*
tuya_cloud_types.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef TUYA_CLOUD_TYPES_H
#define TUYA_CLOUD_TYPES_H

#include "com_def.h"

#define TLS_DISABLE                     0       /* disable tls function */
#define TLS_TUYA_PSK_ONLY               2       /* only enable ciper 0xAE */
#define TLS_TUYA_ECC_PSK                3       /* enable ciper 0xAE && 0xC027 */
#define TLS_TUYA_ECC_ONLY               4       /* only enable ciper 0xC027 */

#define SYSTEM_SMALL_MEMORY_BEGIN       0       /*small memory systems begin */

#define SYSTEM_REALTEK8710_1M           1
#define SYSTEM_REALTEK8710_2M           2
#define SYSTEM_ESP8266_1M               3


#define SYSTEM_SMALL_MEMORY_END         99      /*small memory systems end */

#define SYSTEM_LINUX                    100
#define SYSTEM_LITEOS                   120



#if (ENABLE_8266TLS == 1)
#define TLS_MODE                    TLS_TUYA_PSK_ONLY
#else
#define TLS_MODE                    TLS_DISABLE
#endif


#define OPERATING_SYSTEM            SYSTEM_ESP8266_1M



typedef int BOOL_T;
typedef void *PVOID_T;
typedef signed int INT_T;
typedef unsigned int UINT_T;
typedef unsigned char BYTE_T;
typedef char CHAR_T;
typedef unsigned char UCHAR_T;



#endif // TUYA_CLOUD_TYPES_H
