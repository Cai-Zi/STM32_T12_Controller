#ifndef __MAIN_H
#define __MAIN_H
#ifdef __cplusplus
extern "C" {
#endif
	
#include "sys.h"
#include "stm32f10x.h"

#define HEAT_Pin GPIO_Pin_0
#define HEAT_GPIO_Port GPIOA
#define HEAT PAout(0)

#define SLEEP_Pin GPIO_Pin_8
#define SLEEP_GPIO_Port GPIOA
#define SLEEP PAin(8)

extern u16 volatile NTC_temp;//手柄温度
extern u16 volatile T12_temp;//烙铁头温度
	 
#ifdef __cplusplus
}
#endif
#endif
