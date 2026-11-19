#include <stdio.h>
#include "token_global.h"

int main(void)
{
    int v1 = next_veg_token();
    int n1 = next_nonveg_token();
    issue_from_other_file();
    int v2 = next_veg_token();

    printf("global veg: %d %d\n", v1, v2);
    printf("global nonveg: %d\n", n1);
    printf("main reads shared veg_counter = %d\n", veg_counter);
    printf("main reads shared nonveg_counter = %d\n", nonveg_counter);
    return 0;
}
