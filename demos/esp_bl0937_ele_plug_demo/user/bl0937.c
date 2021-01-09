/**
 * @file bl0937.c
 * @brief bl0937电量统计芯片驱动
 * @date 2020-03-04
 * 
 * @copyright Copyright (c) {2018-2020} 涂鸦科技 www.tuya.com
 * 
 */
 #define _BL0937_GLOBAL
#include "bl0937.h"
#include "gpio.h"
#include "led_indicator.h"
#include "system/sys_timer.h"
#include "system/uni_semaphore.h"
#include "cJSON.h"
#include "smart_wf_frame.h"
//#include "tuya_dltj_flash.h"
#include "tuya_gpio_api.h"

#define DEVICE_MOD "device_mod"
#define COE_SAVE_KEY "coe_save_key"
#define PROD_RSLT_KEY "prod_rslt_key"

#define E_001E_GAIN     4
//--------------------------------------------------------------------------------------------
//Time1定时器定时,时间基数 = 1ms
#define D_TIME1_20MS				20 		
#define D_TIME1_100MS				100 
#define D_TIME1_150MS				150 	
#define D_TIME1_200MS				200 	
#define D_TIME1_300MS				300 	
#define D_TIME1_400MS				400 	
#define D_TIME1_500MS				500 	
#define D_TIME1_1S				1000		//Time1定时器定时1S时间常数
#define D_TIME1_2S				2000 	
#define D_TIME1_3S				3000 	
#define D_TIME1_4S				4000 	
#define D_TIME1_5S				5000 
#define D_TIME1_6S				6000 
#define D_TIME1_8S				8000 
#define D_TIME1_9S				9000 
#define D_TIME1_10S				10000 
#define D_TIME1_11S				11000 
#define D_TIME1_20S				20000 


#define I_CAL_TIME                  10000      //电流检测时间，SEL拉低的时间
#define V_CAL_TIME                  1000       //电压检测时间，SEL拉高的时间
#define D_TIME1_V_OVERFLOW          500        //Time1定时器,电压溢出常数设定为500mS,溢出说明脉宽周期大于500mS
#define D_TIME1_I_OVERFLOW			5000	   //Time1定时器,电流溢出常数设定为5S,溢出说明脉宽周期大于5S
#define D_TIME1_P_OVERFLOW			5000	   //Time1定时器,功率溢出常数设定为5S,溢出说明脉宽周期大于5S
//校正时间，记录在此时间内的脉冲数
#define D_TIME1_CAL_TIME			(18000000/E_001E_GAIN/driver_dltj.p_def)

#define Get_ELE_PORT 			GPIO_ID_PIN(driver_dltj.epin) 				//电量统计口
#define Get_CUR_VOL 			GPIO_ID_PIN(driver_dltj.ivpin) 				//电流电压统计口
#define CUR_VOL_SWITCH 			GPIO_ID_PIN(driver_dltj.ivcpin.pin) 			//电流电压切换口   
#define ENTER_V_MODE 			GPIO_OUTPUT_SET(CUR_VOL_SWITCH, (driver_dltj.ivcpin.type == IO_DRIVE_LEVEL_LOW ? DRV_GPIO_LOW : DRV_GPIO_HIGH))
#define ENTER_I_MODE 			GPIO_OUTPUT_SET(CUR_VOL_SWITCH, (driver_dltj.ivcpin.type == IO_DRIVE_LEVEL_LOW ? DRV_GPIO_HIGH : DRV_GPIO_LOW))
#define GPIO16_ENTER_V          gpio16_output_set(driver_dltj.ivcpin.type == IO_DRIVE_LEVEL_LOW ? 0 : 1)
#define GPIO16_ENTER_I          gpio16_output_set(driver_dltj.ivcpin.type == IO_DRIVE_LEVEL_LOW ? 1 : 0)
#define DEF_V                   driver_dltj.v_def
#define DEF_I                   driver_dltj.i_def
#define DEF_P                   driver_dltj.p_def

#define my_abs(x,y) ((x)>(y) ? (x)-(y):(y)-(x))

enum drv_gpio_level{
	DRV_GPIO_LOW = 0,
	DRV_GPIO_HIGH,
};

typedef struct{
    uint32 v;
    uint32 i;
    uint32 p;
    uint32 e;
    uint32 prod_rslt;
}ELE_CAL_DATA;


SEM_HANDLE ele_cal_busy;
xSemaphoreHandle get_ele_busy;
TIMER_ID get_ele_timer;
BOOL ele_cal_flag = FALSE;
STATIC DLTJ_CONFIG driver_dltj;

u32 P_VAL;
u32 V_VAL;
u32 I_VAL;
u32 E_VAL;
ELE_CAL_DATA ele_cal_data;

//--------------------------------------------------------------------------------------------
u16	U16_P_TotalTimes;			//当前脉冲 功率测量总时间
u16	U16_V_TotalTimes;			//当前脉冲 电压测量总时间
u16	U16_I_TotalTimes;			//当前脉冲 电流测量总时间

u16	U16_P_OneCycleTime;			//功率测量时间参数
u16	U16_V_OneCycleTime;			//电压测量时间参数
u16	U16_I_OneCycleTime;			//电流测量时间参数

u16	U16_P_Last_OneCycleTime;		//功率测量时间参数，上一次数量值
u16	U16_V_Last_OneCycleTime;		//电压测量时间参数，上一次数量值
u16	U16_I_Last_OneCycleTime;		//电流测量时间参数，上一次数量值

u16	U16_P_CNT;				//功率测量脉冲数量
u16	U16_V_CNT;				//电压测量脉冲数量
u16	U16_I_CNT;				//电流测量脉冲数量

u16	U16_P_Last_CNT;				//功率测量脉冲数量，上一次数量值
u16	U16_V_Last_CNT;				//电压测量脉冲数量，上一次数量值
u16	U16_I_Last_CNT;				//电流测量脉冲数量，上一次数量值

BOOL	B_P_TestOneCycle_Mode;			//功率测量模式 1:单周期测量，0:1S定时测量
BOOL	B_V_TestOneCycle_Mode;
BOOL	B_I_TestOneCycle_Mode;

BOOL	B_P_Last_TestOneCycle_Mode;
BOOL	B_V_Last_TestOneCycle_Mode;
BOOL	B_I_Last_TestOneCycle_Mode;
    		
