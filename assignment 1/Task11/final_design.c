#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#define SLAB1_UNITS 100u
#define SLAB2_UNITS 200u
#define RATE1_PAISE 0u
#define RATE2_PAISE 225u
#define RATE3_PAISE 450u
#define FIXED_CHARGE_PAISE 5000u

typedef enum {
    DOMESTIC = 'D',
    COMMERCIAL = 'C'
} Category;

typedef struct {
    uint32_t household_no;
    uint16_t previous;
    uint16_t current;
    Category category;
} MeterReading;

static inline uint32_t units_consumed(uint16_t previous, uint16_t current)
{
    return (uint16_t)(current - previous);
}

static inline uint64_t bill_paise(uint32_t units)
{
    uint64_t first = units < SLAB1_UNITS ? units : SLAB1_UNITS;
    uint64_t second = units > SLAB1_UNITS ?
                      (units < SLAB2_UNITS ? units - SLAB1_UNITS
                                           : SLAB2_UNITS - SLAB1_UNITS) : 0u;
    uint64_t third = units > SLAB2_UNITS ? units - SLAB2_UNITS : 0u;

    return (uint64_t)FIXED_CHARGE_PAISE + first * RATE1_PAISE
           + second * RATE2_PAISE + third * RATE3_PAISE;
}

int main(void)
{
    MeterReading readings[] = {
        {1001u, 1000u, 1050u, DOMESTIC},
        {1002u, 1000u, 1150u, DOMESTIC},
        {1003u, 1000u, 1350u, COMMERCIAL},
        {1004u, 65400u, 120u, DOMESTIC}
    };
    size_t n = sizeof readings / sizeof readings[0];
    uint64_t district_total = 0;
    size_t i;

    printf("Recommended types: uint32_t house, uint16_t readings, ");
    printf("uint32_t units, uint64_t paise\n");
    printf("sizes: uint16_t=%zu, uint32_t=%zu, uint64_t=%zu\n",
           sizeof(uint16_t), sizeof(uint32_t), sizeof(uint64_t));
    printf("UINT32_MAX=%u, UINT64_MAX=%llu\n",
           UINT32_MAX, (unsigned long long)UINT64_MAX);

    for (i = 0; i < n; i++) {
        uint32_t units = units_consumed(readings[i].previous, readings[i].current);
        uint64_t paise = bill_paise(units);
        printf("house %u (%c): %u units -> Rs %llu.%02llu\n",
               readings[i].household_no, (char)readings[i].category, units,
               (unsigned long long)(paise / 100u),
               (unsigned long long)(paise % 100u));
        district_total += paise;
    }
    district_total *= 800000u;
    printf("district total for 800000 houses = Rs %llu.%02llu\n",
           (unsigned long long)(district_total / 100u),
           (unsigned long long)(district_total % 100u));
    return 0;
}
