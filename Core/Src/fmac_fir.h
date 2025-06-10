// fir.h
#pragma once

#include "stm32g4xx_hal.h"
#include <stm32g4xx_hal_fmac.h>
#include <stdbool.h>
void fmac_fir_start(FMAC_HandleTypeDef *hfmac);
bool fmac_fir_filter(int16_t *input, int16_t *output, uint16_t length);
void self_tests();

