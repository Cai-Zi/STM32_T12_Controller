#ifndef __PID_H
#define __PID_H
#include "sys.h"

typedef struct PID_Value
{
	u32 liEkVal[3];                     //��ֵ���棬�����ͷ����Ĳ�ֵ
	u8 uEkFlag[3];                        //���ţ�1���Ӧ��Ϊ������0Ϊ��Ӧ��Ϊ����   
	u8 uKP_Coe;                                //����ϵ��
	u8 uKI_Coe;                                //���ֳ���
	u8 uKD_Coe;                                //΢�ֳ���
	u16 iPriVal;             //��һʱ��ֵ
	u16 iSetVal;             //�趨ֵ
	u16 iCurVal;             //ʵ��ֵ
}PID_ValueStr;

extern volatile u32 nowTime;
extern volatile u32 sleepCount;
extern volatile u32 shutCount;
extern volatile int uk;
void HEAT_Init(void);
void PID_Setup(void);
void PID_Output(void);
void PID_Operation(void);
void TIM4_Counter_Init(u16 arr,u16 psc);
void getClockTime(char timeStr[]);

#endif
