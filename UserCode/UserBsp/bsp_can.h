#ifndef __BSP_FDCAN_H__
#define __BSP_FDCAN_H__
#include "main.h"
#include "can.h"

#define hcan_t CAN_HandleTypeDef

/**
 * @brief       初始化CAN总线
 * @param       无
 * @return      无
 */
void bsp_can_init(void);

/**
 * @brief       初始化CAN过滤器
 * @param       无
 * @return      无
 */
void can_filter_init(void);

/**
 * @brief       发送CAN数据
 * @param       hfdcan    CAN句柄指针
 * @param       id        目标设备ID
 * @param       data      要发送的数据指针
 * @param       len       发送的数据长度
 * @return      0表示成功，1表示失败
 */
uint8_t fdcanx_send_data(hcan_t *hfdcan, uint16_t id, uint8_t *data, uint32_t len);

/**
 * @brief       接收CAN数据
 * @param       hfdcan    CAN句柄指针
 * @param       rec_id    存放接收到的ID的指针
 * @param       buf       存放接收到的数据的缓冲区
 * @return      返回接收到的数据长度，0表示无数据
 */
uint8_t fdcanx_receive(hcan_t *hfdcan, uint16_t *rec_id, uint8_t *buf);

/**
 * @brief       CAN接收中断解析回调函数
 * @param       无
 * @return      无
 */
void fdcan1_rx_callback(void);
void fdcan2_rx_callback(void);
void fdcan3_rx_callback(void);

/**
 * @brief       发送2个电机控制指令（打包两个int16_t电压值）
 * @param       hfdcan         CAN句柄指针
 * @param       can_id         控制指令的CAN ID
 * @param       motor1_voltage 电机1的控制电压
 * @param       motor2_voltage 电机2的控制电压
 * @return      0表示成功，1表示失败
 */
uint8_t fdcan_send_motor_cmd(hcan_t *hfdcan, uint16_t can_id, int16_t motor1_voltage, int16_t motor2_voltage);

/**
 * @brief       发送4个电机控制指令（一次性控制4个电机）
 * @param       hfdcan    CAN句柄指针
 * @param       can_id    控制指令的CAN ID
 * @param       voltages  包含4个电机控制电压的数组
 * @return      0表示成功，1表示失败
 */
uint8_t fdcan_send_motor_cmd_4(hcan_t *hfdcan, uint16_t can_id, int16_t voltages[4]);

#endif /* __BSP_FDCAN_H_ */

