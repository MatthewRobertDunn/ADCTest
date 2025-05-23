/*
 * Project-wide configuration settings
 */

#pragma once
#include "stm32g4xx_hal.h"

//--------Global configuration settings--------
// Set to the value of your APB1 Clock, copy from ADCTest.io
#define APB1_CLOCK_HZ 168000000

// Set to the value of your ADC Clock (from CubeMX)
#define ADC_CLOCK_HZ 168000000

// number of ADC conversion clocks per sample (fixed by hardware)
#define ADC_CONVERSION_CLOCKS 12.5

// Some generic stuff to do with the DAC
#define DAC_MIDPOINT 2048
#define DAC_AMPLITUDE 2047 // max amplitude to avoid overflow
#define DAC_MAX_VALUE DAC_MIDPOINT + DAC_AMPLITUDE

//----------Configuration for the RX signal-------

// Perscaler for the RX DAC (from CubeMX)
#define RX_ADC_PRESCALER 32

#define RX_ADC_CLOCK_HZ ADC_CLOCK_HZ / RX_ADC_PRESCALER

// Number of ADC sampling clocks per sample (from CubeMX)
#define RX_ADC_SAMPLING_CLOCKS 12.5

// Total number of ADC sampling clocks per cycle
#define RX_TOTAL_CLOCKS (RX_ADC_SAMPLING_CLOCKS + ADC_CONVERSION_CLOCKS)

// Sampling rate of RX DAC (in Hz), based on ADC sampling rate
#define RX_ADC_SAMPLING_RATE_HZ (RX_ADC_CLOCK_HZ / RX_TOTAL_CLOCKS)

// Number of samples to analyze from the RX coil per cycle; make it 20mS worth
// #define RX_ANALYSIS_SAMPLES ((unsigned int)(RX_ADC_SAMPLING_RATE_HZ * 0.02))

#define RX_ANALYSIS_SAMPLES 4200

//----------Configuration for the TX signal-------

// The channel to use for the TX signal (from CubeMX)
#define TX_DAC_CHANNEL DAC_CHANNEL_1

// Desired transmission frequency for the TX coil (user configurable)
#define TX_SIGNAL_FREQUENCY_HZ 10000

// Timer autoreload value used to drive the TX DAC (from CubeMX)
#define TX_SIGNAL_TIMER_ARR 839

// Sampling rate of TX DAC (in Hz), based on timer ARR and APB1 clock
#define TX_SIGNAL_SAMPL_RATE_HZ (APB1_CLOCK_HZ / (TX_SIGNAL_TIMER_ARR + 1))

// Number of samples per signal wavelength
#define TX_SIGNAL_WAVELENGTH_SAMPLES (TX_SIGNAL_SAMPL_RATE_HZ / TX_SIGNAL_FREQUENCY_HZ)




//--------Configuration for the headphones

// The channel to use for the TX signal (from CUBEMX)
#define HEADPHONE_DAC_CHANNEL DAC_CHANNEL_2

// Timer autoreload value used to drive the headphone DAC (from CubeMX)
#define HEADPHONE_TIMER_ARR 7635

// Sampling rate of headphone DAC (in Hz), based on timer ARR and APB1 clock
#define HEADPHONE_SAMPL_RATE_HZ (APB1_CLOCK_HZ / (HEADPHONE_TIMER_ARR + 1))

//Number of samples in the headphone circular buffer
#define HEADPHONE_BUFFER_SAMPLES 1100