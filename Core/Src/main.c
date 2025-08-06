/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "oled.h"
#include "mpu6050.h"
#include "madgwick.h"
#include "tb6612.h"
#include "pid.h"
#include "jdy23.h"
#include "battery.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

MPU6050_t mpu6050;
MadgwickFilter filter;
char displayBuffer[40]; // 显示缓冲区
volatile uint8_t data_status = 0; // 数据就绪标志
volatile uint8_t calibration_status = 0; // 校准状态标志
uint32_t display_timer = 0;     // 显示更新计时器
uint32_t last_sample_time = 0;
uint8_t battery_percent = 0; // 电池状态变量
// PID控制
PID_Controller pitch_pid, roll_pid, yaw_pid;
// 当前姿态
float current_roll, current_pitch, current_yaw;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// TIM3中断处理
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3 && calibration_status) {
        // 启动DMA读取
        MPU6050_Read_All_DMA(&mpu6050);
    }
}

// DMA传输完成回调
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1 && calibration_status) {
        data_status = 1;
    }
}

// I2C错误处理
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1 && calibration_status) {
        // 错误计数
        static uint8_t error_count = 0;
        error_count++;

        if(error_count > 5) {
            // 严重错误 - 重启I2C
            HAL_I2C_DeInit(&hi2c1);
            MX_I2C1_Init();
            error_count = 0;
        }

        // 重新尝试读取
        MPU6050_Read_All_DMA(&mpu6050);
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(20);
  OLED_Init();
  JDY23_Init();
  TB6612_Init();

  // PID初始化
  PID_Init(&pitch_pid, 1.2f, 0.05f, 0.2f, 30.0f);  // PITCH PID参数
  PID_Init(&roll_pid,  1.2f, 0.05f, 0.2f, 30.0f);   // ROLL PID参数
  PID_Init(&yaw_pid,   0.8f, 0.01f, 0.1f, 20.0f);   // YAW PID参数

  // 初始化MPU6050
  if (MPU6050_Init()){
	  while(1) {
		  OLED_NewFrame();
		  OLED_PrintASCIIString(0, 1, "MPU6050 Error!", &afont8x6, OLED_COLOR_NORMAL);
		  OLED_ShowFrame();
	  }
  }

  // 上电自动校准陀螺仪
  MPU6050_Calibrate(&mpu6050, 500); // 采集500个样本

  // 初始化Madgwick滤波器
  Madgwick_Init(&filter, 0.1f, 125.0f);

  // 标记校准完成
  calibration_status = 1;

  // 启动定时器
  HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // 检查数据是否就绪
	  if (data_status) {
		  data_status = 0; // 清除标志

		  // 计算采样间隔 (用于滤波器)
		  uint32_t current_time = HAL_GetTick();
		  float dt = (current_time - last_sample_time) / 1000.0f;
		  last_sample_time = current_time;

		  // 更新采样频率
		  if(dt > 0) filter.sampleFreq = 1.0f / dt;

		  // 处理数据
		  MPU6050_Process_Data(&mpu6050);

		  // 更新姿态
		  Madgwick_Update(&filter,
						  mpu6050.Gx, mpu6050.Gy, mpu6050.Gz,
						  mpu6050.Ax, mpu6050.Ay, mpu6050.Az);

		  // 获取欧拉角
		  Get_Euler_Angles(&filter, &current_roll, &current_pitch, &current_yaw);

		  // 检查蓝牙超时
		  JDY23_CheckTimeout();

		  // 计算PID控制量
		  float pitch_output = PID_Calculate(&pitch_pid,  jdy23_cmd.pitch, current_pitch);
		  float roll_output  = PID_Calculate(&roll_pid,  jdy23_cmd.roll,  current_roll);
		  float yaw_output   = PID_Calculate(&yaw_pid,   jdy23_cmd.yaw,   current_yaw);

		  // 电机混控
		  float motorA, motorB, motorC, motorD;
//		  Motor_Mixing(jdy23_cmd.throttle, pitch_output, roll_output, yaw_output,
//					  &motorA, &motorB, &motorC, &motorD);
		  Motor_Mixing(50, pitch_output, roll_output, yaw_output,
		  					  &motorA, &motorB, &motorC, &motorD);

		  /* 电机 0（MA）正转 */
		  TB6612_SetMotor(0, DIR_CW, (uint8_t)motorA);
		  /* 电机 1（MB）反转 */
		  TB6612_SetMotor(1, DIR_CCW, (uint8_t)motorB);
		  /* 电机 2（MC）正转 */
		  TB6612_SetMotor(2, DIR_CW, (uint8_t)motorC);
		  /* 电机 3（MD）反转 */
		  TB6612_SetMotor(3, DIR_CCW, (uint8_t)motorD);
	  }

	  // 20Hz更新显示
	  if (HAL_GetTick() - display_timer >= 50) {
		  display_timer = HAL_GetTick();

		  // 获取电量百分比
		  battery_percent = Battery_GetPercentage();

		  // 在OLED上显示姿态和温度
		  OLED_NewFrame();
		  snprintf(displayBuffer, sizeof(displayBuffer), "T:%2.1fC",mpu6050.Temp);
		  OLED_PrintASCIIString(0, 1, displayBuffer, &afont8x6, OLED_COLOR_NORMAL);
		  snprintf(displayBuffer, sizeof(displayBuffer), "R:%4.1f", current_roll);
		  OLED_PrintASCIIString(0, 11, displayBuffer, &afont8x6, OLED_COLOR_NORMAL);
		  snprintf(displayBuffer, sizeof(displayBuffer), "P:%4.1f", current_pitch);
		  OLED_PrintASCIIString(0, 21, displayBuffer, &afont8x6, OLED_COLOR_NORMAL);
		  snprintf(displayBuffer, sizeof(displayBuffer), "Y:%4.1f", current_yaw);
		  OLED_PrintASCIIString(0, 31, displayBuffer, &afont8x6, OLED_COLOR_NORMAL);
		  OLED_ShowFrame();
	  }

	  // 低功耗等待
	  __WFI();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