BOOL  	B_P_OVERFLOW;       			// 功率脉冲周期 溢出标志位 
BOOL  	B_V_OVERFLOW;       			// 电压脉冲周期 溢出标志位
BOOL  	B_I_OVERFLOW;       			// 电流脉冲周期 溢出标志位

BOOL	B_P_Last_OVERFLOW;       		// 功率脉冲周期 溢出标志位 
BOOL  	B_V_Last_OVERFLOW;       		// 电压脉冲周期 溢出标志位
BOOL  	B_I_Last_OVERFLOW;       		// 电流脉冲周期 溢出标志位

BOOL    B_VI_Test_Mode;				//1:电压测量模式;0:电流测量模式
u16   	U16_VI_Test_Times;				
u16   	U16_Cal_Times;	

u16   	U16_AC_P;				//功率值 1000.0W
u16   	U16_AC_V;				//电压值 220.0V
u16   	U16_AC_I;				//电流值 4.545A
u32   	U32_AC_E;				//用电量   0.01度

u32  	U16_REF_001_E_Pluse_CNT;        	//0.1度电脉冲总数参考值
u32   	U16_E_Pluse_CNT;          	 	//脉冲个数寄存器

u32   	U32_Cal_Times;                 		//校正时间

u32   	U32_P_REF_PLUSEWIDTH_TIME;      	//参考功率 脉冲周期
u32   	U32_V_REF_PLUSEWIDTH_TIME;      	//参考电压 脉冲周期
u32   	U32_I_REF_PLUSEWIDTH_TIME;      	//参考电流 脉冲周期

u32   	U32_P_CURRENT_PLUSEWIDTH_TIME;      	//当前功率 脉冲周期
u32   	U32_V_CURRENT_PLUSEWIDTH_TIME;      	//当前电压 脉冲周期
u32   	U32_I_CURRENT_PLUSEWIDTH_TIME;      	//当前电流 脉冲周期

u16   	U16_P_REF_Data;				//参考功率值,如以1000W校正。1000.0W
u16   	U16_V_REF_Data;				//参考电压  220.0V
u16   	U16_I_REF_Data;				//参考电流  1000W,220V条件下是4.545A

u8    	U8_CURR_WorkMode;
//--------------------------------------------------------------------------------------------

/*-------------------------------------------- 功率、电压、电流计算 -------------------------------------------*/
OPERATE_RET get_ele_coe_flash();


/*********************************************************************************
 * FUNCTION:       dltj_copy
 * DESCRIPTION:    全局变量赋值
 * INPUT:          src：电量统计相关参数结构体
 * OUTPUT:         des：电量统计相关参数结构体
 * RETURN:         none
 * OTHERS:         bl0937芯片引脚配置和参数赋值
 * HISTORY:        2020-03-04
 *******************************************************************************/
VOID dltj_copy(DLTJ_CONFIG *des, DLTJ_CONFIG *src)
{
	des->epin 			= src->epin;
	des->ivpin			= src->ivpin;
	des->ivcpin.pin 	= src->ivcpin.pin;
	des->ivcpin.type 	= src->ivcpin.type;
	des->v_ref 			= src->v_ref;
	des->i_ref 			= src->i_ref;
	des->p_ref			= src->p_ref;
	des->e_ref			= src->e_ref;
    des->v_def          = src->v_def;
    des->i_def          = src->i_def;
    des->p_def          = src->p_def;
    des->dp_vcoe        = src->dp_vcoe;
    des->dp_icoe        = src->dp_icoe;
    des->dp_pcoe        = src->dp_pcoe;
    des->dp_ecoe        = src->dp_ecoe;
    des->dp_pt_rslt     = src->dp_pt_rslt;
}

/*********************************************************************************
 * FUNCTION:       bl0937_init
 * DESCRIPTION:    bl0937芯片引脚配置和参数设置
 * INPUT:          dltj：电量统计相关参数结构体
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         bl0937芯片引脚配置和参数设置
 * HISTORY:        2020-03-04
 *******************************************************************************/
VOID bl0937_init(DLTJ_CONFIG *dltj)
{
    dltj_copy(&(driver_dltj),dltj);
}

