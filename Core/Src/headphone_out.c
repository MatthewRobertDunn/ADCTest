#include "headphone_out.h"
#include "config.h"
#include <stdint.h>
#include "stm32g4xx_hal.h"

// Circular buffer for DMA
uint16_t headphone_buffer[HEADPHONE_BUFFER_SAMPLES];

// Internal triangle generator state
static volatile float tri_value = 0.0f;
static volatile float tri_step;
static volatile int8_t tri_dir = 1;

// Forward declaration
static void fill_buffer(uint32_t offset, uint32_t length);

/**
 * @brief Initializes and starts the headphone output using the specified DAC and timer.
 *
 * This function sets a default frequency for the headphone output, enables
 * DMA transfer callbacks for both half-transfer and transfer-complete
 * interrupts, initializes the headphone buffer with a midpoint value, and
 * starts the timer and DAC in DMA circular mode to continuously drive the
 * headphone output.
 *
 * @param dac Pointer to the DAC handle to be used for headphone output.
 * @param tim Pointer to the timer handle to be used as the trigger for DAC updates.
 */
void headphone_out_start(DAC_HandleTypeDef *dac, TIM_HandleTypeDef *tim)
{
    headphone_set_frequency(500);

    // Fill buffer with midpoint for starters, makes it easier to check if the DAC is turned on
    //Just look for 3.3V / 2 voltage
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

/**
 * @brief Sets the frequency of the generated triangle wave for the headphones.
 *
 * @param [in] frequency The frequency of the triangle wave in Hz.
 */
void headphone_set_frequency(float frequency)
{
    // Compute triangle step: full swing (2 * HEADPHONE_DAC_MAX) * freq / sample rate
    tri_step = (2.0f * DAC_MAX_VALUE * frequency) / HEADPHONE_SAMPLE_RATE_HZ;
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

/**
 * @brief Fills a subset of the headphone_buffer array with a triangle wave, updating the internal state of the triangle generator
 *
 * @param offset Index of the first element to fill
 * @param length Number of elements to fill
 */
static void fill_buffer(uint32_t offset, uint32_t length)
{
    for (uint32_t i = 0; i < length; ++i)
    {
        // Store into buffer
        headphone_buffer[offset + i] = (uint16_t)tri_value;

        // Advance triangle value
        tri_value += tri_dir * tri_step;

        // Reverse direction at bounds
        if (tri_value >= DAC_MAX_VALUE)
        {
            tri_value = DAC_MAX_VALUE;
            tri_dir = -1;
        }
        else if (tri_value <= 0.0f)
        {
            tri_value = 0.0f;
            tri_dir = 1;
        }
    }
}
