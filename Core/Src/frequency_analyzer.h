#pragma once
#include "config.h"
#define BUFFER_SIZE RX_ANALYSIS_SAMPLES     // Number of samples in buffer eg 1024
#define SAMPLE_RATE TX_SIGNAL_SAMPL_RATE_HZ // Sampling rate in Hz eg 200000
#define TARGET_FREQ TX_SIGNAL_FREQUENCY_HZ  // Target frequency in Hz eg 10000
#define M_PI 3.14159265358979323846

// Explicit initialization function
void frequency_analyzer_start(void);

// Analysis function
float frequency_analyzer_analyze(const float buffer[]);
