#include "beeper.h"
#include "delay.h"
#include "main.h"
//������IO��ʼ��
void BEEPER_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
	//��ʼ�� ����������PB9	  �������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);//����͵�ƽ
}
//����������һ��
void beeperOnce(void)
{
//	u8 i;
	if (setData.beeperFlag) {
//		for (i=0; i<20; i++) {
//			Beeper = 1;
//			delay_us(120);
//			Beeper = 0;
//			delay_us(125);
//		}
		Beeper = 1;
		delay_ms(10);
		Beeper = 0;
	}
}
