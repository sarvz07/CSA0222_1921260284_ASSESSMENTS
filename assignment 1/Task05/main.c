#include <stdio.h>
#include "billing.h"

int main(void)
{
    int tests[] = {0, 100, 150, 200, 350};
    int n = sizeof tests / sizeof tests[0];

#ifdef DEBUG
    printf("[DEBUG] compiled with DEBUG defined\n");
#endif
    for (int i = 0; i < n; i++) {
        long paise = compute_bill(tests[i]);
        printf("%3d units -> Rs %ld.%02ld\n", tests[i], paise / 100, paise % 100);
    }
    printf("65400 -> 120 = %u units\n", consumption(65400, 120));
    return 0;
}
