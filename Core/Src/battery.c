#include "battery.h"

static uint16_t adc_raw = 0;
static float battery_voltage = 0.0f;

// ADC初始化
void Battery_Init(void) {
    HAL_ADC_Start(&hadc1);
}

// 获取电池电压
float Battery_GetVoltage(void) {
    // 启动ADC转换
    HAL_ADC_PollForConversion(&hadc1, 10);

    // 读取ADC值
    adc_raw = HAL_ADC_GetValue(&hadc1);

    // 转换为电压值 (3.3V参考电压，12位ADC)
    float adc_voltage = (adc_raw * 3.3f) / 4095.0f;

    // 计算实际电池电压 (考虑分压比)
    battery_voltage = adc_voltage * VOLTAGE_DIVIDER_RATIO;

    return battery_voltage;
}

// 获取电池电量百分比
uint8_t Battery_GetPercentage(void) {
    float voltage = Battery_GetVoltage();

    // 线性计算电量百分比
    float percentage = 100.0f * (voltage - BATTERY_MIN_VOLTAGE) /
                      (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE);

    // 限制在0-100范围
    if (percentage < 0) return 0;
    if (percentage > 100) return 100;
    return (uint8_t)percentage;
}
