#include <stdio.h>
#include <stdint.h>
#include "smart_meter.h"

#define SLOTS_PER_MONTH 2880u

int main(int argc, char **argv)
{
    Tariff tariff;
    Sample samples[] = {
        {17u, 3000u}, {18u, 5000u}, {19u, 8000u},
        {21u, 6000u}, {22u, 4000u}, {23u, 2000u}
    };
    size_t n = sizeof samples / sizeof samples[0];
    uint64_t offpeak_wh = 0;
    uint64_t peak_wh = 0;
    uint64_t total;
    size_t i;
    const char *path = argc > 1 ? argv[1] : "tariff.conf";

    if (!load_tariff(path, &tariff)) {
        printf("Could not load tariff from %s\n", path);
        return 1;
    }

    for (i = 0; i < n; i++) {
        if (is_peak(samples[i].hour, &tariff)) {
            peak_wh += samples[i].watthours;
        } else {
            offpeak_wh += samples[i].watthours;
        }
    }
    offpeak_wh *= SLOTS_PER_MONTH;
    peak_wh *= SLOTS_PER_MONTH;
    total = bill_charge(offpeak_wh, peak_wh, &tariff);

    printf("tariff loaded from %s, %u slots per month\n", path, SLOTS_PER_MONTH);
    printf("fixed=%u paise, rates=%u/%u/%u paise, peak=%u%% from %u:00 to %u:00\n",
           tariff.fixed_charge_paise, tariff.rate_paise[0],
           tariff.rate_paise[1], tariff.rate_paise[2], tariff.peak_percent,
           tariff.peak_start_hour, tariff.peak_end_hour);
    printf("offpeak=%llu Wh, peak=%llu Wh\n",
           (unsigned long long)offpeak_wh, (unsigned long long)peak_wh);
    printf("total bill = Rs %llu.%02llu\n",
           (unsigned long long)(total / 100u), (unsigned long long)(total % 100u));
    printf("max per house per year = %llu Wh\n",
           (unsigned long long)READINGS_PER_YEAR * 5000u);
    printf("max district per year  = %llu Wh\n",
           (unsigned long long)800000u * READINGS_PER_YEAR * 5000u);
    printf("sizeof(Tariff)=%zu, sizeof(uint64_t)=%zu\n",
           sizeof tariff, sizeof(uint64_t));
    return 0;
}
