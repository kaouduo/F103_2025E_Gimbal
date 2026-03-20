/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "algorithm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[256];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
        .name = "defaultTask",
        .cb_mem = &defaultTaskControlBlock,
        .cb_size = sizeof(defaultTaskControlBlock),
        .stack_mem = &defaultTaskBuffer[0],
        .stack_size = sizeof(defaultTaskBuffer),
        .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for INSTask_rtos */
osThreadId_t INSTask_rtosHandle;
uint32_t INSTask_rtosBuffer[256];
osStaticThreadDef_t INSTask_rtosControlBlock;
const osThreadAttr_t INSTask_rtos_attributes = {
        .name = "INSTask_rtos",
        .cb_mem = &INSTask_rtosControlBlock,
        .cb_size = sizeof(INSTask_rtosControlBlock),
        .stack_mem = &INSTask_rtosBuffer[0],
        .stack_size = sizeof(INSTask_rtosBuffer),
        .priority = (osPriority_t)osPriorityRealtime,
};
/* Definitions for GIMBALTask_rtos */
osThreadId_t GIMBALTask_rtosHandle;
uint32_t GIMBALTask_rtosBuffer[256];
osStaticThreadDef_t GIMBALTask_rtosControlBlock;
const osThreadAttr_t GIMBALTask_rtos_attributes = {
        .name = "GIMBALTask_rtos",
        .cb_mem = &GIMBALTask_rtosControlBlock,
        .cb_size = sizeof(GIMBALTask_rtosControlBlock),
        .stack_mem = &GIMBALTask_rtosBuffer[0],
        .stack_size = sizeof(GIMBALTask_rtosBuffer),
        .priority = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void IMU_task(void *argument);
void GIMBAL_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of INSTask_rtos */
    INSTask_rtosHandle = osThreadNew(IMU_task, NULL, &INSTask_rtos_attributes);

    /* creation of GIMBALTask_rtos */
    GIMBALTask_rtosHandle = osThreadNew(GIMBAL_Task, NULL, &GIMBALTask_rtos_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;)
    {
        vofa_start();
        osDelay(1);
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_IMU_task */
/**
 * @brief Function implementing the INSTask_rtos thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_IMU_task */
__weak void IMU_task(void *argument)
{
    /* USER CODE BEGIN IMU_task */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END IMU_task */
}

/* USER CODE BEGIN Header_GIMBAL_Task */
/**
 * @brief Function implementing the GIMBALTask_rtos thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_GIMBAL_Task */
__weak void GIMBAL_Task(void *argument)
{
    /* USER CODE BEGIN GIMBAL_Task */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END GIMBAL_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
