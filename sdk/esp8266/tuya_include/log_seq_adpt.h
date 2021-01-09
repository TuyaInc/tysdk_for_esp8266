/***********************************************************
*  File: log_seq.h
*  Author: nzy
*  Date: 20190422
***********************************************************/
#ifndef __LOG_SEQ_ADPT_H
    #define __LOG_SEQ_ADPT_H

    #include "com_def.h"
    #include "sys_adapter.h"
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  _LOG_SEQ_ADAPT_GLOBALS
    #define __LOG_SEQ_ADAPT_EXT
#else
    #define __LOG_SEQ_ADAPT_EXT extern
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/


typedef BYTE LOG_SEQ_TYPE;
#define LOG_SEQ_TYPE_AP_CFG		1
#define	LOG_SEQ_TYPE_SMT_CFG		2
#define LOG_SEQ_TYPE_HTTP			3
#define LOG_SEQ_TYPE_MQTT			4

typedef BYTE LOG_SEQ_RP_LOG_FILE;
#define LOG_SEQ_RP_LOG_FILE_NULL		0
#define LOG_SEQ_RP_LOG_FILE_AP_SMT_CFG	1
#define	LOG_SEQ_RP_LOG_FILE_HTTP_MQTT	2

#define LOG_FILE_MAX_NUM 	10

#define AP_SMT_CFG_SEQ_LOG_FILE 	"ap_smt_cfg_log_file"
#define HTTP_MQTT_SEQ_LOG_FILE 		"http_mqtt_log_file"

#define AP_CFG_SEQ_NAME 	"ap_cfg"
#define SMT_CFG_SEQ_NAME 	"smt_cfg"
#define HTTP_SEQ_NAME 		"http"
#define MQTT_SEQ_NAME 		"mqtt"

#define LOG_LEN_LMT 1024
#define LOG_MIN_LEN_LMT 10

typedef BYTE APP_CFG_RP_TYPE;
#define APP_CFG_RP_TYPE_TOKEN 				1
#define APP_CFG_RP_TYPE_CODE 				2

typedef BYTE APP_CFG_RP_CODE;
#define APP_CFG_REPORT_SUCCESS 				0
#define APP_CFG_REPORT_DATA_PACKAGE_ERR 	1
#define APP_CFG_REPORT_NOT_FIND_ROUTER		2   
#define APP_CFG_REPORT_ROUTER_PASSWD_ERR 	3   
#define APP_CFG_REPORT_ROUTER_CONNECT_ERR 	4   
#define APP_CFG_REPORT_DHCP_ERR 			5	


typedef BYTE HTTP_RP_TYPE;
#define HTTP_RP_TYPE_TOKEN 					1
#define HTTP_RP_TYPE_API_NAME_CRC32 		10
#define HTTP_RP_TYPE_OPEN_SESSION 			11
#define HTTP_RP_TYPE_PREPAARE_REQ 			12
#define HTTP_RP_TYPE_SEND_REQ				13
#define HTTP_RP_TYPE_GET_RSP_HDR			14
#define HTTP_RP_TYPE_CERTIFY_INFO			15
#define HTTP_RP_TYPE_RECV_NEED_DATA			16
#define HTTP_RP_TYPE_GET_CJASON				17
#define HTTP_RP_TYPE_GET_DECRYPT			18
#define HTTP_RP_TYPE_GET_CONTENT			19
#define HTTP_RP_TYPE_GET_URL				20

typedef INT  HTTP_RP_CODE;
#define HTTP_RP_TYPE_OPEN_SESSION_INVALID_PARAM  	-1
#define HTTP_RP_TYPE_OPEN_SESSION_SOCKET_ERR   	    -6
#define HTTP_RP_TYPE_OPEN_SESSION_URL_PARSE_ERR  	-17
#define HTTP_RP_TYPE_OPEN_SESSION_TCP_CONNECT_ERR  	-7
#define HTTP_RP_TYPE_OPEN_SESSION_TLS_ERR  			-8

