#include "polynomial.h"


double **GENERATE_SAMPLE_MATRIX(int interpolationPoints);

// X=X+rY
//void ADD_VECTOR(double *X, double *Y, double r, int n);

/*
MATRIX Square matrix for which we are finding the general solution
n size of row/column
adjustmentFactors the adjustment factors for the solution an double array of size n
returns the generalized solution
*/
double **FIND_GENERAL_SOLUTION(double **MATRIX, int n, double **adjustmentFactors);
void WRITE_INTO_POLYNOMIAL( POLYNOMIAL * p,double ** solutionMatrix,double *adjustmentFactors, double *Y);
void WRITE_INTO_POLYNOMIAL_SSE(POLYNOMIAL *p, double **solutionMatrix, double *adjustmentFactors, double *Y);