/*
 * led.c
 *
 *  Created on: 2024年2月9日
 *      Author: MrWolf
 *       Brief: 控制扩展板LED灯
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1021.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "led.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
void Sleep(uint32_t n){
	/*
	 * @brief	暂停给定的时间，以毫秒为单位
	 * @param	<uint32_t>毫秒数
	 * @return	无
	 */
	SDK_DelayAtLeastUs((1000*n), SystemCoreClock);
}

void light(int color){
	/*
	 * @brief	让LED灯亮指定的颜色或关闭LED灯
	 * @param	<int>颜色编号
	 * @return	无
	 */
	switch (color){
	case DARK:
		GPIO_PinWrite(GPIO1, 10, 1U);
		GPIO_PinWrite(GPIO1, 14, 1U);
		GPIO_PinWrite(GPIO1, 15, 1U);
		break;
	case WHITE:
		GPIO_PinWrite(GPIO1, 10, 0U);
		GPIO_PinWrite(GPIO1, 14, 0U);
		GPIO_PinWrite(GPIO1, 15, 0U);
		break;
	case RED:
		GPIO_PinWrite(GPIO1, 10, 0U);
		GPIO_PinWrite(GPIO1, 14, 1U);
		GPIO_PinWrite(GPIO1, 15, 1U);
		break;
	case GREEN:
		GPIO_PinWrite(GPIO1, 10, 1U);
		GPIO_PinWrite(GPIO1, 14, 0U);
		GPIO_PinWrite(GPIO1, 15, 1U);
		break;
	case BLUE:
		GPIO_PinWrite(GPIO1, 10, 1U);
		GPIO_PinWrite(GPIO1, 14, 1U);
		GPIO_PinWrite(GPIO1, 15, 0U);
		break;
	case CYAN:
		GPIO_PinWrite(GPIO1, 10, 1U);
		GPIO_PinWrite(GPIO1, 14, 0U);
		GPIO_PinWrite(GPIO1, 15, 0U);
		break;
	case PURPLE:
		GPIO_PinWrite(GPIO1, 10, 0U);
		GPIO_PinWrite(GPIO1, 14, 1U);
		GPIO_PinWrite(GPIO1, 15, 0U);
		break;
	case YELLOW:
		GPIO_PinWrite(GPIO1, 10, 0U);
		GPIO_PinWrite(GPIO1, 14, 0U);
		GPIO_PinWrite(GPIO1, 15, 1U);
		break;
	}
}

void light_time(int color, uint32_t ms){
	/*
	 * @brief	让LED灯以指定颜色亮指定时间，以毫秒为单位
	 * @param	<int>颜色编号 <uint32_t>毫秒数
	 * @return	无
	 */
	light(color);
	Sleep(ms);
	light(DARK);
}
