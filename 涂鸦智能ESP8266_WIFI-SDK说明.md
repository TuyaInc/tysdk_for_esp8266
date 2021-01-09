## 版本记录

| 版本  | 编写/修订说明 | 修订日期 | 备注 |
| :---- | :------------ | :------- | :--- |
| 1.0.0 | 创建文档      | 20200310 |      |

## 概述

涂鸦 Wi-Fi ESP8266 SDK是基于涂鸦云对 ESP8266 进行深度定制开发的 SDK。本说明主要描述基本框架、API 接口及部分原理。

## 开发环境

涂鸦 8266 SDK  4.0.0以上版本无需手动安装交叉编译器。


## 目录结构
```
    |- demos
    |       |- demo_name
    |- apps
    |       |- project_name
    |       |       |- include
    |       |       |- user
    |- sdk
    |       |- esp8266
    |       |       |- fac_include
    |       |       |- tuya_include
    |       |- lib
    |       |       |- fac_lib
    |       |       |- tuya_lib
    |- platforms
    |       |- xtensa-lx106-elf
    |- build_app.sh
```

1）demos 目录为 SDK 应用例程

2）apps 目录为开发者应用目录

app_name 为项目应用名称（更改为项目名称），目录中 include 和 user 目录是头文件和源文件目录，且目录名称不能改变。无此目录可手动添加。

3）sdk 目录为头文件和库文件目录

4）platforms 为平台相关目录，包括编译工具链，编译脚本及其他工具。

### 涂鸦 8266 SDK 编译方法

进入顶层目录

1）编译有日志输出的固件，例如 sample_pk，版本号 1.0.0
./build_app.sh demos/sample_pk sample_pk 1.0.0 debug
2）编译无日志输出的固件，例如 sample_pk，版本号 1.0.0
./build_app.sh demos/sample_pk sample_pk1.0.0 

生成物： xxx_DOUT_TLS_1.0.0.bin   8285全量烧录文件

​				xxx_QIO_TLS_1.0.0.bin        8286全量烧录文件

​				xxx_UA_TLS_1.0.0.bin          用户区烧录文件

​				xxx_UG_TLS_1.0.0.bin          OTA升级文件

## 应用实现接口

1）设置应用的名称和版本号，此函数直接放在应用中即可

```
VOID set_firmware_tp(IN OUT CHAR *firm_name, IN OUT CHAR *firm_ver)
{
	strcpy(firm_name,App_BIN_NAME);
	strcpy(firm_ver,USER_SW_VER);
	return;
}
```

2）快速启动应用回调

```
VOID pre_app_init(VOID)
{
    //设置不涉及PSM扇区操作的上电快速操作
}
```

3）应用初始化回调

```
VOID App_init(VOID)
{
   //wifi 模式
   //设置打印串口
}
```

4）gpio 测试回调函数

```
BOOL gpio_func_test(VOID)
{
	return TRUE;
}
```

5）应用入口函数

```
OPERATE_RET device_init(VOID)
{
	//框架初始化
	//PSM 扇区注册
	//定时器、按键功能创建等
}
```

## 框架基本接口(接入涂鸦云需使用)

#### 获取 SDK 版本号

**函数原型**

```
#include “tuya_smart_api.h”
CHAR *tuya_get_sdk_ver(VOID)
```

**功能说明**

获取 SDK 版本号

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型 | 说明                      |
| ---------- | ------------------------- |
| CHAR*      | 获取 SDK 版本号,如"1.0.0" |

### 设置 Wi-Fi 配置模式

**函数原型**

```
#include “tuya_smart_api.h”
VOID tuya_app_cfg_set(IN CONST WF_CFG_MTHD_SEL mthd, APP_PROD_CB callback);
```

**功能说明**

设置 Wi-Fi 配置模式

**参数说明**

| 参数名称                |      | 说明                                                         |
| ----------------------- | ---- | ------------------------------------------------------------ |
| WF_CFG_MTHD_SEL    mthd | IN   | WCM_OLD 普通模式 WCM_LOW_POWER 低功耗模式 WCM_SPCL_MODE 特殊配网模式 WCM_OLD_CPT 兼容模式 WCM_LOW_POWER_AUTOCFG 低功耗自动模式 WCM_SPCL_AUTOCFG 特殊配网自动模式 |
| App_PROD_CB callback    | IN   | 回调函数设置，WCM_OLD 模式无产测回调，需设置成NULL，其他模式需设置回调函数 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

**注意点**

默认模式为普通模式，此函数必须在 app_init 中调用。

1)普通模式，无产测回调，调用方法 tuya_app_cfg_set(WCM_OLD, NULL); 

2)低功耗模式，必须设置产测回调 tuya_app_cfg_set(WCM_LOW_POWER, prod_test); 

3)特殊配网模式，必须设置产测回调 tuya_app_cfg_set(WCM_SPCL_MODE, prod_test); 

4)兼容模式，必须设置产测回调 tuya_app_cfg_set(WCM_OLD_CPT, prod_test);

5)低功耗自动模式，必须设置产测回调 tuya_app_cfg_set(WCM_LOW_POWER_AUTOCFG, prod_test); 

6)特殊配网自动模式，必须设置产测回调 tuya_app_cfg_set(WCM_SPCL_AUTOCFG, prod_test); 

各种工作模式状态切换流程： 

1）普通模式 

