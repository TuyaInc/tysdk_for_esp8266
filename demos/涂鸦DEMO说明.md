## 版本记录

| 版本  | 编写/修订说明 | 修订日期 | 备注 |
| ----- | ------------- | -------- | ---- |
| 1.0.0 | 创建文档      | 20200328 |      |

## 开发环境

无需安装开发环境，sdk已自带开发环境。

## 应用编译方法

进入顶层目录

1）编译有日志输出的固件，例如 sample_pk，版本号 1.0.0
./build_app.sh demos/sample_pk sample_pk 1.0.0 debug
2）编译无日志输出的固件，例如 sample_pk，版本号 1.0.0
./build_app.sh demos/sample_pk sample_pk1.0.0 
注意：可以自己新建apps目录将工程放入其中，工程路径许改为apps/工程名，其次工程目录中user文件下的Makefile需要更改，可以复制demo中的makefile进行编译。
## 固件下载
原厂烧录软件下载地址 https://www.espressif.com/zh-hans/support/download/other-tools
### 特别声明
整片烧录方法，会破坏涂鸦授权码，烧录完成后需要重新授权才能进行使用。
### 分地址烧录（开发调试使用）
![image.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/goat/20200330/6fd99bc50b5747e480bcb7fb1a08460e.png)

总共选择4个文件，如上图所示：
1) 0x0000 地址为 boot 区
2) 0x1000 地址为 user1区(应用程序区)
3) 0xfc000 地址为初始化射频参数区
4) 0xfe000 地址为初始化系统参数区
### 整片烧录
![image.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/goat/20200330/1d3a6cb4a8504ea78c72d6ffdf83a7f4.png)

整片烧录会擦除之前烧录设备的所有信息，包括涂鸦授权码信息（破坏后需要重新授权才能使用）。整片烧录的文件8266采用 QIO bin 文件，8285采用 DOUT  bin 文件。
### 原厂烧录软件设置
![image.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/goat/20200330/a623d0dcb6864b8899bb32f212aa8b9c.png)

设置项主要包括模式设置和 flash 设置，烧录 QIO bin 选择 QIO 模式，烧录 DOUT  bin 选择 DOUT 模式，flash设置主要是sdk采用的是 1M sdk 还是 2M sdk，1M设置成 8Mbit， 2M 设置成16Mbit-C1。
## 电工插座

### 目录结构

esp_plug_demo 

	├── include  
	
		├── device.h 
	
		├──hw_table.h 
	
	├── user 
	
		├── device.c 
	
		├── hw_table.c 
	
		├── Makefile

### 文件介绍

| 文件名     | 文件类型      | 备注                              |
| :--------- | :------------ | :-------------------------------- |
| device.c   | .c 文件       | 设备相关初始化及产测等            |
| hw_table.c | .c 文件       | 硬件相关的初始化及控制操作等      |
| Makefile   | makefile 文件 | 此文件为编译文件，**勿动**        |
| device.h   | .h 头文件     | 头文件，其中包含关键 固件key 参数 |
| hw_table.h | .h 头文件     | 头文件，硬件控制接口等            |

### 硬件IO口说明

| 功能         | IO口 | 高低有效   |
| ------------ | ---- | ---------- |
| 继电器       | IO12 | 高电平有效 |
| 继电器指示灯 | IO5  | 高电平有效 |
| 开关按键     | IO13 | 低电平有效 |
| wifi 指示灯  | IO4  | 低电平有效 |

### 功能说明

#### 网络状态对应指示灯	

配网中	快闪（２５０ｍｓ闪烁）和慢闪（１５００ｍｓ闪烁）	

入网失败	亮

入网成功	灭

#### 配置说明

1. wifi模式为 WCM_OLD_LOW，上电低功耗模式，长按按键3秒进入配网模式；

2. 上电默认断电状态；

## 电量统计插座

### 目录结构

esp_bl0937_ele_plug_demo 

	├── include 
	
		├── app_dltj.h  
	
		├── app_switch.h 
	
		├── bl0937.h 	
	
		├── device.h 	
	├── user 
	
		├── app_dltj.c 
	
		├── app_switch.c 
	
		├── bl0937.c 	
	
		├── device.c 	
	
		├── Makefile

### 文件介绍

| 文件         | 功能介绍                                                     |
| ------------ | ------------------------------------------------------------ |
| app_dltj.c   | 电量实时参数（电流电压功率）上报逻辑和对应的事件处理，电量增量上报逻辑，离线电量存储，未配网状态下电量的存储 |
| device.c     | 应用模块主框架，包含设备初始化，整机产测部分的回调，操作回调等； |
| app_switch.c | 开关部分功能逻辑，指示灯的显示，倒计时功能；                 |
| bl0937.c     | 电量统计芯片 bl0937 的驱动部分，主要是芯片的硬件初始化，电流、电压、功率和电量的计算，产测校准处理 |
| Makefile     | makefile 文件                                                |

### 硬件IO口说明

| 功能         | IO口 | 高低有效   |
| ------------ | ---- | ---------- |
| 继电器       | IO14 | 高电平有效 |
| 继电器指示灯 | 无   |            |
| 开关按键     | IO3  | 低电平有效 |
| wifi指示灯   | IO13 | 低电平有效 |

### 功能说明

#### 计量部分信息

工作电压 220V，过流点：17A，采样电阻１毫欧；计量芯片：ｂｌ０９３７；

#### 电量统计（CF）

IO４- 电流电压检测（CF１）

IO5 - 电流电压切换（SEL）

IO12 - 电平为直连模式

#### 网络状态对应指示灯

未配网	指示继电器状态

配网中	快闪（２５０ｍｓ闪烁）和慢闪（１５００ｍｓ闪烁）	

配网失败	指示继电器状态

配网成功	指示继电器状态

#### 配置说明

1. wifi模式为 WCM_OLD_LOW，上电低功耗模式，长按按键5秒进入配网模式；

2. 上电默认断电状态；
