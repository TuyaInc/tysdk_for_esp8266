#ifndef _CALCULATE_H__  
#define _CALCULATE_H__       

//attributes
typedef enum
{
	RGBMode = 0,
	CCTMode = 1,
} RGBorCCTFLAG;

typedef enum
{  
    ATTRRED = 0,
    ATTRGREEN = 1,
    ATTRBLUE = 2,
    ATTRWHITE = 3,
    ALLATTRRGBW = 4
} RGBWFLAG;

typedef enum
{  
    ATTRx = 0,
    ATTRy = 1,
    ATTRY = 2,   
    ALLxyY = 3
} xyYFLAG;


#define RGBW_LIGHT			1
#define RGBW_LIGHTSWITH	2
#define RGBW_HOMEKIT		3
#define RGBW_COSTDOWN		4
#define RGBW_TP_JP         5
#define RGBW_TP_NA         6
#define RGBW_TP_EU         7



extern float fCalibration[ALLATTRRGBW][ALLxyY];

/**
 * @brief 
 *
 * @param [in] unsigned char u8LightType  
 * @param [in] float temperature  
 * @param [out] None
 * 
 * @return 
 * 
 * @history
 * 1.Date          : 3/21/2018 11:31:45
 *   Author       : wangjinluan
 *   Modification : Created function
 */
extern void RGBWCalibrationInit(unsigned char u8RGBWType, float temperature);
    
/**
 * @brief 
 *
 * @param [in] float *xyYCali            
 * @param [in] unsigned char u8RGBWflag  
 * @param [in] unsigned char u8xyYflag   
 * @param [out] None
 * 
 * @return extern
 * 
 * @history
 * 1.Date          : 1/26/2018 11:41:50
 *   Author       : wangjinluan
 *   Modification : Created function
 */
extern void RGBWCalibration(float *xyYCali, unsigned char u8RGBWflag, unsigned char u8xyYflag);

/**
 * @brief RGBWSetColortemp
 *
 * @param [in] unsigned char u8RGBWType : which type of RGBW         
 * @param [in] unsigned char target_colortemp : MIN_COLORTEMP~MAX_COLORTEMP
 * @param [in] unsigned char target_brightness : 0~100 
 * @param [in] unsigned char temperature : the current temperature 0~125 
 * @param [out] float *brightness_fac : the brightness fac   
 * @param [out] float *fPWMRGBW :the pwm_fac of every pin(r, g, b, w)    
 * 
 * @return extern
 * 
 * @history
 * 1.Date          : 12/28/2017 16:38:41
 *   Author       : wangjinluan
 *   Modification : Created function
 *
 * 2.Date        : 3/14/2018 15:8:35
 *   Author      : wangjinluan
 *   Modification: add brightness_fac param
 */
extern void RGBWSetColortemp(unsigned char u8RGBWType, unsigned short target_colortemp, unsigned char target_brightness, float temperature, float *brightness_fac, float *fPWMRGBW);

/**
 * @brief RGBWSetRgbw
 *
 * @param [in] unsigned char u8RGBWType : which type of RGBW         
 * @param [in] unsigned char r : 0~255
 * @param [in] unsigned char g : 0~255
 * @param [in] unsigned char b : 0~255
 * @param [in] unsigned char target_brightness : 0~100 
 * @param [in] unsigned char temperature : the current temperature 0~125 
 * @param [out] float *brightness_fac : the brightness fac   
 * @param [out] float *fPWMRGBW : the pwm_fac of every pin(r, g, b, w)                 
 * @param [out] None
 * 
 * @return extern
 * 
 * @history
 * 1.Date          : 12/28/2017 16:38:48
 *   Author       : wangjinluan
 *   Modification : Created function
 *
 * 2.Date        : 3/14/2018 15:8:35
 *   Author      : wangjinluan
 *   Modification: add brightness_fac param
 */
extern void RGBWSetRgbw(unsigned char u8RGBWType, unsigned char r, unsigned char g, unsigned char b, unsigned char target_brightness, float temperature, float *brightness_fac, float *fPWMRGBW);

/**
 * @brief CCTSetBrightness
 *    
 * @param [in] unsigned char target_brightness : 0~100
 * @param [out] float *fPWMCCT : the pwm_fac of dim pin
 * 
 * @return extern
 * 
 * @history
 * 1.Date          : 12/28/2017 16:38:54
 *   Author       : wangjinluan
 *   Modification : Created function
 */
extern void CCTSetBrightness(unsigned char target_brightness, float *fPWMDIM);

/**
 * @brief CCTSetColortemp
 *
 * @param [in] unsigned short target_colortemp : MIN_COLORTEMP~MAX_COLORTEMP
 * @param [out] float *fPWMCCT : the pwm_fac of cct pin               
 * @param [out] None
 * 
 * @return extern
 * 
 * @history
 * 1.Date          : 12/28/2017 16:49:42
 *   Author       : wangjinluan
 *   Modification : Created function
 */
extern void CCTSetColortemp(unsigned short target_colortemp, float *fPWMCCT);

/**
 * @brief DIMSetBrightness
 *
 * @param [in] unsigned char target_brightness : 0~100
 * @param [out] float *fPWMDIM : the pwm_fac of dim pin
 * 
 * @return extern
 * 
 * @history
 * 1.Date          : 12/28/2017 16:49:47
 *   Author       : wangjinluan
 *   Modification : Created function
 */
extern void DIMSetBrightness(unsigned char target_brightness, float *fPWMDIM);

/**
 * @brief 
 *
 * @param [in] unsigned short *major_version  
 * @param [in] unsigned short *minor_version  
 * @param [in] unsigned short *build_version  
 * @param [out] None
 * 
 * @return extern
 * 
 * @history
 * 1.Date          : 3/21/2018 11:22:28
 *   Author       : wangjinluan
 *   Modification : Created function
 */
extern void GetCalVersion(unsigned short *major_version, unsigned short *minor_version, unsigned short *build_version);

extern float GetCurrentPower(unsigned char u8LightType, float *fPWMR, float *fPWMG, float *fPWMB, float *fPWMW);
#endif

