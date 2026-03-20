/**
 * @file bsp_usart.c
 * @author neozng
 * @brief  串口bsp层的实现
 * @version beta
 * @date 2022-11-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "bsp_usart.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/**
 * @brief       自定义串口printf
 * @param       huart     串口句柄指针
 * @param       format    格式化字符串
 * @param       ...       可变参数
 * @return      发送的数据长度
 */
int Uart_Printf(UART_HandleTypeDef *huart, const char *format, ...)
{
    char buffer[512];
    va_list arg;
    int len;
    // 初始化可变参数列表
    va_start(arg, format);
    len = vsnprintf(buffer, sizeof(buffer), format, arg);
    va_end(arg);
    HAL_UART_Transmit(huart, (uint8_t *) buffer, (uint16_t) len, 0xFF);
    return len;
}

