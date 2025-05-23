/*
 * headphone_out.h
 *
 *  Created on: May 23, 2025
 *      Author: matthew
 */

#pragma once
#include "stm32g4xx_hal.h"
void headphone_out_start(DAC_HandleTypeDef *dac, TIM_HandleTypeDef *tim);
void headphone_set_frequency(float frequency);