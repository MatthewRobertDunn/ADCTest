#include "debug_output.h"

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
void usb_send(uint8_t *buffer, unsigned int length)
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


/**
 * @brief Sends data to MPLAB via USB.
 *
 * This function transmits a sequence of 16-bit data values to MPLAB over USB.
 * Each value is wrapped with a start byte and an end byte to delimit the data.
 *
 * @param buffer Pointer to the array of 16-bit data to send.
 * @param length Number of 16-bit data values in the buffer to be sent.
 */

void usb_send_samples_to_mplab(uint16_t *buffer, unsigned int length)
{
    uint8_t tx_buf[4];
    for (unsigned int i = 0; i < length; i++)
    {
        tx_buf[0] = mplab_start_byte;
        tx_buf[1] = buffer[i] & 0xFF;        // Low byte
        tx_buf[2] = (buffer[i] >> 8) & 0xFF; // High byte
        tx_buf[3] = mplab_end_byte;
        usb_send(tx_buf, 4);
    }
}

/**
 * @brief Sends data to MPLAB via USB as a human-readable text.
 *
 * This function transmits a sequence of 16-bit data values to MPLAB over USB.
 * Each value is formatted as a string and separated by a comma. The last value
 * is followed by a CR/LF pair.
 *
 * @param buffer Pointer to the array of 16-bit data to send.
 * @param length Number of 16-bit data values in the buffer to be sent.
 */
void usb_send_as_text(uint16_t *buffer, unsigned int length)
{
    char txt_buf[7];  // Max 5 digits for uint16_t + comma + null terminator

    for (unsigned int i = 0; i < length; i++)
    {
        if (i < length - 1)
            sprintf(txt_buf, "%u,", buffer[i]);
        else
            sprintf(txt_buf, "%u", buffer[i]);

        usb_send((uint8_t *)txt_buf, strlen(txt_buf));
    }

    // Send final CR/LF once
    usb_send((uint8_t *)"\r\n", 2);
}