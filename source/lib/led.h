/*
 * led.h
 *
 *  Created on: 2024年2月9日
 *      Author: MrWolf
 *      Brief: 控制扩展板LED灯文件的头文件
 */

#ifndef LIB_LED_H_
#define LIB_LED_H_

/*
 * @brief	枚举定义颜色编号
 */
#define DARK 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE 4
#define CYAN 5//青色
#define PURPLE 6
#define YELLOW 7

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief	声明函数
 */
void SysTick_Handler(void);

void Sleep(uint32_t n);

void light(int color);

void light_time(int color, uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* LIB_LED_H_ */
