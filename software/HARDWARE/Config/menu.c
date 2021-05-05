#include "oled.h"
#include "stdlib.h" 	 
#include "delay.h"
#include "menu.h"
#include "key.h"
#include "setting.h"
#include "delay.h"
#include "usart.h"
#include "pid.h"
#include "adc.h"
#include "main.h"
/*https://blog.csdn.net/embedded_guzi/article/details/35835755
https://blog.csdn.net/calmuse/article/details/79346742
*/
u16 ShowTemp;
u8 volatile sleepFlag = 0;//�Ƿ�����,1Ϊ����
u8 volatile shutFlag = 0;//�Ƿ�����,1Ϊ�ػ�

u8 volatile nowMenuIndex = 0;
u8 volatile lastMenuIndex = 0;
extern unsigned char logo[];
extern unsigned char logoR[];
extern u8 err;
char batVoltStr[10]={0};
u8 onSign[28] = {0X7F,0XF8,0XFF,0XFC,0XC0,0X0C,0XC2,0X0C,0XC7,0X0C,0XC3,0X8C,0XC1,0XCC,0XC1,0XCC,0XC3,0X8C,0XC7,0X0C,0XCE,0X0C,0X9C,0X0C,0X39,0XFC,0X13,0XF8,};
u8 offSign[28] = {0X7F,0XF8,0XFF,0XFC,0XC0,0X0C,0XC8,0X4C,0XDC,0XEC,0XCF,0XCC,0XC7,0X8C,0XC7,0X8C,0XCF,0XCC,0XDC,0XEC,0XC8,0X4C,0XC0,0X0C,0XFF,0XFC,0X7F,0XF8,};
u8 tempSign[32] = {0x60,0x00,0x90,0x00,0x90,0x00,0x67,0xE0,0x1F,0xF8,0x30,0x0C,0x20,0x04,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x20,0x04,0x78,0x08,0x00,0x00,0x00,0x00};/*"��",0*/
u8 heatSign[28] = {0X01,0XF0,0X07,0XF8,0X0F,0XF8,0X1B,0XFC,0X00,0XFC,0X1F,0XFC,0X3F,0XFC,0X7F,0XFC,0XCF,0XFC,0X03,0XFC,0X1C,0XFC,0X2F,0XF8,0X03,0XF8,0X01,0XF0,};
u8 sleepSign[32] = {0X00,0X00,0X03,0XC0,0X0F,0XF0,0X1F,0XF8,0X38,0XFC,0X60,0X3C,0X40,0X1E,0X00,0X1E,0X18,0X8E,0X1B,0X8E,0X1F,0X8C,0X1D,0X8C,0X11,0X98,0X00,0X10,0X00,0X20,0X00,0X00,};
u8 cnSign[28] = {0X7F,0XF8,0XFF,0XFC,0XC0,0X0C,0XC7,0X8C,0XC7,0X8C,0XC4,0X8C,0XDF,0XEC,0XDF,0XEC,0XC4,0X8C,0XC7,0X8C,0XC7,0X8C,0XC0,0X0C,0XFF,0XFC,0X7F,0XF8,};
u8 enSign[28] = {0X7F,0XF8,0XFF,0XFC,0XC0,0X0C,0XC3,0XCC,0XCF,0XEC,0XC9,0X6C,0XD9,0X2C,0XD1,0X2C,0XD3,0X6C,0XDE,0X4C,0XCC,0X0C,0XC0,0X0C,0XFF,0XFC,0X7F,0XF8,};
u8 lock[35] =  {0X01,0X01,0X0E,0X00,0X0E,0X00,0X03,0XF8,0X3F,0XFC,0X7F,0XFC,0XE3,0XFC,0XC3,0XFC,0XC3,0XCC,0XC2,0X04,0XC2,0X04,0XC3,0XCC,0XC3,0XFC,0XE3,0XFC,0X7F,0XFC,0X3F,0XFC,0X03,0XF8,};


