#ifndef __SETTING_H
#define __SETTING_H			  	 
#include "sys.h"
#include "flash.h"

#pragma pack(1)//���ֽڶ��룬����Ҫ��
//��Ҫ����127��16λ��
//������1+8*3+8*2+8+1+2+1+1
typedef struct _set_Config// �û����ýṹ��
{
	u8 writeFlag;//�Ƿ��һ��д��=1�ֽ�8λ
	int setTemp;//�����¶�
	int sleepTime;//����ʱ��
	int shutTime;//�ػ�ʱ��
	u8 workMode;//����ģʽ
	u8 beeperFlag;//�������Ƿ���
	u8 langFlag;//��������
}set_Config;
#pragma pack()


extern volatile set_Config setData;
extern volatile u32 setDataSize;
void set_Init(void);
void resetData(void);

#endif
