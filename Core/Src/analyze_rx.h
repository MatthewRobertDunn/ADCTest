/*
 * analyze_rx.h
 *
 *  Created on: May 21, 2025
 *      Author: matthew
 * This file contains function prototypes to analyze the received signal from the RX coil
 */
#pragma once
#include <stdint.h>

void analyze_rx_start();
void analyze_rx(uint16_t* buffer, unsigned int length, unsigned int samplingRateHz);