Key_index_struct const Key_table[MENU_NUM]=
{
	//��ǰ, ��, ��, ȷ��, ����, home
    {home, home, home, home, home, xmsj,(*homeWindow)},//��ҳ�棺home
	
	{xmsj, gjsj, tc, xmsjSet, home, home,(*menu_xmsj)},//һ���˵�������ʱ��
	{gjsj, gzms, xmsj, gjsjSet, home, home,(*menu_gjsj)},//һ���˵����ػ�ʱ��
	{gzms, fmq, gjsj, gzmsSet, home, home,(*menu_gzms)},//һ���˵�������ģʽ
	{fmq, yysz, gzms, fmqSet, home, home,(*menu_fmq)},//һ���˵���������
	{xmsjSet,xmsjSet,xmsjSet,xmsj,home,home,(*menu_xmsjSet)},//���ã�����ʱ��
	{gjsjSet,gjsjSet,gjsjSet,gjsj,home,home,(*menu_gjsjSet)},//���ã��ػ�ʱ��
	{gzmsSet,gzmsSet,gzmsSet,gzms,home,home,(*menu_gzmsSet)},//���ã�����ģʽ
	{fmqSet,fmqSet,fmqSet,fmq,home,home,(*menu_fmqSet)},//���ã�������
	
	{yysz, hfmr, fmq, yyszSet, home, home,(*menu_yysz)},//һ���˵�����������
	{hfmr, gybj, yysz, hfmrTip, home, home,(*menu_hfmr)},//һ���˵����ָ�Ĭ������
	{gybj, tc, hfmr, gybjTip, home, home,(*menu_gybj)},//һ���˵������ڱ���
	{tc, xmsj, gybj, home, home, home,(*menu_tc)},//һ���˵����˳�
	{yyszSet,yyszSet,yyszSet,yysz,home,home,(*menu_yyszSet)},//���ã���������
	{hfmrTip,hfmrTip,hfmrTip,hfmr,home,home,(*menu_hfmrTip)},//���ã��ָ�Ĭ������
	{gybjTip,gybjTip,gybjTip,gybj,home,home,(*menu_gybjTip)},//���ã����ڱ���
	
};

void OLED_display(void){
	switch(menuEvent[1]){
		case BM_down: //��ʱ����ת������
			nowMenuIndex=Key_table[nowMenuIndex].down_index;
			break;
		case BM_up: //˳ʱ����ת,����
			nowMenuIndex=Key_table[nowMenuIndex].up_index;
			break;
		case KEY_enter://ȷ��
			nowMenuIndex=Key_table[nowMenuIndex].enter_index;
			break;
		case KEY_esc://����
			nowMenuIndex=Key_table[nowMenuIndex].esc_index;
			break;
		case KEY_home://���� ����������
			nowMenuIndex=Key_table[nowMenuIndex].home_index;
			break;
	}
//	printf("key:%d, last:%d, now:%d\r\n",menuEvent[1],lastMenuIndex,nowMenuIndex);
	if(nowMenuIndex==home && lastMenuIndex!=home) OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=xmsj && nowMenuIndex<=fmqSet && (lastMenuIndex<xmsj | lastMenuIndex>fmqSet)) OLED_Fill(0,0,127,63,0);//���
	if(nowMenuIndex>=yysz && nowMenuIndex<=yyszSet && (lastMenuIndex<yysz | lastMenuIndex>yyszSet)) OLED_Fill(0,0,127,63,0);//���
	Key_table[nowMenuIndex].operate();
	OLED_Refresh_Gram();//ˢ���Դ�
}

