/*
 * adc_tx_generator.h
 *
 *  Created on: May 22, 2025
 *      Author: matthew
 */

#pragma once
#include "stm32g4xx_hal.h"
void tx_sine_init_buffer(void);
void tx_sine_start(DAC_HandleTypeDef *dac, TIM_HandleTypeDef *tim);