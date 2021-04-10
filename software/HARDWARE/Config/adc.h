#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h" //�ǵ���Ӵ�ͷ�ļ�����Ϊconfig.c�õ�GPIO��غ�����
#include "sys.h"

extern u16 NTC_Average;
extern u16 T12_Average;

void Adc_Init(void);//ADC1��ʼ��
u16 Get_Adc(u8 ch); //��ȡһ��ADC��ֵ
u16 Get_Adc_Average(u8 ch,u8 times);//ADC����ֵ���о�ֵ�˲�
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh);//ӳ�亯��
u16 get_NTC_temp(void);
void get_T12_ADC(void);
void get_Vin(void);
u16 get_T12_temp(void);
u16 sleepCheck(void);
#endif
