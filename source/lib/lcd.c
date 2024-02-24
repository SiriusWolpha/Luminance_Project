/*
 * lcd.c
 *
 *  Created on: 2024年2月11日
 *      Author: MrWolf
 *       Brief: 控制扩展版lcd屏幕的基础驱动程序的头文件
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

#include "led.h"
#include "lcd.h"


lpspi_master_handle_t g_m_handle;//lpspi_master_handle_t结构体
volatile bool isTransferCompleted = false;//传输完成标志

void LPSPI_MasterUserCallback(LPSPI_Type *base, lpspi_master_handle_t *handle, status_t status, void *userData){
    //lpspi中断处理函数
	if (status == kStatus_Success)
    {
        __NOP();
    }

    isTransferCompleted = true;
}

void lcd_start(void){
	/*
	 * @brief	打开屏幕背光灯
	 * @param	无
	 * @return	无
	 */
	GPIO_PinWrite(GPIO2, 31, 1U);
}

void lcd_close(void){
	/*
	 * @brief	关闭屏幕背光灯
	 * @param	无
	 * @return	无
	 */
	GPIO_PinWrite(GPIO2, 31, 0U);
}

void lcd_reset(void){
	/*
	 * @brief	重置屏幕
	 * @param	无
	 * @return	无
	 */
	GPIO_PinWrite(GPIO3, 7, 0U);
	Sleep(5);
	GPIO_PinWrite(GPIO3, 7, 1U);
	Sleep(120);
}

void spi_init(void){
	/*
	 * @brief	初始化lpspi通讯
	 * @param	无
	 * @return	无
	 */
	CLOCK_SetMux(kCLOCK_LpspiMux, BOARD_LPSPI_CLOCK_SOURCE_SELECT);
	CLOCK_SetDiv(kCLOCK_LpspiDiv, BOARD_LPSPI_CLOCK_SOURCE_DIVIDER);

    lpspi_master_config_t masterConfig;
    LPSPI_MasterGetDefaultConfig(&masterConfig);
    LPSPI_MasterTransferCreateHandle(SPI_MASTER_BASEADDR, &g_m_handle, LPSPI_MasterUserCallback, NULL);

    masterConfig.baudRate = 50000000U;
    masterConfig.bitsPerFrame = 8;
    masterConfig.whichPcs = kLPSPI_Pcs0;
    LPSPI_MasterInit(SPI_MASTER_BASEADDR, &masterConfig, BOARD_LPSPI_CLK_FREQ);
}

void spi_send(uint8_t *data){
	/*
	 * @brief	通过lpspi发送8位讯息到屏幕控制器
	 * @param	<uint8_t *>要发送的讯息
	 * @return	无
	 */
	lpspi_transfer_t masterXfer;
	isTransferCompleted    = false;
	masterXfer.txData      = data;
	masterXfer.rxData      = NULL;
	masterXfer.dataSize    = 1U;
	masterXfer.configFlags = kLPSPI_MasterPcs0 | kLPSPI_MasterByteSwap;
	LPSPI_MasterTransferNonBlocking(SPI_MASTER_BASEADDR, &g_m_handle, &masterXfer);

	/* Wait transfer complete */
	while (!isTransferCompleted){
	}
}

void lcd_WriteData(uint8_t data){
	/*
	 * @brief	向屏幕控制器写数据
	 * @param	<uint8_t>要发送的数据
	 * @return	无
	 */
	uint8_t *txData = &data;
    GPIO_PinWrite(GPIO3, 1, 0U);//CS low : enable
    GPIO_PinWrite(GPIO3, 4, 1U);//DC high : data mode
    spi_send(txData);
    GPIO_PinWrite(GPIO3, 1, 1U);//CS high : disable
}

void lcd_WriteCmd(uint8_t command){
	/*
	 * @brief	向屏幕控制器写命令
	 * @param	<uint8_t>要发送的命令
	 * @return	无
	 */
	uint8_t *data = &command;
    GPIO_PinWrite(GPIO3, 1, 0U);//CS low : enable
    GPIO_PinWrite(GPIO3, 4, 0U);//DC low : command mode
    spi_send(data);
    GPIO_PinWrite(GPIO3, 1, 1U);//CS high : disable
}