/*=====================================================
 * Function : void HLW8012_Measure_P(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2014/04/14
=====================================================*/
void HLW8012_Measure_P(void)
{
    if (B_P_Last_TestOneCycle_Mode == 1)
    {
		U32_P_CURRENT_PLUSEWIDTH_TIME = U16_P_Last_OneCycleTime*1000;
        //PR_NOTICE("[caojq]single_preoid p_last_time:%d,p_plusewidth_time:%d",U16_P_Last_OneCycleTime,U32_P_CURRENT_PLUSEWIDTH_TIME);
    }
    else
    {
    	if(U16_P_Last_CNT < 2){
			return;
		}
		U32_P_CURRENT_PLUSEWIDTH_TIME = U16_P_Last_OneCycleTime*1000/(U16_P_Last_CNT-1);
        //PR_NOTICE("[caojq]aver_preoid p_last_time:%d,p_last_count:%d,p_plusewidth_time:%d",U16_P_Last_OneCycleTime,U16_P_Last_CNT,U32_P_CURRENT_PLUSEWIDTH_TIME);
    }

	if (U8_CURR_WorkMode == D_CAL_START_MODE)
    {
		U32_P_REF_PLUSEWIDTH_TIME = U32_P_CURRENT_PLUSEWIDTH_TIME;	   // 校正时取U32_P_CURRENT_PLUSEWIDTH_TIME参数作为参考值	 
		return;
	}
	if(U32_P_CURRENT_PLUSEWIDTH_TIME == 0){
		U16_AC_P = 0;
	}else{
		U16_AC_P = U16_P_REF_Data * U32_P_REF_PLUSEWIDTH_TIME/U32_P_CURRENT_PLUSEWIDTH_TIME;
	}
    
    
    if (U16_AC_P == 0xffff)     //开机时U32_P_CURRENT_PLUSEWIDTH_TIME = 0，计算溢出
    {
        U16_AC_P = 0; 
    }
    
    if (B_P_Last_OVERFLOW == TRUE)
    {
        U16_AC_P = 0;
    }
}
/*=====================================================
 * Function : void HLW8012_Measure_V(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2020-03-04
=====================================================*/
void HLW8012_Measure_V(void)
{
    if (B_V_Last_TestOneCycle_Mode == 1)
    {
		U32_V_CURRENT_PLUSEWIDTH_TIME = U16_V_Last_OneCycleTime*1000;  
        //PR_NOTICE("[caojq]single_preoid v_last_time:%d,v_plusewidth_time:%d",U16_V_Last_OneCycleTime,U32_V_CURRENT_PLUSEWIDTH_TIME);
    }
    else
    {
    	if(U16_V_Last_CNT < 2){
			return;
		}
		U32_V_CURRENT_PLUSEWIDTH_TIME = U16_V_Last_OneCycleTime*1000/(U16_V_Last_CNT-1);  
        //PR_NOTICE("[caojq]aver_preoid v_last_time:%d,v_last_count:%d,v_plusewidth_time:%d",U16_V_Last_OneCycleTime,U16_V_Last_CNT,U32_V_CURRENT_PLUSEWIDTH_TIME);
    }
	if (U8_CURR_WorkMode == D_CAL_START_MODE)
	{
		U32_V_REF_PLUSEWIDTH_TIME = U32_V_CURRENT_PLUSEWIDTH_TIME;	   // 校正时取u32_V_Period参数作为参考值	 
		return;
	}
	if(U32_V_CURRENT_PLUSEWIDTH_TIME == 0){
		U16_AC_V = 0;

	}else{
	
		U16_AC_V = U16_V_REF_Data * U32_V_REF_PLUSEWIDTH_TIME/U32_V_CURRENT_PLUSEWIDTH_TIME;
	}
    
    if (U16_AC_V == 0xffff)     //开机时U32_V_CURRENT_PLUSEWIDTH_TIME = 0，计算溢出
    {
        U16_AC_V = 0; 
    }
    
     if (B_V_Last_OVERFLOW == TRUE)
    {
        U16_AC_V = 0;
    }
    
}
/*=====================================================
 * Function : void HLW8012_Measure_I(void)
 * Describe : 
 * Input    : none
 * Output   : none
 * Return   : none
 * Record   : 2014/04/14
=====================================================*/
void HLW8012_Measure_I(void)
{
    if (B_I_Last_TestOneCycle_Mode == 1)
    {
        U32_I_CURRENT_PLUSEWIDTH_TIME = U16_I_Last_OneCycleTime*1000; 
        //PR_NOTICE("[caojq]single_preoid I_last_time:%d,I_plusewidth_time:%d",U16_I_Last_OneCycleTime,U32_I_CURRENT_PLUSEWIDTH_TIME);
    }
    else
    {
    	if(U16_I_Last_CNT < 2){
			return;
		}
         U32_I_CURRENT_PLUSEWIDTH_TIME = U16_I_Last_OneCycleTime*1000/(U16_I_Last_CNT-1); 
//       PR_NOTICE("[caojq]aver_preoid I_last_time:%d,I_last_count:%d,I_plusewidth_time:%d",U16_I_Last_OneCycleTime,U16_I_Last_CNT,U32_I_CURRENT_PLUSEWIDTH_TIME);
    }
	if (U8_CURR_WorkMode == D_CAL_START_MODE)
    {
		U32_I_REF_PLUSEWIDTH_TIME = U32_I_CURRENT_PLUSEWIDTH_TIME;	   // 校正时取U32_V_CURRENT_PLUSEWIDTH_TIME参数作为参考值	 
		return;
	}
	if(U32_I_CURRENT_PLUSEWIDTH_TIME == 0){
		U16_AC_I = 0;
	}else{
		U16_AC_I = U16_I_REF_Data * U32_I_REF_PLUSEWIDTH_TIME/U32_I_CURRENT_PLUSEWIDTH_TIME;
	}

    if (U16_AC_P == 0)
    {
        U16_AC_I = 0;
      
    }
  
    if (U16_AC_I == 0xffff)     //开机时U32_I_CURRENT_PLUSEWIDTH_TIME = 0，计算溢出
    {
        U16_AC_I = 0; 
    }
    
    if (B_I_Last_OVERFLOW == TRUE)
    {
        U16_AC_I = 0;  
    }
}

