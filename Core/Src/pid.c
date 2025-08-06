#include "pid.h"
#include <math.h>

// 初始化PID控制器
void PID_Init(PID_Controller* pid, float Kp, float Ki, float Kd, float output_limit) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_limit = output_limit;
}

// 计算PID输出
float PID_Calculate(PID_Controller* pid, float setpoint, float measurement) {
    float error = setpoint - measurement;

    // 比例项
    float P = pid->Kp * error;

    // 积分项（带抗饱和）
    pid->integral += error;
    if (pid->integral > pid->output_limit) pid->integral = pid->output_limit;
    if (pid->integral < -pid->output_limit) pid->integral = -pid->output_limit;
    float I = pid->Ki * pid->integral;

    // 微分项
    float D = pid->Kd * (error - pid->prev_error);
    pid->prev_error = error;

    // 总和并限幅
    float output = P + I + D;
    if (output > pid->output_limit) output = pid->output_limit;
    if (output < -pid->output_limit) output = -pid->output_limit;

    return output;
}

// 电机混控函数（X型布局）
void Motor_Mixing(float throttle, float pitch_ctrl, float roll_ctrl, float yaw_ctrl,
                 float* motorA, float* motorB, float* motorC, float* motorD) {
    // 基础油门 + 姿态控制混合
    *motorA = throttle - pitch_ctrl - roll_ctrl - yaw_ctrl;  // 前左(A)
    *motorB = throttle - pitch_ctrl + roll_ctrl + yaw_ctrl;  // 前右(B)
    *motorC = throttle + pitch_ctrl + roll_ctrl - yaw_ctrl;  // 后右(C)
    *motorD = throttle + pitch_ctrl - roll_ctrl + yaw_ctrl;  // 后左(D)

    // 电机输出限幅(0-100)
    #define CLAMP(x) (x < 0 ? 0 : (x > 100 ? 100 : x))
    *motorA = CLAMP(*motorA);
    *motorB = CLAMP(*motorB);
    *motorC = CLAMP(*motorC);
    *motorD = CLAMP(*motorD);
}
