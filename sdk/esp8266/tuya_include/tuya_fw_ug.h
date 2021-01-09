#ifndef _TUYA_FW_UG_H
#define _TUYA_FW_UG_H

    #include "com_def.h"
    #include "sys_adapter.h"
    #include "mem_pool.h"
    #include "error_code.h"
    #include "com_struct.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __TUYA_FW_UG_GLOBALS
    #define __TUYA_FW_UG_EXT
#else
    #define __TUYA_FW_UG_EXT extern
#endif

typedef OPERATE_RET(*FILE_HTTPC_CB)(IN CONST BYTE *data,\
                                    IN CONST UINT len);

typedef OPERATE_RET(*FILE_PRE_CB)(IN CONST UINT file_size);

typedef VOID(*FILE_HTTPC_FAIL_CB)(VOID);


/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: httpc_set_mcu_persize
*  Input: size
*  Output: 
*  Return: none
*  note: rangle 256 - 1024
***********************************************************/
__TUYA_FW_UG_EXT \
VOID set_mcu_trans_size(IN CONST UINT persize);

/***********************************************************
*  Function: http_gw_download_file
*  Input: fw
*  Output: 
*  Return: none
*  note: upgrade wifi soc device
***********************************************************/
__TUYA_FW_UG_EXT \
OPERATE_RET http_gw_download_file(IN CONST FW_UG_S *fw);

/***********************************************************
*  Function: http_mcu_download_file
*  Input: fw
*  Output: 
*  Return: none
*  note: upgrade mcu device
***********************************************************/
__TUYA_FW_UG_EXT \
OPERATE_RET http_mcu_download_file(IN CONST FW_UG_S *fw);

/***********************************************************
*  Function: httpc_upgd_mcu_cb_reg
*  Input: callback
*  Output: 
*  Return: none
***********************************************************/
__TUYA_FW_UG_EXT \
VOID httpc_upgd_mcu_cb_reg(IN CONST FILE_HTTPC_CB callback, \
                                    IN CONST FILE_PRE_CB pre_callback,\
                                    IN CONST FILE_HTTPC_FAIL_CB fail_callback);

/***********************************************************
*  Function: httpc_fsize_fw_out
*  Input: callback
*  Output: 
*  Return: send mcu fw size
***********************************************************/
__TUYA_FW_UG_EXT \
OPERATE_RET httpc_fsize_fw_out(IN CONST UINT fsize);



/***********************************************************
*  Function: http_mcu_ota_fail_cb
*  Input: callback
*         pre_callback
*  Output:
*  Return: none
***********************************************************/
__TUYA_FW_UG_EXT \
VOID http_mcu_ota_fail_cb(VOID);


#ifdef __cplusplus
}
#endif
#endif

