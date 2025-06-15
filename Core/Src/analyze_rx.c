#include "analyze_rx.h"
#include <stdint.h>
#include "debug_output.h"
#include "sample_filter.h"
#include "frequency_analyzer.h"

// Define the filter
SampleFilter rx_filter;

void analyze_rx_start()
{
    frequency_analyzer_start();
}

/**
 * @brief Converts an array of 16-bit ADC values to a floating-point array in the range [-1.0f, 1.0f]
 *
 * @param[in] input: Pointer to the input array of 16-bit ADC values
 * @param[out] output: Pointer to the output array of floating-point values
 * @param[in] length: Length of the input and output arrays
 */
void adc_to_float_buffer(const uint16_t *input, float *output, unsigned int length) {
    const float scale = 2.0f / 4095.0f;
    for (unsigned int i = 0; i < length; i++) {
        output[i] = input[i] * scale - 1.0f;
    }
}

/**
 * @brief Analyze the received signal from the RX coil
 *
 * This function is currently a no-op, but is intended to be used to debug/analyze the received signal from the RX coil.
 *
 * @param buffer The buffer of samples from the RX coil
 * @param length The number of samples in the buffer
 * @param samplingRateHz The sampling rate of the ADC (in Hz)
 */
void analyze_rx(uint16_t *buffer, unsigned int length, unsigned int samplingRateHz)
{
	// TODO: Place your debug/analysis of RX buffer here.
	//usb_send_as_text(buffer, length);


    //Convert our buffer to floating points
    float float_buffer[length];
    adc_to_float_buffer(buffer, float_buffer, length);

    //Get the magnitude of our signal
    float magnitude = frequency_analyzer_analyze(float_buffer);

    //Do something with the result.


}
