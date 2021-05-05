/*
==============T20��̨������===============
���ߣ�Bibibili ����CaiZi
΢��������STM32F103C8T6
�ⲿʱ�ӣ�8MHz
===================����===================
OLED��ʾ��(7��SPI)��
	OLED_D0  -> PB13
	OLED_D1	 -> PB15
	OLED_RES -> PB15
	OLED_DC -> PB11
	OLED_CS -> PB12
��ת��������
	BM_CLK -> PB0
	BM_DT  -> PB3
	BM_SW  -> PB1
��������
	BEEPER -> PB9
������أ�
	NTC 	-> PA6
	T12_ADC -> PA4
	HEAT 	-> PA0
	SLEEP 	-> PA8
	Vm		-> PA2
//�˰汾�Ĵ���ʵ�ֹ��ܣ�
//-T12����ͷ���¶Ȳ���
//-�������ķֶ�PID����
//-ͨ����ת�����������¶ȿ���
//-�̰���ת���������ؿɽ�������ģʽ
//-������ת���������ص����ò˵�
//-�ֱ��˶���⣨ͨ������񶯿��أ�
//-�����ѹ���
//-ʱ��������˯��/�ػ�ģʽ��ͨ������δʹ���ӵ�ʱ����
//-OLED�ϵ���Ϣ��ʾ
//-������
//-���û����ô洢��FLASH

*/
#include "stm32f10x.h"
#include "main.h"

void menuHandler(void);

extern unsigned char logo[];
char tempStr[10];//��ص�ѹ�ַ���
float volatile VinVolt;//�����ѹ
u16 volatile NTC_temp,last_NTC_temp;//�ֱ��¶�

u16 volatile T12_temp;//����ͷ�¶�
u16 volatile tempArray[TEMPARRLEN];//�¶����飬��ֵ�˲�
u16 NTC_tempArray[NTC_TEMPARRLEN];//�ֱ��¶����飬��ֵ�˲�
u16 count;
u8 err,err_count;
u8 n;
int main()
{
	u8 i;
	u16 sum;
	KEY_Init();//��ʼ������GPIO
	delay_init();//��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2��2λ��ռ���ȼ���2λ�����ȼ�
	usart_init(115200);//��ʼ������1��������Ϊ115200
	TIM3_Init(19999,71);//1MHz��ÿ20ms��ⰴ��һ�Σ�
	BEEPER_Init();	//BEEPER��ʼ��
	Adc_Init();		//ADC��ʼ��
	HEAT_Init();//����ͷ���ƶ˳�ʼ��
	OLED_Init();	//��ʼ��OLED
	set_Init();//��ȡ�û���������
	
	PID_Setup();//PID��ʼ��
	TIM4_Counter_Init(99,719);//��ʱ1ms�ж�һ��
	OLED_Clear();
	OLED_DrawPointBMP(0,0,logo,128,64,1);//��ʾlogo
	OLED_Refresh_Gram();//ˢ���Դ�
	delay_ms(1000);
	
	beeperOnce();
	//��ʼ���¶�
	NTC_temp = get_NTC_temp();//��ȡ�ֱ��¶�
	T12_temp = NTC_temp;
	for(n=0; n<TEMPARRLEN; n++)
	{
		tempArray[n]=T12_temp;
	}
	
	OLED_Fill(0,0,127,63,0);
	while (1){
		if(count%100==0)//����һ��
		{
			NTC_temp =  get_NTC_temp();//��ȡһ���ֱ��¶�ֵ
			get_Vin();//��ȡһ�������ѹֵ
			
			if(NTC_temp>=50)
			{
				NTC_temp =last_NTC_temp;
			}
			else
			{
			    last_NTC_temp = NTC_temp;
			}
			sum=0;
			for(i=0;i<NTC_TEMPARRLEN-1;i++)
			{
				NTC_tempArray[i] = NTC_tempArray[i+1];//Ԫ��ǰ��
				sum += NTC_tempArray[i+1];
			}
			NTC_tempArray[NTC_TEMPARRLEN-1] = NTC_temp; 
			sum += NTC_tempArray[NTC_TEMPARRLEN-1];
			NTC_temp = (u16)sum/NTC_TEMPARRLEN;//��ֵ�˲�
			//�ֱ����Ӽ��
			if(NTC_temp>=5&&NTC_temp<=50)
			{
				err_count=0;
				err=0;
			}
			if((NTC_temp<5||NTC_temp>50)&&err==0)
			{
				err_count++;
			}
			if(err_count>=8)
			{
				err=1;//�ֱ�δ����
			}
		}
		if(nowMenuIndex==home && count%800==0)//����һ��home����
		{
			homeWindow();//��ʾ������
			OLED_Refresh_Gram();//ˢ���Դ�
		}
		if(menuEvent[0])
		{
			menuHandler();
			beeperOnce();
			if(menuEvent[1]==KEY_enter && nowMenuIndex == home)
			{
				if(sleepFlag) {sleepFlag=0; sleepCount=0;shutCount = 0;}//�̰������ر�����
				else sleepFlag = 1;
			}
			if(shutFlag) {shutFlag=0;shutCount = 0;}//���ⰴ����������
			lastMenuIndex = nowMenuIndex;
			menuEvent[0] = 0;
			OLED_display();
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//д��FLASH
		}
		if(setData.sleepTime>0 && sleepCount>setData.sleepTime*600) {sleepFlag=1;}
		if(setData.shutTime>0 && shutCount>setData.shutTime*600) {shutFlag=1;}
		count++;
		delay_us(100);
	}
}
//�˵�������
void menuHandler(void)
{
	if(menuEvent[1]==BM_up)
	{
		switch(nowMenuIndex){
			case home:
				setData.setTemp+=5;
			break;
			case xmsjSet:
				setData.sleepTime++;
			break;
			case gjsjSet:
				setData.shutTime++;
			break;
			case gzmsSet:
				setData.workMode=!setData.workMode;
			break;
			case fmqSet:
				setData.beeperFlag = !setData.beeperFlag;
			break;
			case yyszSet:
				setData.langFlag = !setData.langFlag;
			break;
		}
	}
	else if(menuEvent[1]==BM_down)
	{
		switch(nowMenuIndex){
			case home:
				setData.setTemp-=5;
			break;
			case xmsjSet:
				setData.sleepTime--;
			break;
			case gjsjSet:
				setData.shutTime--;
			break;
			case gzmsSet:
				setData.workMode=!setData.workMode;
			break;
			case fmqSet:
				setData.beeperFlag = !setData.beeperFlag;
			break;
			case yyszSet:
				setData.langFlag = !setData.langFlag;
			break;
		}
	}
	else {}
	if(setData.setTemp>TEMP_MAX) setData.setTemp=TEMP_MAX;
	if(setData.setTemp<TEMP_MIN) setData.setTemp=TEMP_MIN;
	if(setData.sleepTime>60) setData.sleepTime=60;
	if(setData.sleepTime<0) setData.sleepTime=0;
	if(setData.shutTime>60) setData.shutTime=60;
	if(setData.shutTime<0) setData.shutTime=0;
}
