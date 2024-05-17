#include "samplewindow.h"
#include <math.h>
SAMPLE_WINDOW CREATE_SAMPLE_WINDOW(int windowSize, double *samples, int lower)
{
    SAMPLE_WINDOW sw;
    sw.windowSize = windowSize;
    sw.samples = samples;
    sw.head = 0;
    sw.lower = lower;
    sw.upper = lower + windowSize - 1;
    return sw;
}

void ADD_SAMPLE(SAMPLE_WINDOW *sw, double sample)
{

    (*sw).samples[(*sw).head] = sample;
    (*sw).head = ((*sw).head + 1) % (*sw).windowSize;
    (*sw).lower++;
    (*sw).upper++;
}

void *GET_SAMPLES(double *dest, SAMPLE_WINDOW sw)
{
    for (int i = 0; i < sw.windowSize; i++)
        dest[i] = sw.samples[(sw.head + i) % sw.windowSize];
}

int GET_SHIFT(SAMPLE_WINDOW sw, double x, int Limit)
{

    double upper;
    if (x - sw.windowSize / 2.0 < 0.0)
        upper = sw.windowSize-1;
    
    else if (x + sw.windowSize / 2.0 > Limit)
        upper = Limit - 1;
    
    else
        upper = ceil((x + sw.windowSize / 2.0))-1;
    
    int u = upper;
    return u-sw.upper;
}