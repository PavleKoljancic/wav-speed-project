#include <stdint.h>

typedef struct __swindow
{
    int windowSize;
    double *samples;
    int head;
    int lower;
    int upper;
    uint32_t shift;
} SAMPLE_WINDOW;

SAMPLE_WINDOW CREATE_SAMPLE_WINDOW(int windowSize, double *samples, int lower);

void ADD_SAMPLE(SAMPLE_WINDOW *sw, double sample);

void *GET_SAMPLES(double *dest, SAMPLE_WINDOW sw);

int GET_SHIFT(SAMPLE_WINDOW sw, double x, int Limit);