#define HTTP_RP_TYPE_PREPAARE_REQ_ERR  			    -707
#define HTTP_RP_TYPE_SEND_REQ_ERR  			    	-708
#define HTTP_RP_TYPE_GET_RSP_HDR_ERR  			    -711
#define HTTP_RP_TYPE_CERTIFY_INFO_ERR  			    -719
#define HTTP_RP_TYPE_RECV_NEED_DATA_MALLOC_FAIL     -3
#define HTTP_RP_TYPE_RECV_NEED_DATA_RECV_FAIL     	-918
#define HTTP_RP_TYPE_RECV_NEED_DATA_RECV_DATA_ERR   -932
#define HTTP_RP_TYPE_GET_CJASON_ERR   				-802
#define HTTP_RP_TYPE_GET_DECRYPT_CJASON_NOT_ENOUGH  -803
#define HTTP_RP_TYPE_GET_DECRYPT_MD5_ERR   			-949
#define HTTP_RP_TYPE_GET_DECRYPT_MALLOC_FAIL   		-3
#define HTTP_RP_TYPE_GET_DECRYPT_ERR   				-950
#define HTTP_RP_TYPE_GET_DECRYPT_GET_CJASON_ERR   	-802
#define HTTP_RP_TYPE_GET_CONTENT_DEV_ILLEGAL		-946
#define HTTP_RP_TYPE_GET_CONTENT_DEVICE_RST_FACTORY	-947
#define HTTP_RP_TYPE_GET_CONTENT_EXPIRE				-951
#define HTTP_RP_TYPE_GET_CONTENT_GATEWAY_NOT_EXISTS	-927
#define HTTP_RP_TYPE_GET_CONTENT_ORDER_EXPIRED		-2002
#define HTTP_RP_TYPE_GET_CONTENT_UNKNOW_FAILED		-2006
#define HTTP_RP_TYPE_GET_CONTENT_DEV_REMOVE			-2007
#define HTTP_RP_TYPE_GET_CONTENT_CAMERA_STORAGE_ORDER_UNNORMAL			-2002
#define HTTP_RP_TYPE_GET_CONTENT_CAMERA_STORAGE_ORDER_NOT_EXISTED		-2002
#define HTTP_RP_TYPE_GET_CONTENT_API_WRONG			-948
#define HTTP_RP_TYPE_GET_CONTENT_DEVICE_ALY_BIND 	-1
#define HTTP_RP_TYPE_GET_CONTENT_SIGNATURE_ERR 		-2008
#define HTTP_RP_TYPE_GET_CONTENT_EXPIRE_NO_ERRMSG 	-803

typedef BYTE MQTT_RP_TYPE;
#define MQTT_RP_TYPE_WIFI_SIGNAL 			9
#define MQTT_RP_TYPE_DNS 					10
#define MQTT_RP_TYPE_SOCKET_CONN 			11
#define MQTT_RP_TYPE_CONN					12
#define MQTT_RP_TYPE_SUBSCRIBE				13
#define MQTT_RP_TYPE_LOGIC_CONNING_PROC		14
#define MQTT_RP_TYPE_RECV					15
#define MQTT_RP_TYPE_CONN_RECV				16
#define MQTT_RP_TYPE_PING_SEND				17
#define MQTT_RP_TYPE_PING_TIMEOUT			18
#define MQTT_RP_TYPE_PING_RECONN			19


typedef INT  MQTT_RP_CODE;
#define MQTT_RP_TYPE_DNS_ERR 						-3010
#define MQTT_RP_TYPE_SOCKET_CREATE_ERR 				-3011
#define MQTT_RP_TYPE_SOCKET_SET_ERR 				-3012
#define MQTT_RP_TYPE_SOCKET_TCP_ERR 				-3013
#define MQTT_RP_TYPE_SOCKET_TLS_ERR 				-3014
#define MQTT_RP_TYPE_LOGIC_CONNING_INVALID_PARAM	-3001
#define MQTT_RP_TYPE_LOGIC_CONNING_DATA_ERR			-3016
#define MQTT_RP_TYPE_LOGIC_CONNING_MSGID_ERR		-3017
#define MQTT_RP_TYPE_LOGIC_CONNING_MSG_START_ERR	-3018
#define MQTT_RP_TYPE_PING_TIME						-3999


typedef struct {
	LOG_SEQ_RP_LOG_FILE log_report_file;
	UCHAR rp_log_file_num;
} LOG_SEQ_LOG_FILE_RP;


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

