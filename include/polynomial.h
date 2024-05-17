
typedef struct polynomial
{
    double * coefficients;
    int numOfCoefficients;

}
POLYNOMIAL;

double VALUE_AT(POLYNOMIAL p,double x);

void FREE_POLYNOMIAL(POLYNOMIAL p);
POLYNOMIAL CREATE_POLYNOMIAL(int numOfCoefficients);