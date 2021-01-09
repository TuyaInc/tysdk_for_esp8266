/***********************************************************
*  File: uf_file.h 
*  Author: lql
*  Date: 20181213
***********************************************************/
#ifndef __UF_FILE_H
    #define __UF_FILE_H
    #include "error_code.h"
    #include "tuya_cloud_types.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __UF_FILE_GLOBALS
    #define __UF_FILE_EXT
#else
    #define __UF_FILE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef VOID uFILE;

typedef unsigned char umode_t;
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: init uf mode
*  Input:  none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FILE_EXT \
OPERATE_RET ufinit(VOID);

/***********************************************************
*  Function: fopen
*  Input: filepath mode
*  Output: none
*  Return: none
***********************************************************/
__UF_FILE_EXT \
uFILE* ufopen(IN CONST UCHAR_T *filepath, IN CONST UCHAR_T *mode);

/***********************************************************
*  Function: fclose
*  Input: fd
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FILE_EXT \
OPERATE_RET ufclose(IN uFILE *fd);

/***********************************************************
*  Function: fdelete
*  Input: filepath
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FILE_EXT \
OPERATE_RET ufdelete(IN CONST UCHAR_T *filepath);

/***********************************************************
*  Function: fwrite
*  Input: fd buf len
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufwrite(IN uFILE *fd, IN UCHAR_T *buf,IN CONST UINT_T len);

/***********************************************************
*  Function: fread
*  Input: fd buf len
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufread(IN uFILE *fd, IN UCHAR_T *buf,IN CONST UINT_T len);

/***********************************************************
*  Function: fseek
*  Input: fd offset uf_mode
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufseek(IN uFILE *fd, IN UINT_T offset,IN CONST umode_t uf_mode);

/***********************************************************
*  Function: feof
*  Input: fd
*  Output: none
*  Return: BOOL_T
***********************************************************/
__UF_FILE_EXT \
BOOL_T ufeof(IN uFILE *fd);

/***********************************************************
*  Function: fgetsize
*  Input: filepath
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufgetsize(IN   UCHAR_T *filepath);


/***********************************************************
*  Function: ufprintf
*  Input: stream pFmt
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufprintf(IN uFILE * stream,IN const char* pFmt,...);

/***********************************************************
*  Function: ufscanf
*  Input: stream count pFmt
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufscanf(IN uFILE * stream,IN INT_T count,IN PCHAR_T pFmt,...);

/***********************************************************
*  Function: ufexist
*  Input: filepath
*  Output: none
*  Return: BOOL_T
***********************************************************/
__UF_FILE_EXT \
BOOL_T ufexist(IN   UCHAR_T *filepath);

#ifdef __cplusplus
}

#endif
#endif