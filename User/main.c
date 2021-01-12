/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************

  */
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#if configSUPPORT_STATIC_ALLOCATION
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
/* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle;
/* LED任务句柄 */
static TaskHandle_t LED_Task_Handle;

/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
/* AppTaskCreate任务任务堆栈 */
static StackType_t AppTaskCreate_Stack[128];
/* LED任务堆栈 */
static StackType_t LED_Task_Stack[128];

/* AppTaskCreate 任务控制块 */
static StaticTask_t AppTaskCreate_TCB;
/* LED 任务控制块 */
static StaticTask_t LED_Task_TCB;

/* 空闲任务任务堆栈 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* 定时器任务堆栈 */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* 空闲任务控制块 */
static StaticTask_t Idle_Task_TCB;
/* 定时器任务控制块 */
static StaticTask_t Timer_Task_TCB;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void); /* 用于创建任务 */

static void LED_Task(void *pvParameters); /* LED_Task任务实现 */

/**
	* 使用了静态分配内存，以下这两个函数是由用户实现，函数在task.c文件中有引用
	*	当且仅当 configSUPPORT_STATIC_ALLOCATION 这个宏定义为 1 的时候才有效
	*/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize);

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize);

int main(void)
{

  board_init(); //function initialization

  /* 创建 AppTaskCreate 任务 */
  AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t)AppTaskCreate,       //任务函数
                                           (const char *)"AppTaskCreate",       //任务名称
                                           (uint32_t)128,                       //任务堆栈大小
                                           (void *)NULL,                        //传递给任务函数的参数
                                           (UBaseType_t)3,                      //任务优先级
                                           (StackType_t *)AppTaskCreate_Stack,  //任务堆栈
                                           (StaticTask_t *)&AppTaskCreate_TCB); //任务控制块

  if (NULL != AppTaskCreate_Handle) /* 创建成功 */
    vTaskStartScheduler();          /* 启动任务，开启调度 */

  // while(1);   /* 正常不会执行到这里 */
}

/**
  **********************************************************************
  * @brief  获取空闲任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小
  **********************************************************************
  */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &Idle_Task_TCB;           /* 任务控制块内存 */
  *ppxIdleTaskStackBuffer = Idle_Task_Stack;        /* 任务堆栈内存 */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE; /* 任务堆栈大小 */
}
/**
  *********************************************************************
  * @brief  获取定时器任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小
  **********************************************************************
  */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer = &Timer_Task_TCB;              /* 任务控制块内存 */
  *ppxTimerTaskStackBuffer = Timer_Task_Stack;           /* 任务堆栈内存 */
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH; /*任务堆栈大小*/
}
/**
  *********************************************************************
  * @brief  获取定时器任务的任务堆栈和任务控制块内存
	*					ppxTimerTaskTCBBuffer	:		任务控制块内存
	*					ppxTimerTaskStackBuffer	:	任务堆栈内存
	*					pulTimerTaskStackSize	:		任务堆栈大小

  **********************************************************************
  */
static void AppTaskCreate(void)
{
  taskENTER_CRITICAL(); //进入临界区
  /* 创建LED_Task 任务*/
  LED_Task_Handle = xTaskCreateStatic((TaskFunction_t)LED_Task,       //任务函数
                                      (const char *)"LED_Task",       //任务名称
                                      (uint32_t)128,                  //任务堆栈大小
                                      (void *)NULL,                   //传递给任务函数的参数
                                      (UBaseType_t)4,                 //优先级
                                      (StackType_t *)LED_Task_Stack,  //任务堆栈
                                      (StaticTask_t *)&LED_Task_TCB); //任务控制块

  if (NULL != LED_Task_Handle)

    printf("LED创建成功\r\n");
  else
    printf("LED创建失败\r\n");
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate

  taskEXIT_CRITICAL(); //退出临界区
}
static void LED_Task(void *parameter)
{

  while (1)
  {
    led_on();
    vTaskDelay(700);
    printf("led on\r\n");
    led_off();
    vTaskDelay(700);
    printf("led off\r\n");
  }
}
#endif

typedef struct
{
  u8 data1;
  u8 data2;
  u8 data3;
} _Data1;

typedef struct
{
  u8 data4;
  u8 data5;
  u8 data6;
} _Data2;

typedef struct
{
  u8 receive_data1;
  u8 receive_data2;
  u8 receive_data3;

} _r_queue;

