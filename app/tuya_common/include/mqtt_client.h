/***********************************************************
*  File: mqtt_client.h
*  Author: nzy
*  Date: 20150526
***********************************************************/
#ifndef _MQTT_CLIENT_H
    #define _MQTT_CLIENT_H

    #include "com_def.h"
    #include "libemqtt.h"
    #include "sys_adapter.h"
    #include "mem_pool.h"
    #include "com_struct.h"
    #include "smart_wf_frame.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __MQTT_CLIENT_GLOBALS
    #define __MQTT_CLIENT_EXT
#else
    #define __MQTT_CLIENT_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef VOID (*MQ_CALLBACK)(BYTE *data,UINT len);

#define MQ_DOMAIN_NAME
#ifdef MQ_DOMAIN_NAME // ÓòÃû
#define MQ_DOMAIN_ADDR "mq.gw.airtakeapp.com"
#define MQ_DOMAIN_ADDR1 "mq.gw1.airtakeapp.com"
#define MQ_DOMAIN_PORT_TLS_PSK  8886
#define MQ_DOMAIN_PORT_NO_TLS   1883

#else
#define MQ_DOMAIN_ADDR "192.168.0.19"
#define MQ_DOMAIN_PORT 1883
#endif

// mqtt protocol
#define PRO_DATA_PUSH 4 //add by Anbylk 20160417
#define PRO_CMD 5
#define PRO_ADD_USER 6
#define PRO_DEL_USER 7
// #define PRO_FW_UG_CFM 10 // ·ÏÆú
#define PRO_GW_RESET 11
//#define PRO_HB_REQ 12
#define PRO_TIMER_UG_INF 13 //timer schema upgrage inform
#define PRO_UPGD_REQ 15 // upgrade request
#define PRO_UPGE_PUSH 16 // upgrade progress push
#define PRO_IOT_DA_REQ 22 // iot data request
#define PRO_IOT_DA_RESP 23 // iot data respond
#define PRO_CMD_ACK  26    /* dev -> cloud  dev send ackId to cloud */
#define PRO_MQ_EXT_CFG_INF 27 //extension cfg
#define PRO_UG_SUMER_TABLE 41 //ug sumer timer table
#define PRO_MQ_QUERY_DP  31  /* cloud -> dev query dp stat */
#define PRO_CMD_THINGCONFIG     51      //mq_thingconfig

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: mqtt_client_init
*  Input: alive second
*  Output:
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
VOID mq_client_init(IN CONST CHAR *clientid,\
                    IN CONST CHAR *username,\
                    IN CONST INT alive);

/***********************************************************
*  Function: mq_client_start
*  Input:
*  Output:
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mq_client_start(IN CONST CHAR *topic,IN CONST MQ_CALLBACK callback);

__MQTT_CLIENT_EXT \
VOID mq_disconnect(VOID);

/***********************************************************
*  Function: mq_client_publish
*  Input: msg--> {"devId":"002debug5ccf7f8318b1","dps":{"3":"00f600f400ff00b700b70084"}}
*  Output:
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mq_client_publish(const char* msg,IN CONST UINT seq);

__MQTT_CLIENT_EXT \
OPERATE_RET mq_client_upgd_progress_publish(IN CONST UINT percent);

__MQTT_CLIENT_EXT \
OPERATE_RET mq_client_publish_obj_data(IN CONST BYTE dpid, IN CONST STATS_DATA_S *p_data);

/***********************************************************
*  Function: mqtt_client_smart_pub
*  Input:
*  Output:
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_client_smart_pub(IN CONST UINT pro,IN CONST BYTE *data);

/***********************************************************
*  Function: mqc_obj_resp_ackid
*  Input:
*  Output:
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqc_obj_resp_ackid(IN CONST CHAR *ackid);


#ifdef __cplusplus
}
#endif
#endif

