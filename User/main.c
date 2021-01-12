/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************

  */
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#if configSUPPORT_STATIC_ALLOCATION
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
/* ���������� */
static TaskHandle_t AppTaskCreate_Handle;
/* LED������ */
static TaskHandle_t LED_Task_Handle;

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
/* AppTaskCreate���������ջ */
static StackType_t AppTaskCreate_Stack[128];
/* LED�����ջ */
static StackType_t LED_Task_Stack[128];

/* AppTaskCreate ������ƿ� */
static StaticTask_t AppTaskCreate_TCB;
/* LED ������ƿ� */
static StaticTask_t LED_Task_TCB;

/* �������������ջ */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* ��ʱ�������ջ */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* ����������ƿ� */
static StaticTask_t Idle_Task_TCB;
/* ��ʱ��������ƿ� */
static StaticTask_t Timer_Task_TCB;

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void); /* ���ڴ������� */

static void LED_Task(void *pvParameters); /* LED_Task����ʵ�� */

/**
	* ʹ���˾�̬�����ڴ棬�������������������û�ʵ�֣�������task.c�ļ���������
	*	���ҽ��� configSUPPORT_STATIC_ALLOCATION ����궨��Ϊ 1 ��ʱ�����Ч
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

  /* ���� AppTaskCreate ���� */
  AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t)AppTaskCreate,       //������
                                           (const char *)"AppTaskCreate",       //��������
                                           (uint32_t)128,                       //�����ջ��С
                                           (void *)NULL,                        //���ݸ��������Ĳ���
                                           (UBaseType_t)3,                      //�������ȼ�
                                           (StackType_t *)AppTaskCreate_Stack,  //�����ջ
                                           (StaticTask_t *)&AppTaskCreate_TCB); //������ƿ�

  if (NULL != AppTaskCreate_Handle) /* �����ɹ� */
    vTaskStartScheduler();          /* �������񣬿������� */

  // while(1);   /* ��������ִ�е����� */
}

/**
  **********************************************************************
  * @brief  ��ȡ��������������ջ��������ƿ��ڴ�
	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
	*					pulTimerTaskStackSize	:		�����ջ��С
  **********************************************************************
  */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &Idle_Task_TCB;           /* ������ƿ��ڴ� */
  *ppxIdleTaskStackBuffer = Idle_Task_Stack;        /* �����ջ�ڴ� */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE; /* �����ջ��С */
}
/**
  *********************************************************************
  * @brief  ��ȡ��ʱ������������ջ��������ƿ��ڴ�
	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
	*					pulTimerTaskStackSize	:		�����ջ��С
  **********************************************************************
  */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer = &Timer_Task_TCB;              /* ������ƿ��ڴ� */
  *ppxTimerTaskStackBuffer = Timer_Task_Stack;           /* �����ջ�ڴ� */
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH; /*�����ջ��С*/
}
/**
  *********************************************************************
  * @brief  ��ȡ��ʱ������������ջ��������ƿ��ڴ�
	*					ppxTimerTaskTCBBuffer	:		������ƿ��ڴ�
	*					ppxTimerTaskStackBuffer	:	�����ջ�ڴ�
	*					pulTimerTaskStackSize	:		�����ջ��С

  **********************************************************************
  */
static void AppTaskCreate(void)
{
  taskENTER_CRITICAL(); //�����ٽ���
  /* ����LED_Task ����*/
  LED_Task_Handle = xTaskCreateStatic((TaskFunction_t)LED_Task,       //������
                                      (const char *)"LED_Task",       //��������
                                      (uint32_t)128,                  //�����ջ��С
                                      (void *)NULL,                   //���ݸ��������Ĳ���
                                      (UBaseType_t)4,                 //���ȼ�
                                      (StackType_t *)LED_Task_Stack,  //�����ջ
                                      (StaticTask_t *)&LED_Task_TCB); //������ƿ�

  if (NULL != LED_Task_Handle)

    printf("LED�����ɹ�\r\n");
  else
    printf("LED����ʧ��\r\n");
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate

  taskEXIT_CRITICAL(); //�˳��ٽ���
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
#define QUEUE_SIZE sizeof(uint8_t) //�������ݵ��ֽڴ�С  �ṹ��ע���ֽڶ���

QueueHandle_t Test_Queue = NULL; //�������о��

static TaskHandle_t AppTaskCreate_Handle = NULL; //����������
// static TaskHandle_t LED1_TASK_Handle = NULL;     //����LED1������
// static TaskHandle_t LED2_TASK_Handle = NULL;     //����LED2������
// static TaskHandle_t KEY_TASK_Handle = NULL;      //����KEY������
static TaskHandle_t Receive_Task_Handle = NULL;
static TaskHandle_t Send_Task_Handle = NULL;
/*****************************************��������*********************************************/
static void AppTaskCreate(void); //���ڴ�������
// static void LED1_Task(void *pvParameters); //LED1_Task ����ʵ��
// static void LED2_Task(void *pvParameters); //LED2_Task ����ʵ��
// static void KEY_Task(void *pvParameters);  //KEY_Task ����ʵ��
static void Receive_Task(void *pvParameters);
static void Send_Task(void *pvParameters);

int main(void)
{
  BaseType_t xReturn = pdPASS;
  board_init();

  printf("����KEY1����KEY2���Ͷ�����Ϣ\r\n");
  printf("Receive������յ���Ϣ�ڴ��ڻ���\r\n");

  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,          //������ں���
                        (const char *)"AppTaskCreate",          //��������
                        (uint16_t)512,                          //����ջ��С
                        (void *)NULL,                           //������ں�������
                        (UBaseType_t)1,                         //�������ȼ�
                        (TaskHandle_t *)&AppTaskCreate_Handle); //������ƿ�ָ��
  if (pdPASS == xReturn)
    vTaskStartScheduler(); //�������񣬿�������
  else
    return -1;
}