void gpio_interrupt(void* arg)
{
	uint32 status=GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS,status);
	if((status>>Get_ELE_PORT)&BIT(0)){
		U16_P_TotalTimes = 0;		//完成一次有效的测量，溢出寄存器清零	
		U16_P_CNT++;
		if (B_P_OVERFLOW == TRUE)
		{  
			//从溢出模式转入,开始测量	  
			B_P_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
			U16_P_TotalTimes = 0;		//清溢出寄存器清零
			U16_P_OneCycleTime = 0; 	//清测量寄存器
			U16_P_CNT = 1;				
			B_P_OVERFLOW = FALSE;		//清溢出标志位
		}
		else
		{
			if (B_P_TestOneCycle_Mode == 1)
			{
				if (U16_P_OneCycleTime >= D_TIME1_100MS)
				{
					//单周期测量模式 
					U16_P_Last_OneCycleTime = U16_P_OneCycleTime;
					B_P_Last_TestOneCycle_Mode = B_P_TestOneCycle_Mode;
					B_P_OVERFLOW = FALSE;		//溢出标志位清零
					B_P_Last_OVERFLOW = B_P_OVERFLOW;
					 //清状态参数,重新开始测试
					B_P_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
					U16_P_TotalTimes = 0;		//完成一次有效的测量，溢出寄存器清零
					U16_P_OneCycleTime = 0; 	//清测量寄存器
					U16_P_CNT = 1;
				}
			}
			else
			{
				if (U16_P_OneCycleTime >= D_TIME1_2S)
				{	
					U16_P_Last_OneCycleTime = U16_P_OneCycleTime;
					U16_P_Last_CNT = U16_P_CNT;
					B_P_Last_TestOneCycle_Mode = B_P_TestOneCycle_Mode;
					B_P_OVERFLOW = FALSE;		//溢出标志位清零
					B_P_Last_OVERFLOW = B_P_OVERFLOW;
					//清状态参数,重新开始测试
					B_P_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
					U16_P_TotalTimes = 0;		//完成一次有效的测量，溢出寄存器清零
					U16_P_OneCycleTime = 0; 	//清测量寄存器
					U16_P_CNT = 1;
				}
			}
		}
		//校正模式
		if (U8_CURR_WorkMode == D_CAL_START_MODE)
		{
			//记录单位时间内的用电量
			U16_E_Pluse_CNT++;
		}
		
	//用电量计量，每0.1度电，用电量寄存器增加0.1度
		if (U8_CURR_WorkMode == D_NORMAL_MODE)
		{
			U16_E_Pluse_CNT++;
			if (U16_E_Pluse_CNT == 2 * U16_REF_001_E_Pluse_CNT )
			{
				U16_E_Pluse_CNT = 0;
				U32_AC_E++;
			}
		}
	}
	if((status>>Get_CUR_VOL)&BIT(0)){
		//电压测试模式
		if (B_VI_Test_Mode == 1)
		{
			U16_V_TotalTimes = 0; 
			U16_V_CNT++;
			if (B_V_OVERFLOW == TRUE)
			{				   
				//从溢出模式转入,开始测量	  
				B_V_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
				U16_V_TotalTimes = 0;		//清溢出寄存器清零
				U16_V_OneCycleTime = 0; 	//清测量寄存器
				U16_V_CNT = 1;				
				B_V_OVERFLOW = FALSE;		//清溢出标志位
			}
			else
			{
				if (B_V_TestOneCycle_Mode == 1)
				{
					if (U16_V_OneCycleTime >= D_TIME1_100MS)
					{
						//单周期测量模式 
						U16_V_Last_OneCycleTime = U16_V_OneCycleTime;
						B_V_Last_TestOneCycle_Mode = B_V_TestOneCycle_Mode;
						B_V_OVERFLOW = FALSE;		//溢出标志位清零
						B_V_Last_OVERFLOW = B_V_OVERFLOW;
						 //清状态参数,重新开始测试
						B_V_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
						U16_V_TotalTimes = 0;		//完成一次有效的测量，溢出寄存器清零
						U16_V_OneCycleTime = 0; 	//清测量寄存器
						U16_V_CNT = 1;
					}
				}
				else
				{
					if (U16_V_OneCycleTime >= D_TIME1_300MS)
					{	
						U16_V_Last_OneCycleTime = U16_V_OneCycleTime;
						U16_V_Last_CNT = U16_V_CNT;
						B_V_Last_TestOneCycle_Mode = B_V_TestOneCycle_Mode; 
						B_V_OVERFLOW = FALSE;		//溢出标志位清零
						B_V_Last_OVERFLOW = B_V_OVERFLOW;
						//清状态参数,重新开始测试
						B_V_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
						U16_V_TotalTimes = 0;		//完成一次有效的测量，溢出寄存器清零
						U16_V_OneCycleTime = 0; 	//清测量寄存器
						U16_V_CNT = 1;
						B_V_OVERFLOW = FALSE;		//溢出标志位清零
					}
				}
			}
		 }

	//电流测试模式
		if (B_VI_Test_Mode == 0)
		{
			U16_I_TotalTimes = 0; 
			U16_I_CNT++;
			if (B_I_OVERFLOW == TRUE)
			{
				//从溢出模式转入,开始测量	  
				B_I_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
				U16_I_TotalTimes = 0;		//清溢出寄存器清零
				U16_I_OneCycleTime = 0; 	//清测量寄存器
				U16_I_CNT = 1;				
				B_I_OVERFLOW = FALSE;		//清溢出标志位
			}
			else
			{
				if (B_I_TestOneCycle_Mode == 1)
				{
					if (U16_I_OneCycleTime >= D_TIME1_100MS)
					{
						//单周期测量模式 
						U16_I_Last_OneCycleTime = U16_I_OneCycleTime;
						B_I_Last_TestOneCycle_Mode = B_I_TestOneCycle_Mode;
						B_I_OVERFLOW = FALSE;		//溢出标志位清零
						B_I_Last_OVERFLOW = B_I_OVERFLOW;
						 //清状态参数,重新开始测试
						B_I_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
						U16_I_TotalTimes = 0;		//完成一次有效的测量，溢出寄存器清零
						U16_I_OneCycleTime = 0; 	//清测量寄存器
						U16_I_CNT = 1;
					}
				}
				else
				{
					if (U16_I_OneCycleTime >= D_TIME1_1S)
					{	
						U16_I_Last_OneCycleTime = U16_I_OneCycleTime;
						U16_I_Last_CNT = U16_I_CNT;
						B_I_Last_TestOneCycle_Mode = B_I_TestOneCycle_Mode;  
						B_I_OVERFLOW = FALSE;		//溢出标志位清零
						B_I_Last_OVERFLOW = B_I_OVERFLOW;
						//清状态参数,重新开始测试
						B_I_TestOneCycle_Mode = 0;	//初始化为计数脉冲测量模式
						U16_I_TotalTimes = 0;		//完成一次有效的测量，溢出寄存器清零
						U16_I_OneCycleTime = 0; 	//清测量寄存器
						U16_I_CNT = 1;
					}
				}
			}
		}
	}
}

