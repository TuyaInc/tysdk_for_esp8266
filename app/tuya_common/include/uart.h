/*
 *  Copyright (C) 2010 -2011  Espressif System
 *
 */

#ifndef __UART_H__
#define __UART_H__

typedef enum {
    BIT_RATE_300     = 300,
    BIT_RATE_600     = 600,
    BIT_RATE_1200    = 1200,
    BIT_RATE_2400    = 2400,
    BIT_RATE_4800    = 4800,
    BIT_RATE_9600    = 9600,
    BIT_RATE_19200   = 19200,
    BIT_RATE_38400   = 38400,
    BIT_RATE_57600   = 57600,
    BIT_RATE_74880   = 74880,
    BIT_RATE_115200  = 115200,
    BIT_RATE_230400  = 230400,
    BIT_RATE_460800  = 460800,
    BIT_RATE_921600  = 921600,
    BIT_RATE_1843200 = 1843200,
    BIT_RATE_3686400 = 3686400,
} UART_BautRate; //you can add any rate you need in this range

typedef enum {
    UART0 = 0x0,
    UART1 = 0x1,
} UART_Port;

typedef enum {
    UART_WordLength_5b = 0x0,
    UART_WordLength_6b = 0x1,
    UART_WordLength_7b = 0x2,
    UART_WordLength_8b = 0x3
} UART_WordLength;

typedef enum {
    USART_StopBits_1   = 0x1,
    USART_StopBits_1_5 = 0x2,
    USART_StopBits_2   = 0x3,
} UART_StopBits;

typedef enum {
    USART_Parity_None = 0x2,
    USART_Parity_Even = 0x0,
    USART_Parity_Odd  = 0x1
} UART_ParityMode;

typedef enum {
    PARITY_DIS = 0x0,
    PARITY_EN  = 0x2
} UartExistParity;

typedef enum {
    USART_HardwareFlowControl_None    = 0x0,
    USART_HardwareFlowControl_RTS     = 0x1,
    USART_HardwareFlowControl_CTS     = 0x2,
    USART_HardwareFlowControl_CTS_RTS = 0x3
} UART_HwFlowCtrl;

typedef enum {
    UART_None_Inverse = 0x0,
    UART_Rxd_Inverse  = UART_RXD_INV,
    UART_CTS_Inverse  = UART_CTS_INV,
    UART_Txd_Inverse  = UART_TXD_INV,
    UART_RTS_Inverse  = UART_RTS_INV,
} UART_LineLevelInverse;

/*************************************************************************************
函数功能: 打印串口号设置
输入参数: uart_no   串口号
输出参数: 无
返 回 值: 无
备    注: 默认打印波特率74880
          如uart_io==UART1，则打印信息由IO2端口输出
          tysdk默认所有的打印信息由IO2端口输出，用户在编写应用代码时可通过该接口重配置
*************************************************************************************/
void print_port_init(UART_Port uart_no);

/*************************************************************************************
函数功能: 打印串口参数设置
输入参数: uart_no    串口号
          bit_rate   波特率(300-3686400)
          data_bits  数据位
          parity     奇偶校验位
          stop_bits  停止位
输出参数: 无
返 回 值: 无
备    注: 如uart_io==UART1，则打印信息由IO2端口输出
          tysdk默认所有的打印信息由IO2端口输出，用户在编写应用代码时可通过该接口重配置
*************************************************************************************/
void print_port_full_init(UART_Port uart_no,UART_BautRate bit_rate,UART_WordLength data_bits,\
    UART_ParityMode parity,UART_StopBits stop_bits);

/*************************************************************************************
函数功能: UART0串口初始化配置
输入参数: bit_rate   波特率(300-3686400)
          data_bits  数据位
          parity     奇偶校验位
          stop_bits  停止位
输出参数: 无
返 回 值: 无
备    注: 如果采用该接口初始化UART0，I015电路需做特殊处理，具体可见乐鑫原厂文档
          采用该端口进行串口通讯的好处是，可以避免8266启动后默认信息输出对用户控制板的数据干扰
*************************************************************************************/
void user_uart_full_init(UART_BautRate bit_rate,UART_WordLength data_bits,\
    UART_ParityMode parity,UART_StopBits stop_bits);

/*************************************************************************************
函数功能: UART0串口初始化配置
输入参数: bit_rate   波特率(300-3686400)
          data_bits  数据位
          parity     奇偶校验位
          stop_bits  停止位
输出参数: 无
返 回 值: 无
备    注: esp8266默认UART0的收发IO端口
*************************************************************************************/
void user_uart_raw_full_init(UART_BautRate bit_rate,UART_WordLength data_bits,\
    UART_ParityMode parity,UART_StopBits stop_bits);

/*************************************************************************************
函数功能: UART0串口初始化,esp8266的收发端口设置为I015:TX IO13:RX
输入参数: bit_rate   波特率(300-3686400)
输出参数: 无
返 回 值: 无
备    注: 默认data_bits==8,parity==无,stop_bits==0
          如果采用该接口初始化UART0，I015电路需做特殊处理，具体可见乐鑫原厂文档
          采用该端口进行串口通讯的好处是，可以避免8266启动后默认信息输出对用户控制板的数据干扰
*************************************************************************************/
void user_uart_init(UART_BautRate bit_rate);

/*************************************************************************************
函数功能: UART0串口初始化,esp8266的收发端口设置为IO3:RX IO1:TX
输入参数: bit_rate   波特率(300-3686400)
输出参数: 无
返 回 值: 无
备    注: 默认data_bits==8,parity==无,stop_bits==0
          esp8266默认UART0的收发IO端口
*************************************************************************************/
void user_uart_raw_init(UART_BautRate bit_rate); // no io swap mode

/*************************************************************************************
函数功能: 读串口缓冲数据大小
输入参数: 无
输出参数: 无
返 回 值: 读到的缓冲数据长度
备    注: 无
*************************************************************************************/
uint16 user_uart_read_size(void);

/*************************************************************************************
函数功能: 读取串口数据
输入参数: out_len 缓冲大小
输出参数: out 读取的数据
返 回 值: 读取的数据长度
备    注: 无
*************************************************************************************/
uint16 user_uart_read_data(uint8 *out,uint16 out_len);

/*************************************************************************************
函数功能: 写串口数据
输入参数: in 要写的数据
          in_len 数据长度
输出参数: 无
返 回 值: 无
备    注: 无
*************************************************************************************/
void user_uart_write_data(uint8 *in,uint16 in_len);

#endif
