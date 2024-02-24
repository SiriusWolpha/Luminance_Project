/*
 * lcd.h
 *
 *  Created on: 2024年2月11日
 *      Author: MrWolf
 *       Brief: 控制扩展版lcd屏幕的基础驱动程序
 */

#ifndef LIB_LCD_H_
#define LIB_LCD_H_

#define SPI_MASTER_BASEADDR LPSPI4
/* Select USB1 PLL PFD0 (392.72 MHz) as lpspi clock source */
#define BOARD_LPSPI_CLOCK_SOURCE_SELECT (1U)
/* Clock divider for master lpspi clock source */
#define BOARD_LPSPI_CLOCK_SOURCE_DIVIDER (7U)

#define BOARD_LPSPI_CLK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk) / (BOARD_LPSPI_CLOCK_SOURCE_DIVIDER + 1U))

#define VERTICAL 1//纵向
#define HORIZONTAL 2//横向

#define S_RED 0xF800
#define S_GREEN 0x07E0
#define S_BLUE 0x001F
#define S_WHITE 0xFFFF
#define S_BLACK 0x0000
#define S_YELLOW 0xFFE0
#define S_PURPLE 0xF81F

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

void lcd_start(void);

void lcd_close(void);

void lcd_reset(void);

void spi_init(void);

void spi_send(uint8_t *data);

void lcd_WriteData(uint8_t data);

void lcd_WriteCmd(uint8_t command);

void lcd_init(int direction);

void lcd_writeAddr(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end);

void lcd_WriteData_u16(uint16_t data);

uint16_t convert(uint16_t rgb565);

uint16_t convert_555(uint16_t rgb565);

uint16_t convert_888(uint16_t rgb565);

void lcd_setAreaColor(uint8_t x_start,uint8_t y_start,uint8_t x_end,uint8_t y_end,uint16_t color);

void lcd_setPointColor(uint8_t x, uint8_t y, uint16_t color);

void lcd_screen(uint16_t color);

void lcd_displayImage(const uint16_t img[]);

void lcd_whiteScreen(void);

#endif /* LIB_LCD_H_ */
