#include "board.h"

void board_init()
{
     NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
     USART1_Init_Config(115200);
     GPIO_Config();

}



