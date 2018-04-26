//	�ж�������ʵ��
//	PC0����CO������MQ-9
//	PC1���Ӽ�ȩCH20������CJMCU-1100
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_adc.h"
#include "system.h"
#include "adx345.h"
#include "math.h"
#include "oled.h"
#include "sys.h"
#include "delay.h"
#include <stdio.h>

//�������ݴ���
#include  <math.h>
// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
float ADC_ConvertedValueLocal[NOFCHANEL];        

// �����ʱ
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

//��ʾ������
void display()
{
		//��һ�У�x,y��
		OLED_Clear();
		OLED_ShowCHinese(0,2,0);//��
		OLED_ShowCHinese(16,2,1);//��
		OLED_ShowCHinese(32,2,2);//��
		OLED_ShowCHinese(48,2,3);//��
		OLED_ShowCHinese(64,2,4);//��
		OLED_ShowCHinese(80,2,5);//��
		OLED_ShowCHinese(96,2,6);//��
		//�ڶ���(x,y)
		OLED_ShowCHinese(32,4,7);//��
		OLED_ShowCHinese(48,4,8);//��
		OLED_ShowCHinese(64,4,9);//��
		

		delay_ms(80000);
		OLED_Clear();
}


//��ʱ����
void Delay_ms( uint16_t time_ms )
{
  uint16_t i,j;
  for( i=0;i<time_ms;i++ )
  {
		for( j=0;j<10309;j++ );//��Լ1ms
  }
}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	//��������ת����Ҫ�ı���
	float data_CO = 0.0;
	float data_CH20 = 0.0;
	//��ȩ���ݵĸ�λ��
	uint16_t		dataTemp_CH20=0;
	char			data_CH20_bit[4]={0,0,0,0};
	uint16_t		dataTemp_CO=0;
	uint8_t			data_CO_bit[4]={0,0,0,0};
	//��������
	int k = 0;
	uint8_t			sendData[10]={0,0,0,0,0,0,0,0,0,0};
	
	/* ��ʼ�� */ 
	SYSTICK_Config();

	//��ʱ������ʼ��
	delay_init();	    		  
	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	NVIC_Configuration();
	//��ʼ��OLED   	
	OLED_Init();	
	//��ʾ��λ��ʼ�������	
	display();	
	//����
	OLED_Clear();	
	
	//��ʾCOŨ�Ⱥͼ�ȩ

	OLED_ShowChar(16,2,'C',16);//C
	OLED_ShowChar(24,2,'O',16);//O
	OLED_ShowChar(32,2,':',16);//:
	OLED_ShowCHinese(0,4,10);//��
	OLED_ShowCHinese(16,4,11);//ȩ
	OLED_ShowChar(32,4,':',16);//
	
	
	// ���ô���
	USART_Config();
	
	// ADC ��ʼ��
	ADCx_Init();
	
	
	while (1)
	{	
    //ADC1ͨ��10��PC0��,CO�ĵ�ѹֵ����
		ADC_ConvertedValueLocal[0] =(float) ADC_ConvertedValue[0]/4096*5.0;
//���ݴ���,����ѹֵ����ת��ΪŨ������
		//����CO����
//		sprintf(data_CO_bit,"%.2f",ADC_ConvertedValueLocal[0]);
//		OLED_ShowString(40,2,data_CO_bit,16);
		data_CO = ADC_ConvertedValueLocal[0]*1.104 - 1.478;
		data_CO =  pow(10, data_CO) * 100;
		dataTemp_CO = (int) data_CO;
		data_CO_bit[0]  = dataTemp_CO/1000;				//ǧλ��
		data_CO_bit[1]  = dataTemp_CO/100%10;		//��λ��
		data_CO_bit[2]  = dataTemp_CO/10%10;		//ʮλ��
		data_CO_bit[3]  = dataTemp_CO%10; 		//��λ��		
			//��ʾ2������
		//x,y :�������	 
		//len :���ֵ�λ��
		//size:�����С
		//mode:ģʽ	0,���ģʽ;1,����ģʽ
		//num:��ֵ(0~4294967295);
		
		//��ʾCOת������		
		OLED_ShowNum(40,2,data_CO_bit[0],1,16);
		OLED_ShowNum(48,2,data_CO_bit[1],1,16);
		OLED_ShowChar(56,2,'.',16);
		OLED_ShowNum(64,2,data_CO_bit[2],1,16);
		OLED_ShowNum(72,2,data_CO_bit[3],1,16);
		OLED_ShowChar(80,2,'p',16);
		OLED_ShowChar(88,2,'p',16);
		OLED_ShowChar(96,2,'m',16);
		
		
		//ADC1ͨ��11��PC1������ȩ�ĵ�ѹֵ����
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1]/4096*5.0;
		
		//����CH20����
		data_CH20 = ADC_ConvertedValueLocal[1]*0.627 - 1.095;
		data_CH20 =  pow(10, data_CH20) * 100;
		dataTemp_CH20 = (int) data_CH20;
		data_CH20_bit[0]   = dataTemp_CH20/1000;				//ǧλ��
		data_CH20_bit[1]   = dataTemp_CH20/100%10;		//��λ��
		data_CH20_bit[2]   = dataTemp_CH20/10%10;		//ʮλ��
		data_CH20_bit[3]   = dataTemp_CH20%10; 		//��λ��
		
		//��ʾ��ȩת������	
		OLED_ShowNum(40,4,data_CH20_bit[0],1,16);
		OLED_ShowNum(48,4,data_CH20_bit[1],1,16);
		OLED_ShowChar(56,4,'.',16);
		OLED_ShowNum(64,4,data_CH20_bit[2],1,16);
		OLED_ShowNum(72,4,data_CH20_bit[3],1,16);
		OLED_ShowChar(80,4,'p',16);
		OLED_ShowChar(88,4,'p',16);
		OLED_ShowChar(96,4,'m',16);
		
		sendData[0] = 'C';
		sendData[1] = data_CO_bit[0];
		sendData[2] = data_CO_bit[1];
		sendData[3] = data_CO_bit[2];
		sendData[4] = data_CO_bit[3];
		sendData[5] = 'Q';
		sendData[6] = data_CH20_bit[0];
		sendData[7] = data_CH20_bit[1];
		sendData[8] = data_CH20_bit[2];
		sendData[9] = data_CH20_bit[3];
		
		//��������
		for(k=0;k<10;k++)
			{
				 USART_SendData(USART1,sendData[k]);//ͨ������USARTx���͵�������
				 while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);	
			}

		Delay(0xffffe);	
	}
	
		
		
		


		
		

				 
}
/*********************************************END OF FILE**********************/



