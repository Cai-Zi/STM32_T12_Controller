#include "pid.h"
#include "main.h"
#include "adc.h"
#include "usart.h"
#include "key.h"
#include "menu.h"
#include "setting.h"

#define DBGMCU_CR  (*((volatile u32 *)0xE0042004))
u16 timecount,g_bPIDRunFlag;
int uk,duk;//uk为当前输出值，duk为PID增量
float aggKp,aggKi,aggKd;//激进的PID参数
float consKp,consKi,consKd;//保守的PID参数
float e0,e1,e2;
float st,pt;
u16 sampleT=255;//采样周期100
volatile u32 nowTime = 0;//程序运行时间，单位0.01s
volatile u32 sleepCount = 0;
volatile u32 shutCount = 0;
//PID参数初始化
void PID_Setup(void)
{
	aggKp = 11;//比例参数，设置调节力度
	aggKi = 0.5;//积分参数T/Ti，可以消除稳态误差
	aggKd = 1;//微分参数Td/T，可以预测误差的变化，做到提前决策
	consKp=11;//保守的PID参数
	consKi=3;
	consKd=5;
}
//计算PID输出uk
void PID_Operation(void)
{
	pt = get_T12_temp();//当前温度值
	T12_temp = pt;
	e0=setData.setTemp-pt;
	if(e0>50)//温差>50℃时，进行激进的PID解算
	{
		duk=aggKp*(e0-e1)+aggKp*aggKi*e0+aggKp*aggKd*(e0-2*e1+e2);
		uk=uk+duk;
	}
	else//温差<30℃时，进行保守的PID解算
	{
		duk=consKp*(e0-e1)+consKp*consKi*e0+consKp*consKd*(e0-2*e1+e2);
		uk=uk+duk;
	}
	if(uk>sampleT) uk=sampleT;//防止饱和
	if(uk<0) uk=0;
	e2=e1;
	e1=e0;
}
/* ********************************************************
* 函数名称：PID_Output()                                                                         
* 函数功能：PID输出控制                                         
* 入口参数：无（隐形输入，U(k)）                                                 
* 出口参数：无（控制端）                                                                               
******************************************************** */
void PID_Output(void)
{
	if(uk <= 0)
	{
		HEAT=0; //不加热
	}
	else if(sleepFlag==0&&shutFlag==0) 
	{
		HEAT = 1;//加热
	}
	else HEAT=0; //不加热
	if(uk) uk--;                //只有uk>0，才有必要减“1”
//	if(timecount%10==0) printf("uk:%d,e0:%2.1f,e1:%2.1f,e2:%2.1f\r\n",uk,e0,e1,e2);
	timecount++;
	if(timecount >= sampleT)
	{
		PID_Operation();        //每过0.1*255s调用一次PID运算。
		timecount = 0;       
	}
}
//加热头控制引脚初始化-PB4
void HEAT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  //使能GPIOA时钟
  
	GPIO_InitStructure.GPIO_Pin = HEAT_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(HEAT_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(HEAT_GPIO_Port,HEAT_Pin);//拉低
}
	
//定时器4初始化，为PID提供计时
void TIM4_Counter_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
	
    TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //预分频值
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //时钟分割为0,仍然使用72MHz
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//允许更新中断
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM_Cmd(TIM4,ENABLE);
}
//TIM4定时器中断服务函数
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)//检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//清除TIM4更新中断标志
		PID_Output();//每100us运行一次PID
		nowTime++;//1ms更新一次计时器
		sleepCount++;
		shutCount++;
	}
}
//获取时间字符串
void getClockTime(char timeStr[])
{
	u32 hour=0,min=0,sec=0;
	sec = nowTime/1000;
	hour = sec/3600;
	min = sec%3600/60;
	sec = sec-hour*3600-min*60;
	sprintf((char *)timeStr,"%02d:%02d:%02d",hour,min,sec);//组合时间字符串
}