STATIC VOID hw_test_timer_cb(void)
{
	if (U8_CURR_WorkMode == D_CAL_START_MODE)
    {
        U32_Cal_Times++;
        if (U32_Cal_Times == D_TIME1_CAL_TIME)
        {
            U16_REF_001_E_Pluse_CNT = E_001E_GAIN * U16_E_Pluse_CNT;		//记录校正时间内的脉冲数，此脉冲数表示0.0005度用电量
            PostSemaphore(ele_cal_busy);
        }
    }
    
//功率测量
    if (U16_P_CNT != 0)
    {
        U16_P_OneCycleTime++;
        U16_P_TotalTimes++;
    }

    if (U16_P_TotalTimes >= D_TIME1_P_OVERFLOW)
    {
        B_P_OVERFLOW = TRUE; 		//溢出，
        B_P_Last_OVERFLOW = B_P_OVERFLOW;
        //清状态参数,重新开始测试
        U16_P_TotalTimes = 0;       //清溢出寄存器
        U16_P_OneCycleTime = 0;
        U16_P_CNT = 0;              //等待下一次中断开始计数
        B_P_TestOneCycle_Mode = 0;   //初始化为计数脉冲测量模式      
    }
    else if (U16_P_OneCycleTime == D_TIME1_100MS)
    {
		if (U16_P_CNT < 2)
		{
			// 100ms内只有一次中断，说明周期>100ms,采用单周期测量模式 
			B_P_TestOneCycle_Mode = 1;
		}
		else
		{
			 // 100ms内有2次或以上数量脉冲，说明周期<100ms，采用计数脉冲测量模式
			 B_P_TestOneCycle_Mode = 0;   
		}
    }
    
//电压、电流测量
    if (B_VI_Test_Mode == 1)
    {
        //电压测量 
        if (U16_V_CNT != 0)
    	{
	        U16_V_OneCycleTime++;
            U16_V_TotalTimes++;
        }

        if (U16_V_TotalTimes >= D_TIME1_V_OVERFLOW)
        {
            B_V_OVERFLOW = TRUE; 
            B_V_Last_OVERFLOW = B_V_OVERFLOW;
            //清状态参数,重新开始测试
            U16_V_TotalTimes = 0;       //清溢出寄存器
            U16_V_OneCycleTime = 0;
            U16_V_CNT = 0;              
            B_V_TestOneCycle_Mode = 0;   //初始化为计数脉冲测量模式  
        }
        else if (U16_V_OneCycleTime == D_TIME1_100MS)
        {
			if (U16_V_CNT < 2)
			{
				// 100ms内只有一次中断，说明周期>100ms,采用单周期测量模式 
				B_V_TestOneCycle_Mode = 1;
			}
			else
			{
				// 100ms内有2次或以上数量脉冲，说明周期<100ms，采用计数脉冲测量模式
				B_V_TestOneCycle_Mode = 0;   
			}
        }
    }
    else
    {
        //电流测量   
        if (U16_I_CNT != 0)
    	{
			U16_I_OneCycleTime++;
			U16_I_TotalTimes++;
        }

        if (U16_I_TotalTimes >= D_TIME1_I_OVERFLOW)
        {
            B_I_OVERFLOW = TRUE; 
            B_I_Last_OVERFLOW = B_I_OVERFLOW;
            //清状态参数,重新开始测试
            U16_I_TotalTimes = 0;       //清溢出寄存器
            U16_I_OneCycleTime = 0;
            U16_I_CNT = 0;
            B_I_TestOneCycle_Mode = 0;   //初始化为计数脉冲测量模式      
        }
        else if (U16_I_OneCycleTime == D_TIME1_100MS)
        {
			if (U16_I_CNT < 2)
			{
			// 100ms内只有一次中断，说明周期>100ms,采用单周期测量模式 
			B_I_TestOneCycle_Mode = 1;
			}
			else
			{
			 // 100ms内有2次或以上数量脉冲，说明周期<100ms，采用计数脉冲测量模式
			 B_I_TestOneCycle_Mode = 0;   
			}
        }
    }
      

//电压、电流测量模式切换  B_VI_Test_Mode:(1:电压测量模式) (0:电流测试模式) 
    U16_VI_Test_Times--;
	if(B_VI_Test_Mode == 1){
		if(U16_VI_Test_Times == V_CAL_TIME ){//此条件不会触发,电流电压功率数据同时更新
			ele_cal_flag = TRUE;
		}
	}else{
		if(U16_VI_Test_Times == 7500) { /* 8S接近临界值，可能会出现误测，后改为7.5S */
			ele_cal_flag = TRUE;
		}
	} 
	
    if (U16_VI_Test_Times == 0)
    {
    	ele_cal_flag = FALSE;
        if (B_VI_Test_Mode == 1)
        {
            //转为电流测量模式
            B_VI_Test_Mode = 0;
            if(CUR_VOL_SWITCH < 16){
                ENTER_I_MODE;
            }else{
                GPIO16_ENTER_I;
            }
            U16_VI_Test_Times = I_CAL_TIME;
            //清状态参数
            U16_I_TotalTimes = 0;
            U16_I_OneCycleTime = 0;
            U16_I_CNT = 0;
            B_I_OVERFLOW = FALSE; 
        }
        else
        {
            //转为电压测量模式
            B_VI_Test_Mode = 1;
            if(CUR_VOL_SWITCH < 16){
                ENTER_V_MODE;
            }else{
                GPIO16_ENTER_V;
            }
            U16_VI_Test_Times = V_CAL_TIME;
            //清状态参数
            U16_V_TotalTimes = 0;
            U16_V_OneCycleTime = 0;
            U16_V_CNT = 0;
            B_V_OVERFLOW = FALSE; 
        }
    }
}

/*********************************************************************************
 * FUNCTION:       save_prod_test_data
 * DESCRIPTION:    产测参数保存
 * INPUT:          state：产测结果
 * OUTPUT:         none
 * RETURN:         OPERATE_RET：返回结果
 * OTHERS:         产测结果保存在flash中
 * HISTORY:        2020-03-04
 *******************************************************************************/
STATIC VOID Parameter_Init(void)
{

	U16_AC_P = 0;
	U16_AC_V = 0;
	U16_AC_I = 0;
	P_VAL = 0;
	V_VAL = 0;
	I_VAL = 0;
	E_VAL = 0;

	U16_P_TotalTimes = 0;
	U16_V_TotalTimes = 0;
	U16_I_TotalTimes = 0;

	U16_P_OneCycleTime = 0;
	U16_V_OneCycleTime = 0;
	U16_I_OneCycleTime = 0;
	U16_P_Last_OneCycleTime = 0;
	U16_V_Last_OneCycleTime = 0;
	U16_I_Last_OneCycleTime = 0;

	U16_P_CNT = 0;
	U16_V_CNT = 0;
	U16_I_CNT = 0;
	U16_P_Last_CNT = 0;
	U16_V_Last_CNT = 0;
	U16_I_Last_CNT = 0;

	//初始化单周期测量模式
	B_P_TestOneCycle_Mode = 1;
	B_V_TestOneCycle_Mode = 1;
	B_I_TestOneCycle_Mode = 1;
	B_P_Last_TestOneCycle_Mode = 1;
	B_V_Last_TestOneCycle_Mode = 1;
	B_I_Last_TestOneCycle_Mode = 1;

	//开始测量，置溢出标志位为1  
	B_P_OVERFLOW = 1;
	B_V_OVERFLOW = 1;
	B_I_OVERFLOW = 1;

	B_P_Last_OVERFLOW = 1;
	B_V_Last_OVERFLOW = 1;
	B_I_Last_OVERFLOW = 1;

	//上电初始化为电压测试模式 
	B_VI_Test_Mode = 1;
	if(CUR_VOL_SWITCH < 16){
        ENTER_V_MODE;
	}else{
        GPIO16_ENTER_V;
	}
	U16_VI_Test_Times = V_CAL_TIME;

	U32_AC_E = 0;
	E_VAL = 0;
	U16_E_Pluse_CNT = 0;


	//设置默认值
	U16_P_REF_Data = DEF_P;     // 857 =  85.7W
	U16_V_REF_Data = DEF_V;     // 2202 = 220.2V
	U16_I_REF_Data = DEF_I;     // 386 =  386mA

    U32_Cal_Times = 0;
}

