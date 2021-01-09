/***********************************************************
*  File: uf_flash_file_app.h 
*  Author: lql
*  Date: 20181213
***********************************************************/
#ifndef __UF_FLASH_FILE_APP_H
    #define __UF_FLASH_FILE_APP_H
    #include "error_code.h"
    #include "tuya_cloud_types.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __UF_FLASH_FILE_APP_GLOBALS
    #define __UF_FLASH_FILE_APP_EXT
#else
    #define __UF_FLASH_FILE_APP_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef UCHAR_T uf_mode_t;
#define UF_SEEK_SET 0
#define UF_SEEK_CUR 1
#define UF_SEEK_END 2

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: uf_file_app_init
*  Input: p_encrypt_key encrypt_key_len
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
OPERATE_RET uf_file_app_init(VOID);

/***********************************************************
*  Function: uf_file_app_uninit
*  Input: none
*  Output: none
*  Return: VOID
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
VOID uf_file_app_uninit(VOID);

/***********************************************************
*  Function: uf_file_open
*  Input: name mode
*  Output: none
*  Return: FILE*
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
VOID* uf_file_open(CONST UCHAR_T *name,CONST UCHAR_T *mode);

/***********************************************************
*  Function: uf_file_close
*  Input: uf_hand
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
OPERATE_RET uf_file_close(VOID *fd);

/***********************************************************
*  Function: uf_file_delete
*  Input: name
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
OPERATE_RET uf_file_delete(CONST UCHAR_T *name);

/***********************************************************
*  Function: uf_file_write
*  Input: uf_hand data len
*  Output: write_len
*  Return: OPERATE_RET
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
OPERATE_RET uf_file_write(VOID *fd,UCHAR_T *data,UINT_T len,UINT_T *write_len);

/***********************************************************
*  Function: uf_file_read
*  Input: uf_hand data len
*  Output: read_len
*  Return: OPERATE_RET
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
OPERATE_RET uf_file_read(VOID *fd,UCHAR_T *data,UINT_T len,UINT_T *read_len);

/***********************************************************
*  Function: uf_file_lseek
*  Input: uf_hand offset fromwhere
*  Output:none
*  Return: file offset
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
INT_T uf_file_lseek(VOID *fd,UINT_T offset,uf_mode_t fromwhere);

/***********************************************************
*  Function: uf_get_file_size
*  Input: name
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
OPERATE_RET uf_file_get_file_size(IN       UCHAR_T *name,OUT UINT_T *size);

/***********************************************************
*  Function: uf_eof
*  Input: fd
*  Output: none
*  Return: BOOL_T
***********************************************************/
__UF_FLASH_FILE_APP_EXT \
BOOL_T uf_file_eof(VOID *fd);

#ifdef __cplusplus
}

#endif
#endif