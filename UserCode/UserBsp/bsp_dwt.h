/**
 ******************************************************************************
 * @file	bsp_dwt.h
 * @author  Wang Hongxi
 * @version V1.1.0
 * @date    2022/3/8
 * @brief
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */
#ifndef _BSP_DWT_H
#define _BSP_DWT_H


#include "stdint.h"

/**
 * @brief DWT时间结构体
 */
typedef struct
{
    uint32_t s;     ///< 秒
    uint16_t ms;    ///< 毫秒
    uint16_t us;    ///< 微秒
} DWT_Time_t;

/**
 * @brief       初始化DWT
 * @param       CPU_Freq_mHz CPU频率(MHz)
 * @return      无
 */
void DWT_Init(uint32_t CPU_Freq_mHz);

/**
 * @brief       获取两次调用之间的增量时间(float)
 * @param       cnt_last 上次记录的计数器值的指针
 * @return      增量时间(秒)
 */
float DWT_GetDeltaT(uint32_t *cnt_last);

/**
 * @brief       获取两次调用之间的增量时间(double)
 * @param       cnt_last 上次记录的计数器值的指针
 * @return      增量时间(秒)
 */
double DWT_GetDeltaT64(uint32_t *cnt_last);

/**
 * @brief       获取系统运行时间(秒)
 * @param       无
 * @return      系统运行时间(秒)
 */
float DWT_GetTimeline_s(void);

/**
 * @brief       获取系统运行时间(毫秒)
 * @param       无
 * @return      系统运行时间(毫秒)
 */
float DWT_GetTimeline_ms(void);

/**
 * @brief       获取系统运行时间(微秒)
 * @param       无
 * @return      系统运行时间(微秒)
 */
uint64_t DWT_GetTimeline_us(void);

/**
 * @brief       DWT延时函数
 * @param       Delay 延时时间(秒)
 * @return      无
 */
void DWT_Delay(float Delay);

/**
 * @brief       更新系统时间
 * @param       无
 * @return      无
 */
void DWT_SysTimeUpdate(void);

extern DWT_Time_t SysTime;

#endif /* BSP_DWT_H_ */