/*********************************************************************************
 * FUNCTION:       save_prod_test_data
 * DESCRIPTION:    产测参数保存
 * INPUT:          state：产测结果
 * OUTPUT:         none
 * RETURN:         OPERATE_RET：返回结果
 * OTHERS:         产测结果保存在flash中
 * HISTORY:        2020-03-04
 *******************************************************************************/
OPERATE_RET save_prod_test_data(INT state)
{
	OPERATE_RET op_ret;
    cJSON *root = NULL;
	UCHAR *buf = NULL;
    root = cJSON_CreateObject();
    if(NULL == root) {
		PR_ERR("cJSON_CreateObject error");
		return OPRT_CJSON_GET_ERR;
	}
    
    cJSON_AddNumberToObject(root, "prod_rslt", state);
    buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("buf is NULL");
        cJSON_Delete(root);
        return OPRT_COM_ERROR;
    }    
    cJSON_Delete(root);
    PR_DEBUG("set:%s", buf);
	op_ret = tuya_psm_set_single(DEVICE_MOD,PROD_RSLT_KEY,buf);
    Free(buf);
	if(OPRT_OK != op_ret) {
		PR_DEBUG("msf_set_prod_result err:%d",op_ret);
		return op_ret;
	}
    
    return OPRT_OK;    
}

 /*********************************************************************************
  * FUNCTION:       get_prod_test_data
  * DESCRIPTION:    获取产测结
  * INPUT:          state：产测结果
  * OUTPUT:         none
  * RETURN:         OPERATE_RET：返回结果
  * OTHERS:         获取flash中产测结果
  * HISTORY:        2020-03-04
  *******************************************************************************/
 OPERATE_RET get_prod_test_data(INT *state)
{
	OPERATE_RET op_ret;
    cJSON *root = NULL, *json = NULL;
	UCHAR *buf;

    buf = (UCHAR *)Malloc(256);
	if(NULL == buf) {
		PR_ERR("malloc error");
		return OPRT_MALLOC_FAILED;
	}

	op_ret = tuya_psm_get_single(DEVICE_MOD,PROD_RSLT_KEY,buf,256);
	if(OPRT_OK != op_ret) {
		PR_DEBUG("msf_get_prod_result err:%d",op_ret);
        Free(buf);
		return op_ret;
	}
	PR_DEBUG("get:%s", buf);
	root = cJSON_Parse(buf);
	if(NULL == root) {
		PR_ERR("cjson parse");
        goto JSON_PARSE_ERR;
	}

    json = cJSON_GetObjectItem(root,"prod_rslt");
    if(NULL == json) {
        PR_ERR("cjson get ");
        goto JSON_PARSE_ERR;
	}

    *state = json->valueint;
    cJSON_Delete(root);
    Free(buf);
    return OPRT_OK;

JSON_PARSE_ERR:
    cJSON_Delete(root);
    Free(buf);
    return OPRT_COM_ERROR;
}

STATIC OPERATE_RET set_coefficient(VOID)
{
	OPERATE_RET op_ret;
    cJSON *root = NULL;
	UCHAR *buf = NULL;

    root = cJSON_CreateObject();
    if(NULL == root) {
		PR_ERR("cJSON_CreateObject error");
		return OPRT_CJSON_GET_ERR;
	}
    
    cJSON_AddNumberToObject(root, "Kp", U32_P_REF_PLUSEWIDTH_TIME);
	cJSON_AddNumberToObject(root, "Kv", U32_V_REF_PLUSEWIDTH_TIME);
	cJSON_AddNumberToObject(root, "Ki", U32_I_REF_PLUSEWIDTH_TIME);
	cJSON_AddNumberToObject(root, "Ke", U16_REF_001_E_Pluse_CNT);
    buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("buf is NULL");
        cJSON_Delete(root);
        return OPRT_COM_ERROR;
    }    
	PR_DEBUG("set_coefficient_to_flash:%s",buf);
    cJSON_Delete(root);

    op_ret = tuya_psm_set_single(DEVICE_MOD, COE_SAVE_KEY, buf);
    Free(buf);
    buf = NULL;
    if (OPRT_OK != op_ret) {
        PR_DEBUG("set_coefficient_to_flash_err:%d", op_ret);
        return op_ret;
    }

    return OPRT_OK;
}


STATIC OPERATE_RET get_coefficient(OUT ELE_CAL_DATA *d)
{
	if (NULL == d) {
		PR_ERR("input param is null!");
		return OPRT_INVALID_PARM;
	}
	OPERATE_RET op_ret;
    cJSON *root = NULL, *json = NULL;
	UCHAR *buf;
    buf = (UCHAR *)Malloc(256);
	if(NULL == buf) {
		PR_ERR("malloc error");
		return OPRT_MALLOC_FAILED;
	}

	op_ret = tuya_psm_get_single(DEVICE_MOD,COE_SAVE_KEY,buf,256);
	if(OPRT_OK != op_ret) {
		PR_DEBUG("msf_get_coe err:%d",op_ret);
        Free(buf);
		return op_ret;
	}
    PR_DEBUG("msf_get coe:%s",buf);

	root = cJSON_Parse(buf);
	if(NULL == root) {
		PR_ERR("cjson parse err");
        goto JSON_PARSE_ERR;
	}

    json = cJSON_GetObjectItem(root,"Kv");
    if(NULL == json) {
        goto JSON_PARSE_ERR;
	}else{
		d->v = json->valueint;
		
	}
	json = cJSON_GetObjectItem(root,"Ki");
    if(NULL == json) {
        goto JSON_PARSE_ERR;
	}else{
		d->i = json->valueint;
		
	}
	json = cJSON_GetObjectItem(root,"Kp");
    if(NULL == json) {
        goto JSON_PARSE_ERR;
	}else{
		d->p = json->valueint;
	}
	json = cJSON_GetObjectItem(root,"Ke");
	if(NULL == json) {
        goto JSON_PARSE_ERR;
	}else{
		d->e = json->valueint;
	}
	//读不到产测结果，说明未进行产测，prod_rslt = 2
    op_ret = get_prod_test_data(&d->prod_rslt);
	if(OPRT_OK != op_ret) {
		PR_NOTICE("not get product test data:%d",op_ret);
		d->prod_rslt = 2;	
	}
	// 如果读不到产测结果，又有电压校准值，说明是最早的一批固件升级上来的。最早的一批固件不存储prod_rslt，只存v,i,p,e
	if (2 == d->prod_rslt && d->v) {
		d->prod_rslt = 1;
	}
    PR_DEBUG("get_prod_test_data_success:%d",d->prod_rslt);
    cJSON_Delete(root);
    Free(buf);
    return OPRT_OK;

JSON_PARSE_ERR:
    cJSON_Delete(root);
    Free(buf);
    return OPRT_COM_ERROR;
}

