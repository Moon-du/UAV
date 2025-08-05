/*
 * madgwick.c
 *
 *  Created on: Jul 30, 2025
 *      Author: yingzhuang.feng
 */


#include "madgwick.h"
#include <math.h>

// 初始化滤波器
void Madgwick_Init(MadgwickFilter *filter, float beta, float sampleFreq) {
    filter->q0 = 1.0f;
    filter->q1 = 0.0f;
    filter->q2 = 0.0f;
    filter->q3 = 0.0f;
    filter->beta = beta;
    filter->sampleFreq = sampleFreq;
}

// Madgwick滤波更新
void Madgwick_Update(MadgwickFilter *filter,
                     float gx, float gy, float gz,
                     float ax, float ay, float az) {
    float q0 = filter->q0, q1 = filter->q1, q2 = filter->q2, q3 = filter->q3;
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    // 速率转弧度/秒
    gx *= 0.0174533f; // deg/s to rad/s
    gy *= 0.0174533f;
    gz *= 0.0174533f;

    // 梯度下降算法校正
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // 归一化加速度计数据
        recipNorm = 1.0f / sqrtf(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // 辅助变量避免重复计算
        _2q0 = 2.0f * q0;
        _2q1 = 2.0f * q1;
        _2q2 = 2.0f * q2;
        _2q3 = 2.0f * q3;
        _4q0 = 4.0f * q0;
        _4q1 = 4.0f * q1;
        _4q2 = 4.0f * q2;
        _8q1 = 8.0f * q1;
        _8q2 = 8.0f * q2;
        q0q0 = q0 * q0;
        q1q1 = q1 * q1;
        q2q2 = q2 * q2;
        q3q3 = q3 * q3;

        // 梯度下降目标函数
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;

        // 归一化步长
        recipNorm = 1.0f / sqrtf(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // 应用反馈
        qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz) - filter->beta * s0;
        qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy) - filter->beta * s1;
        qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx) - filter->beta * s2;
        qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx) - filter->beta * s3;
    } else {
        // 无加速度计数据时仅用陀螺仪
        qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
        qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
        qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
        qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);
    }

    // 积分四元数
    q0 += qDot1 * (1.0f / filter->sampleFreq);
    q1 += qDot2 * (1.0f / filter->sampleFreq);
    q2 += qDot3 * (1.0f / filter->sampleFreq);
    q3 += qDot4 * (1.0f / filter->sampleFreq);

    // 归一化四元数
    recipNorm = 1.0f / sqrtf(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;

    // 存储更新后的四元数
    filter->q0 = q0;
    filter->q1 = q1;
    filter->q2 = q2;
    filter->q3 = q3;
}

// 获取欧拉角 (单位: 度)
void Get_Euler_Angles(MadgwickFilter *filter,
                      float *roll, float *pitch, float *yaw) {
    float q0 = filter->q0, q1 = filter->q1, q2 = filter->q2, q3 = filter->q3;

    *roll = atan2f(q0*q1 + q2*q3, 0.5f - q1*q1 - q2*q2) * 57.29578f; // rad to deg
    *pitch = asinf(-2.0f * (q1*q3 - q0*q2)) * 57.29578f;
    *yaw = atan2f(q1*q2 + q0*q3, 0.5f - q2*q2 - q3*q3) * 57.29578f;
}
