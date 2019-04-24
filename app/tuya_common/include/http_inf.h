/***********************************************************
*  File: http_inf.h 
*  Author: nzy
*  Date: 20161028
***********************************************************/
#ifndef _HTTP_INF_H
#define _HTTP_INF_H

#ifdef __cplusplus
extern "C" {
#endif

//#include "tuya_cloud_types.h"
#include "com_def.h"

#include "httpc.h"

#ifdef  __HTTP_INF_GLOBALS
    #define __HTTP_INF_EXT
#else
    #define __HTTP_INF_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef INT (*HTTP_DATA_RECV)(PVOID hand,PVOID buf,UINT len);

typedef struct {
    BOOL chunked;
    UINT content_len; // if(chunked == FALSE) then valid.
    PVOID hand;
    HTTP_DATA_RECV recv; // system auto fill
    PVOID *pri_data; // private data

    INT status_code;
}HTTP_INF_H_S;

// user http respond callback
typedef OPERATE_RET (*HTTP_INF_CB)(HTTP_INF_H_S *hand);

typedef struct {
    BYTE total;
    BYTE cnt;
    CHAR *pos[0];
}HTTP_PARAM_H_S;

#define DEF_URL_LEN 1024
typedef struct {
    HTTP_PARAM_H_S *param_h; // param handle
    CHAR *param_in; // param insert pos
    USHORT head_size; // head_size == "url?" or "url"
    USHORT buf_len; // bufer len
    CHAR buf[0]; // "url?key=value" + "kev=value statistics"
}HTTP_URL_H_S;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: http_inf_client_head
*  Input: url callback
*  Output: pri_data
*  Return: OPERATE_RET
*  Note: Use to detect if network support range
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET http_inf_client_head(IN CONST CHAR *url,\
                                IN CONST HTTP_INF_CB callback,\
                                INOUT PVOID *pri_data, \
                                IN CONST UINT file_offset,\
                                IN CONST UINT file_size);


/***********************************************************
*  Function: http_inf_client_get
*  Input: url callback
*  Output: pri_data
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET http_inf_client_get(IN CONST CHAR *url,\
                                IN CONST HTTP_INF_CB callback,\
                                INOUT PVOID *pri_data);


/***********************************************************
*  Function: http_inf_client_get
*  Input: url callback
*  Output: pri_data
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET http_inf_client_get_file(IN CONST CHAR *url,\
                                IN CONST HTTP_INF_CB callback,\
                                INOUT PVOID *pri_data, \
                                IN CONST UINT file_offset,\
                                IN CONST UINT file_size);

/***********************************************************
*  Function: http_inf_client_post
*  Input: url callback content len
*  Output: pri_data 
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET http_inf_client_post(IN CONST CHAR *url,\
                                 IN CONST HTTP_INF_CB callback,\
                                 IN CONST BYTE *data,\
                                 IN CONST UINT len,\
                                 INOUT PVOID *out);
/***********************************************************
*  Function: http_client_put
*  Input: url callback
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET http_client_put(IN CONST char *url, \
                                    IN CONST HTTP_INF_CB callback, \
                                    IN CONST BYTE *data, \
                                    IN CONST UINT len, \
                                    IN HTTP_HEAD_ADD_CB add_head_cb, \
                                    INOUT PVOID *pri_data);
/***********************************************************
*  Function: create_http_url_h
*  Input: buf_len: if len == 0 then use DEF_URL_LEN
*         param_cnt
*  Output: 
*  Return: HTTP_URL_H_S *
***********************************************************/
__HTTP_INF_EXT \
HTTP_URL_H_S *create_http_url_h(IN CONST USHORT buf_len,\
                                IN CONST USHORT param_cnt);


/***********************************************************
*  Function: del_http_url_h
*  Input: hu_h
*  Output: 
*  Return: HTTP_URL_H_S *
***********************************************************/
__HTTP_INF_EXT \
VOID del_http_url_h(IN HTTP_URL_H_S *hu_h);

/***********************************************************
*  Function: fill_url_head
*  Input: hu_h:http url handle 
*         url_h:url head
*  Output: hu_h
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET fill_url_head(INOUT HTTP_URL_H_S *hu_h,\
                                 IN CONST CHAR *url_h);

/***********************************************************
*  Function: fill_url_param
*  Input: hu_h
*         key
*         value
*  Output: hu_h
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET fill_url_param(INOUT HTTP_URL_H_S *hu_h,\
                           IN CONST CHAR *key,\
                           IN CONST CHAR *value);

#ifdef __cplusplus
}
#endif
#endif

