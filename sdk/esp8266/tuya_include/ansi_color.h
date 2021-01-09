// File: ansi_color.h
// Author: 徐靖航 Xu Jinghang
// comment: ANSI标准显色代码 需要终端程序支持,用于debug
// date: 2017-04-21
// demo:
//		printf(ANSI_RED "This text is red color\n" ANSI_OFF); // 这段代码将以红色显示
#ifndef ANSI_COLOR_H
#define ANSI_COLOR_H

#define ANSI_OFF 		"\033[0m"	// 关闭颜色配置
#define ANSI_BOLD 		"\033[1m"	// 加粗
#define ANSI_UNDERLINE	"\033[4m"	// 下划线
#define ANSI_BLINK		"\033[5m"	// 闪烁
#define ANSI_NEGATIVE	"\033[7m"	// 反显

#define ANSI_BLACK		"\033[30m"	// 黑
#define ANSI_RED		"\033[31m"	// 红
#define ANSI_GREEN		"\033[32m"	// 绿
#define ANSI_YELLOW		"\033[33m"	// 黄
#define ANSI_BLUE		"\033[34m"	// 蓝
#define ANSI_MAGENTA	"\033[35m"	// 紫
#define ANSI_CYAN		"\033[36m"	// 青
#define ANSI_GRAY		"\033[37m"	// 灰

#endif
