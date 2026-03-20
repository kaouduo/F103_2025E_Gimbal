/**
 * @file      my_main.c
 * @brief     None
 * @details   None
 * @author    PJ_Hr
 * @version   V1.0
 * @date      2025/9/25
 * @copyright pjh
 * @email     2643560781@qq.com
 */

#include "gimbal.h"
#include "my_main_def.h"

#include "bsp_usart.h"
// #include "bsp_fdcan.h"
// #include "pid.h"

#include "MahonyAHRS.h"
#include "BMI088driver.h"
#include "algorithm.h"
#include "bsp_dwt.h"

#include "cmsis_os.h"

extern UART_HandleTypeDef huart1;
extern float roll_mahony, pitch_mahony, yaw_mahony;

extern Motor_Data_t motor101, motor102;
float pid_yaw_angle_output = 0.0f;
float pid_pitch_angle_output = 0.0f;
volatile float motor101_angle = 0.0f;
volatile float motor102_angle = 0.0f;

// PID_t PID_yaw_angle = {
//     .pid_type = POSITION_TYPE,
//     .kp = 250.0f,
//     .ki = 0.0001f,
//     .kd = 15.0f, // Kd: 微分系数
//     .sum_error_limit_p = 9600, // 积分限幅上限
//     .sum_error_limit_n = -9600, // 积分限幅下限
//     .output_limit_p = 9600.0f, // 输出限幅上限
//     .output_limit_n = -9600.0f, // 输出限幅下限
// };

// PID_t PID_pitch_angle = {
//     .pid_type = POSITION_TYPE,
//     .kp = 250.0f,
//     .ki = 0.0001f,
//     .kd = 15.0f, // Kd: 微分系数
//     .sum_error_limit_p = 9600, // 积分限幅上限
//     .sum_error_limit_n = -9600, // 积分限幅下限
//     .output_limit_p = 9600.0f, // 输出限幅上限
//     .output_limit_n = -9600.0f, // 输出限幅下限
// };

// PID_t PID_imu_temp = {
//     .pid_type = POSITION_TYPE,
//     .kp = 1000.0f,
//     .ki = 25.0f,
//     .kd = 40.0f, // Kd: 微分系数
//     .sum_error_limit_p = 0, // 积分限幅上限
//     .sum_error_limit_n = 0, // 积分限幅下限
//     .output_limit_p = 8000.0f, // 输出限幅上限
//     .output_limit_n = 0.0f, // 输出限幅下限
// };

#define correct_Time_define 1000 // 上电去0飘 1000次取平均
#define temp_times          300  // 探测温度阈值

float Temperature_PID_Para[3] = {1600, 50, 40};
float H723_Temperature = 0.f;
unsigned int adc_v;
float adcx;

float gyro[3], accel[3], temp; // 陀螺仪原始值
float gyro_correct[3] = {0};   // 0飘初始值
uint8_t attitude_flag = 0;
uint32_t correct_times = 0;
float roll = 0.0f, pitch = 0.0f, yaw = 0.0f; // 欧拉角

// 时间测量变量
float mahony_update_time_us = 0.0f;
float mahony_compute_time_us = 0.0f;
float total_time_us = 0.0f;

static uint8_t first_mahony = 0;

/**
 * @brief 主初始化函数
 * @param 无
 * @return 无
 * */
void My_Main_Init(void) {
    // 关闭中断,防止在初始化过程中发生中断
    // 请不要在初始化过程中使用中断和延时函数！
    // 若必须,则只允许使用DWT_Delay()
    __disable_irq();

    // DWT初始化 (STM32F103 @ 72MHz)
    DWT_Init(72);

    // BSP层初始化
    //    bsp_can_init();

    // 初始化完成,开启中断
    __enable_irq();

    // 应用层初始化（用到的组件也会被初始化）

    // 初始化（1000Hz采样率）
    Mahony_Init(1000.0f); // mahony姿态解算初始化
    // imu heat init
    while (BMI088_init())
    {
    }; // 陀螺仪初始化
    //    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    HAL_UART_Transmit(&huart1, (uint8_t *)"Init OK\r\n", sizeof("Init OK\r\n"), 100);

    //    PID_yaw_angle.target = 0.0f;
    //    PID_pitch_angle.target = 0.0f;
    //    PID_imu_temp.target = 40.0f;

    // 启动陀螺仪零飘校准
    attitude_flag = 1;

    // 任务开始轮询
    // My_Main_Task();
}

