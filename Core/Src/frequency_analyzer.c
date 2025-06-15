#include "frequency_analyzer.h"
#include <math.h>

// Precomputed arrays
static float cos_arr[BUFFER_SIZE];
static float sin_arr[BUFFER_SIZE];
static float window[BUFFER_SIZE];

/**
 * Initializes the frequency analyzer by computing the Hanning window and the
 * trigonometric components for the DFT.
 *
 * This function must be called before calling frequency_analyzer_analyze.
 */
void frequency_analyzer_start(void)
{
    const float w = 2.0f * (float)M_PI * ((float)TARGET_FREQ) / ((float)SAMPLE_RATE);

    for (int n = 0; n < BUFFER_SIZE; n++)
    {
        // Hanning window: 0.5*(1 - cos(2πn/(N-1)))
        window[n] = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * n / (BUFFER_SIZE - 1)));
        // Trigonometric components
        cos_arr[n] = cosf(w * n);
        sin_arr[n] = sinf(w * n);
    }
}

/**
 * Analyzes the given buffer to compute the magnitude of the frequency component
 * at the target frequency defined by TARGET_FREQ.
 *
 * This function performs a single bucket Discrete Fourier Transform (DFT) using precomputed
 * trigonometric and windowing coefficients to calculate the real and imaginary
 * components of the frequency, then computes the magnitude of the frequency
 * component.
 *
 * @param buffer An array of float samples to be analyzed.
 * @return The magnitude of the frequency component
 */

float frequency_analyzer_analyze(const float buffer[])
{
    // Compute dot products
    float real = 0.0f;
    float imag = 0.0f;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        float sample = buffer[i] * window[i];
        real += sample * cos_arr[i];
        imag -= sample * sin_arr[i]; // Negative sign per original Python
    }

    // Calculate magnitude (scaled by 2/N)
    float magnitude = (4.0f / BUFFER_SIZE) * sqrtf(real * real + imag * imag);
    return magnitude;
}

/*
Quality Assurance:
$ ./analyzer_app.exe
Test 1: Pure sine wave at 11000Hz
  Expected: 0.8000, Got: 0.7992

Test 2: Half amplitude
  Expected: 0.4000, Got: 0.3996

Test 3: DC offset
  Expected: ~0.1000, Got: 0.0999

Test 4: Different frequency (5kHz) Attentuation > -100db
  Expected: ~0.0000, Got: 0.0000

*/