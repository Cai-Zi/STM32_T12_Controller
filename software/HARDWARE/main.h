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

// Ĭ���¶ȿ���ֵ�����麸���¶ȣ�300-380��C��
#define TEMP_MIN   150 // ��Ϳ�ѡ�¶�
#define TEMP_MAX   400 // ����ѡ�¶�
#define TEMP_SLEEP 100 // ˯��ģʽ�µ��¶�
#define TEMP_ADD	50 // ǿ��ģʽ�µ��¶�����ֵ

// Ĭ������ͷ�¶�У׼ֵ
#define TEMP100 790  // 100��ʱ��ADCֵ
#define TEMP200 1670 // 200��ʱ��ADCֵ
#define TEMP300 2900 // 300��ʱ��ADCֵ
#define TEMP420 4090 // 420��ʱ��ADCֵ

// Ĭ�ϼ�ʱ��ֵ��0=���ã�
#define TIME_SLEEP 5        // ����˯��ģʽ��ʱ�䣨���ӣ�
#define TIME_SHUT  20       // �رռ�������ʱ�䣨���ӣ�

// �Ƿ���ʾ�ٷֱȣ�0=���ã�
#define percent_dis 1        // �Ƿ���ʾpwm�ٷֱ�   0����ʾ

#define TEMPARRLEN 8 //�¶�����Ԫ�ظ���
#define NTC_TEMPARRLEN 15 //�¶�����Ԫ�ظ���
extern u16 volatile NTC_temp;//�ֱ��¶�
extern u16 volatile T12_temp;//����ͷ�¶�
extern float volatile VinVolt;//�����ѹ
extern u16 volatile tempArray[TEMPARRLEN];//�¶�����

#endif
