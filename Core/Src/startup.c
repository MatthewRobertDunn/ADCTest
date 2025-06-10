#include "startup.h"
#include "stm32g4xx_hal.h"
#include <stdbool.h>
#include "config.h"
#include "tx_sine_generator.h"
#include "analyze_rx.h"
#include "headphone_out.h"

uint16_t adcBuffer[RX_ANALYSIS_SAMPLES];

// Completion flag set by DMA complete callback
volatile bool adcDmaDone = false;


/**
 * @brief Main entry point for the program.
 *
 * This function is the entry point for the program. It sets up the ADC,
 * generates the TX signal, starts the headphone output, and then enters an
 * infinite loop where it continuously captures 1024 samples from the ADC and
 * analyzes them. The result of the analysis is not currently used, but the
 * program could be modified to use it as needed.
 *
 * @param rxADC The ADC peripheral to use for capturing the received signal
 * @param txDAC The DAC peripheral to use for generating the TX signal
 * @param txTIM The timer peripheral to use as the trigger source for the TX DAC
 * @param headphoneDAC The DAC peripheral to use for generating the headphone output
 * @param headphoneTIM The timer peripheral to use as the trigger source for the headphone DAC
 */
void begin(ADC_HandleTypeDef *rxADC, DAC_HandleTypeDef *txDAC, TIM_HandleTypeDef *txTIM, DAC_HandleTypeDef *headphoneDAC, TIM_HandleTypeDef *headphoneTIM)
{

    // Pregenerate our waveforms
    tx_sine_init_buffer();

    // Start our TX signal
    tx_sine_start(txDAC, txTIM);

    // Start our headphone signal
    headphone_out_start(headphoneDAC, headphoneTIM);

    // Make sure LED is off initially
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

    //start our rx_analyze module
    analyze_rx_start();

    while (1)
    {
        adcDmaDone = false;

        // Start ADC + DMA transfer of 1024 samples
        if (HAL_ADC_Start_DMA(rxADC, (uint32_t *)adcBuffer, RX_ANALYSIS_SAMPLES) != HAL_OK)
        {
            // Handle error here if needed
            continue;
        }

        // Wait (blocking) until DMA is done
        while (!adcDmaDone)
        {
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        adcDmaDone = true;
    }
}
