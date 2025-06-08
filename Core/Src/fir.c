//fir.c
#include "fir.h"
#include "config.h"

#define COEFF_VECTOR_B_SIZE     5

/* Filter parameter Q: not used */
#define FILTER_PARAM_Q_NOT_USED 0

/* Filter parameter R: gain in the range [0:7] */
#define GAIN                    1

/* Filter parameter P: number of feed-forward taps or coefficients in the range [2:127] */
#define TAPS_COUNT 65

/* Throughput parameter: extra space in the input buffer (minimum: 0) */
#define MEMORY_PARAMETER_D1     54  //must be less than 255 - TAPS_COUNT, must be multiple of RX_ANALYSIS_SAMPLES
/* Throughput parameter: extra space in the output buffer (minimum: 1) */
#define MEMORY_PARAMETER_D2     (MEMORY_PARAMETER_D1 + 1)

/* FMAC internal memory configuration: size of the input buffer */
#define INPUT_BUFFER_SIZE       (TAPS_COUNT + MEMORY_PARAMETER_D1)

/* FMAC internal memory configuration: base address of the input buffer */
#define OUTPUT_BUFFER_BASE      TAPS_COUNT + INPUT_BUFFER_SIZE

/* FMAC internal memory configuration: size of the input buffer */
#define OUTPUT_BUFFER_SIZE      MEMORY_PARAMETER_D2

#define COEFFICIENT_BUFFER_BASE 0

#define POLLING_TIMEOUT         1000

/* Array of filter coefficients B (feed-forward taps) in Q1.15 format */
static int16_t filter_coefficients[TAPS_COUNT];


void error_handler(){

}

void fir_start(FMAC_HandleTypeDef* hfmac, FMAC_FilterConfigTypeDef* sFmacConfig)
{
	  sFmacConfig->InputBaseAddress  = TAPS_COUNT;
	  sFmacConfig->InputBufferSize   = INPUT_BUFFER_SIZE;
	  sFmacConfig->InputThreshold    = FMAC_THRESHOLD_1;
	  sFmacConfig->CoeffBaseAddress  = TAPS_COUNT;
	  sFmacConfig->CoeffBufferSize   = TAPS_COUNT;
	  sFmacConfig->OutputBaseAddress = OUTPUT_BUFFER_BASE;
	  sFmacConfig->OutputBufferSize  = OUTPUT_BUFFER_SIZE;
	  sFmacConfig->OutputThreshold   = FMAC_THRESHOLD_1;
	  sFmacConfig->pCoeffA           = NULL;
	  sFmacConfig->CoeffASize        = 0;
	  sFmacConfig->pCoeffB           = filter_coefficients;
	  sFmacConfig->CoeffBSize        = COEFF_VECTOR_B_SIZE;
	  sFmacConfig->Filter            = FMAC_FUNC_CONVO_FIR;
	  sFmacConfig->InputAccess       = FMAC_BUFFER_ACCESS_POLLING;
	  sFmacConfig->OutputAccess      = FMAC_BUFFER_ACCESS_POLLING;
	  sFmacConfig->Clip              = FMAC_CLIP_DISABLED;
	  sFmacConfig->P                 = TAPS_COUNT;
	  sFmacConfig->Q                 = FILTER_PARAM_Q_NOT_USED;
	  sFmacConfig->R                 = GAIN;

	  if (HAL_FMAC_FilterConfig(hfmac, sFmacConfig) != HAL_OK)
	  {
	    /* Configuration Error */
		error_handler();
	    return;
	  }
}


void fir_filter(int16_t* input, int16_t* output, uint16_t length)
{

	  //preload buffer, only needed for streaming data instead of blocks
	  if (HAL_FMAC_FilterPreload(g_hfmac, NULL, 0,
	                             NULL, 0) != HAL_OK)
	  {
	    error_handler();
	    return;
	  }


	 //Configure the output buffer
	  uint16_t samplesWritten = length;
	  if (HAL_FMAC_FilterStart(g_hfmac, output, &samplesWritten) != HAL_OK)
	  {
	    /* Processing Error */
	    error_handler();
	  }


	  //
	  uint16_t expectedInputSize = length;
	  if (HAL_FMAC_AppendFilterData(g_hfmac,
	                                input,
	                                &expectedInputSize) != HAL_OK)
	  {
	    error_handler();
	    return;
	  }


	  if (HAL_FMAC_PollFilterData(g_hfmac, POLLING_TIMEOUT) != HAL_OK)
	  {
		  error_handler();
		  return;
	  }

	  return;
}




