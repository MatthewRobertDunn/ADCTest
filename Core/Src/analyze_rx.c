#include "analyze_rx.h"
#include <stdint.h>

/**
 * @brief Analyze the received signal from the RX coil
 *
 * This function is currently a no-op, but is intended to be used to debug/analyze the received signal from the RX coil.
 *
 * @param buffer The buffer of samples from the RX coil
 * @param length The number of samples in the buffer
 * @param samplingRateHz The sampling rate of the ADC (in Hz)
 */
void analyze_rx(uint16_t* buffer, unsigned int length, unsigned int samplingRateHz)
{
	//TODO: Place your debug/analysis of RX buffer here.

}
