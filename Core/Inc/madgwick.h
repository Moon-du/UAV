/*
 * madgwick.h
 *
 *  Created on: Jul 30, 2025
 *      Author: yingzhuang.feng
 */

#ifndef INC_MADGWICK_H_
#define INC_MADGWICK_H_

typedef struct {
    float q0, q1, q2, q3;   // 四元数
    float beta;              // 滤波器增益
    float sampleFreq;        // 采样频率
} MadgwickFilter;

// 函数声明
void Madgwick_Init(MadgwickFilter *filter, float beta, float sampleFreq);
void Madgwick_Update(MadgwickFilter *filter,
                     float gx, float gy, float gz,
                     float ax, float ay, float az);
void Get_Euler_Angles(MadgwickFilter *filter,
                      float *roll, float *pitch, float *yaw);

#endif /* INC_MADGWICK_H_ */
