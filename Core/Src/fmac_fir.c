// fir.c
#include "fmac_fir.h"
#include <stdint.h>
#include <stdbool.h>
#include <stm32g4xx_hal_fmac.h>
#include "debug_output.h"
#include "prng.h"

/* Filter parameter Q: not used */
#define FILTER_PARAM_Q_NOT_USED 0

/* Filter parameter R: gain in the range [0:7] */
#define GAIN 1

/* Filter parameter P: number of feed-forward taps or coefficients in the range [2:127] */
#define TAPS_COUNT 65

/* Array of filter coefficients B (feed-forward taps) in Q1.15 format */
//Should match https://imgur.com/a/fiFAxEh
static int16_t filter_coefficients[TAPS_COUNT] = 
{
	-165, 603, -388, -555, -558, -542, -502, -416, -267, -59, 190, 452, 691, 871, 958, 929, 777, 509, 150, -257, -663, -1013, -1257, -1358, -1293, -1064, -694, -225, 285, 772, 1175, 1439, 1531, 1439, 1175, 772, 285, -225, -694, -1064, -1293, -1358, -1257, -1013, -663, -257, 150, 509, 777, 929, 958, 871, 691, 452, 190, -59, -267, -416, -502, -542, -558, -555, -388, 603, -165
};

/* Throughput parameter: extra space in the input buffer (minimum: 0) */
#define MEMORY_PARAMETER_D1 54 // must be less than 255 - TAPS_COUNT, must be multiple of RX_ANALYSIS_SAMPLES
/* Throughput parameter: extra space in the output buffer (minimum: 1) */
#define MEMORY_PARAMETER_D2 (MEMORY_PARAMETER_D1 + 1)

/* FMAC internal memory configuration: size of the input buffer */
#define INPUT_BUFFER_SIZE (TAPS_COUNT + MEMORY_PARAMETER_D1)

/* FMAC internal memory configuration: base address of the input buffer */
#define OUTPUT_BUFFER_BASE TAPS_COUNT + INPUT_BUFFER_SIZE

/* FMAC internal memory configuration: size of the input buffer */
#define OUTPUT_BUFFER_SIZE MEMORY_PARAMETER_D2

#define COEFFICIENT_BUFFER_BASE 0

#define POLLING_TIMEOUT 1000



FMAC_FilterConfigTypeDef sFmacConfig;
FMAC_HandleTypeDef *g_hfmac = NULL;

void error_handler()
{
}


/**
 * @brief  Initialize the FMAC filter with the given configuration and start the filter.
 * @param  hfmac pointer to a FMAC_HandleTypeDef structure that contains
 *         the configuration information for FMAC module.
 * @note   This function configures the FMAC according to the parameters
 *         specified in the FMAC_FilterConfigTypeDef structure, and
 *         starts the filter.
 * @retval None
 */
void fmac_fir_start(FMAC_HandleTypeDef *hfmac)
{
	sFmacConfig.InputBaseAddress = TAPS_COUNT;
	sFmacConfig.InputBufferSize = INPUT_BUFFER_SIZE;
	sFmacConfig.InputThreshold = FMAC_THRESHOLD_1;
	sFmacConfig.CoeffBaseAddress = 0;
	sFmacConfig.CoeffBufferSize = TAPS_COUNT;
	sFmacConfig.OutputBaseAddress = OUTPUT_BUFFER_BASE;
	sFmacConfig.OutputBufferSize = OUTPUT_BUFFER_SIZE;
	sFmacConfig.OutputThreshold = FMAC_THRESHOLD_1;
	sFmacConfig.pCoeffA = NULL;
	sFmacConfig.CoeffASize = 0;
	sFmacConfig.pCoeffB = filter_coefficients;
	sFmacConfig.CoeffBSize = TAPS_COUNT;
	sFmacConfig.Filter = FMAC_FUNC_CONVO_FIR;
	sFmacConfig.InputAccess = FMAC_BUFFER_ACCESS_POLLING; //Poll for results, wait for interrupt, or use DMA
	sFmacConfig.OutputAccess = FMAC_BUFFER_ACCESS_POLLING;
	sFmacConfig.Clip = FMAC_CLIP_DISABLED;
	sFmacConfig.P = TAPS_COUNT;
	sFmacConfig.Q = FILTER_PARAM_Q_NOT_USED;
	sFmacConfig.R = GAIN;

	if (HAL_FMAC_FilterConfig(hfmac, &sFmacConfig) != HAL_OK)
	{
		/* Configuration Error */
		error_handler();
		return;
	}

	g_hfmac = hfmac;
}


/**
 * @brief  Processes input data through the FIR filter and generates filtered output.
 * @param  input: Pointer to the input data array.
 * @param  output: Pointer to the output data array where the filtered data will be stored.
 * @param  length: Number of samples to be processed.
 * @retval true if filtering is successful, false if an error occurs.
 * @note   This function utilizes the FMAC peripheral for FIR filtering.
 */
bool fmac_fir_filter(int16_t *input, int16_t *output, uint16_t length)
{
	// preload buffer, only needed for streaming data instead of blocks
	if (HAL_FMAC_FilterPreload(g_hfmac, NULL, 0,
							   NULL, 0) != HAL_OK)
	{
		error_handler();
		return false;
	}



	// Configure the output buffer
	uint16_t samplesWritten = length;
	if (HAL_FMAC_FilterStart(g_hfmac, output, &samplesWritten) != HAL_OK)
	{
		/* Processing Error */
		error_handler();
		return false;
	}

	// Configure the input buffer
	uint16_t expectedInputSize = length;
	if (HAL_FMAC_AppendFilterData(g_hfmac,
								  input,
								  &expectedInputSize) != HAL_OK)
	{
		error_handler();
		return false;
	}

	// Do the actual filtering, actual work done will be in samplesWritten
	if (HAL_FMAC_PollFilterData(g_hfmac, POLLING_TIMEOUT) != HAL_OK)
	{
		error_handler();
		return false;
	}

	return true;
}



/**
 * @brief Generate a random 16-bit unsigned integer
 *
 * @return A 16-bit unsigned integer with a random value between 0 and 0xFFFF
 */
uint16_t get_random_uint16(void)
{
   return (uint16_t)(next_random_uint32_t() & 0xFFFF);
}



/**
 * @brief  Test the FIR filter implementation using random noise as input.
 * @note   This function generates white noise as input data, processes it through the FIR filter,
 *         and continuously sends the filtered output via USB for analysis. The output can be used
 *         to determine the frequency response of the filter. The function runs indefinitely, sending
 *         the filtered data every 30 seconds.
 */
void fir_test(){
	uint16_t input[1024];

	//Fill input with white noise to determine frequency response
	for(int i = 0; i < 1024; i++)
	{
		input[i] = get_random_uint16();
	}


	uint16_t output[1024];
	//Null out output so its clean
	for(int i=0; i < 1024; i++)
	{
		output[i] = 0;
	}

	if(fmac_fir_filter((int16_t*)input, (int16_t*)output, 1024))
	{
		while(true)
		{//RUN FFT and print results?
			usb_send_as_text(output, 1024);
			HAL_Delay(30000);
		}
	}
}
