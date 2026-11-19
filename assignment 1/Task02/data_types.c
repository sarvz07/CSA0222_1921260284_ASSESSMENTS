#include <stdio.h>
#include <limits.h>
#include <float.h>

int main(void)
{
    float f = 0.0f;
    double d = 0.0;
    long paise = 0L;
    int i;

    printf("char       %2zu bytes  [%d, %d]\n", sizeof(char), CHAR_MIN, CHAR_MAX);
    printf("short      %2zu bytes  [%d, %d]\n", sizeof(short), SHRT_MIN, SHRT_MAX);
    printf("int        %2zu bytes  [%d, %d]\n", sizeof(int), INT_MIN, INT_MAX);
    printf("long       %2zu bytes  [%ld, %ld]\n", sizeof(long), LONG_MIN, LONG_MAX);
    printf("long long  %2zu bytes  [%lld, %lld]\n", sizeof(long long), LLONG_MIN, LLONG_MAX);
    printf("unsigned short %2zu bytes [0, %u]\n", sizeof(unsigned short), USHRT_MAX);
    printf("float      %2zu bytes  [%g, %g], %d decimal digits\n",
           sizeof(float), FLT_MIN, FLT_MAX, FLT_DIG);
    printf("double     %2zu bytes  [%g, %g], %d decimal digits\n",
           sizeof(double), DBL_MIN, DBL_MAX, DBL_DIG);

    printf("16-bit unsigned register after 65535: %u\n", (unsigned short)65536);

    for (i = 0; i < 4000; i++) {
        f += 0.1f;
        d += 0.1;
        paise += 10L;
    }
    printf("4000 x Rs 0.10 -> float %.8f, double %.8f, integer %ld.%02ld\n",
           f, d, paise / 100, paise % 100);
    printf("expected 400.00000000, float error = %.8f\n", f - 400.0f);

    printf("(int)(70000L * 70000L) = %d, full value = %ld\n",
           (int)(70000L * 70000L), 70000L * 70000L);
    return 0;
}