STATIC VOID timer_init(u32 delay_times)
{
	hw_timer_init(1,hw_test_timer_cb);
	//1ms 中断一次
	hw_timer_arm(delay_times);
    hw_timer_enable();
}

STATIC VOID gpio_intr_init(VOID)
{
    GPIO_ConfigTypeDef gpio_test;
	gpio_test.GPIO_Pin=1<<(Get_ELE_PORT )|( 1<<Get_CUR_VOL);
	gpio_test.GPIO_Mode=GPIO_Mode_Input;
	gpio_test.GPIO_IntrType=GPIO_PIN_INTR_NEGEDGE;
	gpio_test.GPIO_Pullup=GPIO_PullUp_EN;
    gpio_config(&gpio_test);

	gpio_intr_handler_register(gpio_interrupt);
	_xt_isr_unmask(1<<ETS_GPIO_INUM);//Enable GPIO
    if(CUR_VOL_SWITCH < 16){
        GPIO_ConfigTypeDef config = {BIT(CUR_VOL_SWITCH), GPIO_Mode_Output, GPIO_PullUp_EN, GPIO_PIN_INTR_DISABLE};
        gpio_config(&config);
    }else{
        gpio16_output_conf();
    }

}

STATIC VOID get_ele_timer_cb(UINT timerID, PVOID pTimerArg)
{
    if (ele_cal_flag == TRUE) {
        HLW8012_Measure_P();
        HLW8012_Measure_V();
        HLW8012_Measure_I();
        xSemaphoreTake(get_ele_busy, portMAX_DELAY);
        P_VAL = U16_AC_P;
        V_VAL = U16_AC_V;
        I_VAL = U16_AC_I;
        E_VAL += U32_AC_E;
        U32_AC_E = 0;
        xSemaphoreGive(get_ele_busy);
    }
}

STATIC BOOL cal_data_judge(VOID)
{

    BOOL opt = TRUE;
    PR_NOTICE("V:%d I:%d P:%d E:%d",
        U32_V_REF_PLUSEWIDTH_TIME,
        U32_I_REF_PLUSEWIDTH_TIME,
        U32_P_REF_PLUSEWIDTH_TIME,
        U16_REF_001_E_Pluse_CNT);

    if (my_abs(U32_V_REF_PLUSEWIDTH_TIME, driver_dltj.v_ref) > driver_dltj.v_ref / 2)
        opt = FALSE;
    if (my_abs(U32_I_REF_PLUSEWIDTH_TIME, driver_dltj.i_ref) > driver_dltj.i_ref / 2)
        opt = FALSE;
    if (my_abs(U32_P_REF_PLUSEWIDTH_TIME, driver_dltj.p_ref) > driver_dltj.p_ref / 2)
        opt = FALSE;
    if (my_abs(U16_REF_001_E_Pluse_CNT, driver_dltj.e_ref) > driver_dltj.e_ref / 2)
        opt = FALSE;
    ele_cal_data.v = U32_V_REF_PLUSEWIDTH_TIME;
    ele_cal_data.i = U32_I_REF_PLUSEWIDTH_TIME;
    ele_cal_data.p = U32_P_REF_PLUSEWIDTH_TIME;
    ele_cal_data.e = U16_REF_001_E_Pluse_CNT;
    ele_cal_data.prod_rslt = opt ? 1 : 0;
    return opt;
}

void ele_coe_init()
{
    if (1 == ele_cal_data.prod_rslt) { 
        U32_V_REF_PLUSEWIDTH_TIME = ele_cal_data.v;
        U32_I_REF_PLUSEWIDTH_TIME = ele_cal_data.i;
        U32_P_REF_PLUSEWIDTH_TIME = ele_cal_data.p;
        U16_REF_001_E_Pluse_CNT = ele_cal_data.e;
		PR_NOTICE("ele_coe have been set to cal value!");
    } else {
        U32_V_REF_PLUSEWIDTH_TIME = driver_dltj.v_ref;
        U32_I_REF_PLUSEWIDTH_TIME = driver_dltj.i_ref;
        U32_P_REF_PLUSEWIDTH_TIME = driver_dltj.p_ref;
        U16_REF_001_E_Pluse_CNT = driver_dltj.e_ref;
        PR_NOTICE("ele_coe have been set to default value!");
    }
	PR_DEBUG("ele_coe:v=%d,i=%d,p=%d,e=%d",
		U32_V_REF_PLUSEWIDTH_TIME, U32_I_REF_PLUSEWIDTH_TIME,
		U32_P_REF_PLUSEWIDTH_TIME, U16_REF_001_E_Pluse_CNT);
}

/*********************************************************************************
 * FUNCTION:       ele_cnt_init
 * DESCRIPTION:    电量脉冲计数初始化
 * INPUT:          mode：计量芯片的模式(计量模式)
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         计量芯片初始化/硬件定时器初始化/中断初始化/脉冲计数初始化
 * HISTORY:        2020-03-04
 *******************************************************************************/
