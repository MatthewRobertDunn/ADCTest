#include "analyze_rx.h"
#include <stdint.h>
#include "usbd_cdc_if.h"

#define USB_TRANSMIT_TIMEOUT_MS 10
uint8_t mplab_start_byte = 0x03;
uint8_t mplab_end_byte = 0xFC;
uint8_t mplab_prescaler = 0x01;
uint8_t mplab_zero = 0x00;

extern USBD_HandleTypeDef hUsbDeviceFS;

/**
 * @brief Blocking transmit of data over USB CDC
 * @param buffer: Buffer of data to transmit
 * @param length: Number of bytes to transmit
 *
 * @note This function will block until the data is transmitted or a timeout
 *       of 100 ms is reached. If the timeout is reached, the function will
 *       return without doing anything else.
 */
void send_to_usb(uint8_t *buffer, unsigned int length)
{
	// USB CDC Not connected, don't do anything
	if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED)
	{
		return;
	}
	// Do a blocking transmit
	uint32_t timeout = HAL_GetTick() + USB_TRANSMIT_TIMEOUT_MS; // 100 ms timeout
	while (CDC_Transmit_FS(buffer, length) == USBD_BUSY)
	{
		if (HAL_GetTick() > timeout)
		{
			return; // ignore
		}
	}
}

void wait_for_usb_to_complete(){

}

/**
 * @brief Sends data to MPLAB via USB.
 *
 * This function transmits a sequence of 16-bit data values to MPLAB over USB.
 * Each value is wrapped with a start byte and an end byte to delimit the data.
 *
 * @param buffer Pointer to the array of 16-bit data to send.
 * @param length Number of 16-bit data values in the buffer to be sent.
 */

void send_to_mplab(uint16_t *buffer, unsigned int length)
{
    uint8_t tx_buf[4];
    for (unsigned int i = 0; i < length; i++)
    {
        tx_buf[0] = mplab_start_byte;
        tx_buf[1] = buffer[i] & 0xFF;        // Low byte
        tx_buf[2] = (buffer[i] >> 8) & 0xFF; // High byte
        tx_buf[3] = mplab_end_byte;
        send_to_usb(tx_buf, 4);
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
	send_to_mplab(buffer, length);
}
