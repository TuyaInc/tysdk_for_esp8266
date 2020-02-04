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


#ifdef __cplusplus
}
#endif

#endif //_FLASE_MAP_H_
