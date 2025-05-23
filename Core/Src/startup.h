/*
 * matty.h
 *
 *  Created on: May 17, 2025
 *      Author: matthew
 */

#ifndef SRC_STARTUP_H_
#define SRC_STARTUP_H_
#include "stm32g4xx_hal.h"

//Moved all code into here, keep it clean of stm32 jank
void begin(ADC_HandleTypeDef *rxADC, DAC_HandleTypeDef *txDAC, TIM_HandleTypeDef *txTIM, DAC_HandleTypeDef *headphoneDAC, TIM_HandleTypeDef *headphoneTIM);

#endif /* SRC_STARTUP_H_ */