/**
 * @brief 主任务轮询函数
 * @param 无
 * @return 无
 * */
void My_Main_Task(void)
{

    while (1)
    {
        // CarTask();
        // CmdTask();
        // GimbalTask();
    }
}

static uint32_t count = 0;

/**
 * @brief IMU数据处理任务
 * @param argument  任务参数(FreeRTOS标准)
 * @return 无
 * */
void IMU_task(void *argument) {

    My_Main_Init();
    /* USER CODE BEGIN IMU_task */
    /* Infinite loop */
    for (;;)
    {
        BMI088_read(gyro, accel, &temp); // 陀螺仪值读取
                                         //  调试输出原始数据
        //        Uart_Printf(&huart1, "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",
        //                    gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2]);
        if (first_mahony == 0)
        {
            first_mahony++;
            MahonyAHRSinit(accel[0], accel[1], accel[2], 0, 0, 0); // mahony上电快速初始化
            Uart_Printf(&huart1, "Mahony Init with accel: %.3f,%.3f,%.3f\r\n", accel[0], accel[1], accel[2]);
        }

        switch (attitude_flag)
        {
        case 0:
            // 待机状态，等待启动校准
            break;

        case 1:
            // 零飘校准阶段
            gyro_correct[0] += gyro[0];
            gyro_correct[1] += gyro[1];
            gyro_correct[2] += gyro[2];
            correct_times++;
            if (correct_times >= correct_Time_define)
            {
                gyro_correct[0] /= correct_Time_define;
                gyro_correct[1] /= correct_Time_define;
                gyro_correct[2] /= correct_Time_define;
                attitude_flag = 2;
                Uart_Printf(&huart1, "Gyro calibration done! Offset: %.3f,%.3f,%.3f\r\n", gyro_correct[0], gyro_correct[1], gyro_correct[2]);
            }
            break;

        case 2:
        {
            // 姿态解算阶段
            uint32_t t_start, t1, t2, t3;

            t_start = DWT->CYCCNT; // 开始计时

            gyro[0] -= gyro_correct[0];
            gyro[1] -= gyro_correct[1];
            gyro[2] -= gyro_correct[2];

            // 测量 Mahony_update 耗时
            t1 = DWT->CYCCNT;
            Mahony_update(gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2], 0, 0, 0);
            t2 = DWT->CYCCNT;
            mahony_update_time_us = (t2 - t1) / 72.0f; // 72MHz CPU

            // 测量 Mahony_computeAngles 耗时
            Mahony_computeAngles();
            t3 = DWT->CYCCNT;
            mahony_compute_time_us = (t3 - t2) / 72.0f;

            //                PID_yaw_angle.target = yaw_mahony;
            //                PID_pitch_angle.target = pitch_mahony;
            pitch = getPitch(); // 获得pitch
            roll = getRoll();   // 获得roll
            yaw = getYaw();     // 获得yaw

            // 总耗时
            total_time_us = (DWT->CYCCNT - t_start) / 72.0f;

            break;
        }

        default:
            break;
        }

        //        motor101_angle = motor101.angle * RAD2DEG;
        //        pid_yaw_angle_output = PID_Calc(&PID_yaw_angle, motor101_angle);
        //        motor102_angle = motor102.angle * RAD2DEG;
        //        pid_pitch_angle_output = PID_Calc(&PID_pitch_angle, motor102_angle);

        //        fdcan_send_motor_cmd(&hfdcan1, 0x100, (int16_t) pid_yaw_angle_output, (int16_t) pid_pitch_angle_output);

        //        // temperature control
        //        if ((count % 10) == 0) {
        //            uint32_t temp_temperature = 0;

        //            // 100hz 的温度控制pid
        //            temp_temperature = PID_Calc(&PID_imu_temp, temp); //温度pid  //需要调一下pid使得温度在40°左右
        //            htim3.Instance->CCR4 = temp_temperature;

        //            static uint32_t temp_Ticks = 0;
        //            if ((fabsf(temp - PID_imu_temp.target) < 0.5f) && attitude_flag == 0) //接近额定温度之差小于0.5° 开始计数
        //            {
        //                temp_Ticks++;
        //                if (temp_Ticks > temp_times) //计数达到一定次数后 才进入0飘初始化 说明温度已经达到目标
        //                {
        //                    attitude_flag = 1; //go to correct state
        //                }
        //            }
        //        }
        //        count++;

        osDelay(1);
    }
}