void lcd_init(int direction){
	/*
	 * @brief	初始化屏幕
	 * @param	<int>屏幕显示方向
	 * @return	无
	 */
	lcd_reset();
	lcd_WriteCmd(0x11);
	Sleep(120);
	lcd_WriteCmd(0xB1);
	lcd_WriteData(0x01);
	lcd_WriteData(0x2C);
	lcd_WriteData(0x2D);
	lcd_WriteCmd(0xB2);
	lcd_WriteData(0x01);
	lcd_WriteData(0x2C);
	lcd_WriteData(0x2D);
	lcd_WriteCmd(0xB3);
	lcd_WriteData(0x01);
	lcd_WriteData(0x2C);
	lcd_WriteData(0x2D);
	lcd_WriteData(0x01);
	lcd_WriteData(0x2C);
	lcd_WriteData(0x2D);
	lcd_WriteCmd(0xB4); //Column inversion
	lcd_WriteData(0x07);
	//ST7735R Power Sequence
	lcd_WriteCmd(0xC0);
	lcd_WriteData(0xA2);
	lcd_WriteData(0x02);
	lcd_WriteData(0x84);
	lcd_WriteCmd(0xC1);
	lcd_WriteData(0xC5);
	lcd_WriteCmd(0xC2);
	lcd_WriteData(0x0A);
	lcd_WriteData(0x00);
	lcd_WriteCmd(0xC3);
	lcd_WriteData(0x8A);
	lcd_WriteData(0x2A);
	lcd_WriteCmd(0xC4);
	lcd_WriteData(0x8A);
	lcd_WriteData(0xEE);
	lcd_WriteCmd(0xC5); //VCOM
	lcd_WriteData(0x0E);
	lcd_WriteCmd(0x36); //MX, MY, RGB mode
	if(direction == HORIZONTAL){
		lcd_WriteData(0xA0);//横屏
	}else{
		lcd_WriteData(0xC8);//竖屏
	}
	lcd_WriteCmd(0xE0);
	lcd_WriteData(0x0F);
	lcd_WriteData(0x1A);
	lcd_WriteData(0x0F);
	lcd_WriteData(0x18);
	lcd_WriteData(0x2F);
	lcd_WriteData(0x28);
	lcd_WriteData(0x20);
	lcd_WriteData(0x22);
	lcd_WriteData(0x1F);
	lcd_WriteData(0x1B);
	lcd_WriteData(0x23);
	lcd_WriteData(0x37);
	lcd_WriteData(0x00);
	lcd_WriteData(0x07);
	lcd_WriteData(0x02);
	lcd_WriteData(0x10);
	lcd_WriteCmd(0xE1);
	lcd_WriteData(0x0F);
	lcd_WriteData(0x1B);
	lcd_WriteData(0x0F);
	lcd_WriteData(0x17);
	lcd_WriteData(0x33);
	lcd_WriteData(0x2C);
	lcd_WriteData(0x29);
	lcd_WriteData(0x2E);
	lcd_WriteData(0x30);
	lcd_WriteData(0x30);
	lcd_WriteData(0x39);
	lcd_WriteData(0x3F);
	lcd_WriteData(0x00);
	lcd_WriteData(0x07);
	lcd_WriteData(0x03);
	lcd_WriteData(0x10);
	lcd_WriteCmd(0x2A);
	lcd_WriteData(0x00);
	lcd_WriteData(0x00+2);
	lcd_WriteData(0x00);
	lcd_WriteData(0x80+2);
	lcd_WriteCmd(0x2B);
	lcd_WriteData(0x00);
	lcd_WriteData(0x00+3);
	lcd_WriteData(0x00);
	lcd_WriteData(0x80+3);
	lcd_WriteCmd(0xF0); //Enable test command
	lcd_WriteData(0x01);
	lcd_WriteCmd(0xF6); //Disable ram power save mode
	lcd_WriteData(0x00);
	lcd_WriteCmd(0x3A); //65k mode
	lcd_WriteData(0x05);
	lcd_WriteCmd(0x29);//Display on
}

void lcd_writeAddr(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end){
	/*
	 * @brief	选定屏幕中的一块区域用于显示
	 * @param	<uint8_t>x坐标起点 <uint8_t>y坐标起点 <uint8_t>x坐标终点 <uint8_t>y坐标终点
	 * @return	无
	 */
	lcd_WriteCmd(0x2A);
	lcd_WriteData(0x00);
	lcd_WriteData(x_start);
	lcd_WriteData(0x00);
	lcd_WriteData(x_end);
	lcd_WriteCmd(0x2B);
	lcd_WriteData(0x00);
	lcd_WriteData(y_start);
	lcd_WriteData(0x00);
	lcd_WriteData(y_end);
	lcd_WriteCmd(0x2C);
}

