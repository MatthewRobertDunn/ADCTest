#include "startup.h"
#include "stm32g4xx_hal.h"
#include "tx_sine_generator.h"
#include "analyze_rx.h"
#include <stdbool.h>
#include "config.h"

// ADC buffer for 1024 samples (16-bit each)
uint16_t adcBuffer[RX_ANALYSIS_SAMPLES];

// Completion flag set by DMA complete callback
volatile bool adcDmaDone = false;

/**
 * Starts ADC in DMA mode, waits for 1024 samples,
 * and toggles PC6 (typically an LED) after each cycle.
 */
void begin(ADC_HandleTypeDef *rxADC, DAC_HandleTypeDef *txDAC, TIM_HandleTypeDef *txTIM)
{

    //Pregenerate our waveforms
    tx_sine_init_buffer();

    //Start our TX signal
    tx_sine_start(txDAC, txTIM);

    // Make sure LED is off initially
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

    while (1) {
        adcDmaDone = false;

        // Start ADC + DMA transfer of 1024 samples
        if (HAL_ADC_Start_DMA(rxADC, (uint32_t*)adcBuffer, 1024) != HAL_OK) {
            // Handle error here if needed
            continue;
        }

        // Wait (blocking) until DMA is done
        while (!adcDmaDone) {
            // You could add a timeout here if needed
        }

        // Analyze the received signal
        analyze_rx(adcBuffer, RX_ANALYSIS_SAMPLES, RX_ADC_SAMPLING_RATE_HZ);


        // Toggle PC6 to indicate transfer complete
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);

        // Wait 500 ms (blocking delay)
        HAL_Delay(500);
    }
}

/**
 * ADC DMA complete callback
 * This is automatically called by the HAL when DMA finishes.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        adcDmaDone = true;
    }
}
