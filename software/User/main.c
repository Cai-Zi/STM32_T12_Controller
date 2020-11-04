#include "stm32f10x.h"
#include "main.h"

extern unsigned char logo[];
char tempStr[10];//电池电压字符串
u16 volatile NTC_temp;//手柄温度
u16 volatile T12_temp;//烙铁头温度
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
	TIM4_Counter_Init(999,71);//定时1ms中断一次
	PID_Setup();//PID初始化
	
	OLED_Clear();
	OLED_DrawPointBMP(9,0,logo,110,56,1);//显示logo
	OLED_Refresh_Gram();//刷新显存
	delay_ms(100);
	T12_temp = get_T12_temp();
	NTC_temp = get_NTC_temp();//读取手柄温度
	OLED_Fill(0,0,127,63,0);
	while (1){
		sleepCheck();//检测振动开关
		if(nowTime%505==0)//1s更新一次
		{
			NTC_temp = get_NTC_temp();
			printf("ADC:%d\r\n",T12_Average);
		}
		if(nowMenuIndex==home && nowTime%101==0)//0.1s更新一次home界面
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
			if(shutFlag) {shutFlag=0;shutCount = 0;OLED_Display_On();}//任意按键触发开机
			lastMenuIndex = nowMenuIndex;
			menuEvent[0] = 0;
			OLED_display();
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//写入FLASH
		}
		if(setData.sleepTime>0 && sleepCount>setData.sleepTime*60000) {sleepFlag=1;}
		if(setData.shutTime>0 && shutCount>setData.shutTime*60000) {shutFlag=1;}
		if(shutFlag)OLED_Display_Off();
	}
}
