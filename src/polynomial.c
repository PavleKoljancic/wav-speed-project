#include "polynomial.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <immintrin.h>
double VALUE_AT(POLYNOMIAL p, double x)
{
    double result = 0;
    for (int i = 0; i < p.numOfCoefficients; i++)
    {
        result += p.coefficients[i] * pow(x, i);
    }
    return result;
}

void FREE_POLYNOMIAL(POLYNOMIAL p) 
{
    _mm_free(p.coefficients);
    p.coefficients=NULL;
    p.numOfCoefficients=-1;
}

POLYNOMIAL CREATE_POLYNOMIAL(int numOfCoefficients) 
{
    POLYNOMIAL p;
    p.coefficients = (double *)_mm_malloc(numOfCoefficients * sizeof(double), 32);
    p.numOfCoefficients = numOfCoefficients;
    return p;
}