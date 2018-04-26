//	有毒气体检测实验
//	PC0连接CO传感器MQ-9
//	PC1连接甲醛CH20传感器CJMCU-1100
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

//用于数据处理
#include  <math.h>
// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal[NOFCHANEL];        

// 软件延时
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

//显示主界面
void display()
{
		//第一行（x,y）
		OLED_Clear();
		OLED_ShowCHinese(0,2,0);//有
		OLED_ShowCHinese(16,2,1);//毒
		OLED_ShowCHinese(32,2,2);//气
		OLED_ShowCHinese(48,2,3);//体
		OLED_ShowCHinese(64,2,4);//检
		OLED_ShowCHinese(80,2,5);//测
		OLED_ShowCHinese(96,2,6);//仪
		//第二行(x,y)
		OLED_ShowCHinese(32,4,7);//李
		OLED_ShowCHinese(48,4,8);//佳
		OLED_ShowCHinese(64,4,9);//慧
		

		delay_ms(80000);
		OLED_Clear();
}


//延时函数
void Delay_ms( uint16_t time_ms )
{
  uint16_t i,j;
  for( i=0;i<time_ms;i++ )
  {
		for( j=0;j<10309;j++ );//大约1ms
  }
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	//声明数据转化需要的变量
	float data_CO = 0.0;
	float data_CH20 = 0.0;
	//甲醛数据的各位数
	uint16_t		dataTemp_CH20=0;
	char			data_CH20_bit[4]={0,0,0,0};
	uint16_t		dataTemp_CO=0;
	uint8_t			data_CO_bit[4]={0,0,0,0};
	//发送数据
	int k = 0;
	uint8_t			sendData[10]={0,0,0,0,0,0,0,0,0,0};
	
	/* 初始化 */ 
	SYSTICK_Config();

	//延时函数初始化
	delay_init();	    		  
	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	NVIC_Configuration();
	//初始化OLED   	
	OLED_Init();	
	//显示复位初始程序界面	
	display();	
	//清屏
	OLED_Clear();	
	
	//显示CO浓度和甲醛

	OLED_ShowChar(16,2,'C',16);//C
	OLED_ShowChar(24,2,'O',16);//O
	OLED_ShowChar(32,2,':',16);//:
	OLED_ShowCHinese(0,4,10);//甲
	OLED_ShowCHinese(16,4,11);//醛
	OLED_ShowChar(32,4,':',16);//
	
	
	// 配置串口
	USART_Config();
	
	// ADC 初始化
	ADCx_Init();
	
	
	while (1)
	{	
    //ADC1通道10（PC0）,CO的电压值数据
		ADC_ConvertedValueLocal[0] =(float) ADC_ConvertedValue[0]/4096*5.0;
//数据处理,将电压值数据转换为浓度数据
		//处理CO数据
//		sprintf(data_CO_bit,"%.2f",ADC_ConvertedValueLocal[0]);
//		OLED_ShowString(40,2,data_CO_bit,16);
		data_CO = ADC_ConvertedValueLocal[0]*1.104 - 1.478;
		data_CO =  pow(10, data_CO) * 100;
		dataTemp_CO = (int) data_CO;
		data_CO_bit[0]  = dataTemp_CO/1000;				//千位数
		data_CO_bit[1]  = dataTemp_CO/100%10;		//百位数
		data_CO_bit[2]  = dataTemp_CO/10%10;		//十位数
		data_CO_bit[3]  = dataTemp_CO%10; 		//个位数		
			//显示2个数字
		//x,y :起点坐标	 
		//len :数字的位数
		//size:字体大小
		//mode:模式	0,填充模式;1,叠加模式
		//num:数值(0~4294967295);
		
		//显示CO转换数据		
		OLED_ShowNum(40,2,data_CO_bit[0],1,16);
		OLED_ShowNum(48,2,data_CO_bit[1],1,16);
		OLED_ShowChar(56,2,'.',16);
		OLED_ShowNum(64,2,data_CO_bit[2],1,16);
		OLED_ShowNum(72,2,data_CO_bit[3],1,16);
		OLED_ShowChar(80,2,'p',16);
		OLED_ShowChar(88,2,'p',16);
		OLED_ShowChar(96,2,'m',16);
		
		
		//ADC1通道11（PC1），甲醛的电压值数据
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1]/4096*5.0;
		
		//处理CH20数据
		data_CH20 = ADC_ConvertedValueLocal[1]*0.627 - 1.095;
		data_CH20 =  pow(10, data_CH20) * 100;
		dataTemp_CH20 = (int) data_CH20;
		data_CH20_bit[0]   = dataTemp_CH20/1000;				//千位数
		data_CH20_bit[1]   = dataTemp_CH20/100%10;		//百位数
		data_CH20_bit[2]   = dataTemp_CH20/10%10;		//十位数
		data_CH20_bit[3]   = dataTemp_CH20%10; 		//个位数
		
		//显示甲醛转换数据	
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
		
		//发送数据
		for(k=0;k<10;k++)
			{
				 USART_SendData(USART1,sendData[k]);//通过外设USARTx发送单个数据
				 while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==Bit_RESET);	
			}

		Delay(0xffffe);	
	}
	
		
		
		


		
		

				 
}
/*********************************************END OF FILE**********************/



