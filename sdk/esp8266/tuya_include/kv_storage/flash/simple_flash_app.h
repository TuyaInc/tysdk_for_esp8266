/***********************************************************
*  File: simple_flash_app.h 
*  Author: nzy
*  Date: 20160817
***********************************************************/
#ifndef _SIMPLE_FLASH_APP_H
    #define _SIMPLE_FLASH_APP_H

    #include "tuya_cloud_types.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __SIMPLE_FLASH_APP_GLOBALS
    #define __SIMPLE_FLASH_APP_EXT
#else
    #define __SIMPLE_FLASH_APP_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: simple_flash_app_init
*  Input: none
*  Output: 
*  Return: none
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET simple_flash_app_init(IN CONST BYTE_T *p_encrypt_key, IN CONST UINT_T encrypt_key_len);


/***********************************************************
*  Function: simple_flash_app_uninit
*  Input: none
*  Output: 
*  Return: none
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
VOID simple_flash_app_uninit(VOID);

/***********************************************************
*  Function: sf_app_write
*  description: flash write method
*  Input: name->data name
*         data->data
*         len->data len
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET sf_app_write(CONST CHAR_T *name,CONST BYTE_T *data,CONST UINT_T len);

/***********************************************************
*  Function: sf_app_read
*  description: read method
*  Input: name->data name
*         data->write data
*         len->data len
*  Output: data->read data,need free
*          len->read data len 
*  Return: OPERATE_RET
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET sf_app_read(CONST CHAR_T *name,BYTE_T **data,UINT_T *len);

/***********************************************************
*  Function: sf_app_fuzzy_read
*  description: fuzzy read method
*  Input: fuzzy_name->fuzzy name
*         index->find index
*         data->read data
*         len->data len
*  Output: data->read data,need free
*          len->read data len 
*          index->the fine index +1
*  Return: OPERATE_RET
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET sf_app_fuzzy_read(CONST CHAR_T *fuzzy_name,UINT_T *index,\
                              BYTE_T **data,UINT_T *len);

/***********************************************************
*  Function: sf_app_free_data
*  description: free data from sf_app_read/sf_app_fuzzy_read
*  Input: p_data->data need to free
*  Output:
*  Return: OPERATE_RET
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET sf_app_free_data(BYTE_T *p_data);

/***********************************************************
*  Function: sf_app_format
*  Input: none
*  Output: none
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET sf_app_format(VOID);

/***********************************************************
*  Function: sf_fuzzy_delete
*  description: simple flash data delete method
*  Input: fuzzy_name->delete data name
*  Output: none
*  Return: sf_ret
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET sf_app_fuzzy_delete(const CHAR_T *fuzzy_name);

/***********************************************************
*  Function: sf_delete
*  description: data delete method
*  Input: name->delete data name
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET sf_app_delete(CONST CHAR_T *name);

/***********************************************************
*  Function: sf_app_get_encrypt_key
*  Input: p_encrypt_key
*  Output: 
*  Return: OPERATE_RET
***********************************************************/
__SIMPLE_FLASH_APP_EXT \
OPERATE_RET sf_app_get_encrypt_key(CHAR_T **p_encrypt_key);


#ifdef __cplusplus
}
#endif

#endif