![WCM_OLD.jpg](https://images.tuyacn.com/goat/20200312/e1825ba584324f3c9bab83c0f72d2200.jpg)

2）低功耗模式/ 特殊配网模式

![WCM_SPCL_MODE.jpg](https://images.tuyacn.com/goat/20200312/b012a63c2af94936a1aff0a6e07c3d9f.jpg)

3）兼容模式/低功耗自动模式/特殊配网自动模式

![WCM_SPCL_AUTOCFG.jpg](https://images.tuyacn.com/goat/20200312/e1ba4d7f7970400eaf6fa01af26e7d05.jpg)



### 设备初始化

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_device_init(IN CONST CHAR *product_id,IN CONST SMART_FRAME_CB cb,CONST CHAR *App_ver);
```

**功能说明**

注册数据处理函数和应用版本号

**参数说明**

| 参数名称          | 参数类型 | 说明                                                         |
| ----------------- | -------- | ------------------------------------------------------------ |
| CHAR* product_id  | IN       | 产品 ID                                                      |
| SMART_FRAME_CB cb | IN       | 命令回调函数指针, VOID (*)(SMART_CMD_E cmd,cJSON *root)  <1>  cmd  命令类型 0 表示局域网下发的命令 1 表示外网下发的命令 <2> root  命令数据 例，{"1":100,"2":200}，其中"1"和"2"为数据 ID (dpid) 编号,100 和 200 为对应dpid的值 |
| CHAR* App_ver     | IN       | 应用版本号，如"1.0.0"                                        |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

**说明**

设备初始化具体流程如下

![img](https://images.tuyacn.com/goat/20191101/7e828db49a0e4c1ab56ed12585954611.png) 

### 设备首次激活成功回调

**函数原型**

```
#include “tuya_smart_api.h”
VOID tuya_active_reg(IN CONST SYN_DATA_CB callback)
```

**功能说明**

注册产品功能，设备首次激活会调用 callback

**参数说明**

| 参数名称             | 参数类型 | 说明                                       |
| -------------------- | -------- | ------------------------------------------ |
| SYN_DATA_CB callback | IN       | 命令回调函数指针, VOID(*SYN_DATA_CB)(VOID) |

**返回值**

| 返回值类型 | 说明     |
| ---------- | -------- |
| 无         | 详见附录 |

### 获取设备ID

**函数原型**

```
#include “tuya_smart_api.h”
CHAR *tuya_get_devid(VOID)
```

**功能说明**

注册产品功能，设备首次激活会调用 callback

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型 | 说明        |
| ---------- | ----------- |
| CHAR*      | 返回设备 ID |

### 数据上报

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_obj_dp_report(IN CONST CHAR *data)
```

**功能说明**

数据上报函数

**参数说明**

| 参数名称   | 参数类型 | 说明                                 |
| ---------- | -------- | ------------------------------------ |
| CHAR* data | IN       | data 上报的数据,例 {"1":100,"2":200} |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

**说明**

调用此接口固件会保存各 DP 的数据状态，如再次上传的 DP 数据与保存的状态相同则忽略上传，推荐使用该函数，可使 APP、云端、固件三方性能最佳，数据上报流程图

![img](https://images.tuyacn.com/goat/20191101/22dd5cfde92244d291d6cd41e0ffc11f.png) 

### 数据上报(透传)

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_obj_dp_trans_report(IN CONST CHAR *data)
```

**功能说明**

数据上报函数(透传)，数据透传到服务器，固件内部不做状态处理。

**参数说明**

| 参数名称  | 参数类型 | 说明                                |
| --------- | -------- | ----------------------------------- |
| CHAR*data | IN       | data 上报的数据,例{"1":100,"2":200} |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

### RAW数据上报

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_raw_dp_report(IN CONST BYTE dpid,IN CONST BYTE *data, IN CONST UINT len)
```

**功能说明**

RAW 数据上报，数据透传到服务器，固件内部不做状态处理。

**参数说明**

| 参数名称   | 参数类型 | 说明               |
| ---------- | -------- | ------------------ |
| BYTE dpid  | IN       | 功能点 dp 序号     |
| BYTE* data | IN       | 原始二进制数据     |
| UINT len   | IN       | 原始二进制数据长度 |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

### 恢复出厂设置

**函数原型**

```
#include “tuya_smart_api.h”
VOID tuya_dev_reset_factory(VOID)
```

**功能说明**

恢复出厂设置

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

**说明**

清除配网和设备信息 

1)设备已激活，调用会将设备重置成 smartconfig 配网状态并清除激活信息 

2)设备未激活，重复调用该函数会导致设备在 smartconfig、ap 配网状态来回切换

![img](https://images.tuyacn.com/goat/20191101/61c2cc4ea70c46c3a1b7018237aaed55.png)

### 恢复出厂设置至指定状态

**函数原型**

```
#include “tuya_smart_api.h”
VOID tuya_dev_reset_select(NW_CFG_MODE_E mode)
```

**功能说明**

清除配网和设备信息，恢复出厂设置至指定状态

**参数说明**

| 参数名称           | 参数类型 | 说明                                                   |
| ------------------ | -------- | ------------------------------------------------------ |
| NW_CFG_MODE_E mode | IN       | 配置状态  NW_SMART_CFG EZ状态  NW_AP_CFG AP状态 AP状态 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### 获取设备状态

**函数原型**

```
#include “tuya_smart_api.h”
GW_STAT_E tuya_get_gw_status(VOID)
```

**功能说明**

获取设备状态

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型 | 说明                                                         |
| ---------- | ------------------------------------------------------------ |
| GW_STAT_E  | UN_INIT = 0,未初始化，比如生产信息未写入 PROD_TEST, 产品产测模式 UN_ACTIVE,  未激活 ACTIVE_RD,  激活就绪态 STAT_WORK,  正常工作态 |

### 获取 Wi-Fi 工作状态

**函数原型**

```
#include “tuya_smart_api.h”
GW_WIFI_STAT_E tuya_get_wf_status(VOID)
```

**功能说明**

获取 Wi-Fi 状态

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型     | 说明                                                         |
| -------------- | ------------------------------------------------------------ |
| GW_WIFI_STAT_E | STAT_LOW_POWER = 0, 低功耗状态(射频处于关闭状态) STAT_UNPROVISION = 1, EZ状态(未配置) STAT_AP_STA_UNCONN, AP状态(未配置) STAT_AP_STA_CFG_UNC, AP 和 STA 混合,STA 未连接状态(保留) STAT_AP_STA_CONN, AP 和 STA混合,STA 已连接状态(保留) STAT_STA_UNCONN, STA 未连接状态 STAT_STA_CONN, STA已连接状态 |

### 获取 Wi-Fi 信号强度

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_get_wf_rssi(IN UCHAR *ssid, OUT CHAR *rssi)
```

**功能说明**

获取 Wi-Fi 信号强度

**参数说明**

| 参数名称   | 参数类型 | 说明      |
| ---------- | -------- | --------- |
| CHAR* ssid | IN       | ssid 名称 |
| CHAR* rssi | OUT      | rssi 强度 |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

### 

### 获取 Wi-Fi 信号强度

**函数原型**

```
#include “tuya_smart_api.h”
BOOL tuya_get_cloud_stat(VOID)
```

**功能说明**

获取 Wi-Fi 信号强度

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型 | 说明                     |
| ---------- | ------------------------ |
| BOOL       | TRUE 已连接 FALSE 未连接 |

### 获取设备升级状态

**函数原型**

```
#include “tuya_smart_api.h”
BOOL tuya_get_ug_stat(VOID)
```

**功能说明**

获取设备升级状态

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型 | 说明                     |
| ---------- | ------------------------ |
| BOOL       | TRUE 升级中 FALSE 未升级 |

### 获取本地时间

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_get_local_time(OUT struct tm *st_time)
```

**功能说明**

获取本地时间，设备激活后才能使用

**参数说明**

| 参数名称           | 参数类型 | 说明     |
| ------------------ | -------- | -------- |
| struct tm* st_time | OUT      | 本地时间 |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

### 注册PSM模块

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_psm_register_module(IN CONST CHAR *module_name,IN CONST CHAR *partition_key)
```

**功能说明**

注册PSM模块名称和扇区名称，每个扇区最多保存4K数据

**参数说明**

| 参数名称             | 参数类型 | 说明     |
| -------------------- | -------- | -------- |
| CHAR * module_name   | IN       | 模块名字 |
| CHAR * partition_key | IN       | 扇区名称 |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |



### 写入PSM数据

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_psm_set_single(IN CONST CHAR *module,IN CONST CHAR *variable,IN CONST CHAR *value)
```

**功能说明**

写入 PSM 数据，只支持字符串类型。

**参数说明**

| 参数名称           | 参数类型 | 说明     |
| ------------------ | -------- | -------- |
| CHAR * module_name | IN       | 模块名字 |
| CHAR * variable    | IN       | 变量名   |
| CHAR * value       | IN       | 变量值   |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

### 读取 PSM 数据

**函数原型**

```
#include “tuya_smart_api.h”
OPERATE_RET tuya_psm_get_single(IN CONST CHAR *module,IN CONST CHAR *variable,OUT CHAR *value,IN CONST unsigned max_len)
```

**功能说明**

读取 PSM 数据，只支持字符串类型。

**参数说明**

| 参数名称           | 参数类型 | 说明           |
| ------------------ | -------- | -------------- |
| CHAR * module_name | IN       | 模块名字       |
| CHAR * variable    | IN       | 变量名         |
| CHAR * value       | OUT      | 变量值         |
| unsigned max_len   | IN       | 缓冲区最大长度 |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

## 按键接口

按键使用流程如下

![img](https://images.tuyacn.com/goat/20191101/7e8490f41e49465cb4991aec29ebfe69.png) 

### 按键初始化

**函数原型**

```
#include “tuya_key_api.h”
OPERATE_RET tuya_kb_init(VOID)
```

**功能说明**

按键初始化

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

### 设置是否支持连击按键计数

**函数原型**

```
#include “tuya_key_api.h”
VOID tuya_set_kb_seq_enable(IN BOOL enable)
```

**功能说明**

设置是否支持连击按键计数(即 SEQ_KEY 类型)，如果不调用该接口则默认所有的按键处理均支持连击按键计数功能

**参数说明**

| 参数名称    | 参数类型 | 说明                                                         |
| ----------- | -------- | ------------------------------------------------------------ |
| BOOL enable | IN       | TRUE 支持连击按键计数事件，可触发 SEQ_KEY 类型事件； FALSE 不支持连击按键计数，仅支持触发 NORMAL_KEY、LONG_KEY 事件 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### 设置按键高电平有效

**函数原型**

```
#include “tuya_key_api.h”
VOID tuya_set_kb_detect_high_valid(BOOL is_high)
```

**功能说明**

设置按键高电平有效

**参数说明**

| 参数名称     | 参数类型 | 说明                                             |
| ------------ | -------- | ------------------------------------------------ |
| BOOL is_high | IN       | is_high <1> TRUE 高电平有效 <2> FALSE 低电平有效 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### 设置按键触发类型

**函数原型**

```
#include “tuya_key_api.h”
VOID tuya_set_kb_trig_type(IN CONST INT gpio_no,IN CONST KEY_TRIGGER_TP_E trig_ty,IN CONST BOOL down_trig_cont)
```

**功能说明**

设置按键触发类型，默认如果不调用此函数，则按键事件均为弹起触发类型。

**参数说明**

| 参数名称    | 参数类型 | 说明     |
| ----------- | -------- | -------- |
| INT gpio_no | IN       | 按键编号 |
| KEY_TRIGGER_TP_E trig_ty | IN       | trig_ty: <1> KEY_UP_TRIG 弹起触发 <2> KEY_DOWN_TRIG 按下触发，仅支持 NORMAL_KEY 以及 
 LONG_KEY |
| BOOL down_trig_cont      | IN       | down_trig_cont: <1> 仅当trig_ty == KEY_DOWN_TRIG 时有效 <2> 为 TURE 时 ，如果用户一直按着不放，则间隔 400ms 会再次出发 NORMAL_KEY 事件 <3>为  FALSE时 ，用户一直按着不放，仅触发一次 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### 注册按键回调函数

**函数原型**

```
#include “tuya_key_api.h”
OPERATE_RET tuya_kb_reg_proc(IN CONST INT gpio_no,IN CONST INT long_key_time,IN CONST KEY_CALLBACK call_back);
```

**功能说明**

注册按键回调函数

**参数说明**

| 参数名称               | 参数类型 | 说明                                                         |
| ---------------------- | -------- | ------------------------------------------------------------ |
| INT gpio_no            | IN       | 按键编号                                                     |
| INT long_key_time      | IN       | long_key_time:长按键触发时间(ms)，如设置为 0 则屏蔽长按键回调函数 |
| KEY_CALLBACK call_back | IN       | 回调函数 VOID(*)(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt) ， gpio_no  GPIO 序号 type 按键类型 <1> NORMAL_KEY 普通按键 <2> SEQ_KEY 说明:连击按键,仅对弹起触发的按键类型有效，比如快速按下两次回调参数 type == SEQ_KEY, cnt == 2 (cnt 表示连续点击次数) <3> LONG_KEY 长按按键 cnt 按键次数 |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

## LED 接口

LED 使用流程如下

![img](https://images.tuyacn.com/goat/20191101/72bddafd9e8b41dc9e4f01aac81952c4.png) 

### 创建 LED 控制句柄

**函数原型**

```
#include “tuya_led_api.h”
OPERATE_RET tuya_create_led_handle(IN CONST INT gpio_no,OUT LED_HANDLE *handle)
```

**功能说明**

创建 LED 控制句柄

**参数说明**

| 参数名称           | 参数类型 | 说明           |
| ------------------ | -------- | -------------- |
| INT gpio_no        | IN       | 按键编号       |
| LED_HANDLE *handle | IN       | handle LED句柄 |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

### LED 控制

**函数原型**

```
#include “tuya_led_api.h”
VOID tuya_set_led_type(IN CONST LED_HANDLE handle,IN CONST LED_LT_E type,IN CONST USHORT flh_mstime)
```

**功能说明**

led 初始化

**参数说明**

| 参数名称          | 参数类型 | 说明                                                         |
| ----------------- | -------- | ------------------------------------------------------------ |
| LED_HANDLE handle | IN       | handle LED句柄                                               |
| LED_LT_E type     | IN       | 控制类型 <1> OL_LOW LED 低电平 <2> OL_HIGH LED 高电平 <3> OL_FLASH_LOW LED 低电平闪烁 <4> OL_FLASH_HIGH LED 高电平闪烁 |
| USHORT flh_mstime | IN       | flh_mstime 闪烁间隔时间                                      |

**返回值**

| 返回值类型  | 说明     |
| ----------- | -------- |
| OPERATE_RET | 详见附录 |

## UART 接口

串口使用流程

![img](https://images.tuyacn.com/goat/20191101/3d4cf7d5999845bab32095437269e9b4.png) 

### 打印串口号设置

**函数原型**

```
#include “uart.h”
void print_port_init(UART_Port uart_no)
```

**功能说明**

打印串口号设置，默认打印波特率74880如 uart_io ==UART1，则打印信息由 IO2 端口输出，tysdk 默认所有的打印信息由 IO2 端口输出，用户在编写应用代码时可通过该接口重配置

**参数说明**

| 参数名称          | 参数类型 | 说明   |
| ----------------- | -------- | ------ |
| UART_Port uart_no | IN       | 串口号 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### 打印串口参数设置

**函数原型**

```
#include “uart.h”
void print_port_full_init(UART_Port uart_no, UART_BautRate bit_rate, UART_WordLength data_bits, UART_ParityMode parity,UART_StopBits stop_bits);
```

**功能说明**

打印串口参数设置

**参数说明**

| 参数名称                  | 参数类型 | 说明                |
| ------------------------- | -------- | ------------------- |
| UART_Port uart_no         | IN       | 打印串口号          |
| UART_BautRate bit_rate    | IN       | 波特率(300-3686400) |
| UART_WordLength data_bits | IN       | 数据位              |
| UART_ParityMode parity    | IN       | 奇偶校验位          |
| UART_StopBits stop_bits   | IN       | 停止位              |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### UART0 串口初始化配置

**函数原型**

```
#include “uart.h”
void user_uart_full_init(UART_BautRate bit_rate,UART_WordLength data_bits, UART_ParityMode parity,UART_StopBits stop_bits);
```

**功能说明**

UART0 串口初始化配置，如果采用该接口初始化 UART0，I015 电路需做特殊处理，具体可见乐鑫原厂文档。采用该端口进行串口通讯的好处是，可以避免 8266 启动后默认信息输出对用户控制板的数据干扰

**参数说明**

| 参数名称                  | 参数类型 | 说明                |
| ------------------------- | -------- | ------------------- |
| UART_BautRate bit_rate    | IN       | 波特率(300-3686400) |
| UART_WordLength data_bits | IN       | 数据位              |
| UART_ParityMode parity    | IN       | 奇偶校验位          |
| UART_StopBits stop_bits   | IN       | 停止位              |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### UART0 串口初始化配置(默认UART0)

**函数原型**

```
#include “uart.h”
void user_uart_raw_full_init(UART_BautRate bit_rate,UART_WordLength data_bits, UART_ParityMode parity,UART_StopBits stop_bits);
```

**功能说明**

UART0 串口初始化配置(默认 UART0)

**参数说明**

| 参数名称                  | 参数类型 | 说明                |
| ------------------------- | -------- | ------------------- |
| UART_BautRate bit_rate    | IN       | 波特率(300-3686400) |
| UART_WordLength data_bits | IN       | 数据位              |
| UART_ParityMode parity    | IN       | 奇偶校验位          |
| UART_StopBits stop_bits   | IN       | 停止位              |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### UART0 串口初始化（收发端口设置为 I015:TX IO13:RX）

**函数原型**

```
#include “uart.h”
void user_uart_init(UART_BautRate bit_rate)
```

**功能说明**

UART0串口初始化（收发端口设置为 I015:TX IO13:RX），默认 data_bits=8,parity=无, stop_bits=0 如果采用该接口初始化 UART0，I015 电路需做特殊处理，具体可见乐鑫原厂文档。采用该端口进行串口通讯的好处是，可以避免 8266 启动后默认信息输出对用户控制板的数据干扰。

**参数说明**

| 参数名称               | 参数类型 | 说明                |
| ---------------------- | -------- | ------------------- |
| UART_BautRate bit_rate | IN       | 波特率(300-3686400) |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### UART0 串口初始化(收发端口设置为 IO3:RX IO1:TX)

**函数原型**

```
#include “uart.h”
void user_uart_raw_init(UART_BautRate bit_rate)
```

**功能说明**

UART0 串口初始化（收发端口设置为 I015:TX IO13:RX），默认 data_bits=8,parity=无,stop_bits=0。

**参数说明**

| 参数名称               | 参数类型 | 说明                |
| ---------------------- | -------- | ------------------- |
| UART_BautRate bit_rate | IN       | 波特率(300-3686400) |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### 读串口缓冲数据大小

**函数原型**

```
#include “uart.h”
uint16 user_uart_read_size(void)
```

**功能说明**

读串口缓冲数据大小

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | IN       | 无   |

**返回值**

| 返回值类型 | 说明       |
| ---------- | ---------- |
| uint16     | 缓冲区大小 |

### 读串口数据

**函数原型**

```
#include “uart.h”
uint16 user_uart_read_data(uint8 *out,uint16 out_len)
```

**功能说明**

读串口数据

**参数说明**

| 参数名称       | 参数类型 | 说明       |
| -------------- | -------- | ---------- |
| uint8 * out    | OUT      | 读取数据   |
| uint16 out_len | OUT      | 缓冲区长度 |

**返回值**

| 返回值类型 | 说明           |
| ---------- | -------------- |
| uint16     | 读取的数据长度 |

### 写串口数据

**函数原型**

```
#include “uart.h”
void user_uart_write_data(uint8 *in,uint16 in_len)
```

**功能说明**

写串口数据

**参数说明**

| 参数名称      | 参数类型 | 说明       |
| ------------- | -------- | ---------- |
| uint8 * in    | IN       | 写入的数据 |
| uint16 in_len | IN       | 写入长度   |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

## GPIO 接口

### 读取 GPIO 电平

**函数原型**

```
#include “tuya_gpio_api.h”
INT tuya_read_gpio_level(USHORT gpio_no)
```

**功能说明**

读取 GPIO 电平

**参数说明**

| 参数名称       | 参数类型 | 说明     |
| -------------- | -------- | -------- |
| USHORT gpio_no | IN       | GPIO序号 |

**返回值**

| 返回值类型 | 说明              |
| ---------- | ----------------- |
| INT        | 0 低电平 1 高电平 |

### 设置 GPIO 电平

**函数原型**

```
#include “tuya_gpio_api.h”
VOID tuya_write_gpio_level(USHORT gpio_no, UCHAR level)
```

**功能说明**

设置 GPIO 电平

**参数说明**

| 参数名称       | 参数类型 | 说明              |
| -------------- | -------- | ----------------- |
| USHORT gpio_no | IN       | GPIO 序号         |
| UCHAR level    | IN       | 0 低电平 1 高电平 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

## 定时器接口

### 添加定时器

**函数原型**

```
#include “sys_timer.h”
OPERATE_RET sys_add_timer(IN CONST P_TIMER_FUNC pTimerFunc, IN CONST PVOID pTimerArg, OUT TIMER_ID *p_timerID)
```

**功能说明**

添加一个系统定时器

**参数说明**

| 参数名称                | 参数类型 | 说明         |
| ----------------------- | -------- | ------------ |
| P_TIMER_FUNC pTimerFunc | IN       | 回调函数     |
| PVOID pTimerArg         | IN       | 回调函数传参 |
| TIMER_ID * p_timerID    | OUT      | 定时器ID编号 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 停止定时器

**函数原型**

```
#include “sys_timer.h”
OPERATE_RET sys_stop_timer(IN CONST TIMER_ID timerID)
```

**功能说明**

停止一个系统定时器

**参数说明**

| 参数名称         | 参数类型 | 说明           |
| ---------------- | -------- | -------------- |
| TIMER_ID timerID | IN       | 定时器 ID 编号 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 定时器是否运行

**函数原型**

```
#include “sys_timer.h”
BOOL IsThisSysTimerRun(IN CONST TIMER_ID timerID)
```

**功能说明**

定时器是否运行

**参数说明**

| 参数名称         | 参数类型 | 说明           |
| ---------------- | -------- | -------------- |
| TIMER_ID timerID | IN       | 定时器 ID 编号 |

**返回值**

| 返回值类型 | 说明                       |
| ---------- | -------------------------- |
| BOOL       | TRUE 正在运行 FALSE 未运行 |

### 启动定时器

**函数原型**

```
#include “sys_timer.h”
OPERATE_RET sys_start_timer(IN CONST TIMER_ID timerID, IN CONST TIME_MS timeCycle, IN CONST TIMER_TYPE timer_type)
```

**功能说明**

启动定时器

**参数说明**

| 参数名称              | 参数类型 | 说明                                                      |
| --------------------- | -------- | --------------------------------------------------------- |
| TIMER_ID timerID      | IN       | 定时器 ID 编号                                            |
| TIME_MS timeCycle     | IN       | 定时周期(单位毫秒)                                        |
| TIMER_TYPE timer_type | IN       | 定时器类型 <1>TIMER_ONCE 单次执行 <2>TIMER_CYCLE 循环执行 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

## 系统层接口

### 系统休眠

**函数原型**

```
#include “uni_system.h”
VOID SystemSleep(IN CONST TIME_MS msTime)
```

**功能说明**

系統休眠

**参数说明**

| 参数名称       | 参数类型 | 说明               |
| -------------- | -------- | ------------------ |
| TIME_MS msTime | IN       | 休眠时间(单位毫秒) |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 系统重启

**函数原型**

```
#include “uni_system.h”
VOID SystemReset(VOID)
```

**功能说明**

系统重启

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### 比较两个字符串

**函数原型**

```
#include “uni_system.h”
int strcasecmp(const char *s1, const char *s2)
```

**功能说明**

比较两个字符串是否相等

**参数说明**

| 参数名称  | 参数类型 | 说明     |
| --------- | -------- | -------- |
| char * s1 | IN       | 字符串 1 |
| char * s2 | IN       | 字符串 2 |

**返回值**

| 返回值类型 | 说明                         |
| ---------- | ---------------------------- |
| int        | 0 字符串相等 非 0 字符串不等 |

### ASCII 转 HEX

**函数原型**

```
#include “uni_system.h”
void ascs2hex(unsigned char *hex,unsigned char *ascs,int srclen)
```

**功能说明**

ASCII 码转 HEX

**参数说明**

| 参数名称             | 参数类型 | 说明        |
| -------------------- | -------- | ----------- |
| unsigned char * hex  | OUT      | 输出的hex   |
| unsigned char * ascs | IN       | 源asc码     |
| int srclen           | IN       | 原asc码长度 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### Hex 转 Str

**函数原型**

```
#include “uni_system.h”
void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
```

**功能说明**

Hex转Str

**参数说明**

| 参数名称      | 参数类型 | 说明         |
| ------------- | -------- | ------------ |
| BYTE * pbDest | OUT      | 输出的字符串 |
| BYTE * pbSrc  | IN       | 源 hex 码    |
| int nLen      | IN       | 源 hex 长度  |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

### 获取随机数

**函数原型**

```
#include “uni_system.h”
OPERATE_RET tuya_get_random_data(UCHAR_T* dst, INT_T size, INT_T range)
```

**功能说明**

获取随机数

**参数说明**

| 参数名称     | 参数类型 | 说明       |
| ------------ | -------- | ---------- |
| UCHAR_T* dst | OUT      | 随机数指针 |
| INT_T size   | IN       | 源 hex 码  |
| INT_T range  | IN       | 随机数范围 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |



## 任务接口

### 任务创建

**函数原型**

```
#include “uni_thread.h”
OPERATE_RET CreateAndStart(OUT THRD_HANDLE *pThrdHandle, IN CONST P_THRD_FUNC pThrdFunc, IN CONST PVOID pThrdFuncArg, IN CONST STACK_SIZE stack_size,IN CONST TRD_PRI pri, IN CONST CHAR *thrd_name);
```

**功能说明**

任务创建

**参数说明**

| 参数名称                  | 参数类型 | 说明         |
| ------------------------- | -------- | ------------ |
| THRD_HANDLE * pThrdHandle | OUT      | 任务句柄     |
| THRD_FUNC pThrdFunc       | IN       | 任务处理函数 |
| PVOID pThrdFuncArg        | IN       | 任务参数     |
| STACK_SIZE stack_size     | IN       | 任务堆栈大小 |
| TRD_PRI pri               | IN       | 任务优先级   |
| CHAR *thrd_name           | IN       | 任务名称     |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 任务删除

**函数原型**

```
#include “uni_thread.h”
VOID DeleteThrdHandle(IN CONST THRD_HANDLE thrdHandle)
```

**功能说明**

任务删除

**参数说明**

| 参数名称               | 参数类型 | 说明     |
| ---------------------- | -------- | -------- |
| THRD_HANDLE thrdHandle | IN       | 任务句柄 |

**返回值**

| 返回值类型 | 说明 |
| ---------- | ---- |
| 无         | 无   |

## 信号量接口

### 信号量创建

**函数原型**

```
#include “uni_semaphore.h”
SEM_HANDLE CreateSemaphore(VOID)
```

**功能说明**

信号量创建

**参数说明**

| 参数名称 | 参数类型 | 说明 |
| -------- | -------- | ---- |
| 无       | 无       | 无   |

**返回值**

| 返回值类型 | 说明       |
| ---------- | ---------- |
| SEM_HANDLE | 信号量句柄 |

### 信号量初始化

**函数原型**

```
#include “uni_semaphore.h”
OPERATE_RET InitSemaphore(IN CONST SEM_HANDLE semHandle, IN CONST UINT semCnt, IN CONST UINT sem_max)
```

**功能说明**

信号量初始化

**参数说明**

| 参数名称             | 参数类型 | 说明         |
| -------------------- | -------- | ------------ |
| SEM_HANDLE semHandle | IN       | 信号量句柄   |
| UINT semCnt          | IN       | 信号量初始值 |
| UINT sem_max         | IN       | 信号量最大值 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 信号量发送

**函数原型**

```
#include “uni_semaphore.h”
OPERATE_RET WaitSemaphore(IN CONST SEM_HANDLE semHandle)
```

**功能说明**

信号量发送

**参数说明**

| 参数名称             | 参数类型 | 说明       |
| -------------------- | -------- | ---------- |
| SEM_HANDLE semHandle | IN       | 信号量句柄 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 信号量接收

**函数原型**

```
#include “uni_semaphore.h”
OPERATE_RET PostSemaphore(IN CONST SEM_HANDLE semHandle)
```

**功能说明**

信号量接收

**参数说明**

| 参数名称             | 参数类型 | 说明       |
| -------------------- | -------- | ---------- |
| SEM_HANDLE semHandle | IN       | 信号量句柄 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

## 互斥锁接口

### 互斥锁创建

**函数原型**

```
#include “uni_mutex.h”
OPERATE_RET CreateMutexAndInit(OUT MUTEX_HANDLE *pMutexHandle)
```

**功能说明**

互斥锁创建

**参数说明**

| 参数名称                    | 参数类型 | 说明       |
| --------------------------- | -------- | ---------- |
| MUTEX_HANDLE * pMutexHandle | OUT      | 互斥锁句柄 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 互斥锁锁定

**函数原型**

```
#include “uni_mutex.h”
OPERATE_RET MutexLock(IN CONST MUTEX_HANDLE mutexHandle)
```

**功能说明**

互斥锁锁定

**参数说明**

| 参数名称                 | 参数类型 | 说明       |
| ------------------------ | -------- | ---------- |
| MUTEX_HANDLE MutexHandle | IN       | 互斥锁句柄 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 互斥锁解锁

**函数原型**

```
#include “uni_mutex.h”
OPERATE_RET MutexUnLock(IN CONST MUTEX_HANDLE mutexHandle)
```

**功能说明**

互斥锁解锁

**参数说明**

| 参数名称                 | 参数类型 | 说明       |
| ------------------------ | -------- | ---------- |
| MUTEX_HANDLE MutexHandle | IN       | 互斥锁句柄 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

## 消息队列接口

### 消息队列创建

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET CreateMsgQueAndInit(OUT MSG_QUE_HANDLE *pMsgQueHandle)
```

**功能说明**

消息队列创建

**参数说明**

| 参数名称                       | 参数类型 | 说明             |
| ------------------------------ | -------- | ---------------- |
| MSG_QUE_HANDLE * pMsgQueHandle | OUT      | 消息管理结构句柄 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 消息添加

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET AddMsgNodeToQueue(IN CONST MSG_QUE_HANDLE msgQueHandle, IN CONST MSG_ID msgID, IN CONST P_MSG_DATA pMsgData,IN CONST MSG_DATA_LEN msgDataLen, IN CONST MSG_TYPE msgType)
```

**功能说明**

消息添加

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |
| MSG_ID msgID                | IN       | 消息 ID          |
| P_MSG_DATA pMsgData         | IN       | 消息数据         |
| MSG_DATA_LEN msgDataLen     | IN       | 消息数据长度     |
| MSG_TYPE msgType            | IN       | 消息类型         |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 获取消息节点

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET GetMsgNodeFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle, IN CONST MSG_ID msgID, OUT P_MSG_LIST *ppMsgListNode)
```

**功能说明**

获取指定ID的消息节点

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |
| MSG_ID msgID                | IN       | 消息 ID          |
| P_MSG_LIST * ppMsgListNode  | OUT      | 消息节点         |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 获取首先入链的消息节点

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET GetFirstMsgFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle, OUT P_MSG_LIST *ppMsgListNode)
```

**功能说明**

获取首先入链的消息节点

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |
| P_MSG_LIST * ppMsgListNode  | OUT      | 消息节点         |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 获取消息节点总数

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET GetMsgNodeNum(IN CONST MSG_QUE_HANDLE msgQueHandle,OUT PINT pMsgNodeNum)
```

**功能说明**

获取队列中的消息节点总数

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |
| PINT pMsgNodeNum            | OUT      | 消息节点数       |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 删除消息节点

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET DelAndFreeMsgNodeFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle, IN CONST P_MSG_LIST pMsgListNode)
```

**功能说明**

从链中删除消息节点并释放消息节点内存

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |
| P_MSG_LIST pMsgListNode     | IN       | 消息节点         |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 释放消息队列

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET ReleaseMsgQue(IN CONST MSG_QUE_HANDLE msgQueHandle)
```

**功能说明**

释放消息队列所占用内存

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 投递消息

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET PostMessage(IN CONST MSG_QUE_HANDLE msgQueHandle, IN CONST MSG_ID msgID, IN CONST P_MSG_DATA pMsgData, IN CONST MSG_DATA_LEN msgDataLen)
```

**功能说明**

投递消息

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |
| MSG_ID msgID                | IN       | 消息 ID          |
| P_MSG_DATA pMsgData         | IN       | 消息数据         |
| MSG_DATA_LEN msgDataLen     | IN       | 消息数据长度     |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 投递紧急消息

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET PostInstancyMsg(IN CONST MSG_QUE_HANDLE msgQueHandle, IN CONST MSG_ID msgID, IN CONST P_MSG_DATA pMsgData, IN CONST MSG_DATA_LEN msgDataLen)
```

**功能说明**

投递紧急消息

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |
| MSG_ID msgID                | IN       | 消息 ID          |
| P_MSG_DATA pMsgData         | IN       | 消息数据         |
| MSG_DATA_LEN msgDataLen     | IN       | 消息数据长度     |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |

### 等待消息

**函数原型**

```
#include “uni_msg_queue.h”
OPERATE_RET WaitMessage(IN CONST MSG_QUE_HANDLE msgQueHandle, OUT P_MSG_LIST *ppMsgListNode)
```

**功能说明**

等待消息

**参数说明**

| 参数名称                    | 参数类型 | 说明             |
| --------------------------- | -------- | ---------------- |
| MSG_QUE_HANDLE msgQueHandle | IN       | 消息管理结构句柄 |
| P_MSG_LIST * ppMsgListNode  | OUT      | 消息节点         |

**返回值**

| 返回值类型  | 说明 |
| ----------- | ---- |
| OPERATE_RET | 附录 |



## 附录

### 返回值通用错误码

| 返回值宏定义                   | 返回值 |
| :----------------------------- | :----- |
| OPRT_OK                        | 0      |
| OPRT_COM_ERROR                 | 1      |
| OPRT_INVALID_PARM              | 2      |
| OPRT_MALLOC_FAILED             | 3      |
| OPRT_INIT_MUTEX_ATTR_FAILED    | 4      |
| OPRT_SET_MUTEX_ATTR_FAILED     | 5      |
| OPRT_DESTROY_MUTEX_ATTR_FAILED | 6      |
| OPRT_INIT_MUTEX_FAILED         | 7      |
| OPRT_MUTEX_LOCK_FAILED         | 8      |
| OPRT_MUTEX_TRYLOCK_FAILED      | 9      |
| OPRT_MUTEX_LOCK_BUSY           | 10     |
| OPRT_MUTEX_UNLOCK_FAILED       | 11     |
| OPRT_MUTEX_RELEASE_FAILED      | 12     |
| OPRT_INIT_SEM_FAILED           | 13     |
| OPRT_WAIT_SEM_FAILED           | 14     |
| OPRT_POST_SEM_FAILED           | 15     |
| OPRT_THRD_STA_UNVALID          | 16     |
| OPRT_THRD_CR_FAILED            | 17     |
| OPRT_THRD_JOIN_FAILED          | 18     |
| OPRT_THRD_SELF_CAN_NOT_JOIN    | 19     |
| OPRT_TIMERID_EXIST             | 20     |
| OPRT_TIMERID_NOT_FOUND         | 21     |
| OPRT_TIMERID_UNVALID           | 22     |
| OPRT_GET_IDLE_TIMERID_ERROR    | 23     |
| OPRT_MSG_NOT_FOUND             | 24     |
| OPRT_MSG_LIST_EMPTY            | 25     |
| OPRT_PSM_FLH_RET_ERR           | 26     |
| OPRT_PSM_FLH_TM_ERR            | 27     |
| OPRT_PSM_E_INVAL               | 28     |
| OPRT_PSM_E_IO                  | 29     |
| OPRT_PSM_E_EXIST               | 30     |
| OPRT_PSM_E_NOENT               | 31     |
| OPRT_PSM_FAIL                  | 32     |
| OPRT_PSM_E_NOSPC               | 33     |
| OPRT_PSM_E_METADATA_CRC        | 34     |
| OPRT_PSM_E_CRC                 | 35     |
| OPRT_WIFI_SCAN_FAIL            | 36     |
| OPRT_WF_MAC_SET_FAIL           | 37     |
| OPRT_WF_CONN_FAIL              | 38     |
| OPRT_WF_NW_CFG_FAIL            | 39     |
| OPRT_SET_SOCK_ERR              | 41     |
| OPRT_SOCK_CONN_ERR             | 42     |
| OPRT_CR_MUTEX_ERR              | 43     |
| OPRT_CR_TIMER_ERR              | 44     |
| OPRT_CR_THREAD_ERR             | 45     |
| OPRT_BUF_NOT_ENOUGH            | 46     |
| OPRT_URL_PARAM_OUT_LIMIT       | 47     |
| OPRT_HTTP_OS_ERROR             | 48     |
| OPRT_HTTP_PR_REQ_ERROR         | 49     |
| OPRT_HTTP_SD_REQ_ERROR         | 50     |
| OPRT_HTTP_RD_ERROR             | 51     |
| OPRT_HTTP_AD_HD_ERROR          | 52     |
| OPRT_HTTP_GET_RESP_ERROR       | 53     |
| OPRT_HTTP_AES_INIT_ERR         | 54     |
| OPRT_HTTP_AES_OPEN_ERR         | 55     |
| OPRT_HTTP_AES_SET_KEY_ERR      | 56     |
| OPRT_HTTP_AES_ENCRYPT_ERR      | 57     |
| OPRT_TY_WS_PART_ERR            | 58     |
| OPRT_CR_CJSON_ERR              | 59     |
| OPRT_PSM_SET_ERROR             | 60     |
| OPRT_PSM_GET_ERROR             | 61     |
| OPRT_CJSON_PARSE_ERR           | 62     |
| OPRT_CJSON_GET_ERR             | 63     |
| OPRT_CR_HTTP_URL_H_ERR         | 64     |
| OPRT_HTTPS_HANDLE_FAIL         | 65     |
| OPRT_HTTPS_RESP_UNVALID        | 66     |
| OPRT_MEM_PARTITION_EMPTY       | 67     |
| OPRT_MEM_PARTITION_FULL        | 68     |
| OPRT_MEM_PARTITION_NOT_FOUND   | 69     |
| OPRT_CR_QUE_ERR                | 70     |
| OPRT_SND_QUE_ERR               | 71     |
| OPRT_NOT_FOUND_DEV             | 72     |
| OPRT_NOT_FOUND_DEV_DP          | 73     |
| OPRT_DP_ATTR_ILLEGAL           | 74     |
| OPRT_DP_TYPE_PROP_ILLEGAL      | 75     |
| OPRT_DP_REPORT_CLOUD_ERR       | 76     |
| OPRT_NO_NEED_SET_PRODINFO      | 77     |
| OPRT_NW_INVALID                | 78     |
| OPRT_SELECT_ERR                | 79     |
| OPRT_SELECT_TM                 | 80     |
| OPRT_SEND_ERR                  | 81     |
| OPRT_DEV_NOT_BIND              | 82     |
| OPRT_FW_UG_FAILED              | 83     |
| OPRT_VER_FMT_ERR               | 84     |
| OPRT_FW_NOT_EXIST              | 85     |
| OPRT_SEM_CR_ERR                | 86     |
| OPRT_SELECT_TIMEOUT            | 87     |
| OPRT_GW_MQ_OFFLILNE            | 88     |
| OPRT_NOT_SPT_CLX_DP            | 89     |
| OPRT_RECV_ERR                  | 90     |
| OPRT_UG_PKG_NOT_ENOUGH         | 91     |
| OPRT_SCMA_INVALID              | 92     |
| OPRT_PRODECT_KEY_NULL          | 93     |
| OPRT_DEVICE_VER_NULL           | 94     |
| OPRT_MSG_OUT_OF_LMT            | 95     |
| OPRT_NOT_FOUND_AUTH_SSID       | 96     |
| OPRT_SOCKET_FAULT              | 97     |
| OPRT_MQ_PUBLISH_TIMEOUT        | 98     |
| OPRT_GW_NOT_EXIST              | 99     |
| OPRT_GW_SCHEMA_SIZE_LMT_OUT    | 100    |
| OPRT_DEV_DP_CNT_INVALID        | 101    |
| OPRT_TOKEN_OVERTIME            | 102    |
| OPRT_CR_TIMER_FAILED           | 103    |
| OPRT_PUB_NO_PERMISSION         | 104    |
| OPRT_CRC32_FAILED              | 105    |
| OPRT_OPEN_FILE_FAILED          | 106    |
| OPRT_DONOT_FOUND_MODULE        | 107    |
| OPRT_PRINT_LOG_LEVEL_HIGHER    | 108    |
| OPRT_FORMAT_STRING_FAILED      | 109    |
| OPRT_STRING_TOO_LONG           | 110    |
| OPRT_CMD_OUT_OF_TIME_NOT_EXEC  | 111    |
| OPRT_CMD_NOT_EXEC              | 112    |
| OPRT_REPORT_OUTOF_MINUTE_RATE  | 113    |
| OPRT_MCU_UART_SEND_ERROR       | 114    |
| OPRT_RECV_DA_NOT_ENOUGH        | 115    |
| OPRT_UG_FILE_HD_INVALID        | 116    |
| OPRT_UG_FILE_CRC_FAIL          | 117    |
| OPRT_HTTPS_NO_SUPPORT_RANGE    | 118    |
| OPRT_GW_FW_SIZE_OVER_MAX_SIZE  | 119    |
| OPRT_HMAC_DISMATCH             | 120    |
| OPRT_NOT_FOUND                 | 121    |
| OPRT_UG_FLASH_BIN_CHECKSUM_ERR | 122    |
| OPRT_LOG_SEQ_LOCKED            | 123    |
| OPRT_LOG_SEQ_DONT_LOCKED       | 124    |
| OPRT_HTTP_NO_ENCRPT            | 125    |
| OPRT_HTTP_GW_RESET             | 126    |
| OPRT_HTTP_GW_FAC_RESET         | 127    |