OPERATE_RET ele_cnt_init(INT mode)
{
    OPERATE_RET op_ret;
    U8_CURR_WorkMode = mode;
    if (U8_CURR_WorkMode != D_CAL_START_MODE) {
        op_ret = get_ele_coe_flash();
        ele_coe_init();
    } else {
        U32_V_REF_PLUSEWIDTH_TIME = 0;
        U32_I_REF_PLUSEWIDTH_TIME = 0;
        U32_P_REF_PLUSEWIDTH_TIME = 0;
        U16_REF_001_E_Pluse_CNT = 0;
        PR_DEBUG("enter ele cal mode");
    }
    Parameter_Init();
    timer_init(1000);
    gpio_intr_init();

    vSemaphoreCreateBinary(get_ele_busy); //创造信号量
    op_ret = sys_add_timer(get_ele_timer_cb, NULL, &get_ele_timer);
    if(OPRT_OK != op_ret) {
		hw_timer_disable();
		_xt_isr_unmask(0<<ETS_GPIO_INUM);//disable GPIO
		PR_ERR("add get_ele_timer err,ele cnt is disable");
    	return op_ret;
    }else {
    	sys_start_timer(get_ele_timer,500,TIMER_CYCLE);
    }

	if(U8_CURR_WorkMode == D_CAL_START_MODE){
      
		ele_cal_busy = CreateSemaphore();
		if(NULL == ele_cal_busy){
            hw_timer_disable();
		    _xt_isr_unmask(0<<ETS_GPIO_INUM);//disable GPIO
	        sys_delete_timer(get_ele_timer); 
			return OPRT_COM_ERROR;
		}
	    op_ret = InitSemaphore(ele_cal_busy,0,1);
	    if(OPRT_OK != op_ret) {
            hw_timer_disable();
		    _xt_isr_unmask(0<<ETS_GPIO_INUM);//disable GPIO
	        sys_delete_timer(get_ele_timer); 
            ReleaseSemaphore(ele_cal_busy);       
            return op_ret;
	    }
		WaitSemaphore(ele_cal_busy);
        hw_timer_disable();
		_xt_isr_unmask(0<<ETS_GPIO_INUM);//disable GPIO
	    sys_delete_timer(get_ele_timer); 
	    ReleaseSemaphore(ele_cal_busy); 
	    PR_DEBUG("prod test has last for %d mS",D_TIME1_CAL_TIME);
		if(FALSE == cal_data_judge()){
		    INT pt_result = 0; // 如果之前产测成功过，这次失败不会保存数据，防止生产时将正确数据覆盖
		    if( OPRT_OK == get_prod_test_data(&pt_result) && 1 == pt_result){
                return OPRT_COM_ERROR;
		    }else{ // 如果之前未产测成功，失败会保存数据，主要用于调试。
                op_ret = set_coefficient();
                if(OPRT_OK != op_ret){
                    return op_ret;
                }
                op_ret = save_prod_test_data(0);
                if(OPRT_OK != op_ret){
                    return op_ret;
                }
		    }
			return OPRT_COM_ERROR;
		}else{
            op_ret = set_coefficient();
            if(OPRT_OK != op_ret){
                return op_ret;
            }
            op_ret = save_prod_test_data(1);;
            if(OPRT_OK != op_ret){
                return op_ret;
            }
			return OPRT_OK;
		}
	}

	return OPRT_OK;
}

/*********************************************************************************
 * FUNCTION:       get_ele_par
 * DESCRIPTION:    获取电量实时参数
 * INPUT:          p/v/i:功率/电压/电流
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         获取电量芯片实时参数
 * HISTORY:        2020-03-04
 *******************************************************************************/
VOID get_ele_par(OUT u32 *P,OUT u32 *V,OUT u32 *I)
{
	xSemaphoreTake(get_ele_busy,portMAX_DELAY);
	*P = P_VAL;
	*V = V_VAL;
	*I = I_VAL;
	xSemaphoreGive(get_ele_busy);
}

/*********************************************************************************
 * FUNCTION:       get_ele
 * DESCRIPTION:    获取增加电量参数
 * INPUT:          E:增加电量值
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         获取电量芯片增加电量参数
 * HISTORY:        2020-03-04
 *******************************************************************************/
VOID get_ele(OUT UINT *E)
{
	xSemaphoreTake(get_ele_busy,portMAX_DELAY);
	*E = E_VAL;
	E_VAL = 0;
	xSemaphoreGive(get_ele_busy);
}

 /*********************************************************************************
  * FUNCTION:       report_coe_data
  * DESCRIPTION:    上报电量统计校准参数及产测结果位
  * INPUT:          none
  * OUTPUT:         none
  * RETURN:         none
  * OTHERS:         电量统计校准参数和产测结果
  * HISTORY:        2020-03-04
  *******************************************************************************/
OPERATE_RET report_coe_data(VOID)
 {
     cJSON *root = NULL;
     root = cJSON_CreateObject();
     if(NULL == root) {
         return OPRT_CR_CJSON_ERR;
     }
     
     char dpid_str[8];
     sprintf(dpid_str, "%d", driver_dltj.dp_pt_rslt);
     cJSON_AddNumberToObject(root,dpid_str,ele_cal_data.prod_rslt);
     sprintf(dpid_str, "%d", driver_dltj.dp_vcoe);
     cJSON_AddNumberToObject(root,dpid_str,ele_cal_data.v);
     sprintf(dpid_str, "%d", driver_dltj.dp_icoe);
     cJSON_AddNumberToObject(root,dpid_str,ele_cal_data.i);
     sprintf(dpid_str, "%d", driver_dltj.dp_pcoe);
     cJSON_AddNumberToObject(root,dpid_str,ele_cal_data.p);
     sprintf(dpid_str, "%d", driver_dltj.dp_ecoe);
     cJSON_AddNumberToObject(root,dpid_str,ele_cal_data.e);
 
     CHAR *out;
     out=cJSON_PrintUnformatted(root);
     cJSON_Delete(root);
     if(NULL == out) {
         PR_ERR("cJSON_PrintUnformatted err:");
         return OPRT_CJSON_GET_ERR;
     }
     
     PR_DEBUG("to report:[%s]", out);

     OPERATE_RET op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,out);
     Free(out);
     if(OPRT_OK != op_ret) {
         PR_ERR("report dltj param err:%d",op_ret);
         return OPRT_DP_REPORT_CLOUD_ERR;
     }

     PR_DEBUG("report pt_rst:%d, v_ref:%d,i_ref:%d, p_ref:%d, e_ref:%d",\
     ele_cal_data.v,ele_cal_data.i,ele_cal_data.p,ele_cal_data.e,ele_cal_data.prod_rslt);
     return OPRT_OK;
 }

OPERATE_RET get_ele_coe_flash()
{
	OPERATE_RET op_ret = OPRT_OK;
		INT result = 0;
		op_ret = get_coefficient(&ele_cal_data);
		if (OPRT_OK != op_ret || 1 != ele_cal_data.prod_rslt) {
			PR_NOTICE("not get ele cal coe:%d", op_ret);
			return OPRT_COM_ERROR;
		} else {
			PR_DEBUG("get ele cal coe success.");
			return OPRT_OK;
		}
}