void lcd_WriteData_u16(uint16_t data){
	/*
	 * @brief	向屏幕控制器写数据(16位)
	 * @param	<uint16_t>要发送的数据
	 * @return	无
	 */
	GPIO_PinWrite(GPIO3, 1, 0U);//CS low : enable
	GPIO_PinWrite(GPIO3, 4, 1U);//DC high : data mode
	uint8_t high8 = (uint8_t)(data >> 8);
	uint8_t low8 = (uint8_t)(data &0x00FF);
	uint8_t *p_high8 = &high8;
	uint8_t *p_low8 = &low8;
	spi_send(p_high8);
	spi_send(p_low8);
	GPIO_PinWrite(GPIO3, 1, 1U);//CS high : disable
}

uint16_t convert(uint16_t rgb565){
	/*
	 * @brief	将rgb565格式颜色数据转换为bgr565格式颜色数据
	 * @param	<uint16_t>要转换的rgb565格式颜色数据
	 * @return	转换后的bgr565格式的颜色数据
	 */
	uint16_t blue = (rgb565 << 11) & 0xF800;
	uint16_t green = (rgb565 << 5) & 0x07E0;
	uint16_t red = (rgb565 >> 11) & 0x001F;
	return blue | green | red;
}

uint16_t convert_555(uint16_t rgb565){
	/*
	 * @brief	将rgb565格式颜色数据转换为rgb555格式颜色数据
	 * @param	<uint16_t>要转换的rgb565格式颜色数据
	 * @return	转换后的rgb555格式的颜色数据
	 */
	uint16_t r = (rgb565 >> 11) & 0x1F;
	uint16_t g = (rgb565 >> 5) & 0x3F;
	uint16_t b = rgb565 & 0x1F;
	uint16_t result = ((r << 10) | (g << 5) | b) & 0x7FFF;
	return result;
}

uint16_t convert_888(uint16_t rgb565){
	/*
	 * @brief	将rgb565格式颜色数据转换为rgb888格式颜色数据
	 * @param	<uint16_t>要转换的rgb565格式颜色数据
	 * @return	转换后的rgb888格式的颜色数据
	 */
	uint16_t r = (rgb565 >> 11) & 0x1F;
	uint16_t g = (rgb565 >> 5) & 0x3F;
	uint16_t b = rgb565 & 0x1F;
	uint16_t result = ((r << 3) << 16)| ((g << 2) << 8) | (b << 3);
	return result;
}

void lcd_setAreaColor(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end,uint16_t color){
	/*
	 * @brief	将屏幕指定区域设置为指定颜色
	 * @param	<uint8_t>x起始坐标 <uint8_t>y起始坐标 <uint8_t>x结束坐标 <uint8_t>y结束坐标 <uint16_t>rgb565格式颜色
	 * @return	无
	 */
	lcd_writeAddr(x_start,y_start,x_end,y_end);
	//计算填充区域的长度和宽度，终点坐标减起点坐标+1
	uint8_t x_len = x_end - x_start + 1;//计算x坐标的长度
	uint8_t y_len = y_end - y_start + 1;//计算y坐标的长度
	for(int i = 0; i < x_len; i++){
		for(int j = 0; j < y_len; j++){
			lcd_WriteData_u16(convert(color));
		}
	}
}

void lcd_setPointColor(uint8_t x, uint8_t y, uint16_t color){
	/*
	 * @brief	将屏幕指定像素点设置为指定颜色
	 * @param	<uint8_t>x坐标 <uint8_t>y坐标 <uint16_t>rgb565格式颜色
	 * @return	无
	 */
	lcd_writeAddr(x,y,x,y);
	lcd_WriteData_u16(convert(color));
}

void lcd_screen(uint16_t color){
	/*
	 * @brief	将屏幕设置为指定颜色
	 * @param	<uint16_t>rgb565格式颜色
	 * @return	无
	 */
	lcd_writeAddr(0, 0, 160, 160);
	for(int i = 0; i < 161; i++){
		for(int j = 0; j < 161; j++){
			lcd_WriteData_u16(convert(color));
		}
	}
}

void lcd_displayImage(const uint16_t img[]){
	/*
	 * @brief	全屏显示图片
	 * @param	<const uint16_t []>要显示的图片
	 * @return	无
	 */
	lcd_writeAddr(1, 1, 128, 160);
	for(int i = 0; i < 20480; i++){
		lcd_WriteData_u16(img[i]);
	}
}

void lcd_whiteScreen(void){
	/*
	 * @brief	让屏幕白屏
	 * @param	无
	 * @return	无
	 */
	lcd_WriteCmd(0x01);
}
