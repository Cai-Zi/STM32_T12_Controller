#ifndef __MENU_H
#define __MENU_H			  	 
#include "sys.h"
#include "stdlib.h"

typedef enum// �˵�����
{
	home,//��ҳ�棺HOME
	xmsj,//һ���˵�������ʱ��
	gjsj,//һ���˵����ػ�ʱ��
	gzms,//һ���˵�������ģʽ
	fmq,//һ���˵���������
	xmsjSet,//���ã�����ʱ��
	gjsjSet,//���ã��ػ�ʱ��
	gzmsSet,//���ã�����ģʽ
	fmqSet,//���ã�������
	yysz,//һ���˵�����������
	hfmr,//һ���˵����ָ�Ĭ������
	gybj,//һ���˵������ڱ���
	tc,//һ���˵����˳�
	yyszSet,//���ã���������
	hfmrTip,//���ã��ָ�Ĭ������
	gybjTip,//���ã����ڱ���
	
	MENU_NUM, // �˵�ҳ����
}MENU_INDEX_LIST;

typedef struct{
	u8 current_index;	//��ŵ�ǰ����������ţ�
	u8 down_index;		//��Ű��¡�down�����£�����ʱ��Ҫ��ת���������ţ�
	u8 up_index;		//��Ű��¡�up�����ϣ�����ʱ��Ҫ��ת���������ţ�
	u8 enter_index;		//��Ű��¡�enter�����룩����ʱ��Ҫ��ת�������ţ�
	u8 esc_index;		//��Ű��¡�esc���˳�������ʱ��Ҫ��ת�������ţ�
	u8 home_index;		//��Ű��¡�home�������棩����ʱ��Ҫ��ת�������ţ�
	void (*operate)();	//����ָ���������ŵ�ǰ����������Ҫִ�еĺ�������ڵ�ַ��
}Key_index_struct;


extern volatile u8 nowMenuIndex;
extern volatile u8 lastMenuIndex;
extern volatile u8 sleepFlag;
extern volatile u8 shutFlag;

void OLED_display(void);
void homeWindow(void);//������
void menu_xmsj(void);//һ���˵�������ʱ��
void menu_gjsj(void);//һ���˵����ػ�ʱ��
void menu_gzms(void);//һ���˵�������ģʽ
void menu_fmq(void);//һ���˵���������
void menu_yysz(void);//һ���˵�����������
void menu_hfmr(void);//һ���˵����ָ�Ĭ������
void menu_gybj(void);//һ���˵������ڱ���
void menu_tc(void);//һ���˵����˳�

void menu_xmsjSet(void);//���ã�����ʱ��
void menu_gjsjSet(void);//���ã��ػ�ʱ��
void menu_gzmsSet(void);//���ã�����ģʽ
void menu_fmqSet(void);//���ã�������
void menu_yyszSet(void);//���ã���������
void menu_hfmrTip(void);//���ã��ָ�Ĭ������
void menu_gybjTip(void);//���ã����ڱ���
#endif