#define QUEUE_LEN 2
#define QUEUE_SIZE sizeof(uint8_t) //所传数据的字节大小  结构体注意字节对齐

QueueHandle_t Test_Queue = NULL; //创建队列句柄

static TaskHandle_t AppTaskCreate_Handle = NULL; //创建任务句柄
// static TaskHandle_t LED1_TASK_Handle = NULL;     //创建LED1任务句柄
// static TaskHandle_t LED2_TASK_Handle = NULL;     //创建LED2任务句柄
// static TaskHandle_t KEY_TASK_Handle = NULL;      //创建KEY任务句柄
static TaskHandle_t Receive_Task_Handle = NULL;
static TaskHandle_t Send_Task_Handle = NULL;
/*****************************************函数声明*********************************************/
static void AppTaskCreate(void); //用于创建任务
// static void LED1_Task(void *pvParameters); //LED1_Task 任务实现
// static void LED2_Task(void *pvParameters); //LED2_Task 任务实现
// static void KEY_Task(void *pvParameters);  //KEY_Task 任务实现
static void Receive_Task(void *pvParameters);
static void Send_Task(void *pvParameters);

int main(void)
{
  BaseType_t xReturn = pdPASS;
  board_init();

  printf("按下KEY1或者KEY2发送队列消息\r\n");
  printf("Receive任务接收到消息在串口回显\r\n");

  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,          //任务入口函数
                        (const char *)"AppTaskCreate",          //任务名字
                        (uint16_t)512,                          //任务栈大小
                        (void *)NULL,                           //任务入口函数参数
                        (UBaseType_t)1,                         //任务优先级
                        (TaskHandle_t *)&AppTaskCreate_Handle); //任务控制块指针
  if (pdPASS == xReturn)
    vTaskStartScheduler(); //启动任务，开启调度
  else
    return -1;
}

