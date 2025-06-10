
#ifndef SAMPLEFILTER_H_
#define SAMPLEFILTER_H_

/*

FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 250000 Hz

* 0 Hz - 10000 Hz
gain = 0
desired attenuation = -10 dB
actual attenuation = -16.71983860692917 dB

* 11000 Hz - 12000 Hz
gain = 1
desired ripple = 5 dB
actual ripple = 1.4188648649899362 dB

* 13000 Hz - 25000 Hz
gain = 0
desired attenuation = -10 dB
actual attenuation = -16.71983860692917 dB

*/

#define SAMPLEFILTER_TAP_NUM 41

typedef struct
{
    float history[SAMPLEFILTER_TAP_NUM];
    unsigned int last_index;
} SampleFilter;

unsigned int SampleFilter_run(SampleFilter *f, float *input, float *output, unsigned int length);
void SampleFilter_init(SampleFilter *f);
void SampleFilter_put(SampleFilter *f, float input);
float SampleFilter_get(SampleFilter *f);
#endif
