/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    MIMXRT1021_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1021.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include <stdint.h>
#include <string.h>
#include "fsl_gpio.h"
#include "lib/led.h"
#include "lib/ui.h"
#include "lib/lightSensor.h"
#include "asset/images.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */
int pwm = 100;//公有变量，记录屏幕的占空比
uint16_t lux = 180;//公有变量，记录当前环境光的值
SemaphoreHandle_t xMutex;

void T_Read(void *pvParameters){
	/*
	 * @brief	读取环境光传感器任务
	 */
	while(1){
		lux = read();
		pwm = convert2pwm(lux);
		vTaskDelay(pdMS_TO_TICKS(100));//延迟确保任务能被正确调度
	}
}

void T_Luminance(void *pvParameters){
	/*
	 * @brief	设置屏幕亮度任务
	 */
	while(1){
		ui_luminance(pwm);
		vTaskDelay(pdMS_TO_TICKS(1));//延迟1ms确保任务能被正确调度
	}
}

void T_UpdateUI(void *pvParameters){
	/*
	 * @brief	更新ui任务
	 */
	while(1){
		char str[6];
		char str_pwm[4];
		snprintf(str, sizeof(str), "%5u", lux);
		snprintf(str_pwm, sizeof(str_pwm), "%3u", pwm);
		Text txtF = ui_label((char*)&str, S_BLUE, S_BLACK, true);
		Text txtG = ui_label((char*)&str_pwm, S_BLUE, S_BLACK, true);
		ui_displayText(&txtF, 66, 145, true);
		vTaskDelay(pdMS_TO_TICKS(1));//延迟1ms确保任务能被正确调度
		ui_displayText(&txtG, 90, 153, true);
		vTaskDelay(pdMS_TO_TICKS(1));//延迟1ms确保任务能被正确调度
	}
}

int main(void) {

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif
    ui_init();//初始化ui

    //定义要显示的图片与文字
    Image imgA = ui_image((uint16_t *)&img0, 128, 160);
    Text txtA = ui_label("   By Mr.Wolf   ", S_YELLOW, S_BLACK, true);
    Text txtB = ui_label("-Luminance Test-", S_PURPLE, S_BLACK, true);
    Text txtC = ui_label("===Statistics===", S_RED, S_BLACK, true);
    Text txtD = ui_label("+Lux:         lx", S_GREEN, S_BLACK, true);
    Text txtE = ui_label("+Scr Light:    %", S_GREEN, S_BLACK, true);

    //将图片和文字显示在屏幕上
    ui_displayImage(&imgA, 1, 1);
    ui_displayText(&txtA, 2, 121, false);
    ui_displayText(&txtB, 2, 129, false);
    ui_displayText(&txtC, 2, 137, false);
    ui_displayText(&txtD, 2, 145, false);
    ui_displayText(&txtE, 2, 153, false);

    xMutex = xSemaphoreCreateMutex();
    //设置FreeRTOS要运行的任务
    xTaskCreate(T_Read, "T_Read", configMINIMAL_STACK_SIZE, (void *)xMutex, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(T_Luminance, "T_Luminance", configMINIMAL_STACK_SIZE, (void *)xMutex, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(T_UpdateUI, "UpdateUI", configMINIMAL_STACK_SIZE, (void *)xMutex, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();//启动FreeRTOS的任务调度

    while(1) {
        light_time(RED, 500);
        Sleep(500);
    }
    return 0 ;
}
