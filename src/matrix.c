#include "matrix.h"
#include <math.h>
#include <immintrin.h>
#include <string.h>
double **GENERATE_SAMPLE_MATRIX(int interpolationPoints)
{
    double **MATRIX = (double **)_mm_malloc(interpolationPoints * sizeof(double *), 32);
    for (int i = 0; i < interpolationPoints; i++)
    {

        MATRIX[i] = (double *)_mm_malloc(interpolationPoints * sizeof(double), 32);

        for (int j = 0; j < interpolationPoints; j++)
        {
            MATRIX[i][j] = pow(i, j);
        }
    }
    return MATRIX;
}

//X=X+rY
 void ADD_VECTOR(double *X, double *Y, double r, int n)
 {
     for (int i = 0; i < n; i++)
         X[i] += r * Y[i];
 }
// V1= V1+rV2
void ADD_VECTOR_SEE(__m256d *vector1 , __m256d vector2, double r)
{

    //(*vector1)= _mm256_fmadd_pd((*vector1), vector2, _mm256_set1_pd(r));
    (*vector1)=_mm256_add_pd((*vector1), _mm256_mul_pd(vector2, _mm256_set1_pd(r)));
}

/*
MATRIX Square matrix for which we are finding the general solution
n size of row/column
adjustmentFactors the adjustment factors for the solution an double array of size n
returns the generalized solution
*/
double **FIND_GENERAL_SOLUTION(double **MATRIX, int n, double **adjustmentFactors)
{
    // GJ Elimination till triangular MATRIX
    int step = 0;
    double **SolutionMatrix = (double **)_mm_malloc(2 * (n - 1) + 1 * sizeof(double *), 32);
    for (int i = 0; i < n - 1; i++)
    {
        SolutionMatrix[step] = (double *)_mm_malloc(n * sizeof(double), 32);
        step++;
        for (int j = 0; j < i + 1; j++)
            SolutionMatrix[step - 1][j] = 0;
        for (int j = i + 1; j < n; j++)
        {
            double ratio = 0.0;
            if (MATRIX[i][i] != 0)
            {
                ratio = -MATRIX[j][i] / MATRIX[i][i];
                ADD_VECTOR(MATRIX[j], MATRIX[i], ratio, n);
            }
            SolutionMatrix[step - 1][j] = ratio;
        }
    }

    for (int i = n - 1; i >= 0; i--)
    {
        SolutionMatrix[step] = (double *)_mm_malloc(n * sizeof(double), 32);
        step++;
        for (int j = i; j < n; j++)
            SolutionMatrix[step - 1][j] = 0;
        for (int j = i - 1; j >= 0; j--)
        {
            double ratio = 0.0;

            if (MATRIX[i][i] != 0)
            {
                ratio = -MATRIX[j][i] / MATRIX[i][i];
                ADD_VECTOR(MATRIX[j], MATRIX[i], ratio, n);
            }
               SolutionMatrix[step - 1][j] = ratio;
        }
    }

    // Adjustment factors

    for (int i = 0; i < n; i++)
        (*adjustmentFactors)[i] = MATRIX[i][i];

    return SolutionMatrix;
}
void WRITE_INTO_POLYNOMIAL(POLYNOMIAL *p, double **solutionMatrix, double *adjustmentFactors, double *Y)
{
    
    int step = 0;
    for (int i = 0; i < (*p).numOfCoefficients; i++)
    {
        double *V = solutionMatrix[step];
        ADD_VECTOR(Y, V, Y[i], (*p).numOfCoefficients);
        step++;
    }
    for (int i = (*p).numOfCoefficients - 2; i > 0; i--)
    {
        double *V = solutionMatrix[step];
        ADD_VECTOR(Y, V, Y[i], (*p).numOfCoefficients);
        step++;
    }

    for (int i = 0; i < (*p).numOfCoefficients; i++)
        (*p).coefficients[i] = Y[i] / adjustmentFactors[i];
}

typedef union __du {
    __m256d vector;
    double elements[4];
}VECTOR_ARRAY_UNION; 

void WRITE_INTO_POLYNOMIAL_SSE(POLYNOMIAL *p, double **solutionMatrix, double *adjustmentFactors, double *Y)
{
    __m256d vector1, vector2;

     vector1 = _mm256_loadu_pd(Y);
    int step = 0;
    for (int i = 0; i < (*p).numOfCoefficients; i++)
    {   
        vector2 = _mm256_load_pd( solutionMatrix[step]);
        ADD_VECTOR_SEE(&vector1, vector2, ((double *)&vector1)[i]);
        step++;
    }
    for (int i = (*p).numOfCoefficients - 2; i > 0; i--)
    {
        vector2 = _mm256_load_pd( solutionMatrix[step]);
        ADD_VECTOR_SEE(&vector1, vector2, ((double *)&vector1)[i]);
        step++;
    }

    for (int i = 0; i < (*p).numOfCoefficients; i++)
        (*p).coefficients[i] = ((double *)&vector1)[i] / adjustmentFactors[i];
}
