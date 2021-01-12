#include "bsp_gpio.h"   

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����LED��ص�GPIO����ʱ��*/
		RCC_APB2PeriphClockCmd( LED1_GPIO_CLK|LED2_GPIO_CLK|KEY_GPIO_CLK, ENABLE);
		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;	
		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	

		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;	
		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

				/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN|KEY2_GPIO_PIN;	
		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);

	    GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);
		GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);
		
}
void led1_off(void)
{

  GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);

}

void led1_on(void)
{
 GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);
}


void led2_off(void)
{

  GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);

}

void led2_on(void)
{
 GPIO_ResetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);
}
/*********************************************END OF FILE**********************/




