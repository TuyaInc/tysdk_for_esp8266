/***********************************************************
*  File: psm_flash.h 
*  Author: nzy
*  Date: 20150818
***********************************************************/
#ifndef _PSM_FLASH_H
    #define _PSM_FLASH_H
    
    #include "sys_adapter.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __PSM_FLASH_GLOBALS
    #define __PSM_FLASH_EXT
#else
    #define __PSM_FLASH_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
/** Size of one partition within the psm flash block */
#define PARTITION_SIZE (1 << 12) /* 4KB */

// flash describe
typedef struct {
    /** The start address on flash */
    UINT  fl_start;
    /** The size on flash  */
    UINT  fl_size;
}FLASH_DESC_S;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
__PSM_FLASH_EXT \
OPERATE_RET psm_flash_init(VOID);

__PSM_FLASH_EXT \
OPERATE_RET psm_flash_erase(IN CONST UINT start,IN CONST UINT size);

__PSM_FLASH_EXT \
OPERATE_RET psm_flash_write(IN CONST UINT dst,IN CONST BYTE *src,IN CONST UINT size);

__PSM_FLASH_EXT \
OPERATE_RET psm_flash_read(IN CONST UINT src,OUT BYTE *dst,IN CONST UINT size);

#ifdef __cplusplus
}
#endif

#endif

