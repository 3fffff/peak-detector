#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "peak_detector.h"

struct peak_detector pd = {
    0,
};

void peak_det_init(struct peak_detector *pd,
                   // uint32_t lag,
                   float threshold_eps_mul,
                   float influence, float influenceAvg, uint8_t use_plus, uint8_t use_minus)
{
    pd->lag = PD_DEFAULT_LAG;
    pd->threshold = threshold_eps_mul;
    pd->influence = influence;
    pd->influenceAvg = influenceAvg;
    pd->idx = 0;
    pd->plus = use_plus;
    pd->minus = use_minus;
    pd->sum = 0;
    pd->avgSum = 0;
    for (int i = 0; i < pd->lag; i++)
        pd->buffer[i] = 0;
}

void peak_det_push(struct peak_detector *pd, float sample)
{
    pd->prev_peak = pd->peak;
    pd->peak = 0;
    float sampleAvg = sample;
    float deviation = sample - pd->avgSum;
    if (fabsf(deviation) > pd->threshold * pd->prevStddev && pd->prevStddev != 0 && pd->idx > pd->lag)
    {
        sample = pd->influence * sample + (1.0 - pd->influence) * pd->prevData;
        sampleAvg = pd->influenceAvg * sample + (1.0 - pd->influenceAvg) * pd->prevData;
        if (sample > 1.07F * pd->avgSum && pd->plus == 1)
            pd->peak = 1;
        else if (sample < -1.07F * pd->avgSum && pd->minus == 1)
            pd->peak = -1;
    }

    uint32_t nextCounter = pd->idx;
    int32_t oldCounter = nextCounter - pd->lag;
    float oldval = pd->buffer[oldCounter < 0 ? nextCounter : oldCounter % pd->lag];
    float prevAvg = pd->avgSum;
    if (oldCounter >= 0)
        pd->sum -= pd->buffer[oldCounter % pd->lag];
    if (oldCounter < 0)
        pd->buffer[nextCounter] = sampleAvg;
    else
        pd->buffer[oldCounter % pd->lag] = sampleAvg;
    pd->sum += sampleAvg;
    pd->idx++;
    if (oldCounter < 0)
        pd->avgSum = pd->sum / (nextCounter + 1);
    else
        pd->avgSum += (sample - oldval) / pd->lag;
    // pd->avgSum = pd->sum / pd->lag;

    pd->Var = pd->Var + (powf(sample - prevAvg, 2) - powf(oldval - prevAvg, 2) - (powf(sample - oldval, 2) / pd->lag)) / pd->lag;

    pd->prevStddev = sqrtf(fabsf(pd->Var));
    pd->prevData = sample;
}

int8_t peak_det_get_peak(struct peak_detector *pd)
{
    return pd->peak;
}

int8_t* peak_detect(float *y, uint32_t len)
{

    int8_t *result = malloc(len);

    peak_det_init(&pd, 4.0, 0.01F, 0.9F, 1, 0);
    for (int i = 0; i < len; i++)
    {
        peak_det_push(&pd, y[i]);
        result[i] = pd.peak;
    }
    return result;
}