static void AppTaskCreate(void)//��������
{
  BaseType_t xReturn = pdPASS;

  taskENTER_CRITICAL(); //�����ٽ���

  Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,
                            (UBaseType_t)QUEUE_SIZE);
  if (NULL != Test_Queue)
    printf("����Test_Queue��Ϣ���гɹ���\r\n");

  xReturn = xTaskCreate((TaskFunction_t)Receive_Task,          //������ں���
                        (const char *)"Receive_Task",          //��������
                        (uint16_t)512,                         //����ջ��С
                        (void *)NULL,                          //������ں�������
                        (UBaseType_t)2,                        //�������ȼ�
                        (TaskHandle_t *)&Receive_Task_Handle); //������ƿ�ָ��
  if (pdPASS == xReturn)
    printf("����LED1_Task����ɹ�\r\n");

  xReturn = xTaskCreate((TaskFunction_t)Send_Task,          //������ں���
                        (const char *)"Send_Task",          //��������
                        (uint16_t)512,                      //����ջ��С
                        (void *)NULL,                       //������ں�������
                        (UBaseType_t)3,                     //�������ȼ�
                        (TaskHandle_t *)&Send_Task_Handle); //������ƿ�ָ��
  if (pdPASS == xReturn)
    printf("����LED2_Task����ɹ�\r\n");

  // xReturn = xTaskCreate((TaskFunction_t)LED1_Task,          //������ں���
  //                       (const char *)"LED1_Task",          //��������
  //                       (uint16_t)512,                      //����ջ��С
  //                       (void *)NULL,                       //������ں�������
  //                       (UBaseType_t)2,                     //�������ȼ�
  //                       (TaskHandle_t *)&LED1_TASK_Handle); //������ƿ�ָ��
  // if (pdPASS == xReturn)
  //   printf("����LED1_Task����ɹ�\r\n");

  // xReturn = xTaskCreate((TaskFunction_t)LED2_Task,          //������ں���
  //                       (const char *)"LED2_Task",          //��������
  //                       (uint16_t)512,                      //����ջ��С
  //                       (void *)NULL,                       //������ں�������
  //                       (UBaseType_t)3,                     //�������ȼ�
  //                       (TaskHandle_t *)&LED2_TASK_Handle); //������ƿ�ָ��
  // if (pdPASS == xReturn)
  //   printf("����LED2_Task����ɹ�\r\n");

  // xReturn = xTaskCreate((TaskFunction_t)KEY_Task,          //������ں���
  //                       (const char *)"KEY_Task",          //��������
  //                       (uint16_t)512,                     //����ջ��С
  //                       (void *)NULL,                      //������ں�������
  //                       (UBaseType_t)4,                    //�������ȼ�
  //                       (TaskHandle_t *)&KEY_TASK_Handle); //������ƿ�ָ��
  // if (pdPASS == xReturn)
  //   printf("����KEY_Task����ɹ�\r\n");

  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate ����
  taskEXIT_CRITICAL();               //�˳��ٽ���
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
      printf("�����յ������ݣ�\r\n");
      printf("%d\r\n", r_queue.receive_data1);
      printf("%d\r\n", r_queue.receive_data2);
      printf("%d\r\n", r_queue.receive_data3);
      // printf("%d\r\n", r_queue.receive_data4);
      // printf("%d\r\n", r_queue.receive_data5);
      // printf("%d\r\n", r_queue.receive_data6);
    }
    else
      printf("���ݽ��մ��󣬴������ ��0x%lx\r\n", xReturn);
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
    if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY1_GPIO_PIN) == 0) //����1���·������ݵ�����
    {
      printf("������ϢSend_Data1!\r\n");
      xReturn = xQueueSend(Test_Queue,
                           &Data_1,
                           0);
      if (pdPASS == xReturn)
        printf("������ϢSend_Data1�ɹ�!\r\n");

      printf("������ϢSend_Data2!\r\n");
      xReturn = xQueueSendToFront(Test_Queue,
                                  &Data_2,
                                  0);

      if (pdPASS == xReturn)
        printf("������ϢSend_Data2�ɹ�!\r\n");
    }

    // {
    //   printf("������ϢSend_Data2!\r\n");
    //   xReturn = xQueueSend(Test_Queue,
    //                        &Data_2,
    //                        0);
    //   if (pdPASS == xReturn)
    //     printf("������ϢSend_Data2�ɹ�!\r\n");
    // }
    vTaskDelay(200);
  }
}
// static void LED1_Task(void *pvParameters) //LED1����
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
// static void LED2_Task(void *pvParameters) //LED2����
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
// static void KEY_Task(void *pvParameters) //KEY����
// {
//   while (1)
//   {
//     if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY1_GPIO_PIN) == 0) //��������
//     {
//       printf("����LED1����\r\n");
//       vTaskSuspend(LED1_TASK_Handle); //����LED1����
//       //vTaskSuspendAll();
//       //vTaskDelete(LED1_TASK_Handle);
//       vTaskDelay(200);
//     }
//     if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY2_GPIO_PIN) == 0) //��������
//     {
//       printf("�ָ�LED1����\r\n");
//       vTaskResume(LED1_TASK_Handle); //����LED1����
//       //xTaskResumeAll();
//       //vTaskDelete(LED2_TASK_Handle);
//       vTaskDelay(200);
//     }
//     vTaskDelay(200); //20ms
//   }
// }
/*********************************************END OF FILE**********************/
