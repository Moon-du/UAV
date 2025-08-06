/*
 * mpu6050.h
 *
 *  Created on: Jul 30, 2025
 *      Author: yingzhuang.feng
 */
#include <stdint.h>

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

// MPU6050 I2C地址
#define MPU6050_ADDR 0xD0  // (0x68 << 1)

// 寄存器定义
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1   0x6B
#define WHO_AM_I     0x75
#define ACCEL_XOUT_H 0x3B
#define TEMP_OUT_H   0x41
#define GYRO_XOUT_H  0x43

// 加速度计灵敏度 (g/LSB)
#define AFS_SEL_0 16384.0
#define AFS_SEL_1 8192.0
#define AFS_SEL_2 4096.0
#define AFS_SEL_3 2048.0

// 陀螺仪灵敏度 (°/s/LSB)
#define GFS_SEL_0 131.0
#define GFS_SEL_1 65.5
#define GFS_SEL_2 32.8
#define GFS_SEL_3 16.4

typedef struct {
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    int16_t Temperature_RAW;

    float Ax, Ay, Az;       // 加速度 (g)
    float Gx, Gy, Gz;       // 角速度 (°/s)
    float Temp;             // 温度 (°C)

    float Gyro_Offset_X;    // 陀螺仪X轴零偏
    float Gyro_Offset_Y;    // 陀螺仪Y轴零偏
    float Gyro_Offset_Z;    // 陀螺仪Z轴零偏

    // DMA缓冲区
    uint8_t dma_buffer[14];
} MPU6050_t;

// 函数声明
uint8_t MPU6050_Init();
void MPU6050_Read_All_DMA(MPU6050_t *DataStruct);
void MPU6050_Process_Data(MPU6050_t *DataStruct);
void MPU6050_Calibrate(MPU6050_t *DataStruct, uint16_t numSamples);


#endif /* INC_MPU6050_H_ */
