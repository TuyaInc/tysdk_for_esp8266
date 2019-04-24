/***********************************************************
*  File: package.h 
*  Author: nzy
*  Date: 20150915
***********************************************************/
#ifndef _PACKAGE_H
    #define _PACKAGE_H

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __PACKAGE_GLOBALS
    #define __PACKAGE_EXT
#else
    #define __PACKAGE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#pragma pack(1)
typedef struct {
    unsigned int offset;
    unsigned int len;
    unsigned int sum;
}UG_FILE_S;

#define SW_VER_LMT 10
#define UG_PKG_HEAD 0x55aa55aa
#define UG_PKG_TAIL 0xaa55aa55
#define SW_VER USER_SW_VER
    
typedef struct { // big-end
    unsigned int head; // 0x55aa55aa
    unsigned char sw_ver[SW_VER_LMT+1];
    unsigned int ugf_cnt;
    UG_FILE_S ugf[0];
}UG_PKG_FH_S;

typedef struct { // big-end
    unsigned int sum;
    unsigned int tail; // 0xaa55aa55
}UG_PKG_TAIL_S;
#pragma pack()

// package structure: UG_PKG_FH_S + UG_PKG_TAIL_S + actual file

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/

#ifdef __cplusplus
}
#endif
#endif

