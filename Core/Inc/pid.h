#ifndef __PID_H
#define __PID_H

#include <stdint.h>

typedef struct {
    float Kp, Ki, Kd;
    float integral;
    float prev_error;
    float output_limit;
} PID_Controller;

void PID_Init(PID_Controller* pid, float Kp, float Ki, float Kd, float output_limit);
float PID_Calculate(PID_Controller* pid, float setpoint, float measurement);

void Motor_Mixing(float throttle, float pitch_ctrl, float roll_ctrl, float yaw_ctrl,
                 float* motorA, float* motorB, float* motorC, float* motorD);

#endif
