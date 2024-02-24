/*
 * ui.h
 *
 *  Created on: 2024年2月19日
 *      Author: MrWolf
 *       Brief: 基于lcd屏幕驱动程序的简易ui，支持显示图片与文字的头文件
 */

#ifndef LIB_UI_H_
#define LIB_UI_H_

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1021.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_lpspi.h"
#include "fsl_iomuxc.h"
#include "FreeRTOS.h"
#include "task.h"

#include "led.h"
#include "lcd.h"

/*
 * @Brief: 使用结构体存储图片相关资源
 */
typedef struct{
	uint16_t* img; /*!<指向图片数组的指针>*/
	int xsize; /*!<图片的宽度，此屏幕最大支持到128>*/
	int ysize; /*!<图片的高度，此屏幕最大支持到160>*/
} Image;

/*
 * @Brief: 使用结构体存储文本相关资源
 */
typedef struct{
	char* txt; /*!<指向文本的指针>*/
	uint16_t color; /*!<文本颜色，使用rgb565格式>*/
	uint16_t bgColor; /*!<背景颜色，使用rgb565格式，若hasBg为false则忽略此项>*/
	bool hasBg; /*!<是否启用背景颜色>*/
} Text;

void ui_init(void);

void ui_luminance(int percentage);

Image ui_image(uint16_t* img, int xsize, int ysize);

Text ui_label(char* txt, uint16_t color, uint16_t bg, bool hasBg);

void ui_displayText(Text* ui_label, int xpos, int ypos, bool flag);

void ui_displayImage(Image* img, int xpos, int ypos);

void ui_solidScreen(uint16_t color);

#endif /* LIB_UI_H_ */
