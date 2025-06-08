// fir.h
#pragma once
#include "stm32g4xx_hal.h"

FMAC_HandleTypeDef *g_hfmac = NULL;
void fir_start(FMAC_HandleTypeDef *hfmac, FMAC_FilterConfigTypeDef *sFmacConfig);
void fir_filter(int16_t *input, int16_t *output, uint16_t length);
void self_tests();
