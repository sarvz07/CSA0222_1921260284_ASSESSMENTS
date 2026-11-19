#include "split.h"

int main(void)
{
    unsigned short previous = 0;
    unsigned short current = 0;
    int units;

    if (!read_readings(&previous, &current)) {
        return 1;
    }
    units = (unsigned short)(current - previous);
    print_bill(units, energy_charge(units));
    return 0;
}
