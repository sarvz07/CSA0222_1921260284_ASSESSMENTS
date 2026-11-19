#ifndef SMART_METER_H
#define SMART_METER_H

#include <stdint.h>

#define MAX_SLABS 3
#define WH_PER_UNIT 1000u
#define READINGS_PER_YEAR 35040u

typedef struct {
    uint32_t fixed_charge_paise;
    uint32_t slab_units[MAX_SLABS];
    uint32_t rate_paise[MAX_SLABS];
    uint32_t peak_start_hour;
    uint32_t peak_end_hour;
    uint32_t peak_percent;
} Tariff;

typedef struct {
    uint16_t hour;
    uint32_t watthours;
} Sample;

int load_tariff(const char *path, Tariff *t);
int is_peak(unsigned hour, const Tariff *t);
uint64_t base_charge(uint64_t watthours, const Tariff *t);
uint64_t bill_charge(uint64_t offpeak_wh, uint64_t peak_wh, const Tariff *t);

#endif
