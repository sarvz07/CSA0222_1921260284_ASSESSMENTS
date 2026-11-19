#include <stdio.h>
#include "token_global.h"

void issue_from_other_file(void)
{
    printf("other file issued veg token %d\n", next_veg_token());
}
