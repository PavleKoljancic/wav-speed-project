#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>  // portable to all x86 compilers

void print128_num(__m128 var)
{
    float val[4];
    memcpy(val, &var, sizeof(val));
    printf("Numerical: %f %f  %f  %f  \n", 
           val[0], val[1], val[2], val[3]);
}
int main()
{
    __m128 vector1 = _mm_set_ps(5.5, 4.0, 2.5, 1.0); // high element first, opposite of C array order.  Use _mm_setr_ps if you want "little endian" element order in the source.
    __m128 vector2 = _mm_set_ps(7.0, 8.0, 9.0, 0.0);

    __m128 sum = _mm_add_ps(vector1, vector2); // result = vector1 + vector 2

    print128_num(sum);
    return 0;
}

