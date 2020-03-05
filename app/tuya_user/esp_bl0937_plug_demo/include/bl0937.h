/**
 * @File: bl0937.h 
 * @Author: caojq 
 * @Last Modified time: 2019-02-13 15:09:01 
 * @Description: bl0937电量统计芯片驱动
 */
#ifndef __BL0937_H__
#define __BL0937_H__

#include "sys_adapter.h"
#include "esp_common.h"
#include "com_def.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef _BL0937_GLOBAL
    #define _BL0937_EXT
#else
    #define _BL0937_EXT extern
#endif
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define D_ERR_MODE                	0x00        //错误提示模式
#define D_NORMAL_MODE		      	0x10	    //正常工作模式
#define D_CAL_START_MODE		    0x21	    //校正模式，启动
#define D_CAL_END_MODE		        0x23	    //校正模式，完成
//--------------------------------------------------------------------------------------------

#ifndef __IO_TYPE_CONFIG__
#define __IO_TYPE_CONFIG__
typedef enum {
	IO_DRIVE_LEVEL_HIGH,		// 高电平有效
	IO_DRIVE_LEVEL_LOW,			// 低电平有效
	IO_DRIVE_LEVEL_NOT_EXIST	// 该IO不存在
}IO_DRIVE_TYPE;
#endif

typedef struct{
	IO_DRIVE_TYPE type;	// 有效电平类型
	unsigned char pin;	// 引脚号
}IO_CONFIG;

typedef struct{
	unsigned char dp_vcoe;
	unsigned char dp_icoe;
	unsigned char dp_pcoe;
	unsigned char dp_ecoe;
	unsigned char dp_pt_rslt;
	unsigned char epin;
	unsigned char ivpin;
	IO_CONFIG	  ivcpin;
	unsigned int  v_ref;
	unsigned int  i_ref;
	unsigned int  p_ref;
	unsigned int  e_ref;
    unsigned int  v_def;
    unsigned int  i_def;
    unsigned int  p_def;
}DLTJ_CONFIG;

/***********************************************************
*************************function define********************
***********************************************************/

/*********************************************************************************
 * FUNCTION:       bl0937_init
 * DESCRIPTION:    bl0937芯片引脚配置和参数设置
 * INPUT:          dltj：电量统计相关参数结构体
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         bl0937芯片引脚配置和参数设置
 * HISTORY:        2020-03-04
 *******************************************************************************/
_BL0937_EXT \
VOID bl0937_init(DLTJ_CONFIG *dltj);

/*********************************************************************************
 * FUNCTION:       ele_cnt_init
 * DESCRIPTION:    电量脉冲计数初始化
 * INPUT:          mode：计量芯片的模式(计量模式)
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         计量芯片初始化/硬件定时器初始化/中断初始化/脉冲计数初始化
 * HISTORY:        2020-03-04
 *******************************************************************************/
_BL0937_EXT \
OPERATE_RET ele_cnt_init(INT mode);

 /*********************************************************************************
  * FUNCTION:       report_coe_data
  * DESCRIPTION:    上报电量统计校准参数及产测结果位
  * INPUT:          none
  * OUTPUT:         none
  * RETURN:         none
  * OTHERS:         电量统计校准参数和产测结果
  * HISTORY:        2020-03-04
  *******************************************************************************/
_BL0937_EXT \
OPERATE_RET report_coe_data(VOID);

/*********************************************************************************
 * FUNCTION:       get_ele_par
 * DESCRIPTION:    获取电量实时参数
 * INPUT:          p/v/i:功率/电压/电流
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         获取电量芯片实时参数
 * HISTORY:        2020-03-04
 *******************************************************************************/
_BL0937_EXT \
VOID get_ele_par(OUT UINT *P,OUT UINT *V,OUT UINT *I);

/*********************************************************************************
 * FUNCTION:       get_ele
 * DESCRIPTION:    获取增加电量参数
 * INPUT:          E:增加电量值
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         获取电量芯片增加电量参数
 * HISTORY:        2020-03-04
 *******************************************************************************/
_BL0937_EXT \
VOID get_ele(OUT UINT *E);

#ifdef __cplusplus
}
#endif
#endif

