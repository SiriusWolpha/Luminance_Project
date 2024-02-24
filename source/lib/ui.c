/*
 * ui.c
 *
 *  Created on: 2024年2月19日
 *      Author: MrWolf
 *       Brief: 基于lcd屏幕驱动程序的简易ui，支持显示图片与文字
 */
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
#include "ui.h"
#include "asset/font.h"

void ui_init(void){
	/*
	 * @brief	初始化lpspi通讯并配置好屏幕
	 * @param	无
	 * @return	无
	 */
    spi_init();
    lcd_init(VERTICAL);
    lcd_start();
    ui_solidScreen(0x0000);
}

void ui_luminance(int percentage){
	/*
	 * @brief	调整屏幕亮度(应该被放入一个无限循环中)
	 * @param	<int>屏幕亮度(取值0到100)
	 * @return	无
	 */
	if(percentage > 100){
		percentage = 100;
	}else if(percentage < 1){
		percentage = 1;
	}
	if(percentage > 35){
		lcd_close();
		SDK_DelayAtLeastUs((100*(100-percentage)), SystemCoreClock);
		lcd_start();
		SDK_DelayAtLeastUs((100*percentage), SystemCoreClock);
	}else{
		lcd_start();
		SDK_DelayAtLeastUs((100*percentage), SystemCoreClock);
		lcd_close();
		SDK_DelayAtLeastUs((100*(100-percentage)), SystemCoreClock);
	}
}

Image ui_image(uint16_t *img, int xsize, int ysize){
	/*
	 * @brief	得到一个赋值完成的Image结构体
	 * @param	<uint16_t *>指向图片数组的指针 <int>图片的宽 <int>图片的高
	 * @return	赋值完成的Image结构体
	 */
	Image newImg;
	newImg.img = img;
	newImg.xsize = xsize;
	newImg.ysize = ysize;
	return newImg;
}

Text ui_label(char* txt, uint16_t color, uint16_t bg, bool hasBg){
	/*
	 * @brief	得到一个赋值完成的Text结构体
	 * @param	<char *>指向文本数组的指针 <uint16_t>文本颜色(rgb565格式) <uint16_t>背景颜色(rgb565格式，若hasBg为false则无效) <bool>是否启用背景颜色
	 * @return	赋值完成的Text结构体
	 */
	Text newText;
	newText.txt = txt;
	newText.color = color;
	newText.bgColor = bg;
	newText.hasBg = hasBg;
	return newText;
}

void ui_displayText(Text* ui_label, int xpos, int ypos, bool flag){
	/*
	 * @brief	在屏幕上显示文本，使用\n换行
	 * @param	<Text *>指向文本结构体的指针 <int>起始x坐标 <int>起始y坐标 <bool>是否允许延时(设为true如果放在FreeRTOS的任务调度中)
	 * @return	无
	 */
	int length = strlen(ui_label->txt);
	int thisX = xpos;
	int thisY = ypos;
	int initX = xpos;
	int count = 0;
	for(int i=0; i<length; i++){
		//遍历每一个文字
		if(ui_label->txt[i] == 0x0A){
			//如果遇到换行符
			thisY += 8;
			ypos += 8;
			thisX = initX;
			xpos = initX;
			count = 0;
			continue;
		}
		if(count == 16){
			//如果一行占满
			count = 0;
			thisY += 8;
			ypos += 8;
			thisX = initX;
			xpos = initX;
			if(ui_label->txt[i] == 0x20){
				//如果行首是空格
				continue;
			}
		}
		for(int j=0; j<8; j++){
			//遍历单个文字的每一排(8排)
			for(int k=0; k<8; k++){
				//遍历每排的每个像素点(8个)
				if((font[(ui_label->txt[i]*8+j)]&(1 << k)) == 0){
					//背景点
					if(ui_label->hasBg){
						lcd_setPointColor(thisX, thisY, ui_label->bgColor);
					}
				}else{
					//有效文本点
					lcd_setPointColor(thisX, thisY, ui_label->color);
				}
				thisX += 1;
				if(flag == true && (k == 3)){// || k == 7)){
					vTaskDelay(pdMS_TO_TICKS(1));
				}
			}
			thisX = xpos;
			thisY += 1;
		}
		thisY = ypos;
		thisX += 8;
		xpos += 8;
		count++;
	}
}

void ui_displayImage(Image* img, int xpos, int ypos){
	/*
	 * @brief	在屏幕上显示图片
	 * @param	<Image *>指向图片组件的指针 <int>起始x坐标 <int>起始y坐标
	 * @return	无
	 */
	lcd_writeAddr(xpos, ypos, (xpos+img->xsize-1), (ypos+img->ysize-1));
	for(int i = 0; i < (img->xsize*img->ysize); i++){
		lcd_WriteData_u16(img->img[i]);
	}
}

void ui_solidScreen(uint16_t color){
	/*
	 * @brief	将屏幕填充为指定颜色
	 * @param	<uint16_t>颜色，为rgb565格式
	 * @return	无
	 */
	lcd_screen(color);
}
