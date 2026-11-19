#include "billing.h"
#include <stdio.h>

long compute_bill(int units)
{
    long first = units < SLAB1_UNITS ? units : SLAB1_UNITS;
    long second = units > SLAB1_UNITS ?
                  (units < SLAB2_UNITS ? units : SLAB2_UNITS) - SLAB1_UNITS : 0;
    long third = units > SLAB2_UNITS ? units - SLAB2_UNITS : 0;
    long paise = FIXED_CHARGE_PAISE;

    paise += first * RATE1_PAISE;
    paise += second * RATE2_PAISE;
    paise += third * RATE3_PAISE;
#ifdef DEBUG
    printf("[DEBUG] slabs %ld/%ld/%ld -> %ld paise\n", first, second, third, paise);
#endif
    return paise;
}

unsigned int consumption(unsigned short previous, unsigned short current)
{
    unsigned int units = (unsigned short)(current - previous);
#ifdef DEBUG
    printf("[DEBUG] %u -> %u = %u units\n",
           (unsigned)previous, (unsigned)current, units);
#endif
    return units;
}
