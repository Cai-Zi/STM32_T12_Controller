#include "setting.h"
#include "main.h"

volatile set_Config setData;
volatile u32 setDataSize=sizeof(setData)/2;
	
//�ָ�Ĭ������
void resetData(void)
{
	setData.writeFlag = 0x11;
	setData.setTemp = 280;//�����¶ȡ�
	setData.sleepTime = TIME_SLEEP;//����ʱ��0-60min��0minΪ���Զ�����
	setData.shutTime = TIME_SHUT;//�ػ�ʱ��0-60min��0minΪ���Զ��ػ�
	setData.workMode = 1;//����ģʽ,1Ϊ��׼,0Ϊǿ��
	setData.beeperFlag = 1;//�������Ƿ���,1Ϊ����,0Ϊ�ر�
	setData.langFlag = 1;//��������,1Ϊ����,0ΪӢ��
}
void set_Init(void)
{
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//��FLASH�ж�ȡ�ṹ��
	if(setData.writeFlag!=0x11){//�Ƿ��һ��д��
		resetData();
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//д��FLASH
	}
}

