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

#define HIGH_WATER_MARK_DEBUG           0


#if (ENABLE_8266TLS == 1)
#define TLS_MODE                    TLS_TUYA_PSK_ONLY
#else
#define TLS_MODE                    TLS_DISABLE
#endif


#define OPERATING_SYSTEM            SYSTEM_ESP8266_1M



typedef int BOOL_T;
typedef void *PVOID_T;
typedef signed int INT_T;
typedef unsigned int TIME_T;
typedef unsigned int UINT_T;
typedef unsigned char BYTE_T;
typedef BYTE_T *PBYTE_T;
typedef char CHAR_T;
typedef CHAR_T *PCHAR_T;
typedef signed char SCHAR_T;
typedef unsigned char UCHAR_T;
typedef short SHORT_T;
typedef unsigned short USHORT_T;
typedef short *PSHORT_T;
typedef long LONG_T;
typedef unsigned long ULONG_T;
typedef unsigned short WORD_T;
typedef WORD_T *PWORD_T;
typedef unsigned int DWORD_T;
typedef DWORD_T *PDWORD_T;

typedef unsigned int *PUINT_T;
typedef long long int INT64_T;
typedef INT64_T *PINT64_T;
typedef unsigned long long int UINT64_T;
typedef UINT64_T *PUINT64_T;
typedef short INT16_T;
typedef INT16_T *PINT16_T;
typedef unsigned short UINT16_T;
typedef UINT16_T *PUINT16_T;
typedef char INT8_T;
typedef INT8_T *PINT8_T;
typedef unsigned char UINT8_T;
typedef UINT8_T *PUINT8_T;
typedef VOID VOID_T;


#endif // TUYA_CLOUD_TYPES_H
