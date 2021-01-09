/***********************************************************
*  File: flash_map.h 
*  Author: nzy
*  Date: 20190327
*  Desc: Description all flash details
***********************************************************/
#ifndef _FLASE_MAP_H_
#define _FLASE_MAP_H_

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __FLASE_MAP_GLOBALS
    #define __FLASE_MAP_EXT
#else
    #define __FLASE_MAP_EXT extern
#endif

#define PARTITION_SIZE (1 << 12) /* 4KB */

#if (ESP8266_1M == 1)
// firmware execute area
#define FM_UG_APP1_AREA 0x1000
#define FM_UG_APP2_AREA 0x81000
#define FM_EXEC_AREA_SIZE (480*1024)

// timer schema
#define FW_TM_SCHEMA_ADDR 0x80000
#define FW_TM_SCHEMA_SIZE   4096
// rf cal addr
#define FW_RF_CAL_BAK_ADDR 0x79000
#define FW_RF_CAL_BAK_SIZE 4096
// user parma addr
#define USER_PARAM_ADDR 0xF9000
#define USER_SEC_SIZE   4096

#else
// firmware execute area
#define FM_UG_APP1_AREA 0x1000
#define FM_UG_APP2_AREA 0x101000
#define FM_EXEC_AREA_SIZE (800*1024)

// data storage
// kv storage details 
#define FM_SF_FLASH_START 0xE1000 // start from 900*1024,Reserve some flash space for fireware
#define FM_SF_FLASH_SIZE 0x10000 // 64K
#define FM_SF_FLASH_SWAP_START 0xF1000 // Reserve some flash space for sf data flash size
#define FM_SF_FLASH_SWAP_SIZE 0x4000 //16K
#define FM_SF_FLASH_KEY_ADDR (0x100000-PARTITION_SIZE) // save data encryption key
#define FM_SF_FLASH_KEY_SIZE PARTITION_SIZE

// uf partition table details
#define UF_PATI_NUM 1
#define UF_PATI1_START_ADDR 0x1E1000 // 1024*1024+900*1024
#define UF_PATI1_FLASH_SZ 0x10000 // 64K 

// timer schema
#define FW_TM_SCHEMA_ADDR  0x1FA000
#define FW_TM_SCHEMA_SIZE 4096
// rf cal addr
#define FW_RF_CAL_BAK_ADDR 0x1FB000
#define FW_RF_CAL_BAK_SIZE 4096

#define USER_PARAM_ADDR 0xF5000  //oem param
#define USER_SEC_SIZE   4096

#endif

#ifdef __cplusplus
}
#endif

#endif //_FLASE_MAP_H_
