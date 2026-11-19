#include <stdio.h>

#define DOUBLE(x) ((x) + (x))
#define MESSAGE "billing demo"

int main(void)
{
    int x = 21;
#ifdef DEBUG
    printf("[DEBUG] x = %d\n", x);
#endif
    printf("%s: DOUBLE(%d) = %d\n", MESSAGE, x, DOUBLE(x));
    return 0;
}
