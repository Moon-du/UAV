#include "tb6612.h"
#include "tim.h"
#include "gpio.h"

/*------------- 用户根据原理图修改的宏 -------------*/
/* 第 1 片 TB6612 (MA/MB) */
#define PWMA_TIM    &htim1
#define PWMA_CH     TIM_CHANNEL_1
#define AIN1_Port   GPIOA
#define AIN1_Pin    GPIO_PIN_0
#define AIN2_Port   GPIOA
#define AIN2_Pin    GPIO_PIN_1

#define PWMB_TIM    &htim1
#define PWMB_CH     TIM_CHANNEL_2
#define BIN1_Port   GPIOA
#define BIN1_Pin    GPIO_PIN_2
#define BIN2_Port   GPIOA
#define BIN2_Pin    GPIO_PIN_3

/* 第 2 片 TB6612 (MC/MD) */
#define PWMC_TIM   &htim1
#define PWMC_CH    TIM_CHANNEL_3
#define CIN1_Port  GPIOA
#define CIN1_Pin   GPIO_PIN_4
#define CIN2_Port  GPIOA
#define CIN2_Pin   GPIO_PIN_5

#define PWMD_TIM   &htim1
#define PWMD_CH    TIM_CHANNEL_4
#define DIN1_Port  GPIOA
#define DIN1_Pin   GPIO_PIN_6
#define DIN2_Port  GPIOA
#define DIN2_Pin   GPIO_PIN_7

#define MAX_SPEED   100

/*------------- 内部静态函数 -------------*/
static inline void __SetPwm(TIM_HandleTypeDef *tim, uint32_t ch, uint8_t spd)
{
    __HAL_TIM_SET_COMPARE(tim, ch, spd);
}

static inline void __SetDir(GPIO_TypeDef *p1, uint16_t n1,
                            GPIO_TypeDef *p2, uint16_t n2,
                            MotorDir dir)
{
    switch (dir) {
    case DIR_CW:
        HAL_GPIO_WritePin(p1, n1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(p2, n2, GPIO_PIN_RESET);
        break;
    case DIR_CCW:
        HAL_GPIO_WritePin(p1, n1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(p2, n2, GPIO_PIN_SET);
        break;
    case DIR_STOP:
    default:
        HAL_GPIO_WritePin(p1, n1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(p2, n2, GPIO_PIN_RESET);
        break;
    }
}

/*------------- API 实现 -------------*/
void TB6612_Init(void)
{
    /* 启动所有 PWM 通道 */
    HAL_TIM_PWM_Start(PWMA_TIM, PWMA_CH);
    HAL_TIM_PWM_Start(PWMB_TIM, PWMB_CH);
    HAL_TIM_PWM_Start(PWMC_TIM, PWMC_CH);
    HAL_TIM_PWM_Start(PWMD_TIM, PWMD_CH);

    /* GPIO 方向脚已在 CubeMX 配置为推挽输出，无需额外初始化 */
}

void TB6612_SetMotor(MotorId id, MotorDir dir, uint8_t speed)
{
    if (speed > MAX_SPEED) speed = MAX_SPEED;

    switch (id) {
    case MOTOR_A: /* 第 1 片 MA */
        __SetPwm(PWMA_TIM, PWMA_CH, speed);
        __SetDir(AIN1_Port, AIN1_Pin, AIN2_Port, AIN2_Pin, dir);
        break;

    case MOTOR_B: /* 第 1 片 MB */
        __SetPwm(PWMB_TIM, PWMB_CH, speed);
        __SetDir(BIN1_Port, BIN1_Pin, BIN2_Port, BIN2_Pin, dir);
        break;

    case MOTOR_C: /* 第 2 片 MC */
        __SetPwm(PWMC_TIM, PWMC_CH, speed);
        __SetDir(CIN1_Port, CIN1_Pin, CIN2_Port, CIN2_Pin, dir);
        break;

    case MOTOR_D: /* 第 2 片 MD */
        __SetPwm(PWMD_TIM, PWMD_CH, speed);
        __SetDir(DIN1_Port, DIN1_Pin, DIN2_Port, DIN2_Pin, dir);
        break;

    default:
        break;
    }
}

void TB6612_Brake(MotorId id)
{
    TB6612_SetMotor(id, DIR_STOP, 100);   /* PWM=100，方向脚同时为高 -> 刹车 */
}

void TB6612_Coast(MotorId id)
{
    TB6612_SetMotor(id, DIR_STOP, 0);     /* PWM=0，方向脚为低 -> 滑行 */
}
