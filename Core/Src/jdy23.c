#include "jdy23.h"
#include <string.h>
#include <stdlib.h>

JDY23_Command_t jdy23_cmd = {0};
uint8_t jdy23_rx_buffer[JDY23_RX_BUFFER_SIZE];

// 初始化蓝牙模块
void JDY23_Init(void) {
    // 重置指令结构体
    JDY23_ResetCommand();

    // 启动DMA接收
    HAL_UART_Receive_DMA(&huart2, jdy23_rx_buffer, JDY23_RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT); // 禁用半传输中断
}

// 重置指令为安全值
void JDY23_ResetCommand(void) {
    jdy23_cmd.throttle = 0;
    jdy23_cmd.roll = 0;
    jdy23_cmd.pitch = 0;
    jdy23_cmd.yaw = 0;
}

// 检查指令超时
void JDY23_CheckTimeout(void) {
    if (HAL_GetTick() - jdy23_cmd.last_update > JDY23_CMD_TIMEOUT) {
        JDY23_ResetCommand();  // 超时后重置指令
    }
}

// 处理接收到的数据
void JDY23_ProcessData(uint8_t *data, uint16_t size) {
    // 查找帧头
    for (int i = 0; i < size; i++) {
        if (data[i] == 'T' && i + 15 < size && data[i + 15] == '\n') {
            // 控制指令: TxxxRyyyPpppYyyy\n
            char buffer[5] = {0};

            // 解析油门
            buffer[0] = data[i+1]; buffer[1] = data[i+2]; buffer[2] = data[i+3];
            jdy23_cmd.throttle = atoi(buffer);

            // 解析滚转
            buffer[0] = data[i+5]; buffer[1] = data[i+6]; buffer[2] = data[i+7];
            jdy23_cmd.roll = atoi(buffer);

            // 解析俯仰
            buffer[0] = data[i+9]; buffer[1] = data[i+10]; buffer[2] = data[i+11];
            jdy23_cmd.pitch = atoi(buffer);

            // 解析偏航
            buffer[0] = data[i+13]; buffer[1] = data[i+14]; buffer[2] = data[i+15];
            jdy23_cmd.yaw = atoi(buffer);

            jdy23_cmd.last_update = HAL_GetTick();
            return;
        }
    }
}

// DMA接收完成回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        JDY23_ProcessData(jdy23_rx_buffer, JDY23_RX_BUFFER_SIZE);
        HAL_UART_Receive_DMA(&huart2, jdy23_rx_buffer, JDY23_RX_BUFFER_SIZE);
    }
}
