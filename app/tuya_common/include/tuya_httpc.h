/***********************************************************
*  File: tuya_httpc.h
*  Author: nzy
*  Date: 20150527
***********************************************************/
#ifndef _TUYA_HTTPC_H
    #define _TUYA_HTTPC_H

    #include "com_def.h"
    #include "sys_adapter.h"
    #include "mem_pool.h"
    #include "error_code.h"
    #include "com_struct.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __TUYA_HTTPC_GLOBALS
    #define __TUYA_HTTPC_EXT
#else
    #define __TUYA_HTTPC_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

#if (ENABLE_8266TLS == 1)

    //强制预发
    //#define TY_SMART_DOMAIN_AY "https://a3-cn.wgine.com/gw.json"		//China
    //线上
    #define TY_SMART_DOMAIN_AY "https://a3.tuyacn.com/gw.json"		//China
    #define TY_SMART_DOMAIN_AZ "https://a3.tuyaus.com/gw.json"		//American
    #define TY_SMART_DOMAIN_EU "https://a3.tuyaeu.com/gw.json"		//Europe
    #define TY_SMART_MQTT      "mq.gw.tuyacn.com"
    #define TY_SMART_MQTTBAK   "mq.gw1.tuyacn.com"


#else

    #if 0
    //预发
    #define TY_SMART_DOMAIN_AY "http://a.gw.cn.wgine.com/gw.json"
    #define TY_SMART_DOMAIN_AZ "http://a.gw.getairtake.com/gw.json"
    #define TY_SMART_DOMAIN_EU "http://a.gw.tuyaeu.com/gw.json"					//Europe
    #define TY_SMART_MQTT "mq.mb.cn.wgine.com"
    #define TY_SMART_MQTTBAK "mq.mb.cn.wgine.com"
    #else
    //线上
    #define TY_SMART_DOMAIN_AY "http://a.gw.tuyacn.com/gw.json"                  //China
    #define TY_SMART_DOMAIN_AZ "http://a.gw.tuyaus.com/gw.json"					//American
    #define TY_SMART_DOMAIN_EU "http://a.gw.tuyaeu.com/gw.json"					//Europe
    #define TY_SMART_MQTT      "mq.gw.tuyacn.com"
    #define TY_SMART_MQTTBAK   "mq.gw1.tuyacn.com"
    #endif

#endif

#define WX_TIMEZONE "+08:00"

// gw interface
#define TI_GW_RESET "s.gw.reset" // gw reset
#define TI_GW_INFO_UP "s.gw.update" // update gw base info
#define TI_DEV_INFO_UP "s.gw.dev.update" // dev info upgrade

// device timer interface
#define TI_GET_GW_DEV_TIMER_COUNT "s.gw.dev.timer.count" //get gw or dev timer count
#define TI_GET_GW_DEV_TIMER "tuya.device.timer.get" //get gw or dev star
#define TI_DEV_TIMER_LOG "tuya.device.timer.log.upload" // tuya device timer log

// firmware upgrade by gw
#define TI_FW_STAT "s.gw.upgrade.updatestatus"
#define TI_FW_SELF_UG_INFO "tuya.device.upgrade.silent.get"
#define TI_FW_UG_INFO_NEW "tuya.device.upgrade.get"

#define TI_GW_DEV_PK_ACTV "s.gw.dev.pk.active" // product Id acktive process
#define TI_GW_DEV_FK_ACTV "s.gw.dev.fk.active" // oem productKey active process
#define TI_GW_DEV_PK_ACTV_WX "s.gw.dev.pk.active.wx" // WeChat acktive process
#define TI_GW_TOKEN_GET "s.gw.token.get"    //gatewaye token get data

#define TI_GW_EXIST "s.gw.exist" // gateway is exist?
#define TI_UG_RST_LOG "atop.online.debug.log" // ug log
#define TI_GW_GET_WTH "tuya.device.public.data.get" //weather data get
#define TI_UG_RUNSTAT_LOG "tuya.device.log.report" // ug runstat