#if (ESP8266_1M != 1)
/***********************************************************
*  Function: insert_smt_cfg_err_log
*  Desc:	 insert a "log sequence" write to flash
*  Input:	 smt_cfg_type
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET insert_smt_cfg_err_log(APP_CFG_RP_CODE smt_cfg_type);

/***********************************************************
*  Function: insert_ap_cfg_err_log
*  Desc:	 insert a "log sequence" write to flash
*  Input:	 ap_cfg_type
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET insert_ap_cfg_err_log(APP_CFG_RP_CODE ap_cfg_type);

/***********************************************************
*  Function: insert_router_err_log
*  Desc:	 insert a "log sequence" write to flash
*  Input:	 router_err_type
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET insert_router_err_log(APP_CFG_RP_CODE router_err_type);

/***********************************************************
*  Function: set_mqtt_seq_log
*  Desc:	 insert a "log sequence" not write to flash
*  Input:	 mqtt_type
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET set_mqtt_seq_log(MQTT_RP_TYPE mqtt_type);
/***********************************************************
*  Function: set_http_err_log
*  Desc:	 insert a "log sequence"  write to flash
*  Input:	 http_type  err_code
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET set_http_err_log(HTTP_RP_TYPE http_type,INT err_code);
/***********************************************************
*  Function: set_mqtt_err_log
*  Desc:	 insert a "log sequence"  write to flash
*  Input:	 mqtt_type  err_code
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET set_mqtt_err_log(MQTT_RP_TYPE mqtt_type,INT err_code);
/***********************************************************
*  Function: get_cfg_report_log
*  Desc:	 get cfg log flag
*  Input:	 null
*  Return:	 LOG_SEQ_LOG_FILE_RP
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
LOG_SEQ_LOG_FILE_RP* get_cfg_report_log(VOID);
/***********************************************************
*  Function: log_seq_delete_log
*  Desc:	 log_seq_delete_log
*  Input:	 log_file  num 
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET log_seq_delete_log(IN LOG_SEQ_RP_LOG_FILE log_file,IN CHAR file_num);
/***********************************************************
*  Function: log_seq_read_log_data
*  Desc:	 log_seq_read_log_data
*  Input:	 log_file  num 
*  OUTput:	 log_data  data_len 
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET log_seq_read_log_data(IN LOG_SEQ_RP_LOG_FILE log_file,IN CHAR file_num,OUT CHAR** log_data,OUT UINT*len);
/***********************************************************
*  Function: set_log_rp_file_rst
*  Desc:	 set_log_rp_file_rst
*  Return:	 void
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
VOID set_log_rp_file_rst(VOID);
/***********************************************************
*  Function: log_seq_init
*  Desc:	 log_seq_init
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET log_seq_init(VOID);
/***********************************************************
*  Function: log_seq_upload_realtime_log
*  Desc:	 log_seq_upload_realtime_log
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET log_seq_upload_realtime_log(VOID);

/***********************************************************
*  Function: get_log_seq_write_st
*  Desc:	 get_log_seq_write_st
*  Return:	 TRUE FALSE
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
BOOL get_log_seq_write_st(VOID);
/***********************************************************
*  Function: set_log_seq_write_st
*  Desc:	 set_log_seq_write_st
*  Return:	 
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
VOID set_log_seq_write_st(BOOL st);


#else
/***********************************************************
*  Function: insert_smt_cfg_err_log
*  Desc:	 insert a "log sequence" write to flash
*  Input:	 smt_cfg_type
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET insert_smt_cfg_err_log(APP_CFG_RP_CODE smt_cfg_type);

/***********************************************************
*  Function: insert_ap_cfg_err_log
*  Desc:	 insert a "log sequence" write to flash
*  Input:	 ap_cfg_type
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET insert_ap_cfg_err_log(APP_CFG_RP_CODE ap_cfg_type);


/***********************************************************
*  Function: get_cfg_report_log
*  Desc:	 get cfg log flag
*  Input:	 null
*  Return:	 LOG_SEQ_LOG_FILE_RP
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
LOG_SEQ_LOG_FILE_RP* get_cfg_report_log(VOID);
/***********************************************************
*  Function: log_seq_delete_log
*  Desc:	 log_seq_delete_log
*  Input:	 log_file  num 
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET log_seq_delete_log(IN LOG_SEQ_RP_LOG_FILE log_file,IN CHAR file_num);
/***********************************************************
*  Function: log_seq_read_log_data
*  Desc:	 log_seq_read_log_data
*  Input:	 log_file  num 
*  OUTput:	 log_data  data_len 
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET log_seq_read_log_data(IN LOG_SEQ_RP_LOG_FILE log_file,IN CHAR file_num,OUT CHAR** log_data,OUT UINT*len);
/***********************************************************
*  Function: set_log_rp_file_rst
*  Desc:	 set_log_rp_file_rst
*  Return:	 void
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
VOID set_log_rp_file_rst(VOID);

/***********************************************************
*  Function: set_http_err_log
*  Desc:	 insert a "log sequence"  write to flash
*  Input:	 http_type  err_code
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET set_http_err_log(CHAR*api_name,OPERATE_RET op_ret);

/***********************************************************
*  Function: log_seq_upload_realtime_log
*  Desc:	 log_seq_upload_realtime_log
*  Input:	 
*  Return:	 OPRT_OK: success  Other: fail
***********************************************************/
__LOG_SEQ_ADAPT_EXT \
OPERATE_RET log_seq_upload_realtime_log(VOID);

#endif

#ifdef __cplusplus
}
#endif
#endif

