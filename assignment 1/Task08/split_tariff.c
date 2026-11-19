#include "split.h"

long energy_charge(int units)
{
    return FIXED_CHARGE_PAISE + (long)units * RATE_PAISE;
}
