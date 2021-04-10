/*********************************************************************************
* ����    ��ͨ��stm32��spi1��ȡmax6675���¶�ֵ����ͨ��uart1���ͳ���
 *          
 * ʵ��ƽ̨��STM32������
 * ��汾  ��ST3.0.0
 * Ӳ�����ӣ� ------------------------------------
 *           |PA6-SPI1-MISO��MAX6675-SO          |
 *           |PA7-SPI1-MOSI��MAX6675-SI          |
 *           |PA5-SPI1-SCK ��MAX6675-SCK         |
 *           |PA4-SPI1-NSS ��MAX6675-CS          |
 *            ------------------------------------
**********************************************************************************/
#include "max6675.h"
#include "usart.h"
/*
 * ��������SPI1_Init
 * ����  �MMAX6675 �ӿڳ�ʼ��
 * ����  ���� 
 * ���  ����
 * ����  ����
 */																						  
void SPI_MAX6675_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;	
	
	/* ʹ�� SPI1 ʱ�� */                         
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	/* ---------ͨ��I/O��ʼ��----------------
	 * PA5-SPI1-SCK :MAX6675_SCK
	 * PA6-SPI1-MISO:MAX6675_SO
	 * PA7-SPI1-MOSI:MAX6675_SI	 
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ---------����I/O��ʼ��----------------*/
	/* PA4-SPI1-NSS:MAX6675_CS */							// Ƭѡ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);						  
	GPIO_SetBits(GPIOA, GPIO_Pin_4);						// ����CSN
 
	SPI_Cmd(SPI1, DISABLE); // SPI���費ʹ�ܣ��ȹر�SPI���ٽ�������
	/* SPI1 ���� */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	
	/* ʹ�� SPI1  */
	SPI_Cmd(SPI1, ENABLE); 
//	GPIO_SetBits(GPIOA, GPIO_Pin_4);//���߿�ʼʹ��
}

/*
 *
 *
 *
 */
u8 MAX6675_ReadByte(void)
{
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, 0xff); //ͨ������SPIx����һ�����ݣ���������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
}
/*
D15ʼ������
D14~ D3��Ӧ���ȵ�żģ�������ѹ������ת����
D2���ڼ���ȵ�ż�Ƿ����(D2Ϊ1�����ȵ�ż�Ͽ�) 
D1ΪMAX6675�ı�ʶ��
D0Ϊ��̬��
*/
float getMax6675_Data(void)
{
	u16 t,tempData;
	u8 c,flag;
	float temprature;
	MAX6675_CS = 0;//����CS��ʹSO��ʼ�������
	c = MAX6675_ReadByte();//��ȡ��8λ
	tempData = c;
//	printf("��8λ:%x\r\n",c);
	tempData = tempData<<8;//����8λ��������8λ
	c = MAX6675_ReadByte();//��ȡ��8λ
//	printf("��8λ:%x\r\n",c);
	MAX6675_CS = 1;//����CS��ʹSOֹͣ������ݣ�MAX6675������һ��ת��
	
	tempData = tempData|c;//����8λ�ϲ���tempData
	flag = tempData&0x0004;	//�õ�D2���ݣ�flag�������ȵ�ż������״̬
	printf("flag:%d ",flag);
	t = tempData<<1;		//����һλ��ɾ����D15
	t = t>>4;//ɾ����D0-2
	temprature = t*0.25;
	printf("ԭʼ����:%d ",t);
	printf("�¶ȣ�%4.2f\r\n",temprature);
	if(tempData!=0)							//max6675�����ݷ���
	{
		if(flag==0)//�ȵ�ż������
		{
			return temprature;//���ض�������ԭʼ����
		}	
		else							
		{
			return MAX6675Left;//�ȵ�ż����
		}
	
	}
	else								//max6675û�����ݷ���
	{
		return MAX6675Error;
	}
}
