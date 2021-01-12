#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H
#include "stm32f10x.h"

#define LED1_GPIO_PORT    	GPIOA			              /* GPIO�˿� */
#define LED1_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO�˿�ʱ�� */
#define LED1_GPIO_PIN		GPIO_Pin_8			        /* ���ӵ�SCLʱ���ߵ�GPIO */


#define LED2_GPIO_PORT    	GPIOD			              /* GPIO�˿� */
#define LED2_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO�˿�ʱ�� */
#define LED2_GPIO_PIN		GPIO_Pin_2			        /* ���ӵ�SCLʱ���ߵ�GPIO */

#define KEY_GPIO_PORT    	GPIOC			              /* GPIO�˿� */
#define KEY_GPIO_CLK        RCC_APB2Periph_GPIOC
#define KEY1_GPIO_PIN		GPIO_Pin_1			        /* ���ӵ�SCLʱ���ߵ�GPIO */
#define KEY2_GPIO_PIN		GPIO_Pin_13			        /* ���ӵ�SCLʱ���ߵ�GPIO */

void led1_off(void);
void led1_on(void);

void led2_off(void);
void led2_on(void);
void GPIO_Config(void); 
#endif
