/***********************************************************
*  File: uni_system.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_SYSTEM_H
#define _UNI_SYSTEM_H

    #include "sys_adapter.h"
    
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_SYSTEM_GLOBAL
    #define _UNI_SYSTEM_EXT
#else
    #define _UNI_SYSTEM_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define STRCASE_CMP

struct ArrPosition
{
    unsigned short pStart;
    unsigned short pEnd;
};

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/\
_UNI_SYSTEM_EXT \
VOID GetSystemTime(OUT TIME_S *pSecTime,OUT TIME_MS *pMsTime);

_UNI_SYSTEM_EXT \
VOID SystemSleep(IN CONST TIME_MS msTime);

/***********************************************************
*  Function: SystemReset 系统重启
*  Input: msTime
*  Output: none 
*  Return: none
*  Date: 120427
***********************************************************/
_UNI_SYSTEM_EXT \
VOID SystemReset(VOID);

#ifndef STRCASE_CMP
_UNI_SYSTEM_EXT \
int strcasecmp(const char *s1, const char *s2);
#endif

_UNI_SYSTEM_EXT \
int strncasecmp(const char *s1, const char *s2, size_t n);

_UNI_SYSTEM_EXT \
unsigned char asc2hex(char asccode);

_UNI_SYSTEM_EXT \
void ascs2hex(unsigned char *hex,unsigned char *ascs,int srclen);

_UNI_SYSTEM_EXT \
void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen);

_UNI_SYSTEM_EXT \
VOID Int2HexStr(UINT val, CHAR *hexstr);


/***********************************************************
*  Function: GetSystemTime 获取系统时间
*  Input: none
*  Output: pSecTime->s 
*          pMsTime->ms
*  Return: none
*  Date: 120427
***********************************************************/
_UNI_SYSTEM_EXT \
VOID GetSystemTime(OUT TIME_S *pSecTime,OUT TIME_MS *pMsTime);

/***********************************************************
*  Function: SystemIsrStatus->direct system interrupt status
*  Input: none
*  Output: none 
*  Return: BOOL
***********************************************************/
_UNI_SYSTEM_EXT \
BOOL SystemIsrStatus(VOID);

/***********************************************************
*  Function: tuya_get_random_data
*  Input: dst size
*  Output: none
*  Return: VOID
***********************************************************/
_UNI_SYSTEM_EXT \
OPERATE_RET tuya_get_random_data(UCHAR_T* dst, INT_T size, INT_T range);

_UNI_SYSTEM_EXT \
OPERATE_RET uni_system_init(VOID);

_UNI_SYSTEM_EXT \
INT Add_Pkcs(char *p, int len);

_UNI_SYSTEM_EXT \
int decodeArr(CONST CHAR *str, struct ArrPosition arrPosition[],int arr_pos_num);

_UNI_SYSTEM_EXT \
OPERATE_RET uni_system_set_lowpower_mode(VOID);

_UNI_SYSTEM_EXT \
BOOL uni_system_is_lowpower_mode(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

