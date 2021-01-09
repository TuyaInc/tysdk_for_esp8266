/**
 * @File: app_switch.h
 * @Author: caojq
 * @Last Modified time: 2019-10-31
 * @Description: 电量统计平台免开发方案优化代码
 */

#ifndef  __APP_SWITCH_H__
#define  __APP_SWITCH_H__
#include "tuya_smart_api.h"
#include "com_def.h"
#ifdef  _APP_SWITCH_GLOBAL
    #define  __APP_SWITCH_EXT
#else
    #define  __APP_SWITCH_EXT extern
#endif

#define DPID_NOT_EXIST (-1)

//IO配置定义
#ifndef __IO_TYPE_CONFIG__
#define __IO_TYPE_CONFIG__
typedef enum 
{
    IO_DRIVE_LEVEL_HIGH,        // 高电平有效
    IO_DRIVE_LEVEL_LOW,         // 低电平有效
    IO_DRIVE_LEVEL_NOT_EXIST    // 该IO不存在
}IO_DRIVE_TYPE;
typedef struct
{
    IO_DRIVE_TYPE type; // 有效电平类型
    UCHAR pin;  // 引脚号
}IO_CONFIG;
#endif

/***********************************************************
*************************variable define********************
************************************************************/
typedef enum{
    INIT_CH_OPEN,  //上电默认通电
    INIT_CH_CLOSE, //上电默认断电
    INIT_CH_MEM    //上电后断电记忆
}INIT_CH_STAT;

typedef struct
{
    IO_CONFIG       relay;          // 继电器
    LED_HANDLE      relay_handle;   // 继电器句柄
    IO_CONFIG       button;         // 控制按键
    IO_CONFIG       led;            // 状态指示灯
    LED_HANDLE      led_handle;     // 状态指示灯句柄
    UINT             dpid;           // 该通道绑定的dpid
    UINT            cd_dpid;        // 该通道绑定的倒计时dpid 小于0表示不存在
    INT             cd_sec;         // 通道倒计时 -1 停止
    BOOL            stat;           // 通道状态 TRUE - 有效; FALSE - 无效
    INIT_CH_STAT    init_ch_stat;
}CTRL_CHANNEL_CONFIG;

typedef enum{
    WFL_OFF,    //wifi灯常灭
    WFL_ON,     //wifi灯常开
    WFL_DIR_RL,  //wifi灯指示继电器
    WFL_FLASH_VERY_FAST,
    WFL_FLASH_FAST,
    WFL_FLASH_SLOW,
    WFL_ENTER_PT,
    WFL_END_PT
}WFL_STAT;

typedef struct
{
    IO_CONFIG               wfl_io;     //wifi灯GPIO口
    LED_HANDLE	            wfl_handle; //wifi灯句柄
    WFL_STAT                wfl_cs;     //wifi灯连接时指示状态
    WFL_STAT                wfl_ucs;    //wifi灯长时间未连接时指示状态
    unsigned char           press_time;     //长按触发重置时间
    unsigned char           wcm_mode;
}WIFI_LED;

typedef struct{
    IO_CONFIG               tled;   //总继电器指示灯
    IO_CONFIG               tbt;    //总按键
    LED_HANDLE              tled_handle;
}TOTAL_CH;
// HW_TABLE结构体类型
// 抽象描述硬件配置
typedef struct
{
    UCHAR                   channel_num;
    TOTAL_CH                tch;            //总控通道
    WIFI_LED                wf_led;         //wifi指示灯
    CTRL_CHANNEL_CONFIG		*channels;			// 通道列表 *!* 不要重新指向其他位置
}HW_TABLE;

//事件类型定义
typedef enum{
    CHAN_EVENT_CLOSE= 0,   //通道关闭+上报+记忆
    CHAN_EVENT_OPEN,    //通道打开+上报+记忆
    CHAN_EVENT_TOGGLE   //通道取反+上报+记忆
}CHAN_EVENT_TYPE;

typedef enum{
    KEY_ALL_PRIOR_CLOSE = 0,//优先全关
    KEY_ALL_PRIOR_OPEN,//优先全开
    KEY_SINGLE_TOGGLE//控制单个取反
}KEY_TYPE;

typedef enum{
    CTRL_SW_CLOSE = 0,      //通道关闭
    CTRL_SW_OPEN,    //通道打开
    CTRL_SW_TOGGLE,     //通道取反
    CTRL_SW_STAT    //根据通道之前状态控制通道
}CTRL_SW;


typedef enum{
    APP_SW_MODE_NORMAL,     //开关正常工作模式
    APP_SW_MODE_PRODTEST    //开关产测模式
}APP_SW_MODE;

__APP_SWITCH_EXT \
VOID app_send_press_deal(IN UINT ch_dp_id,IN BOOL ch_state);//放到app下发开关状态dp点处理语句之中

__APP_SWITCH_EXT \
VOID app_send_countdown_deal(IN UINT cd_dp_id,IN INT cd_secs);//放到app下发倒计时dp点处理语句之中

__APP_SWITCH_EXT \
OPERATE_RET app_switch_init(IN APP_SW_MODE mode);//放到flash初始化之后

__APP_SWITCH_EXT \
OPERATE_RET upload_all_switch_state(VOID);//放到上电同步函数之中

__APP_SWITCH_EXT \
VOID ctrl_switch_state(IN UCHAR channel_num,IN CTRL_SW ctrl_sw);

__APP_SWITCH_EXT \
VOID set_pt_key_en(IN BOOL if_en);          //允许产测时使用按键

__APP_SWITCH_EXT \
VOID set_wfl_state(IN WFL_STAT wfl_stat);//设定WIFI灯状态

__APP_SWITCH_EXT \
OPERATE_RET save_pt_end_flag(IN INT state);

__APP_SWITCH_EXT \
OPERATE_RET get_pt_end_flag(OUT INT *state);

__APP_SWITCH_EXT \
VOID total_channel_event_deal(IN BOOL on_or_off);

__APP_SWITCH_EXT \
BOOL judge_any_sw(IN BOOL on_or_off);

__APP_SWITCH_EXT \
VOID reset_power_stat(VOID);

#endif


