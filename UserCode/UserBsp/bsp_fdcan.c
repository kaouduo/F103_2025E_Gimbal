#include "bsp_can.h"
#include <string.h>
#include "usart.h"
#include "gimbal.h"
#include "my_main_def.h"

extern CAN_HandleTypeDef hcan;
#define hfdcan1 hcan

Motor_Data_t motor101, motor102;

uint8_t rx_data1[8] = {0};
uint16_t rec_id1;

void bsp_can_init(void)
{
	can_filter_init();
	HAL_CAN_Start(&hfdcan1);
	HAL_CAN_ActivateNotification(&hfdcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void can_filter_init(void)
{
	CAN_FilterTypeDef can_filter;

	can_filter.FilterActivation = ENABLE;
	can_filter.FilterBank = 0;
	can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filter.FilterIdHigh = 0x0000;
	can_filter.FilterIdLow = 0x0000;
	can_filter.FilterMaskIdHigh = 0x0000;
	can_filter.FilterMaskIdLow = 0x0000;
	can_filter.FilterFIFOAssignment = CAN_RX_FIFO0;

	HAL_CAN_ConfigFilter(&hfdcan1, &can_filter);
}

uint8_t fdcanx_send_data(hcan_t *hfdcan, uint16_t id, uint8_t *data, uint32_t len)
{
    CAN_TxHeaderTypeDef pTxHeader;
    uint32_t txMailbox;
    pTxHeader.StdId = id;
    pTxHeader.ExtId = id;
    pTxHeader.IDE = CAN_ID_STD;
    pTxHeader.RTR = CAN_RTR_DATA;
    pTxHeader.DLC = len;
    pTxHeader.TransmitGlobalTime = DISABLE;

	if(HAL_CAN_AddTxMessage(hfdcan, &pTxHeader, data, &txMailbox) != HAL_OK)
		return 1;
	return 0;
}

uint8_t fdcanx_receive(hcan_t *hfdcan, uint16_t *rec_id, uint8_t *buf)
{
	CAN_RxHeaderTypeDef pRxHeader;
	uint8_t len = 0;

	if(HAL_CAN_GetRxMessage(hfdcan, CAN_RX_FIFO0, &pRxHeader, buf) == HAL_OK)
	{
		*rec_id = pRxHeader.StdId;
		len = pRxHeader.DLC;
		return len;
	}
	return 0;
}

void fdcan1_rx_callback(void)
{
	uint8_t temp_buf[8];
	uint16_t temp_id;

	if (fdcanx_receive(&hfdcan1, &temp_id, temp_buf) > 0)
	{
		int32_t raw_angle;
		int16_t raw_speed;

		memcpy(&raw_angle, &temp_buf[0], 4);
		memcpy(&raw_speed, &temp_buf[4], 2);

		if (temp_id == 0x101) {
			motor101.id = temp_id;
			motor101.angle = raw_angle / 1000.0f;
			motor101.speed = raw_speed / 10.0f;
		}
		else if (temp_id == 0x102) {
			motor102.id = temp_id;
			motor102.angle = raw_angle / 1000.0f;
			motor102.speed = raw_speed / 10.0f;
		}
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_ptr)
{
    if(hcan_ptr == &hfdcan1)
	{
		fdcan1_rx_callback();
	}
}

uint8_t fdcan_send_motor_cmd(hcan_t *hfdcan, uint16_t can_id, int16_t motor1_voltage, int16_t motor2_voltage)
{
	uint8_t tx_data[8] = {0};

	memcpy(&tx_data[0], &motor1_voltage, 2);
	memcpy(&tx_data[2], &motor2_voltage, 2);

	tx_data[4] = 0;
	tx_data[5] = 0;
	tx_data[6] = 0;
	tx_data[7] = 0;

	return fdcanx_send_data(hfdcan, can_id, tx_data, 8);
}

uint8_t fdcan_send_motor_cmd_4(hcan_t *hfdcan, uint16_t can_id, int16_t voltages[4])
{
	uint8_t tx_data[8];

	memcpy(tx_data, voltages, 8);

	return fdcanx_send_data(hfdcan, can_id, tx_data, 8);
}