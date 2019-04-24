#ifndef _SMART_LINK_H
#define	_SMART_LINK_H



/***********************************************************/
#define		ACTIVE_VERSION_V3			//V3版本激活流程
//#define		DEBUG_SMARTLINK_LOG					//开启日志
//#define	SCAN_ALL_CHANNEL				//扫描所有信道

/***********************************************************/
#ifdef __SAMRT_LINK_GLOBALS
	#define		SMART_LINK_EXTERN
#else
	#define		SMART_LINK_EXTERN 	extern
#endif

/***********************************************************/
#include "com_def.h"
#include "mem_pool.h"
#include "sys_adapter.h"
#include "wmtime.h"
//
#include "aes_inf.h"
#include "gw_intf.h"
#include "tuya_ws_db.h"
#include "cJSON.h"
//
#include "lwip/sockets.h"
//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
//

/***********************************************************/
#define		MAX_SCAN_WLAN_AP				32				//最大扫描AP数
#define 	MLAN_MAC_ADDR_LENGTH    		6
#define 	MLAN_MAX_SSID_LENGTH    		33
#define 	WLAN_PSK_MAX_LENGTH				65
#define	 	REGISTER_MAX_LENGTH				33

#pragma pack(1)
typedef struct{
	uint8_t region[2];
	uint8_t token[8];
	uint8_t key[4];
} REGIST_TYPE_S;
#pragma pack()


typedef enum{
	SNIFFER_STATUS_CONTINUE = 1,							//继续收包
	SNIFFER_STATUS_MULTICAST,								//组播包
	SNIFFER_STATUS_CHANNEL_LOCKED,							//锁定信道,停止信道切换
	SNIFFER_STATUS_SUCCESS,									//正确的数据包,清超时定时器
	SNIFFER_STATUS_CRC_ERR,									//CRC错误	
	SNIFFER_STATUS_COMPLETE,								//接收完成
} SNIFFER_STATUS_E;	 


typedef enum {
	SMARTLINK_STEP_HEAD = 0,
	SMARTLINK_STEP_MAGIC,
	SMARTLINK_STEP_PREFIX,
	SMARTLINK_STEP_DATA,
	SMARTLINK_STEP_FINISH,
	SMARTLINK_STEP_END,
	SMARTLINK_STEP_TLINK,
} TYPE_SMARTLINK_STEP;

typedef struct {
	xSemaphoreHandle semaphore_smartlink;
	xSemaphoreHandle semaphore_scan_wlan;
	xTimerHandle timer_channel_poll;
	xTimerHandle timer_receive_overtime;

	uint8_t prov_scan_channel[14];
	uint8_t stop_channel_pool;
#ifdef SCAN_ALL_CHANNEL
	uint8_t calc_channel[14];
#endif
	//
	uint8_t *airkiss_buffer;
} SMARTLINK_INFO;

#pragma pack(1)
typedef struct {
	//
	uint8_t bssid[6];
	uint8_t source[6];
	//
	uint8_t state;
	int8_t security;
	SMARTLINK_TYPE link_type;
	//
	CONFIG_PATH_E config_path;
	CONFIG_SOURCE_E config_source;
	uint8_t config_time;
	char ssid[MLAN_MAX_SSID_LENGTH];
	char plain_pass[WLAN_PSK_MAX_LENGTH];
	char regist_code[REGISTER_MAX_LENGTH];
	//
	uint8_t airkiss_random;
} LINK_CFG_S;
#pragma pack()

#pragma pack(1)
typedef struct {
	unsigned char channel;
	signed char rssi;
	unsigned char bssid[6];
} INFO_SACN_WLAN_S;
#pragma pack()


typedef VOID(*WIFI_FUNCTION_TEST)(OPERATE_RET *ret,CHAR *rssi_result);
/***********************************************************
*	Function: get_smartlink_info
*	Input: 
*	Output: 
*	Return: 
***********************************************************/
SMARTLINK_INFO * get_smartlink_info(VOID);

/***********************************************************
*	Function: get_link_cfg
*	Input: 
*	Output: 
*	Return: 
***********************************************************/
LINK_CFG_S * get_link_cfg(VOID);

/***********************************************************
*	Function: get_current_wifi_channel
*	Input: none
*	Output: none 
*	Return: none
***********************************************************/
uint8_t get_current_wifi_channel(VOID);

/***********************************************************
*	Function: get_current_wifi_channel
*	Input: none
*	Output: none 
*	Return: none
***********************************************************/
VOID set_current_wifi_channel(uint8_t channel);

/***********************************************************
*	Function: get_scan_wlan_info
*	Input: none
*	Output: none 
*	Return: none
***********************************************************/
INFO_SACN_WLAN_S *get_scan_wlan_info(VOID);

/***********************************************************
*  Function: smartlink_thrd
*  Input: none
*  Output: none 
*  Return: none
***********************************************************/
VOID smartlink_thrd(PVOID pArg);

/***********************************************************
*  Function: start_wifi_function_test
*  Input: none
*  Output: none 
*  Return: none
***********************************************************/
OPERATE_RET start_wifi_function_test(UCHAR *ssid,WIFI_FUNCTION_TEST test_cb);



#endif

