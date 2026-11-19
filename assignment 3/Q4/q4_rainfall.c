#include <stdio.h>

#define DISTRICTS 38
#define MONTHS 12
#define YEARS 2

static void row_totals(int districts, int months, const int rain[districts][months], long totals[])
{
    for (int i = 0; i < districts; i++) {
        long sum = 0;
        for (int j = 0; j < months; j++) sum += rain[i][j];
        totals[i] = sum;
    }
}

static void column_averages(int districts, int months, const int rain[districts][months], double averages[])
{
    for (int j = 0; j < months; j++) {
        long sum = 0;
        for (int i = 0; i < districts; i++) sum += rain[i][j];
        averages[j] = (double)sum / districts;
    }
}

static void maximum(int districts, int months, const int rain[districts][months],
                    int *district, int *month, int *value)
{
    *value = rain[0][0];
    *district = 0;
    *month = 0;
    for (int i = 0; i < districts; i++) {
        for (int j = 0; j < months; j++) {
            if (rain[i][j] > *value) {
                *value = rain[i][j];
                *district = i;
                *month = j;
            }
        }
    }
}

static int deficient_districts(int districts, int months, const int rain[districts][months],
                               double threshold, int list[])
{
    int count = 0;
    for (int i = 0; i < districts; i++) {
        long sum = 0;
        for (int j = 0; j < months; j++) sum += rain[i][j];
        if ((double)sum / months < threshold) list[count++] = i;
    }
    return count;
}

static double state_average(int districts, int months, const int rain[districts][months])
{
    long sum = 0;
    for (int i = 0; i < districts; i++) {
        for (int j = 0; j < months; j++) sum += rain[i][j];
    }
    return (double)sum / (districts * months);
}

static long three_d_total(int districts, int months, const int rain[YEARS][districts][months], int year)
{
    long sum = 0;
    for (int i = 0; i < districts; i++) {
        for (int j = 0; j < months; j++) sum += rain[year][i][j];
    }
    return sum;
}

static void hand_check(void)
{
    int small[3][4] = {
        {100, 200, 300, 400},
        {50, 60, 70, 80},
        {10, 20, 30, 40}
    };
    long totals[3];
    double averages[4];
    int deficient[3];
    int district, month, value;
    double average = state_average(3, 4, small);
    int count;

    row_totals(3, 4, small, totals);
    column_averages(3, 4, small, averages);
    maximum(3, 4, small, &district, &month, &value);
    count = deficient_districts(3, 4, small, 0.6 * average, deficient);

    printf("3 x 4 hand check\n");
    for (int i = 0; i < 3; i++) {
        printf("district %d total = %ld (average %.2f)\n", i, totals[i], (double)totals[i] / 4);
    }
    printf("column averages:");
    for (int j = 0; j < 4; j++) printf(" %.2f", averages[j]);
    printf("\nstate average = %.2f, 60%% = %.2f\n", average, 0.6 * average);
    printf("wettest = rain[%d][%d] = %d\n", district, month, value);
    printf("deficient districts:");
    for (int i = 0; i < count; i++) printf(" %d", deficient[i]);
    printf("\n");
}

static void full_dataset(void)
{
    int rain[DISTRICTS][MONTHS];
    int years[YEARS][DISTRICTS][MONTHS];
    long totals[DISTRICTS];
    double averages[MONTHS];
    int deficient[DISTRICTS];
    int district, month, value;
    double average;
    int count;

    for (int i = 0; i < DISTRICTS; i++) {
        for (int j = 0; j < MONTHS; j++) {
            rain[i][j] = 40 + ((i * 37 + j * 53 + i * j) % 220);
            if (i % 5 == 0) rain[i][j] = 30 + ((i * 11 + j * 17) % 50);
            for (int y = 0; y < YEARS; y++) years[y][i][j] = rain[i][j] + y * 5;
        }
    }

    row_totals(DISTRICTS, MONTHS, rain, totals);
    column_averages(DISTRICTS, MONTHS, rain, averages);
    maximum(DISTRICTS, MONTHS, rain, &district, &month, &value);
    average = state_average(DISTRICTS, MONTHS, rain);
    count = deficient_districts(DISTRICTS, MONTHS, rain, 0.6 * average, deficient);

    printf("\n%d x %d dataset\n", DISTRICTS, MONTHS);
    printf("row 0 total = %ld, row 37 total = %ld\n", totals[0], totals[DISTRICTS - 1]);
    printf("monthly averages:");
    for (int j = 0; j < MONTHS; j++) printf(" %.2f", averages[j]);
    printf("\nstate average = %.2f, 60%% = %.2f\n", average, 0.6 * average);
    printf("wettest = district %d, month %d, %d mm\n", district, month, value);
    printf("deficient districts (%d):", count);
    for (int i = 0; i < count; i++) printf(" %d", deficient[i]);
    printf("\n");
    printf("3-D extension: year 0 total = %ld, year 1 total = %ld\n",
           three_d_total(DISTRICTS, MONTHS, years, 0),
           three_d_total(DISTRICTS, MONTHS, years, 1));
    printf("row-major check: rain[0][0] at %p, rain[1][0] at %p, gap %td bytes, row size %zu bytes\n",
           (void *)&rain[0][0], (void *)&rain[1][0],
           (char *)&rain[1][0] - (char *)&rain[0][0], sizeof rain[0]);
}

int main(void)
{
    hand_check();
    full_dataset();
    return 0;
}
