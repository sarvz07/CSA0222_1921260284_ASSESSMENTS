#include <stdio.h>
#include "split.h"

void print_bill(int units, long paise)
{
    printf("%d units -> Rs %ld.%02ld\n", units, paise / 100, paise % 100);
}
