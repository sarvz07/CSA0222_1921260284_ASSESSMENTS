#include <stdio.h>
#include <string.h>
#include "smart_meter.h"

int load_tariff(const char *path, Tariff *t)
{
    FILE *fp = fopen(path, "r");
    char line[128];
    char key[64];
    unsigned long value;
    int found = 0;

    if (fp == NULL) {
        return 0;
    }
    memset(t, 0, sizeof *t);
    while (fgets(line, sizeof line, fp) != NULL) {
        if (sscanf(line, "%63s %lu", key, &value) != 2) {
            continue;
        }
        if (strcmp(key, "fixed_charge_paise") == 0) {
            t->fixed_charge_paise = (uint32_t)value;
            found++;
        } else if (strcmp(key, "slab1_units") == 0) {
            t->slab_units[0] = (uint32_t)value;
            found++;
        } else if (strcmp(key, "slab2_units") == 0) {
            t->slab_units[1] = (uint32_t)value;
            found++;
        } else if (strcmp(key, "rate1_paise") == 0) {
            t->rate_paise[0] = (uint32_t)value;
            found++;
        } else if (strcmp(key, "rate2_paise") == 0) {
            t->rate_paise[1] = (uint32_t)value;
            found++;
        } else if (strcmp(key, "rate3_paise") == 0) {
            t->rate_paise[2] = (uint32_t)value;
            found++;
        } else if (strcmp(key, "peak_start_hour") == 0) {
            t->peak_start_hour = (uint32_t)value;
            found++;
        } else if (strcmp(key, "peak_end_hour") == 0) {
            t->peak_end_hour = (uint32_t)value;
            found++;
        } else if (strcmp(key, "peak_percent") == 0) {
            t->peak_percent = (uint32_t)value;
            found++;
        }
    }
    fclose(fp);
    return found == 9;
}

int is_peak(unsigned hour, const Tariff *t)
{
    return hour >= t->peak_start_hour && hour < t->peak_end_hour;
}

uint64_t base_charge(uint64_t watthours, const Tariff *t)
{
    uint64_t remaining = watthours;
    uint64_t numerator = 0;
    uint64_t lower = 0;
    int i;

    for (i = 0; i < 2; i++) {
        uint64_t upper = (uint64_t)t->slab_units[i] * WH_PER_UNIT;
        uint64_t width = upper - lower;
        uint64_t part = remaining < width ? remaining : width;

        numerator += part * t->rate_paise[i];
        remaining -= part;
        lower = upper;
    }
    numerator += remaining * t->rate_paise[2];
    return (numerator + WH_PER_UNIT / 2u) / WH_PER_UNIT;
}

uint64_t bill_charge(uint64_t offpeak_wh, uint64_t peak_wh, const Tariff *t)
{
    uint64_t peak_base = base_charge(peak_wh, t);
    uint64_t surcharge = (peak_base * t->peak_percent + 50u) / 100u;

    return (uint64_t)t->fixed_charge_paise + base_charge(offpeak_wh, t) + surcharge;
}