#define TI_GW_GET_EXT_CFG "tuya.device.extension.config.get"
#define TI_GW_DYN_CFG_GET "tuya.device.dynamic.config.get"
#define TI_GW_DYN_CFG_ACK "tuya.device.dynamic.config.ack"
#define TI_GW_GET_ASTRO   "tuya.p.weather.sunriseset.list"
#define TI_GET_CTEI_CODE "tuya.device.sn.info.get" // get cteicode

// HTTP/HTTPS  POST 1.0
#define TI_GET_REGION_CFG_GET "tuya.device.region.config.get"

typedef struct{
    CHAR std_TimeZone[TIME_ZONE_LEN+1];
    CHAR dst_intvals[INTR_LEN_LMT+1];
    CHAR cfg_ackid[CFG_VER_LEN+1];
}TIMER_CFG_S;

//根据需要增加各种升级错误状态              wangwei
typedef enum{
    OTA_NORMAL,
    OTA_PACK_HEAD_ERR,   /*SOC固件头错误，请检查bin文件是否正确*/
    OTA_GET_CONTENT_ERR, /*http拉包失败*/
    OTA_CRC_ERR,         /*crc校验失败*/
    OTA_VER_FORMAT_ERR,  /*非法版本号*/
    OTA_UART_SEND_TIMEOUT, /*MCU升级串口发包超时失败*/
    OTA_FW_SIZE_OVER_MAX, /*gw升级包超过480kb限制*/
    OTA_HMAC_DISMATCH, /*固件包hmac计算不匹配*/
    OTA_FLASH_BIN_CHECKSUM_ERR, /* flash中bin文件和校验失败 */
}OTA_EXECPTION;

typedef OPERATE_RET(*ASTRO_TIMER_CB)(IN CONST BYTE *data,\
                                     IN CONST UINT len);

/***********************************************************
*  Function: httpc_ug_log
*  Input:
*  Output:
*  Return: OPERATE_RET
*  note: 上报设备日志，如重启原因，离线时间，联网状态等
***********************************************************/
OPERATE_RET httpc_ug_log(VOID);

/***********************************************************
*  Function: httpc_ug_log_custom
*  Input:
*  Output:
*  Return: OPERATE_RET
*  note: 上报设备日志，如重启原因，离线时间，联网状态等
***********************************************************/
OPERATE_RET httpc_ug_log_custom(IN CONST CHAR *log, IN CONST INT log_len);

/***********************************************************
*  Function: httpc_aes_init
*  Input:
*  Output:
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_aes_init(VOID);

/***********************************************************
*  Function: httpc_aes_init
*  Input:
*  Output:
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_aes_set(IN CONST BYTE *key,IN CONST BYTE *iv);

/***********************************************************
*  Function: httpc_gw_active
*  Input:
*  Output:
*  Return: OPERATE_RET
***********************************************************/
#if (defined(SW_VER) && defined(DEF_NAME) && defined(DEV_ETAG))
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_active();
#endif

/***********************************************************
*  Function: http_gw_get_cloud_url
*  Input:
*  Output:
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET http_gw_get_cloud_url(VOID);

/***********************************************************
*  Function: httpc_gw_reset
*  Input:
*  Output:
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_reset(VOID);

/***********************************************************
*  Function: httpc_gw_update
*  Input:
*  Output:
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_update(VOID);

/***********************************************************
*  Function: httpc_dev_update
*  Input:
*  Output:
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_dev_update(IN CONST DEV_DESC_IF_S *dev_if);

/***********************************************************
*  Function: httpc_get_fw_ug_info
*  Input: etag
*  Output: p_fw_ug
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_get_fw_ug_info(IN CONST CHAR *etag,OUT FW_UG_S *p_fw_ug);

/***********************************************************
*  Function: httpc_get_self_fw_ug_info
*  Input: etag
*  Output: p_fw_ug
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET httpc_get_self_fw_ug_info(OUT FW_UG_S *p_fw_ug);

/***********************************************************
*  Function: httpc_get_fw_ug_info_new
*  Input: etag
*  Output: p_fw_ug
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_get_fw_ug_info_new(IN CONST UPGD_TYPE_E type,OUT FW_UG_S *p_fw_ug);

/***********************************************************
*  Function: httpc_up_fw_ug_stat
*  Input: etag devid stat
*  Output:
*  Return: OPERATE_RET
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_up_fw_ug_stat(IN CONST CHAR *devid,\
                                IN CONST FW_UG_STAT_E stat);

/***********************************************************
*  Function: httpc_gw_dev_active
*  Input: dev_if
*  Output:
*  Return: OPERATE_RET
*  说明:用于单品设备绑定+实体网关附带虚拟设备绑定
***********************************************************/
#if (defined(SW_VER) && defined(DEF_NAME) && defined(DEV_ETAG))
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_dev_active(IN CONST DEV_DESC_IF_S *dev_if);
#endif

