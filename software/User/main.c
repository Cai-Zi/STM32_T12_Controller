/*
==============T20焊台控制器===============
作者：Bibibili 蔡子CaiZi
微处理器：STM32F103C8T6
外部时钟：8MHz
===================引脚===================
OLED显示屏(7脚SPI)：
	OLED_D0  -> PB13
	OLED_D1	 -> PB15
	OLED_RES -> PB15
	OLED_DC -> PB11
	OLED_CS -> PB12
旋转编码器：
	BM_CLK -> PB0
	BM_DT  -> PB3
	BM_SW  -> PB1
蜂鸣器：
	BEEPER -> PB9
控制相关：
	NTC 	-> PA6
	T12_ADC -> PA4
	HEAT 	-> PA0
	SLEEP 	-> PA8
//此版本的代码实现功能：
//-T12烙铁头的温度测量
//-加热器的分段PID控制
//-通过旋转编码器进行温度控制
//-短按旋转编码器开关可进入休眠模式
//-长按旋转编码器开关的设置菜单
//-手柄运动检测（通过检查振动开关）
//-时间驱动的睡眠/关机模式（通过计算未使用烙的时长）
//-OLED上的信息显示
//-蜂鸣器
//-将用户设置存储到FLASH

*/
#include "stm32f10x.h"
#include "main.h"

extern unsigned char logo[];
char tempStr[10];//电池电压字符串
u16 volatile NTC_temp;//手柄温度
u16 volatile T12_temp;//烙铁头温度
u16 count;
int main()
{
	KEY_Init();//初始化按键GPIO
	delay_init();//初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2，2位抢占优先级和2位子优先级
	usart_init(115200);//初始化串口1，波特率为115200
	TIM3_Init(19999,71);//1MHz，每20ms检测按键一次；
	BEEPER_Init();	//BEEPER初始化
	Adc_Init();		//ADC初始化
	HEAT_Init();//加热头控制端初始化
	OLED_Init();	//初始化OLED
	set_Init();//读取用户设置数据
	TIM4_Counter_Init(9999,719);//定时100ms中断一次
	PID_Setup();//PID初始化
	
	
	OLED_Clear();
	OLED_DrawPointBMP(0,0,logo,128,64,1);//显示logo
	OLED_Refresh_Gram();//刷新显存
	delay_ms(1000);
	T12_temp = get_T12_temp();
	NTC_temp = get_NTC_temp();//读取手柄温度
	OLED_Fill(0,0,127,63,0);
	while (1){
		PID_Output();//运行PID
		if(count%700==0)//更新一次
		{
			NTC_temp = get_NTC_temp();
			printf("ADC:%d\r\n",T12_Average);
		}
		if(nowMenuIndex==home && count%400==0)//更新一次home界面
		{
			homeWindow();//显示主界面
			OLED_Refresh_Gram();//刷新显存
		}
		if(menuEvent[0])
		{
			beeperOnce();
			if(menuEvent[1]==KEY_enter && nowMenuIndex == home)
			{
				if(sleepFlag) {sleepFlag=0; sleepCount=0;shutCount = 0;}//短按触发关闭休眠
				else sleepFlag = 1;
			}
			if(shutFlag) {shutFlag=0;shutCount = 0;}//任意按键触发开机
			lastMenuIndex = nowMenuIndex;
			menuEvent[0] = 0;
			OLED_display();
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//写入FLASH
		}
		if(setData.sleepTime>0 && sleepCount>setData.sleepTime*60000) {sleepFlag=1;}
		if(setData.shutTime>0 && shutCount>setData.shutTime*60000) {shutFlag=1;}
		count++;
	}
}
