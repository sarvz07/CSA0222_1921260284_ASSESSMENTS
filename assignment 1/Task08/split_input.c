#include <stdio.h>
#include "split.h"

int read_readings(unsigned short *previous, unsigned short *current)
{
    int p, c;

    printf("Enter previous and current: ");
    if (scanf("%d %d", &p, &c) != 2) {
        return 0;
    }
    if (p < 0 || p > 65535 || c < 0 || c > 65535) {
        return 0;
    }
    *previous = (unsigned short)p;
    *current = (unsigned short)c;
    return 1;
}