/***********************************************************
*  Function: httpc_gw_dev_active_pk
*  Input: dev_if
*  Output:
*  Return: OPERATE_RET
*  说明:通过prodect_key激活,用于单品设备绑定+实体网关附带虚拟设备绑定
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_dev_active_pk(IN CONST DEV_DESC_IF_S *dev_if);

/***********************************************************
*  Function: httpc_gw_dev_timer_count
*  Input: posix 本地定时时间戳
*  Output:
*  Return: OPERATE_RET
*  None:
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_dev_timer_count(IN CONST INT posix, OUT INT *count, OUT INT *fechTime);

/***********************************************************
*  Function: httpc_gw_dev_timer
*  Input: offset 偏移位置
*         limit  数量
*  Output: scharr 定时记录,格式[{},{},{}]
*  Return: OPERATE_RET
*  参照返回值列表
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_gw_dev_timer(IN CONST INT offset, IN CONST INT limit, CHAR **scharr);

/***********************************************************
*  Function: httpc_dev_timer_log
*  Input: timer_info 定时执行信息
*         [{"id":"xxxx","dps":{"1",true},"dpsTime":157889543}]
*  Output:
*  Return: OPERATE_RET
*  参照返回值列表
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_dev_timer_log(IN CONST CHAR *timer_info);

/***********************************************************
*  Function: httpc_get_astor_timer
*  Input:    plon  经度
             plat  维度
             pfun_cb 处理接收数据的回调函数
*  Output:
*  Return: OPERATE_RET
*  Note:
***********************************************************/
__TUYA_HTTPC_EXT \
OPERATE_RET httpc_get_astro_timer(IN CONST CHAR *plon,IN CONST CHAR *plat,
                                        IN CONST ASTRO_TIMER_CB pfun_cb);

/***********************************************************
*  Function: httpc_get_weather
*  Input: paraIn 参数数组,如:["w.temp","w.pm25"]
*  Output: ppOut 参数结果,如{"w.temp":20,"w.pm25":17}
*  Return: OPERATE_RET
*  参照返回值列表
***********************************************************/
OPERATE_RET httpc_get_weather(IN CHAR *paraIn,OUT CHAR **ppOut,OUT UINT *nxtTime);

OPERATE_RET httpc_dynamic_cfg_get(TIMER_CFG_S *cfg_t);

OPERATE_RET httpc_dynamic_cfg_ack(CHAR *ackid);

VOID set_ota_errno(OTA_EXECPTION type);

/***********************************************************
*  Function: httpc_get_chcode
*  Input:
*  Output: ch_code
*  Return: OPERATE_RET
*  参照返回值列表
***********************************************************/
OPERATE_RET httpc_get_chcode(INOUT CH_CODE_ST *ch_code);

/***********************************************************
*  Function: httpc_post_ctcc_code
*  Input: url post地址
*         send_buf post数据
*         send_len post数据长度
*  Output: ch_code
*  Return: OPERATE_RET
*  参照返回值列表
***********************************************************/
OPERATE_RET httpc_post_ctcc_code(IN CONST CHAR* url,IN CONST CHAR* send_buf, IN CONST UINT send_len);


#ifdef __cplusplus
}
#endif
#endif

