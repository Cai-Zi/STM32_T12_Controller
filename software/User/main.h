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

#ifdef __cplusplus
}
#endif
#endif