void homeWindow(void)
{
	
	u8 line1Index[] = {42,43,44};
	u8 line2Index[] = {45,46};
	char tempStr[4];//�¶��ַ���	
	u16 bai,shi,ge;
	char VinStr[6];//�����ѹ�ַ���
	char timeStr[10];
	getClockTime(timeStr);
	OLED_ShowString(0,0,(u8*)timeStr,16,0);//ʱ��000:00

	if(VinVolt<10.0) sprintf((char *)VinStr,"%1.2f",VinVolt);//��ϵ�ѹ�ַ���
	else sprintf((char *)VinStr,"%2.1f",VinVolt);//��ϵ�ѹ�ַ���
	OLED_ShowString(52,0, (u8 *)VinStr,16,1);//�����ѹ

	sprintf((char *)tempStr,"%d",setData.setTemp);//����¶��ַ���
	OLED_ShowString(88,0, (u8 *)tempStr,16,0);//�����¶�
	OLED_DrawPointBMP(112,0,tempSign,16,16,0);//��
	OLED_Fill(0,15,127,15,1);//ˮƽ�ָ���// ���趨�㸽���ȶ���ʾ�¶�
	
	if(err==0)//������ʾ
	{
		// ���趨�㸽���ȶ���ʾ�¶�
		if ((ShowTemp != setData.setTemp) || (abs(ShowTemp - T12_temp) > 10)) ShowTemp = T12_temp;
		if (abs(ShowTemp - setData.setTemp) <= 1) ShowTemp = setData.setTemp;
		bai = (u16)ShowTemp/100;
		shi = (u16)ShowTemp%100/10;
		ge = (u16)ShowTemp%10;
		OLED_DrawPointNum(0,17,bai*6,1);//��ǰ�¶�-��λ
		OLED_DrawPointNum(25,17,shi*6,1);//��ǰ�¶�-ʮλ
		OLED_DrawPointNum(50,17,ge*6,1);//��ǰ�¶�-��λ
		if(PWM_SHOW)OLED_ShowNum(78,48,(u16)100*uk/255,3,16);//����PWM�ٷֱ�
	}else
	{
		OLED_ShowString(0,17, (u8 *)"          ",16,1);
		OLED_ShowString(0,33, (u8 *)"          ",16,1);
		OLED_ShowString(0,48, (u8 *)"             ",16,1);
	    OLED_ShowChineseWords(12,22,line1Index,3,1);
	    OLED_ShowChineseWords(21,40,line2Index,2,1);
//		if(PWM_SHOW)OLED_DrawPointBMP(76,48,lock,20,14,1);//��ʾ����
	}
	
	OLED_ShowNum(78,24,(u16)NTC_temp,2,16);//�ֱ��¶�
	OLED_DrawPointBMP(94,24,tempSign,16,16,1);//��

	if(shutFlag||err==1){
		OLED_DrawPointBMP(112,24,sleepSign,16,15,1);//���߱�־
		if(PWM_SHOW)OLED_ShowString(104,48, (u8 *)"OFF",16,1);//��������
		else OLED_ShowString(94,48, (u8 *)"OFF",16,1);//��������
			
	}
	else if(sleepFlag){
		OLED_DrawPointBMP(112,24,sleepSign,16,15,1);//���ȱ�־
		if(PWM_SHOW)OLED_ShowString(104,48, (u8 *)" ON",16,1);//��������
		else OLED_ShowString(88,48, (u8 *)" ON ",16,1);//��������
	}
	else{
		OLED_DrawPointBMP(112,24,heatSign,16,15,1);//���ȱ�־
		if(PWM_SHOW)OLED_ShowString(104,48, (u8 *)" ON",16,1);//��������
		else OLED_ShowString(88,48, (u8 *)" ON ",16,1);//��������
	}
	
}

