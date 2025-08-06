#include <stdint.h>
#include "adc.h"

#ifndef INC_BATTERY_H_
#define INC_BATTERY_H_

#define BATTERY_MIN_VOLTAGE 3.3f  // 最低工作电压(3.3V)
#define BATTERY_MAX_VOLTAGE 4.2f  // 满电电压(4.2V)
#define VOLTAGE_DIVIDER_RATIO 2.0f // 分压比(10k+10k)

void Battery_Init(void);
uint8_t Battery_GetPercentage(void);
float Battery_GetVoltage(void);

#endif /* BATTERY_H_ */

