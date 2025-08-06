/*
 * mpu6050.c
 *
 *  Created on: Jul 30, 2025
 *      Author: yingzhuang.feng
 */

#include "i2c.h"
#include "mpu6050.h"
#include "oled.h"
#include <math.h>

// MPU6050初始化
uint8_t MPU6050_Init() {
    uint8_t check;
    uint8_t Data;

    // 检测设备ID
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, WHO_AM_I, 1, &check, 1, 100);
    if (check != 0x68) return 1; // 设备ID错误

    // 唤醒设备
    Data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1, 1, &Data, 1, 100);

    // 设置采样率 = 1kHz / (1+7) = 125Hz
    Data = 0x07;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV, 1, &Data, 1, 100);

    // 设置DLPF带宽为94Hz
    Data = 0x02;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, CONFIG, 1, &Data, 1, 100);

    // 设置加速度计量程 ±4g
    Data = 0x08;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG, 1, &Data, 1, 100);

    // 设置陀螺仪量程 ±500°/s
    Data = 0x08;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG, 1, &Data, 1, 100);

    return 0;
}

// 启动DMA传输读取MPU6050数据
void MPU6050_Read_All_DMA(MPU6050_t *DataStruct) {
    // 启动I2C读取操作(DMA模式)
    HAL_I2C_Mem_Read_DMA(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H, 1, DataStruct->dma_buffer, 14);
}

// 处理DMA接收到的数据
void MPU6050_Process_Data(MPU6050_t *DataStruct) {
    // 解析原始数据
    DataStruct->Accel_X_RAW = (int16_t)(DataStruct->dma_buffer[0] << 8 | DataStruct->dma_buffer[1]);
    DataStruct->Accel_Y_RAW = (int16_t)(DataStruct->dma_buffer[2] << 8 | DataStruct->dma_buffer[3]);
    DataStruct->Accel_Z_RAW = (int16_t)(DataStruct->dma_buffer[4] << 8 | DataStruct->dma_buffer[5]);
    DataStruct->Temperature_RAW = (int16_t)(DataStruct->dma_buffer[6] << 8 | DataStruct->dma_buffer[7]);
    DataStruct->Gyro_X_RAW = (int16_t)(DataStruct->dma_buffer[8] << 8 | DataStruct->dma_buffer[9]);
    DataStruct->Gyro_Y_RAW = (int16_t)(DataStruct->dma_buffer[10] << 8 | DataStruct->dma_buffer[11]);
    DataStruct->Gyro_Z_RAW = (int16_t)(DataStruct->dma_buffer[12] << 8 | DataStruct->dma_buffer[13]);

    // 转换为物理量 (使用±4g量程)
    DataStruct->Ax = DataStruct->Accel_X_RAW / AFS_SEL_1;
    DataStruct->Ay = DataStruct->Accel_Y_RAW / AFS_SEL_1;
    DataStruct->Az = DataStruct->Accel_Z_RAW / AFS_SEL_1;

    // 转换为物理量 (使用±500°/s量程)
    DataStruct->Gx = DataStruct->Gyro_X_RAW / GFS_SEL_1;
    DataStruct->Gy = DataStruct->Gyro_Y_RAW / GFS_SEL_1;
    DataStruct->Gz = DataStruct->Gyro_Z_RAW / GFS_SEL_1;

    // 应用零偏校准
    DataStruct->Gx -= DataStruct->Gyro_Offset_X;
    DataStruct->Gy -= DataStruct->Gyro_Offset_Y;
    DataStruct->Gz -= DataStruct->Gyro_Offset_Z;

    // 温度转换 (°C)
    DataStruct->Temp = (float)DataStruct->Temperature_RAW / 340.0 + 36.53;
}

// 陀螺仪校准
void MPU6050_Calibrate(MPU6050_t *DataStruct, uint16_t numSamples) {
	OLED_NewFrame();
	OLED_PrintASCIIString(0, 26, "Calibrating...", &afont8x6, OLED_COLOR_NORMAL);
	OLED_ShowFrame();
	// 禁用所有中断
	__disable_irq();

	float sumX = 0, sumY = 0, sumZ = 0;
	uint8_t Rec_Data[14]; // 用于轮询读取的缓冲区

	// 使用轮询方式安全读取
	for (uint16_t i = 0; i < numSamples; i++) {
		// 阻塞式读取
		HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H, 1, Rec_Data, 14, 100);

		// 解析原始数据
		DataStruct->Gyro_X_RAW = (int16_t)(Rec_Data[8] << 8 | Rec_Data[9]);
		DataStruct->Gyro_Y_RAW = (int16_t)(Rec_Data[10] << 8 | Rec_Data[11]);
		DataStruct->Gyro_Z_RAW = (int16_t)(Rec_Data[12] << 8 | Rec_Data[13]);

		// 转换为物理量 (使用±500°/s量程)
		float gx = DataStruct->Gyro_X_RAW / GFS_SEL_1;
		float gy = DataStruct->Gyro_Y_RAW / GFS_SEL_1;
		float gz = DataStruct->Gyro_Z_RAW / GFS_SEL_1;

		sumX += gx;
		sumY += gy;
		sumZ += gz;

		HAL_Delay(5);
	}

	// 计算平均值作为零偏
	DataStruct->Gyro_Offset_X = sumX / numSamples;
	DataStruct->Gyro_Offset_Y = sumY / numSamples;
	DataStruct->Gyro_Offset_Z = sumZ / numSamples;

	// 重新启用中断
	__enable_irq();
}
