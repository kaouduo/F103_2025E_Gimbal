/**
 * @file      my_main.h
 * @brief     None
 * @details   None
 * @author    PJ_Hr
 * @version   V1.0
 * @date      2025/9/25
 * @copyright pjh
 * @email     2643560781@qq.com
 */

#ifndef _MY_MAIN_H
#define _MY_MAIN_H

#include "my_main_def.h"

/**
 * @brief 主初始化函数
 * @param 无
 * @return 无
 * */
void My_Main_Init(void);

/**
 * @brief 主任务轮询函数
 * @param 无
 * @return 无
 * */
void My_Main_Task(void);

/**
 * @brief 电机数据结构体
 * */
typedef struct
{
    float angle;
    float speed;
    uint16_t id;
} Motor_Data_t;

#endif //_MY_MAIN_H