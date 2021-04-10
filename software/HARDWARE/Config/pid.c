#include "pid.h"
#include "main.h"
#include "adc.h"
#include "usart.h"
#include "key.h"
#include "menu.h"
#include "setting.h"


#define DBGMCU_CR  (*((volatile u32 *)0xE0042004))
u16 timecount,g_bPIDRunFlag;
volatile int uk;//ukΪ��ǰ���ֵ
int duk;//dukΪPID����
float aggKp,aggKi,aggKd;//������PID����
float consKp,consKi,consKd;//���ص�PID����
float e0,e1,e2;
float st,pt;
u16 PWM_T = 255;//PWM�������
volatile u32 nowTime = 0;//��������ʱ�䣬��λ0.01s
volatile u32 sleepCount = 0;
volatile u32 shutCount = 0;
u16 tim4_count;
extern u8 err;
extern float volatile VinVolt;//�����ѹ
extern u16 volatile NTC_temp,last_NTC_temp;//�ֱ��¶�
//PID������ʼ��
void PID_Setup(void)
{
	aggKp = 11;//�������������õ�������
	aggKi = 0.5;//���ֲ���T/Ti������������̬���
	aggKd = 1;//΢�ֲ���Td/T������Ԥ�����ı仯��������ǰ����
	consKp=0.5;//���ص�PID����
	consKi=0.6;
	consKd=6;
}
//����PID���uk
void PID_Operation(void)
{
	pt = T12_temp;//��ǰ�¶�ֵ
	if(sleepFlag) e0 = TEMP_SLEEP - pt;//˯��ģʽ�µ��¶�
	else if(setData.workMode==0) //ǿ��ģʽ�������¶�
	{
		if(setData.setTemp + TEMP_ADD > TEMP_MAX) e0 = TEMP_MAX - pt;//���ܳ������ֵ
		else  e0 = setData.setTemp + TEMP_ADD - pt;
	}
	else e0 = setData.setTemp - pt;//��׼ģʽ
	if(e0>50) uk=PWM_T;//�²�>50��ʱ��ȫ�ټ���
	else if(e0>30)//�²�30~50��ʱ�����м�����PID����
	{
		duk=aggKp*(e0-e1)+aggKp*aggKi*e0+aggKp*aggKd*(e0-2*e1+e2);
		uk=uk+duk;
	}
	else//�²�<30��ʱ�����б��ص�PID����
	{
		duk=consKp*(e0-e1)+consKp*consKi*e0+consKp*consKd*(e0-2*e1+e2);
		uk=uk+duk;
	}
	if(uk > PWM_T) uk = PWM_T;//��ֹ����
	if(uk < 0) uk = 0;
	e2=e1;
	e1=e0;
	
}
/* ********************************************************
* �������ƣ�PID_Output()                                                                         
* �������ܣ�PID�������                                         
* ��ڲ������ޣ��������룬U(k)��                                                 
* ���ڲ������ޣ����ƶˣ�                                                                               
******************************************************** */

void PID_Output(void)
{
	T12_temp = get_T12_temp();//��ǰ�¶�ֵ
	if(shutFlag==0&&err==0) PID_Operation();//����PID����
	else uk = 0;//������
	TIM_SetCompare1(TIM2,uk); //�����PWM-PA0������
}
//����ͷ�������ų�ʼ��-PB4
void HEAT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIO����ʱ��
	    
 
   //����PA0����Ϊ�����������,���PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM2_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA
	
	
   //��ʼ��TIM2
	TIM_TimeBaseStructure.TIM_Period = PWM_T-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =719; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM2 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1������ֵ<�Զ���װ��ֵʱ������ߵ�ƽ
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2 OC1

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR1�ϵ�Ԥװ�ؼĴ���

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2
}
	
//��ʱ��4��ʼ����ΪPID�ṩ��ʱ
void TIM4_Counter_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
	
    TIM_TimeBaseInitStructure.TIM_Period = arr; //�Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //Ԥ��Ƶֵ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; // ���ϼ���
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //ʱ�ӷָ�Ϊ0,��Ȼʹ��72MHz
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//��������ж�
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_Cmd(TIM4,ENABLE);
}
//TIM4��ʱ���жϷ�����
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)//���TIM4�����жϷ������
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//���TIM4�����жϱ�־
		tim4_count++;
		if(tim4_count%2==0){
			PID_Output();//����PID
		}

		if(tim4_count>=100)
		{	
			tim4_count=0;
			nowTime++;//100ms����һ�μ�ʱ��
			sleepCount++;
			shutCount++;
			sleepCheck();//����񶯿���
		}

	}
}
//��ȡʱ���ַ���
void getClockTime(char timeStr[])
{
	u32 min=0,sec=0;
	sec = nowTime/10;
//	hour = sec/3600;
	min = sec/60;
	sec = sec-min*60;
	sprintf((char *)timeStr,"%03d:%02d",min,sec);//���ʱ���ַ���
}

