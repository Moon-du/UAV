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
        if (data[i] == 'T') {
            // 查找帧尾
            int end = i + 1;
            while (end < size && data[end] != '\n') {
                end++;
            }
            if (end < size) {
                // 提取命令字符串
                char command[end - i + 1];
                memcpy(command, &data[i], end - i);
                command[end - i] = '\0';

                // 解析命令
                char *token = strtok(command, "R");
                if (token && token[0] == 'T') {
                    jdy23_cmd.throttle = atoi(token + 1);

                    token = strtok(NULL, "P");
                    if (token) {
                        jdy23_cmd.roll = atof(token);

                        token = strtok(NULL, "Y");
                        if (token) {
                            jdy23_cmd.pitch = atof(token);

                            token = strtok(NULL, "\n");
                            if (token) {
                                jdy23_cmd.yaw = atof(token);
                            }
                        }
                    }
                }
                jdy23_cmd.last_update = HAL_GetTick();
                return;
            }
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

