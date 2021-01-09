/***********************************************************
*  File: uni_storge.h 
*  Author: nzy
*  Date: 20170920
***********************************************************/
#ifndef _UNI_STORGE_H
    #define _UNI_STORGE_H

    #include "tuya_cloud_types.h"
    #include "flash_map.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __UNI_STORGE_GLOBALS
    #define __UNI_STORGE_MODULE_EXT
#else
    #define __UNI_STORGE_MODULE_EXT extern
#endif

#if (ESP8266_1M != 1)
/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct {
    UINT_T start_addr; // user physical flash start address 
    UINT_T flash_sz; // user flash size
    UINT_T block_sz; // flash block/sector size

    // For data backup and power down protection data recovery
    UINT_T swap_start_addr; // swap flash start address
    UINT_T swap_flash_sz; // swap flash size    

    //for restore factor of flash encryption key 
    UINT_T key_restore_addr;
}UNI_STORGE_DESC_S;

typedef struct
{
    UINT_T uf_partition_start_addr;
    UINT_T uf_partiton_flash_sz;
}UF_PARTITION;

typedef struct
{
    UINT_T sector_size;
    INT_T uf_partition_num;
    UF_PARTITION uf_partition[3];
}UF_PARTITION_TABLE_S;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: uni_flash_read
*  Input: addr size
*  Output: dst
*  Return: none
***********************************************************/
__UNI_STORGE_MODULE_EXT \
OPERATE_RET uni_flash_read(IN CONST UINT_T addr, OUT BYTE_T *dst, IN CONST UINT_T size);

/***********************************************************
*  Function: uni_flash_write
*  Input: addr src size
*  Output: none
*  Return: none
***********************************************************/
__UNI_STORGE_MODULE_EXT \
OPERATE_RET uni_flash_write(IN CONST UINT_T addr, IN CONST BYTE_T *src, IN CONST UINT_T size);

/***********************************************************
*  Function: uni_flash_erase
*  Input: addr size
*  Output: 
*  Return: none
***********************************************************/
__UNI_STORGE_MODULE_EXT \
OPERATE_RET uni_flash_erase(IN CONST UINT_T addr, IN CONST UINT_T size);

/***********************************************************
*  Function: uni_get_storge_desc
*  Input: none
*  Output: none
*  Return: UNI_STORGE_DESC_S
***********************************************************/
__UNI_STORGE_MODULE_EXT \
UNI_STORGE_DESC_S *uni_get_storge_desc(VOID);

/***********************************************************
*  Function: uni_get_uf_file_desc
*  Input: none
*  Output: none
*  Return: UF_PARTITION_TABLE_S
***********************************************************/
__UNI_STORGE_MODULE_EXT \
UF_PARTITION_TABLE_S *uni_get_uf_file_desc(VOID);

#endif
#ifdef __cplusplus
}
#endif
#endif

