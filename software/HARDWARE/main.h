#ifndef __MAIN_H
#define __MAIN_H

#include "sys.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_adc.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "max6675.h"
#include "pid.h"
#include "adc.h"
#include "beeper.h"
#include "flash.h"
#include "menu.h"
#include "oled.h"
#include "setting.h"

#define HEAT_Pin GPIO_Pin_0
#define HEAT_GPIO_Port GPIOA
#define HEAT PAout(0)

#define SLEEP_Pin GPIO_Pin_8
#define SLEEP_GPIO_Port GPIOA
#define SLEEP PAin(8)

// 默认温度控制值（建议焊接温度：300-380°C）
#define TEMP_MIN   150 // 最低可选温度
#define TEMP_MAX   400 // 最大可选温度
#define TEMP_SLEEP 100 // 睡眠模式下的温度
#define TEMP_ADD	50 // 强力模式下的温度升高值

// 默认烙铁头温度校准值
#define TEMP100 790  // 100℃时的ADC值
#define TEMP200 1670 // 200℃时的ADC值
#define TEMP300 2900 // 300℃时的ADC值
#define TEMP420 4090 // 420℃时的ADC值

// 默认计时器值（0=禁用）
#define TIME_SLEEP 5        // 进入睡眠模式的时间（分钟）
#define TIME_SHUT  20       // 关闭加热器的时间（分钟）

// 是否显示百分比（0=禁用）
#define percent_dis 1        // 是否显示pwm百分比   0不显示

#define TEMPARRLEN 8 //温度数组元素个数
#define NTC_TEMPARRLEN 15 //温度数组元素个数
extern u16 volatile NTC_temp;//手柄温度
extern u16 volatile T12_temp;//烙铁头温度
extern float volatile VinVolt;//输入电压
extern u16 volatile tempArray[TEMPARRLEN];//温度数组

#endif