static void AppTaskCreate(void)//创建任务
{
  BaseType_t xReturn = pdPASS;

  taskENTER_CRITICAL(); //进入临界区

  Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,
                            (UBaseType_t)QUEUE_SIZE);
  if (NULL != Test_Queue)
    printf("创建Test_Queue消息队列成功！\r\n");

  xReturn = xTaskCreate((TaskFunction_t)Receive_Task,          //任务入口函数
                        (const char *)"Receive_Task",          //任务名字
                        (uint16_t)512,                         //任务栈大小
                        (void *)NULL,                          //任务入口函数参数
                        (UBaseType_t)2,                        //任务优先级
                        (TaskHandle_t *)&Receive_Task_Handle); //任务控制块指针
  if (pdPASS == xReturn)
    printf("创建LED1_Task任务成功\r\n");

  xReturn = xTaskCreate((TaskFunction_t)Send_Task,          //任务入口函数
                        (const char *)"Send_Task",          //任务名字
                        (uint16_t)512,                      //任务栈大小
                        (void *)NULL,                       //任务入口函数参数
                        (UBaseType_t)3,                     //任务优先级
                        (TaskHandle_t *)&Send_Task_Handle); //任务控制块指针
  if (pdPASS == xReturn)
    printf("创建LED2_Task任务成功\r\n");

  // xReturn = xTaskCreate((TaskFunction_t)LED1_Task,          //任务入口函数
  //                       (const char *)"LED1_Task",          //任务名字
  //                       (uint16_t)512,                      //任务栈大小
  //                       (void *)NULL,                       //任务入口函数参数
  //                       (UBaseType_t)2,                     //任务优先级
  //                       (TaskHandle_t *)&LED1_TASK_Handle); //任务控制块指针
  // if (pdPASS == xReturn)
  //   printf("创建LED1_Task任务成功\r\n");

  // xReturn = xTaskCreate((TaskFunction_t)LED2_Task,          //任务入口函数
  //                       (const char *)"LED2_Task",          //任务名字
  //                       (uint16_t)512,                      //任务栈大小
  //                       (void *)NULL,                       //任务入口函数参数
  //                       (UBaseType_t)3,                     //任务优先级
  //                       (TaskHandle_t *)&LED2_TASK_Handle); //任务控制块指针
  // if (pdPASS == xReturn)
  //   printf("创建LED2_Task任务成功\r\n");

  // xReturn = xTaskCreate((TaskFunction_t)KEY_Task,          //任务入口函数
  //                       (const char *)"KEY_Task",          //任务名字
  //                       (uint16_t)512,                     //任务栈大小
  //                       (void *)NULL,                      //任务入口函数参数
  //                       (UBaseType_t)4,                    //任务优先级
  //                       (TaskHandle_t *)&KEY_TASK_Handle); //任务控制块指针
  // if (pdPASS == xReturn)
  //   printf("创建KEY_Task任务成功\r\n");

  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate 任务
  taskEXIT_CRITICAL();               //退出临界区
}
static void Receive_Task(void *pvParameters)
{
  _r_queue r_queue;
  BaseType_t xReturn = pdTRUE;
  //uint32_t r_queue;

  while (1)
  {
    xReturn = xQueueReceive(Test_Queue,
                            &r_queue,
                            portMAX_DELAY);
    if (pdTRUE == xReturn)
    {
      printf("队列收到的数据：\r\n");
      printf("%d\r\n", r_queue.receive_data1);
      printf("%d\r\n", r_queue.receive_data2);
      printf("%d\r\n", r_queue.receive_data3);
      // printf("%d\r\n", r_queue.receive_data4);
      // printf("%d\r\n", r_queue.receive_data5);
      // printf("%d\r\n", r_queue.receive_data6);
    }
    else
      printf("数据接收错误，错误代码 ：0x%lx\r\n", xReturn);
  }
}
static void Send_Task(void *pvParameters)
{

  // _Data Data_;
  BaseType_t xReturn = pdTRUE;
  //uint32_t Send_Data2 = 2;
  // Data_.data1 = 1;
  // Data_.data2 = 2;
  // Data_.data3 = 3;
  _Data1 Data_1 = {
      1,
      2,
      3};
  _Data2 Data_2 = {
      5,
      5,
      7};

  // uint32_t Send_Data1 = 1;

  while (1)
  {
    if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY1_GPIO_PIN) == 0) //按键1按下发送数据到队列
    {
      printf("发送消息Send_Data1!\r\n");
      xReturn = xQueueSend(Test_Queue,
                           &Data_1,
                           0);
      if (pdPASS == xReturn)
        printf("发送消息Send_Data1成功!\r\n");

      printf("发送消息Send_Data2!\r\n");
      xReturn = xQueueSendToFront(Test_Queue,
                                  &Data_2,
                                  0);

      if (pdPASS == xReturn)
        printf("发送消息Send_Data2成功!\r\n");
    }

    // {
    //   printf("发送消息Send_Data2!\r\n");
    //   xReturn = xQueueSend(Test_Queue,
    //                        &Data_2,
    //                        0);
    //   if (pdPASS == xReturn)
    //     printf("发送消息Send_Data2成功!\r\n");
    // }
    vTaskDelay(200);
  }
}
// static void LED1_Task(void *pvParameters) //LED1任务
// {
//   while (1)
//   {
//     led1_on();
//     vTaskDelay(700);
//     //printf("LED1 ON!\r\n");

//     led1_off();
//     vTaskDelay(700);
//     //printf("LED1 OFF!\r\n");
//   }
// }
// static void LED2_Task(void *pvParameters) //LED2任务
// {
//   while (1)
//   {
//     led2_on();
//     vTaskDelay(200);
//     //printf("LED2 ON!\r\n");

//     led2_off();
//     vTaskDelay(200);
//     //printf("LED2 OFF!\r\n");
//   }
// }
// static void KEY_Task(void *pvParameters) //KEY任务
// {
//   while (1)
//   {
//     if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY1_GPIO_PIN) == 0) //按键按下
//     {
//       printf("挂起LED1任务！\r\n");
//       vTaskSuspend(LED1_TASK_Handle); //挂起LED1任务
//       //vTaskSuspendAll();
//       //vTaskDelete(LED1_TASK_Handle);
//       vTaskDelay(200);
//     }
//     if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY2_GPIO_PIN) == 0) //按键按下
//     {
//       printf("恢复LED1任务！\r\n");
//       vTaskResume(LED1_TASK_Handle); //挂起LED1任务
//       //xTaskResumeAll();
//       //vTaskDelete(LED2_TASK_Handle);
//       vTaskDelay(200);
//     }
//     vTaskDelay(200); //20ms
//   }
// }
/*********************************************END OF FILE**********************/
