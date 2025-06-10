

#include "sample_filter.h"

static float filter_taps[SAMPLEFILTER_TAP_NUM] = {
    -1782602357767.64,
    2095297255781.407,
    -1379195695042.4727,
    1398625849855.5117,
    1607575306443.8113,
    -2549653655968.633,
    990404391196.423,
    -583459131841.0562,
    -1695406844398.456,
    2662601597609.127,
    -776155130699.3868,
    249024583293.89105,
    1477560390221.4546,
    -2165865343256.721,
    -316975526545.1579,
    269203717879.69778,
    -1249398938017.6335,
    2747403307825.0527,
    1088023452916.0675,
    -913270081799.8206,
    -2347474295370.7563,
    -913270081799.8206,
    1088023452916.0675,
    2747403307825.0527,
    -1249398938017.6335,
    269203717879.69778,
    -316975526545.1579,
    -2165865343256.721,
    1477560390221.4546,
    249024583293.89105,
    -776155130699.3868,
    2662601597609.127,
    -1695406844398.456,
    -583459131841.0562,
    990404391196.423,
    -2549653655968.633,
    1607575306443.8113,
    1398625849855.5117,
    -1379195695042.4727,
    2095297255781.407,
    -1782602357767.64};

unsigned int SampleFilter_run(SampleFilter *f, float *input, float *output, unsigned int length)
{
    unsigned int write_index = 0;
    if (length <= SAMPLEFILTER_TAP_NUM)
    {
        // Error, data must be longer than number of taps
        return 0;
    }

    // Warm up the filter
    int i;
    for (i = 0; i < SAMPLEFILTER_TAP_NUM; i++)
    {
        SampleFilter_put(f, input[i]);
    }

    for (; i < length - SAMPLEFILTER_TAP_NUM; i++)
    {
        SampleFilter_put(f, input[i]);
        output[write_index] = SampleFilter_get(f);
        write_index++;
    }

    return write_index;
}

void SampleFilter_init(SampleFilter *f)
{
    int i;
    for (i = 0; i < SAMPLEFILTER_TAP_NUM; ++i)
        f->history[i] = 0;
    f->last_index = 0;
}

void SampleFilter_put(SampleFilter *f, float input)
{
    f->history[f->last_index++] = input;
    if (f->last_index == SAMPLEFILTER_TAP_NUM)
        f->last_index = 0;
}

float SampleFilter_get(SampleFilter *f)
{
    double acc = 0;
    int index = f->last_index, i;
    for (i = 0; i < SAMPLEFILTER_TAP_NUM; ++i)
    {
        index = index != 0 ? index - 1 : SAMPLEFILTER_TAP_NUM - 1;
        acc += f->history[index] * filter_taps[i];
    };
    return acc;
}
