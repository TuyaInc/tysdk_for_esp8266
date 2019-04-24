/***********************************************************
*  File: com_def.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _COM_DEF_H
#define _COM_DEF_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
#ifndef LITTLE_END
#define LITTLE_END // 主机为小端模式
#endif

#ifndef NULL
    #ifdef __cplusplus
    #define NULL 0
    #else
    #define NULL ((void *)0)
    #endif
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

/* 通用变量类型重定义 */
#ifndef VOID
#define VOID void
#endif /* VOID */

#ifndef CONST
#define CONST const
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef SIZEOF
#define SIZEOF sizeof
#endif

#ifndef INLINE
#define INLINE inline
#endif

typedef unsigned char BYTE;
typedef BYTE *PBYTE;
typedef unsigned short WORD;
typedef WORD *PWORD;
typedef unsigned int DWORD;
typedef DWORD *PDWORD;
typedef unsigned int UINT;
typedef unsigned int *PUINT;

#ifndef BOOL
typedef int BOOL;
typedef BOOL *PBOOL;
#endif

typedef float FLOAT;
typedef FLOAT *PFLOAT;
typedef signed int INT;
typedef int *PINT;
typedef void *PVOID;
typedef signed char CHAR;
typedef unsigned char UCHAR;
typedef char *PCHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef short *PSHORT;
typedef long LONG;
typedef unsigned long ULONG;
typedef long *PLONG;
#if defined(LINUX)
typedef long long DLONG;
typedef DLONG *PDLONG;
typedef unsigned long long DDWORD;
typedef DDWORD *PDDWORD;
#endif
typedef size_t SIZE_T; // result of the sizeof

/* 常用操作宏定义 */
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define MAKEWORD(a, b) ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b) ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l) ((WORD)(l))
#define HIWORD(l) ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w) ((BYTE)(w))
#define HIBYTE(w) ((BYTE)(((WORD)(w) >> 8) & 0xFF))

/* 字节序置换 */
#define WORD_SWAP(X) ((X << 8) | (X >> 8))
#define DWORD_SWAP(X)(((X)&0xff)<<24) + \
                     (((X)&0xff00)<<8) + \
                     (((X)&0xff0000)>>8) + \
                     (((X)&0xff000000)>>24)
#define DDWORD_SWAP(X) { \
    unsigned long temp_low, temp_high; \
    temp_low = DWORD_SWAP((unsigned long)((X) & 0xffffffff)); \
    temp_high = DWORD_SWAP((unsigned long)((X) >> 32)); \
    X = 0; \
    X |= temp_low; \
    X <<= 32; \
    X |= temp_high;\
}
#ifdef LITTLE_END
#define NTOHS(X) WORD_SWAP(X)
#define HTONS(X) WORD_SWAP(X)
#define NTOHL(X) DWORD_SWAP(X)
#define HTONL(X) DWORD_SWAP(X)
#define NTOHLL(X) DDWORD_SWAP(X)
#define HTONLL(X) DDWORD_SWAP(X)
#else
#define NTOHS(X) X
#define HTONS(X) X
#define NTOHL(X) X
#define HTONL(X) X
#define NTOHLL(X)
#define HTONLL(X)
#endif

#define OFFSOF(s,m) ((SIZE_T)(&(((s*)0)->m)))   // 结构体成员偏移宏
#define CNTSOF(a)   (sizeof(a)/sizeof(a[0]))    // 数组元素的个数宏 
#define CNTR_OF(ptr, type, member) \
        ({(type *)( (char *)ptr - OFFSOF(type,member) );}) // continer of

#define INCREMENT(x,n) (x+n) // 增量

typedef INT OPERATE_RET; // 操作结果返回值
/*
#define OPRT_COMMON_START 0 // 通用返回值开始
#define OPRT_OK INCREMENT(OPRT_COMMON_START,0)              // 执行成功
#define OPRT_COM_ERROR INCREMENT(OPRT_COMMON_START,1)       // 通用错误
#define OPRT_INVALID_PARM INCREMENT(OPRT_COMMON_START,2)    // 无效的入参
#define OPRT_MALLOC_FAILED INCREMENT(OPRT_COMMON_START,3)   // 内存分配失败
#define OPRT_COMMON_END OPRT_MALLOC_FAILED // 通用返回值结束

// 应用程序框架用户通用返回起始值
#define OPRT_USER_START INCREMENT(OPRT_COMMON_END,1)
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
