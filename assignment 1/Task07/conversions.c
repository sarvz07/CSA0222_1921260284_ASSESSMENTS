#include <stdio.h>
#include <limits.h>

int main(void)
{
    unsigned char byte = (unsigned char)300;
    double rate = 1.15;
    unsigned int positive = 1u;
    int negative = -1;

    printf("5 / 2               = %d, but 5 / 2.0 = %.1f\n", 5 / 2, 5 / 2.0);
    printf("450 / 100           = %d, but 450 / 100.0 = %.2f\n",
           450 / 100, 450 / 100.0);
    printf("(unsigned char)300  = %u\n", (unsigned)byte);
    printf("(int)3000000000L    = %d (INT_MAX = %d)\n", (int)3000000000L, INT_MAX);
    printf("1.15 * 100          = %.17g\n", rate * 100.0);
    printf("(long)(1.15*100)    = %ld, with +0.5 before cast = %ld\n",
           (long)(rate * 100.0), (long)(rate * 100.0 + 0.5));
    printf("1u + (-1)           = %u\n", positive + negative);
    printf("-7 %% 3              = %d\n", -7 % 3);
    return 0;
}
