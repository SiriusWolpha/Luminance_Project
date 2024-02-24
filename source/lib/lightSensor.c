/*
 * lightSensor.c
 *
 *  Created on: 2024年2月22日
 *      Author: MrWolf
 *       Brief: 控制环境光传感器测量亮度
 */
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

#include "lightSensor.h"
#include "led.h"

#define DELAY 5//单位为微秒

void scl(uint8_t value){
	/*
	 * @brief	设置i2c的scl时钟线为指定电平
	 * @param	<uint8_t>要设置的时钟线的电平，取值0(低电平)或1(高电平)
	 * @return	无
	 */
	GPIO2->GDIR |= (1UL << 2);
	GPIO_PinWrite(GPIO2, 2, value);
}

void sda(uint8_t value){
	/*
	 * @brief	设置i2c的sda数据线为指定电平
	 * @param	<uint8_t>要设置的数据线的电平，取值0(低电平)或1(高电平)
	 * @return	无
	 */
	GPIO2->GDIR |= (1UL << 3);
	GPIO_PinWrite(GPIO2, 3, value);
}

uint8_t sda_read(void){
	/*
	 * @brief	读取当前i2c的sda数据线的电平
	 * @param	无
	 * @return	<uint8_t>当前数据线的电平，取值0(低电平)或1(高电平)
	 */
	GPIO2->GDIR |= (1UL << 3);
	GPIO_PinWrite(GPIO2, 3, 0);
	SDK_DelayAtLeastUs(2, SystemCoreClock);
	GPIO2->GDIR &= ~(1UL << 3);
	return GPIO_PinRead(GPIO2, 3);

}

void i2c_start(void){
	/*
	 * @brief	主机发送i2c起始(Start)信号
	 * @param	无
	 * @return	无
	 */
	scl(1);
	sda(1);
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
	sda(0);
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
}

void i2c_stop(void){
	/*
	 * @brief	主机发送i2c停止(Stop)信号
	 * @param	无
	 * @return	无
	 */
	scl(1);
	sda(0);
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
	sda(1);
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
}

void i2c_write(uint8_t data){
	/*
	 * @brief	主机通过i2c发送数据
	 * @param	<uint8_t>要发送的数据
	 * @return	无
	 */
	printf("Prepare to send byte : 0x%02X \n", data);
	scl(0);
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
	for(uint8_t bit=0x80; bit>0; bit >>= 1){
		if(data & bit){
			sda(1);
		}else{
			sda(0);
		}
		SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
		scl(1);
		SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
		scl(0);
		SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
	}
	int idd = 0;
	while(sda_read()){
		if(idd >= 300){
			printf("Software I2C Error : Acknowledgment Error \n");
			break;
		}
		idd++;
	}
	scl(1);
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
}

uint8_t i2c_read(bool hasAck){
	/*
	 * @brief	主机通过i2c读取数据
	 * @param	<bool>是否发送ack信号，为true发送ack，为false发送nack
	 * @return	读取的数据
	 */
	uint8_t data = 0;
	GPIO2->GDIR |= (1UL << 3);
	GPIO_PinWrite(GPIO2, 3, 0);
	SDK_DelayAtLeastUs(1, SystemCoreClock);
	GPIO2->GDIR &= ~(1UL << 3);
	SDK_DelayAtLeastUs(3, SystemCoreClock);


	for(int i=0; i<8; i++){
		data <<= 1;
		scl(0);
		int idd = 0;
		uint8_t i0 = 0;
		uint8_t i1 = 1;
		i0 = GPIO_PinRead(GPIO2, 3);
		while(idd<5){
			i1 = GPIO_PinRead(GPIO2, 3);
			if(i0 == 1 && i1 == 0){
				data |= 0;
				break;
			}else if(i0 == 0 && i1 == 1){
				data |= 1;
				break;
			}
			i0 = i1;
			idd++;
		}
		scl(1);
		SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
	}
	SDK_DelayAtLeastUs(1, SystemCoreClock);
	scl(0);
	sda(1);
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
	if(!hasAck){
		sda(0);
	}
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);
	scl(1);
	SDK_DelayAtLeastUs(DELAY, SystemCoreClock);

	return data;
}

uint16_t read(void){
	/*
	 * @brief	打开环境光传感器并读取一次当前的环境光的值，单位为lux
	 * @param	无
	 * @return	当前检测到的环境光的值
	 */
	i2c_start();

	i2c_write((0x29 << 1) | 0);
	i2c_write(0x80);
	i2c_write(0x03);
	i2c_stop();

	vTaskDelay(pdMS_TO_TICKS(1000));

	i2c_start();
	i2c_write((0x29 << 1) | 0);
	i2c_write(0x94);
	i2c_stop();
	vTaskDelay(pdMS_TO_TICKS(1));

	i2c_start();
	i2c_write((0x29 << 1) | 1);
	uint8_t getVal[4];
	int icc = 0;
	for(uint8_t regAddr=0x94; regAddr<=0x97; regAddr++){
		uint8_t ans = i2c_read(regAddr != 0x97);
		printf("Register 0x%02X : 0x%02X \n", regAddr, ans);
		getVal[icc] = ans;
		icc++;
	}
	uint16_t d0 = (((uint16_t)getVal[1] << 8) | getVal[0]);
	uint16_t d1 = (((uint16_t)getVal[3] << 8) | getVal[2]);
	uint16_t cal = calculate(d0, d1);
	printf("Luminance : %d \n", cal);
	i2c_stop();
	vTaskDelay(pdMS_TO_TICKS(1));
	return cal;
}

uint16_t calculate(uint16_t d0, uint16_t d1){
	/*
	 * @brief	将读取到的数据通过计算转化为环境光的值，单位为lux
	 * 			计算公式来自BH1730FVC-TR环境光传感器Datasheet第13面
	 * @param	<uint16_t>DATA0 <uint16_t>DATA1
	 * @return	计算所得的环境光的值
	 */
	float Gain = 1.0;
	float Tint = 2.8;
	uint8_t ITIME = 0xda;
	float ITIME_ms = (Tint * 964 * (0xff - ITIME)) / 1000;
	float Lx = 0;
	if(d1/d0 < 0.26){
		Lx = (1.290 * d0 - 2.733 * d1) / Gain * 102.6 / ITIME_ms;
	}else if(d1/d0 < 0.55){
		Lx = (0.795 * d0 - 0.859 * d1) / Gain * 102.6 / ITIME_ms;
	}else if(d1/d0 < 1.09){
		Lx = (0.510 * d0 - 0.345 * d1) / Gain * 102.6 / ITIME_ms;
	}else if(d1/d0 < 2.13){
		Lx = (0.276 * d0 - 0.130 * d1) / Gain * 102.6 / ITIME_ms;
	}else{
		Lx = 0;
	}
	return (uint16_t)Lx;
}

int convert2pwm(uint16_t lum){
	/*
	 * @brief	根据人眼对亮度的感知曲线，应用伽马修正得到对应的操作屏幕背光灯的pwm波的占空比
	 * @param	<uint16_t>环境光强度
	 * @return	<int>占空比
	 */
	double b = pow((lum/180.0), (1/2.2));
	if(b > 1){
		b = 1;
	}else if(b < 0.01){
		b = 0.01;
	}
	return ceil(b * 100);
}


