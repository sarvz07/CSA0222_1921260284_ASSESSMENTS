#include <stdio.h>
#include <limits.h>

#define BILL_PAISE 12345L

static void compare(int n)
{
    float f = 0.0f;
    double d = 0.0;
    int total_int = 0;
    long total_long = 0L;
    long long total_ll = 0LL;
    long exact = BILL_PAISE * (long)n;
    int i;

    for (i = 0; i < n; i++) {
        f += 123.45f;
        d += 123.45;
        total_int += (int)BILL_PAISE;
        total_long += BILL_PAISE;
        total_ll += (long long)BILL_PAISE;
    }

    printf("n = %d, each bill Rs 123.45, expected Rs %ld.%02ld\n",
           n, exact / 100, exact % 100);
    printf("  float     = %.6f  (diff from exact: %+.6f)\n",
           f, (double)f - (double)exact / 100.0);
    printf("  double    = %.6f  (diff from exact: %+.6f)\n",
           d, d - (double)exact / 100.0);
    printf("  int       = %d\n", total_int);
    printf("  long      = %ld\n", total_long);
    printf("  long long = %lld\n", total_ll);
    printf("  paise     = %ld.%02ld\n", exact / 100, exact % 100);
}

int main(void)
{
    printf("sizes: int=%zu, long=%zu, long long=%zu, float=%zu, double=%zu\n",
           sizeof(int), sizeof(long), sizeof(long long),
           sizeof(float), sizeof(double));
    printf("INT_MAX=%d, LONG_MAX=%ld, LLONG_MAX=%lld\n",
           INT_MAX, LONG_MAX, LLONG_MAX);
    compare(4000);
    compare(60000);
    compare(800000);
    return 0;
}
