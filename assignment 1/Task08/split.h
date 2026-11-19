#ifndef SPLIT_H
#define SPLIT_H

#define FIXED_CHARGE_PAISE 5000L
#define RATE_PAISE 225L

int read_readings(unsigned short *previous, unsigned short *current);
long energy_charge(int units);
void print_bill(int units, long paise);

#endif
