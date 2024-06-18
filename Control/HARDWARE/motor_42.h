#ifndef	_MOTOR_42_H_
#define	_MOTOR_42_H_

#include "stm32f4xx_hal.h"
void Motor_init(void);
void Motor_state(uint8_t motor,uint8_t dir,uint8_t state);

#endif


