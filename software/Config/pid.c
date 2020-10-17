#include "pid.h"
#include "main.h"
#include "adc.h"
#include "usart.h"
#include "key.h"
#include "menu.h"

#define DBGMCU_CR  (*((volatile u32 *)0xE0042004))
u16 timecount,g_bPIDRunFlag;
int uk,duk;//uk为当前输出值，duk为PID增量
float kp,ki,kd;
float e0,e1,e2;
float st,pt;
u16 sampleT=100;//采样周期100
volatile u32 nowTime = 0;//程序运行时间，单位0.01s
volatile u32 sleepCount = 0;
volatile u32 shutCount = 0;
//PID参数初始化
void PID_Setup(void)
{
	ki = sampleT/60.0;//积分参数
	kp = 20.0;//比例参数，设置调节力度T/Ti，可以消除稳态误差
	kd = 40/sampleT;//微分参数Td/T，可以预测误差的变化，做到提前决策
	st = 20;//设定目标温度
}
//计算PID输出uk
void PID_Operation(void)
{
	pt = readThermistor();//当前温度值
	e0=st-pt;
	if(e0>10) uk = 100;//温差>10℃时，全速加热
	else//否则进行PID解算
	{
		duk=kp*(e0-e1)+kp*ki*e0+kp*kd*(e0-2*e1+e2);
		uk=uk+duk;
		if(uk>100) uk=100;//防止饱和
		if(uk<0) uk=0;
	}
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
		HEAT=0; //不加热
	else if(heatFlag) HEAT = 1;//加热
	else HEAT=0; //不加热
	if(uk) uk--;                //只有uk>0，才有必要减“1”
//	if(timecount%10==0) printf("uk:%d,e0:%2.1f,e1:%2.1f,e2:%2.1f\r\n",uk,e0,e1,e2);
	timecount++;
	if(timecount >= 100)
	{
		PID_Operation();        //每过0.1*100s调用一次PID运算。
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
	
	//STM32没有彻底释放PB3作为普通IO口使用，切换到SW调试可释放PB3、PB4、PA15
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	DBGMCU_CR &=0xFFFFFFDF;  //如果没有这段代码，PB3就会一直是低电平
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

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)//检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//清除TIM4更新中断标志
		PID_Output();//每0.01s运行一次PID
		nowTime++;
		sleepCount++;
		shutCount++;
	}
}

void getClockTime(char timeStr[])
{
	u8 hour=0,min=0,sec=0;
	sec = nowTime/100;
	hour = sec/3600;
	min = sec%3600/60;
	sec = sec-hour*3600-min*60;
	sprintf((char *)timeStr,"%02d:%02d:%02d",hour,min,sec);//组合时间字符串
}

