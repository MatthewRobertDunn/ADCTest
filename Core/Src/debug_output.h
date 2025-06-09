#pragma once
#include <stdint.h>
void usb_send(uint8_t *buffer, unsigned int length);
void usb_send_samples_to_mplab(uint16_t *buffer, unsigned int length);
void usb_send_as_text(uint16_t *buffer, unsigned int length);
