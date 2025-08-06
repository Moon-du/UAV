#include <stdint.h>
#include "usart.h"

#ifndef __JDY23_H
#define __JDY23_H

#define JDY23_RX_BUFFER_SIZE 64
#define JDY23_CMD_TIMEOUT 100  // 指令超时时间(ms)

typedef struct {
    float throttle;  // 0-100%
    float roll;      // -30 to 30度
    float pitch;     // -30 to 30度
    float yaw;       // -20 to 20度
    uint32_t last_update; // 最后更新时间
} JDY23_Command_t;

extern JDY23_Command_t jdy23_cmd;
extern DMA_HandleTypeDef hdma_usart2_rx;

void JDY23_Init(void);
void JDY23_ProcessData(uint8_t *data, uint16_t size);
void JDY23_CheckTimeout(void);
void JDY23_ResetCommand(void);

#endif
