#include <stdio.h>
#include <limits.h>

#define SLAB1_UNITS 100
#define SLAB2_UNITS 200
#define RATE1_PAISE 0L
#define RATE2_PAISE 225L
#define RATE3_PAISE 450L
#define FIXED_CHARGE_PAISE 5000L
#define METER_MAX 65535

typedef struct {
    unsigned int house;
    char category;
    unsigned short previous;
    unsigned short current;
    int rolled;
} Reading;

static unsigned int units_of(const Reading *r)
{
    return (unsigned short)(r->current - r->previous);
}

static long bill_of(unsigned int units)
{
    long first = units < SLAB1_UNITS ? (long)units : SLAB1_UNITS;
    long second = units > SLAB1_UNITS ?
                  ((units < SLAB2_UNITS ? (long)units : SLAB2_UNITS) - SLAB1_UNITS) : 0;
    long third = units > SLAB2_UNITS ? (long)units - SLAB2_UNITS : 0;

    return FIXED_CHARGE_PAISE + first * RATE1_PAISE
           + second * RATE2_PAISE + third * RATE3_PAISE;
}

static void print_result(const char *name, long expected, long actual)
{
    printf("%-24s expected Rs %8ld.%02ld  actual Rs %8ld.%02ld  %s\n",
           name, expected / 100, expected % 100,
           actual / 100, actual % 100, expected == actual ? "PASS" : "FAIL");
}

static void run_tests(void)
{
    static const struct {
        const char *name;
        Reading r;
        long expected;
    } cases[] = {
        {"slab 1: 50 units",      {1001, 'R', 1000, 1050, 0},  5000L},
        {"slab 2: 150 units",     {1002, 'R', 1000, 1150, 0}, 16250L},
        {"slab 3: 350 units",     {1003, 'C', 1000, 1350, 0}, 95000L},
        {"boundary: 100 units",   {1004, 'R', 0, 100, 0},       5000L},
        {"boundary: 200 units",   {1005, 'R', 0, 200, 0},      27500L},
        {"boundary: 0 units",     {1006, 'R', 500, 500, 0},     5000L},
        {"rollover: 65400->120",  {1007, 'R', 65400, 120, 1},  52700L}
    };
    size_t n = sizeof cases / sizeof cases[0];
    long expected_total = 0;
    long actual_total = 0;
    long five_expected = 5000L + 16250L + 95000L + 27500L + 5000L;
    long five_actual = 0;
    size_t i;

    puts("Test table");
    for (i = 0; i < n; i++) {
        long actual = bill_of(units_of(&cases[i].r));
        print_result(cases[i].name, cases[i].expected, actual);
        expected_total += cases[i].expected;
        actual_total += actual;
    }
    print_result("sum of 7 bills", expected_total, actual_total);

    five_actual += bill_of(units_of(&cases[0].r));
    five_actual += bill_of(units_of(&cases[1].r));
    five_actual += bill_of(units_of(&cases[2].r));
    five_actual += bill_of(units_of(&cases[4].r));
    five_actual += bill_of(units_of(&cases[5].r));
    print_result("hand-summed 5 bills", five_expected, five_actual);
}

static int read_reading(Reading *r)
{
    int previous;
    int current;
    int rolled;
    int count;

    printf("Enter house category rolled(0/1) previous current: ");
    count = scanf("%u %c %d %d %d", &r->house, &r->category, &rolled,
                  &previous, &current);
    if (count != 5) {
        return 0;
    }
    if (previous < 0 || previous > METER_MAX || current < 0 || current > METER_MAX) {
        return -1;
    }
    if (current < previous && rolled != 1) {
        return -2;
    }
    r->previous = (unsigned short)previous;
    r->current = (unsigned short)current;
    r->rolled = rolled;
    return 1;
}

int main(void)
{
    Reading r;
    int status;

    run_tests();
    puts("");
    status = read_reading(&r);
    if (status == 0) {
        puts("Invalid input: letters or missing fields (scanf did not read 5 items)");
        return 1;
    }
    if (status == -1) {
        puts("Invalid input: reading outside 0..65535");
        return 1;
    }
    if (status == -2) {
        puts("Invalid input: current < previous without a rollover flag");
        return 1;
    }

    {
        unsigned int units = units_of(&r);
        long fixed = FIXED_CHARGE_PAISE;
        long energy = bill_of(units) - fixed;
        long total = bill_of(units);

        printf("house %u, category %c: %u units\n", r.house, r.category, units);
        printf("fixed Rs %ld.%02ld + energy Rs %ld.%02ld = total Rs %ld.%02ld\n",
               fixed / 100, fixed % 100, energy / 100, energy % 100,
               total / 100, total % 100);
    }
    return 0;
}
