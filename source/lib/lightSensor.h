/*
 * lightSensor.h
 *
 *  Created on: 2024年2月22日
 *      Author: MrWolf
 *       Brief: 控制环境光传感器测量亮度的头文件
 */

#ifndef LIB_LIGHTSENSOR_H_
#define LIB_LIGHTSENSOR_H_

#include <stdio.h>
#include <math.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1021.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_common.h"
#include "FreeRTOS.h"
#include "task.h"

#include "led.h"

uint16_t read(void);

uint16_t calculate(uint16_t d0, uint16_t d1);

int convert2pwm(uint16_t lum);

#endif /* LIB_LIGHTSENSOR_H_ */
