#ifndef __TB6612_H_
#define __TB6612_H_

#include <stdint.h>

/**
 * @brief 电机编号
 */
typedef enum {
    MOTOR_A = 0,   /**< 第 1 路电机(M1) */
    MOTOR_B = 1,   /**< 第 2 路电机(M2) */
    MOTOR_C = 2,   /**< 第 3 路电机(M1) */
    MOTOR_D = 3    /**< 第 4 路电机(M2) */
} MotorId;

/**
 * @brief 旋转方向
 */
typedef enum {
    DIR_STOP = 0,
    DIR_CW   = 1,  /* 正转 */
    DIR_CCW  = 2   /* 反转 */
} MotorDir;

/**
 * @brief 初始化 TB6612（定时器 PWM + GPIO 方向脚）
 */
void TB6612_Init(void);

/**
 * @brief 设置电机速度/方向
 * @param id   电机编号
 * @param dir  方向
 * @param speed 速度 0~100
 */
void TB6612_SetMotor(MotorId id, MotorDir dir, uint8_t speed);

/**
 * @brief 电机制动（短接刹车）
 * @param id 电机编号
 */
void TB6612_Brake(MotorId id);

/**
 * @brief 电机滑行（高阻态）
 * @param id 电机编号
 */
void TB6612_Coast(MotorId id);

#endif /* __TB6612_H_ */
