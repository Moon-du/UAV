#include <stdint.h>
#include "usart.h"

#ifndef __JDY23_H
#define __JDY23_H

#define JDY23_RX_BUF_LEN   128      // 环形缓冲区大小
#define JDY23_FRAME_MAX    64       // 单帧最大长度
#define JDY23_CMD_TIMEOUT  100      // ms, 无数据超时

/* 控制命令结构体（全部用定点，乘 100） */
typedef struct {
    int16_t  throttle;   // 0..10000   (0.00~100.00 %)
    int16_t  roll;       // -3000..3000 (-30.00~30.00°)
    int16_t  pitch;      // -3000..3000
    int16_t  yaw;        // -2000..2000
    uint32_t last_update;
} JDY23_Command_t;

extern JDY23_Command_t jdy23_cmd;
extern DMA_HandleTypeDef hdma_usart2_rx;

void JDY23_Init(void);
void JDY23_CheckTimeout(void);
void JDY23_ResetCommand(void);

#endif

