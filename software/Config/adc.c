#include "adc.h"
#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "math.h"
#include "main.h"
#include "pid.h"
/* 
   ===热敏电阻演示代码===
   为了消除噪声读数，采样ADC几次，然后平均样本以获得更稳定的测量值，用readThermistor函数实现。
   http://www.thermistors.cn/news/293.html
*/ 
#define sampleNum 10

float ADC_max = 4095.0; //最大采样值，12位ADC
/*使用beta方程计算阻值。*/ 
float beta = 3950.0; //商家给出的电阻对应25°C下的bata值
float roomTemp = 298.15; //以开尔文为单位的室温25°C
float balanceR = 9900.0;//参考电阻
float roomTempR = 10000.0; //NTC热敏电阻在室温25°C下具有典型的电阻
float currentTemperature = 0; //保存当前温度
u16 ch1Value[2*sampleNum];//ADC采样值
u16 NTC_Average=0;
u16 T12_Average=0;
u16 S_temp2Volt[]={
	0,55,113,173,235,299,365,432,502,573,//0~90℃
	645,719,795,872,950,1029,1109,1190,1273,1356,//100~190℃
	1440,1525,1611,1698,1785,1873,1962,2051,2141,2232,
	2323,2414,2506,2599,2692,2786,2880,2974,3069,3164,
	3260,3356,3452,3549,3645,3743,3840,3938,4036,4135,
	4234,4333,4432,4532,4632,4732,4832,4933,5034,5136,//500~590℃
	5237,5339,5442,5544,5648,5751,5855,5960,6065,6169};//S型热电偶分度表，单位：uV，参考温度：0℃
u16 S_caliVolt[]={0,55,113,173,235,299};//S型热电偶参考端温度0~50℃时的校正值，实际电压-校正值，再查分度表表
#define ADC1_DR_Address    ((u32)0x4001244C)		//ADC1的地址

//初始化ADC-PA0引脚
															   
void  Adc_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  //使能GPIOA时钟

	//PA6 作为模拟通道输入引脚   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	
	GPIO_InitStructure.GPIO_Pin = SLEEP_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SLEEP_GPIO_Port, &GPIO_InitStructure);  
	
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);				//配置ADC时钟，为PCLK2的6分频，即12MHz
	
	ADC_DeInit(ADC1);  //复位ADC1 
	
	//ADC1初始化
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 			//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器  
	
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);		//配置ADC1通道4为239.5个采样周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_239Cycles5);		//配置ADC1通道6为239.5个采样周期 
	
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
 
	ADC_ResetCalibration(ADC1);				//复位校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));				//等待校准寄存器复位完成
 
	ADC_StartCalibration(ADC1);				//ADC校准
	while(ADC_GetCalibrationStatus(ADC1));				//等待校准完成
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
}

//获得ADC值
//ch:通道值 0~9
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5个周期	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

//ch:通道值 0~9，采样times次后作均值滤波
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}

/*函数说明：仿Arduino,将一个数字从一个范围重新映射到另一个范围
也就是说，fromLow的值将映射到toLow，fromlhigh到toHigh的值等等。
*/
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh)
{
	return ((value-fromLow)*(toHigh-toLow)/(fromHigh-fromLow)+toLow);
}
/*二分法查找数字在数组中的索引，确定数字对应的索引
ipArray：包含数字的数组
start: 查找的起始索引0~len
end: 查找的结束索引start~len
value: 要查找的数值
*/
int SearchIndex(u16 ipArray[], int start, int end ,int  value)  //（二分法）
{
	int middle = (start + end) / 2;
	if (middle == start)
		return middle;
	else if (value < ipArray[middle])  
		return SearchIndex(ipArray, start, middle, value);
	else
		return SearchIndex(ipArray, middle, end, value);
}
/*函数功能：读取模拟引脚，如下所示。
  通过模数转换将电压信号转换为数字表示。但是，这样做了多次，因此我们可以对其进行平均以消除测量误差。
  然后使用该平均数来计算热敏电阻的电阻。此后，电阻用于计算热敏电阻的温度。最后，温度转换为摄氏度。
  有关此过程的详细信息和一般理论，请参阅allaboutcircuits.com文章。
  原理图：
         3.3V ----====-------  | ---------====--------(GND)
                R_balance      |      R_thermistor 
                               | 
                             ADC引脚
*/ 
u16 get_NTC_temp(void)
{
  float rThermistor = 0; //保存热敏电阻的电阻值
  float tKelvin = 0; //以开尔文温度保存温度
  float tCelsius = 0; //以摄氏温度保存温度
	NTC_Average = Get_Adc_Average(6,10);
  /*公式计算热敏电阻的电阻。*/ 
  rThermistor = balanceR * NTC_Average/(ADC_max - NTC_Average); 
  tKelvin =(beta * roomTemp)/(beta +(roomTemp * log(rThermistor / roomTempR)));  
  tCelsius = tKelvin  -  273.15; //将开尔文转换为摄氏温度
  return tCelsius;//以摄氏度返回温度
}
//获取热电偶的电压，根据分度表转换为温度
u16 get_T12_temp(void)
{
	u16 nowTemp,nowIndex;
	u16 nowVolt,nowCaliVolt;
	if(HEAT) 
	{
		HEAT=0;//先停止加热
		delay_ms(1);
		T12_Average = Get_Adc_Average(4,10);//获取采样值
		HEAT=1;//继续加热
	}
	else T12_Average = Get_Adc_Average(4,10);//获取采样值
	nowCaliVolt = S_caliVolt[(u16)NTC_temp/10]+(NTC_temp%10)*(S_caliVolt[(u16)NTC_temp/10+1]-S_caliVolt[(u16)NTC_temp/10])/10;
	nowVolt = T12_Average*3.3*2000/4095-nowCaliVolt;//热电偶当前电压uV
	nowIndex = SearchIndex(S_temp2Volt,0,70,nowVolt);
	nowTemp = nowIndex*10+10*(nowVolt-S_temp2Volt[nowIndex])/(S_temp2Volt[nowIndex+1]-S_temp2Volt[nowIndex]);
//	printf("%d℃\r\n",nowTemp);
	return nowTemp;//以摄氏度返回温度
}

u16 get_sleepSign(void)
{
	u16 nowSleep;
	nowSleep = SLEEP;
	if(nowSleep==0)
	{
		sleepCount=0;
		shutCount = 0;
	}
//	printf("%d\r\n",nowSleep);
	return nowSleep;
}
