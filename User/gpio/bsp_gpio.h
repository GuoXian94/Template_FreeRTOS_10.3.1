#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H
#include "stm32f10x.h"

#define LED1_GPIO_PORT    	GPIOA			              /* GPIO端口 */
#define LED1_GPIO_CLK 	    RCC_APB2Periph_GPIOA		/* GPIO端口时钟 */
#define LED1_GPIO_PIN		GPIO_Pin_8			        /* 连接到SCL时钟线的GPIO */


#define LED2_GPIO_PORT    	GPIOD			              /* GPIO端口 */
#define LED2_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define LED2_GPIO_PIN		GPIO_Pin_2			        /* 连接到SCL时钟线的GPIO */

#define KEY_GPIO_PORT    	GPIOC			              /* GPIO端口 */
#define KEY_GPIO_CLK        RCC_APB2Periph_GPIOC
#define KEY1_GPIO_PIN		GPIO_Pin_1			        /* 连接到SCL时钟线的GPIO */
#define KEY2_GPIO_PIN		GPIO_Pin_13			        /* 连接到SCL时钟线的GPIO */

void led1_off(void);
void led1_on(void);

void led2_off(void);
void led2_on(void);
void GPIO_Config(void); 
#endif
