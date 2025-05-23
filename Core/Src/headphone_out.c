#include "headphone_out.h"
#include "config.h"
#include <stdint.h>
#include "stm32g4xx_hal.h"

// Circular buffer for DMA
uint16_t headphone_buffer[HEADPHONE_BUFFER_SAMPLES];

// Internal triangle generator state, todo, export
static float tri_value;
static float tri_step;
static int8_t tri_dir;

// Forward declaration
static void fill_buffer(uint32_t offset, uint32_t length);

void headphone_out_start(DAC_HandleTypeDef *dac, TIM_HandleTypeDef *tim)
{
    headphone_set_frequency(500);

    //Enable callbacks to fill headphone buffers.
    __HAL_DMA_ENABLE_IT(dac->DMA_Handle2, DMA_IT_HT);   // half-transfer
    __HAL_DMA_ENABLE_IT(dac->DMA_Handle2, DMA_IT_TC);   // transfer-complete


    //Fill buffer with midpoint for starters, makes it easier to check if the DAC is turned on
    for (int i = 0; i < HEADPHONE_BUFFER_SAMPLES; i++)
    {
    	headphone_buffer[i] = (uint16_t)(DAC_MIDPOINT);
    }

    // Start timer and DAC with DMA circular mode
    HAL_TIM_Base_Start(tim);
    HAL_DAC_Start_DMA(dac, HEADPHONE_DAC_CHANNEL,
                      (uint32_t *)headphone_buffer,
                      HEADPHONE_BUFFER_SAMPLES,
                      DAC_ALIGN_12B_R);
}

void headphone_set_frequency(float frequency) {
    // Compute triangle step: full swing (2 * HEADPHONE_DAC_MAX) * freq / sample rate
    tri_step = (2.0f * DAC_MAX_VALUE * frequency) / HEADPHONE_SAMPL_RATE_HZ;
    tri_value = 0.0f;
    tri_dir = 1;
}

// DMA half-transfer complete callback
void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    // Fill first half
    fill_buffer(0, HEADPHONE_BUFFER_SAMPLES / 2);
}

// DMA full-transfer complete callback
void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    // Fill second half
    fill_buffer(HEADPHONE_BUFFER_SAMPLES / 2, HEADPHONE_BUFFER_SAMPLES / 2);
}

// Fill a segment of the circular buffer with triangle samples
static void fill_buffer(uint32_t offset, uint32_t length)
{
    for (uint32_t i = 0; i < length; ++i) {
        // Advance triangle value
        tri_value += tri_dir * tri_step;
        // Reverse direction at bounds
        if (tri_value >= DAC_MAX_VALUE) {
            tri_value = DAC_MAX_VALUE;
            tri_dir = -1;
        } else if (tri_value <= 0.0f) {
            tri_value = 0.0f;
            tri_dir = 1;
        }
        // Store into buffer
        headphone_buffer[offset + i] = (uint16_t)tri_value;
    }
}
