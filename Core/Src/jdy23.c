#include "jdy23.h"
#include <string.h>
#include <stdio.h>

/* ---------------- 内部变量 ---------------- */
static uint8_t  rx_dma_buf[2][JDY23_RX_BUF_LEN];  // 双缓冲
static uint8_t  rx_idle_buf[JDY23_FRAME_MAX];

JDY23_Command_t jdy23_cmd = {0};

/* ---------------- 对外接口 ---------------- */
void JDY23_Init(void)
{
    JDY23_ResetCommand();

    /* 启动双缓冲 DMA + 空闲中断 */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_dma_buf[0], JDY23_RX_BUF_LEN);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);   // 关 Half-Transfer
}

void JDY23_ResetCommand(void)
{
    jdy23_cmd.throttle = 0;
    jdy23_cmd.roll     = 0;
    jdy23_cmd.pitch    = 0;
    jdy23_cmd.yaw      = 0;
}

void JDY23_CheckTimeout(void)
{
    if (HAL_GetTick() - jdy23_cmd.last_update > JDY23_CMD_TIMEOUT)
        JDY23_ResetCommand();
}

/* ---------------- 内部解析 ---------------- */
static void parse_frame(uint8_t *frame, uint16_t len)
{
    /* 安全终止 */
    if (len >= JDY23_FRAME_MAX) len = JDY23_FRAME_MAX - 1;
    frame[len] = '\0';

    int   t; float r, p, y;
    if (sscanf((char *)frame, "T%dR%fP%fY%f", &t, &r, &p, &y) == 4)
    {
        /* 范围检查 */
        if (t < 0 || t > 100) return;
        if (r < -30.0f || r > 30.0f) return;
        if (p < -30.0f || p > 30.0f) return;
        if (y < -20.0f || y > 20.0f) return;

        /* 写入全局结构体（乘 100 转定点） */
        jdy23_cmd.throttle   = t * 100;
        jdy23_cmd.roll       = (int16_t)(r * 100.0f);
        jdy23_cmd.pitch      = (int16_t)(p * 100.0f);
        jdy23_cmd.yaw        = (int16_t)(y * 100.0f);
        jdy23_cmd.last_update = HAL_GetTick();
    }
}

/* ---------------- CubeIDE 回调 ---------------- */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,
                                uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        /* 当前空闲中断所用缓冲区索引 */
        static uint8_t buf_idx = 0;
        uint8_t *buf = rx_dma_buf[buf_idx];

        /* 找到完整 '\n' 帧 */
        uint16_t start = 0;
        for (uint16_t i = 0; i < Size; i++)
        {
            if (buf[i] == '\n')
            {
                uint16_t frame_len = i - start + 1;
                if (frame_len <= JDY23_FRAME_MAX)
                {
                    memcpy(rx_idle_buf, &buf[start], frame_len);
                    parse_frame(rx_idle_buf, frame_len);
                }
                start = i + 1;
            }
        }

        /* 切换双缓冲 */
        buf_idx ^= 1;
        HAL_UARTEx_ReceiveToIdle_DMA(huart,
                                     rx_dma_buf[buf_idx],
                                     JDY23_RX_BUF_LEN);
    }
}