/*
listIndex����������鶨λ,1~8
mode�����е���ʾģʽ
str1~3�������е��ַ���
hzIndex�������е�12�������������е�����
*/
void menuListWindow(u8 mode[])
{	

	char TimeStr[6];
	u8 line1Index[] = {0,1,2,3};
	u8 line2Index[] = {4,5,2,3};
	u8 line3Index[] = {6,7,8,9};
	u8 modeIndex[2];
	u8 line4Index[] = {14,15,16};
	//��һ��
	OLED_ShowString(0,0,(u8*)"1.",16,mode[0]);
	
	OLED_ShowChineseWords(16,0,line1Index,4,mode[0]);

	sprintf((char *)TimeStr,"%-2dmin",setData.sleepTime);//���ʱ���ַ���
	OLED_ShowString(88,0,(u8 *)TimeStr,16,mode[4]);
	//�ڶ���
	OLED_ShowString(0,16,(u8*)"2.",16,mode[1]);
	
	OLED_ShowChineseWords(16,16,line2Index,4,mode[1]);
	sprintf((char *)TimeStr,"%-2dmin",setData.shutTime);//���ʱ���ַ���
	OLED_ShowString(88,16,(u8*)TimeStr,16,mode[5]);
	//������
	OLED_ShowString(0,32,(u8*)"3.",16,mode[2]);
	
	OLED_ShowChineseWords(16,32,line3Index,4,mode[2]);
	
	if(setData.workMode) {modeIndex[0] = 10;modeIndex[1] = 11;}
	else {modeIndex[0] = 12;modeIndex[1] = 13;}
	OLED_ShowChineseWords(88,32,modeIndex,2,mode[6]);
	//������
	OLED_ShowString(0,48,(u8*)"4.",16,mode[3]);
	
	OLED_ShowChineseWords(16,48,line4Index,3,mode[3]);
	if(setData.beeperFlag) OLED_DrawPointBMP(88,50,onSign,16,14,mode[7]);//�򿪱�־
	else OLED_DrawPointBMP(88,50,offSign,16,14,mode[7]);//�رձ�־
}
void menu_xmsj(void){
	u8 mode[]={0,1,1,1,1,1,1,1};
	menuListWindow(mode);
}//һ���˵�������ʱ��
void menu_gjsj(void){
	u8 mode[]={1,0,1,1,1,1,1,1};
	menuListWindow(mode);
}//һ���˵����ػ�ʱ��
void menu_gzms(void){
	u8 mode[]={1,1,0,1,1,1,1,1};
	menuListWindow(mode);
}//һ���˵�������ģʽ
void menu_fmq(void){
	u8 mode[]={1,1,1,0,1,1,1,1};
	menuListWindow(mode);
}//һ���˵���������
void menu_xmsjSet(void){
	u8 mode[]={0,1,1,1,0,1,1,1};
	menuListWindow(mode);
}//���ã�����ʱ��
void menu_gjsjSet(void){
	u8 mode[]={1,0,1,1,1,0,1,1};
	menuListWindow(mode);
}//���ã��ػ�ʱ��
void menu_gzmsSet(void){
	u8 mode[]={1,1,0,1,1,1,0,1};
	menuListWindow(mode);
}//���ã�����ģʽ
void menu_fmqSet(void){
	u8 mode[]={1,1,1,0,1,1,1,0};
	menuListWindow(mode);
}//���ã�������
void menuListWindow2(u8 mode[])
{	
	u8 line4Index[] = {31,32};
    u8 line3Index[] = {27,28,29,30};
	u8 line2Index[] = {23,24,25,26,19,20};
    u8 line1Index[] = {17,18,19,20};
	//��һ��
	OLED_ShowString(0,0,(u8*)"5.",16,mode[0]);
	
	OLED_ShowChineseWords(16,0,line1Index,4,mode[0]);
	if(setData.langFlag) OLED_DrawPointBMP(88,0,cnSign,16,14,mode[4]);//���ı�־
	else OLED_DrawPointBMP(88,0,enSign,16,14,mode[4]);//Ӣ�ı�־
	//�ڶ���
	OLED_ShowString(0,16,(u8*)"6.",16,mode[1]);
	
	OLED_ShowChineseWords(16,16,line2Index,6,mode[1]);
	//������
	OLED_ShowString(0,32,(u8*)"7.",16,mode[2]);
	
	OLED_ShowChineseWords(16,32,line3Index,4,mode[2]);
	//������
	OLED_ShowString(0,48,(u8*)"8.",16,mode[3]);
	
	OLED_ShowChineseWords(16,48,line4Index,2,mode[3]);
}
void menu_yysz(void){
	u8 mode[]={0,1,1,1,1};
	menuListWindow2(mode);
}//һ���˵�����������
void menu_hfmr(void){
	u8 mode[]={1,0,1,1,1};
	menuListWindow2(mode);
}//һ���˵����ָ�Ĭ������
void menu_gybj(void){
	u8 mode[]={1,1,0,1,1};
	menuListWindow2(mode);
}//һ���˵������ڱ���
void menu_tc(void){
	u8 mode[]={1,1,1,0,1};
	menuListWindow2(mode);
}//һ���˵����˳�
void menu_yyszSet(void){
	u8 mode[]={0,1,1,1,0};
	menuListWindow2(mode);
}//���ã���������
void menu_hfmrTip(void){
	OLED_DrawPointBMP(114,18,onSign,16,14,1);//�򿪱�־
	resetData();
}//���ã��ָ�Ĭ������
void menu_gybjTip(void){
	u8 bbIndex[] = {37,38};
	u8 rqIndex[] = {35,36};
	u8 zzIndex[] = {33,34};
	u8 czIndex[] = {40,41};
	OLED_Fill(0,0,127,63,0);//���
	OLED_ShowChineseWords(0,0,zzIndex,2,1);
	OLED_ShowString(32,0,(u8*)": ",16,1);

	OLED_ShowChineseWords(48,0,czIndex,2,1);
	
	
	OLED_ShowChineseWords(0,16,rqIndex,2,1);
	OLED_ShowString(32,16,(u8*)REA_TIME,16,1);
	
	
	OLED_ShowChineseWords(0,32,bbIndex,2,1);
	OLED_ShowString(32,32,(u8*)VERSION,16,1);
	
	OLED_ShowString(0,48,(u8*)"QQ",16,1);
	OLED_ShowChinese(16,48,39,16,1);
	OLED_ShowString(32,48,(u8*)": 1091996634",16,1);
}//���ã����ڱ���
