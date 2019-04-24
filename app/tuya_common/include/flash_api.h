/***********************************************************
*  File: flash_api.h 
*  Author: nzy
*  Date: 20150821
***********************************************************/
#ifndef _FLASH_API_H
    #define _FLASH_API_H
    
    #include "sys_adapter.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __FLASH_API_GLOBALS
    #define __FLASH_API_EXT
#else
    #define __FLASH_API_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define MAP_FLH_STR_START 0x7b000 // for psm storage
#define MAP_FLH_STR_SIZE 0x5000
#define TRSP_FLH_STR_START 0xFC000 // for transparent storage
#define TRSP_FLH_STR_SIZE 0x4000
#define FIXED_PROD_INFO 0xFB000 // fixed address production info
#define FIXED_PI_SZ 0x1000

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: flash_api_init
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET flash_api_init(VOID);

/***********************************************************
*  Function: msf_register_module
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_register_module(IN CONST CHAR *module_name,IN CONST CHAR *partition_key);

/***********************************************************
*  Function: msf_open
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
*  note:必须与msf_close调用成对出现
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_open(IN CONST CHAR *module_name);

/***********************************************************
*  Function: msf_set
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
*  note:必须在msf_open msf_close之间调用
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_set(IN CONST CHAR *variable, IN CONST CHAR *value);

/***********************************************************
*  Function: msf_get
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
*  note:必须在msf_open msf_close之间调用
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_get(IN CONST CHAR *variable, OUT CHAR *value, IN CONST INT value_len);

/***********************************************************
*  Function: msf_commit
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
*  note:必须在msf_open msf_close之间调用
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_commit(VOID);

/***********************************************************
*  Function: msf_get_free_space
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
*  note:必须在msf_open msf_close之间调用
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_get_free_space(VOID);

/***********************************************************
*  Function: msf_close
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
*  note:必须与msf_open调用成对出现
***********************************************************/
__FLASH_API_EXT \
VOID msf_close(VOID);

/***********************************************************
*  Function: msf_all_erase_and_init
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_all_erase_and_init();

/***********************************************************
*  Function: msf_get_single
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_get_single(IN CONST CHAR *module,IN CONST CHAR *variable,\
                           OUT CHAR *value,IN CONST unsigned max_len);

/***********************************************************
*  Function: msf_set_single
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
__FLASH_API_EXT \
OPERATE_RET msf_set_single(IN CONST CHAR *module,IN CONST CHAR *variable,\
                           IN CONST CHAR *value);

#ifdef __cplusplus
}
#endif
#endif

