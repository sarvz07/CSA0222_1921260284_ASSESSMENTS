#include <stdio.h>

#define RUPEES_TO_PAISE(r) ((long)((r) * 100.0))
#define BAD_SQUARE(x) x * x
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int larger(int a, int b)
{
    return a > b ? a : b;
}

int main(void)
{
    int a = 1;
    int i = 3;
    int j = 2;

    printf("SQUARE(5)      = %d\n", SQUARE(5));
    printf("BAD_SQUARE(a+1)= %d\n", BAD_SQUARE(a + 1));
    printf("SQUARE(a+1)    = %d\n", SQUARE(a + 1));
    int macro_result = MAX(i++, j);
    printf("MAX(i++, j)    = %d, i is now %d\n", macro_result, i);

    i = 3;
    int function_result = larger(i++, j);
    printf("larger(i++, j) = %d, i is now %d\n", function_result, i);
    printf("MIN(7, 4)      = %d\n", MIN(7, 4));
    printf("Rs 2.25        = %ld paise\n", RUPEES_TO_PAISE(2.25));
    return 0;
}
