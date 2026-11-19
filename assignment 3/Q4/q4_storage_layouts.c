#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DISTRICTS 38
#define MONTHS 12
#define PASSES 2000

static long total_2d(const int a[DISTRICTS][MONTHS])
{
    long sum = 0;
    for (int i = 0; i < DISTRICTS; i++) {
        for (int j = 0; j < MONTHS; j++) sum += a[i][j];
    }
    return sum;
}

static long total_flat(const int *a)
{
    long sum = 0;
    for (int i = 0; i < DISTRICTS; i++) {
        for (int j = 0; j < MONTHS; j++) sum += a[i * MONTHS + j];
    }
    return sum;
}

static long total_rows(int *const a[DISTRICTS])
{
    long sum = 0;
    for (int i = 0; i < DISTRICTS; i++) {
        for (int j = 0; j < MONTHS; j++) sum += a[i][j];
    }
    return sum;
}

static int maximum_flat(const int *a)
{
    int best = a[0];
    for (int i = 0; i < DISTRICTS * MONTHS; i++) {
        if (a[i] > best) best = a[i];
    }
    return best;
}

static double time_2d(const int a[DISTRICTS][MONTHS])
{
    long sum = 0;
    clock_t start = clock();
    for (int p = 0; p < PASSES; p++) sum += total_2d(a);
    clock_t finish = clock();
    printf("2-D array total %ld, %.4f s for %d passes\n", sum / PASSES,
           (double)(finish - start) / CLOCKS_PER_SEC, PASSES);
    return (double)(finish - start) / CLOCKS_PER_SEC;
}

static double time_flat(const int *a)
{
    long sum = 0;
    clock_t start = clock();
    for (int p = 0; p < PASSES; p++) sum += total_flat(a);
    clock_t finish = clock();
    printf("flat array total %ld, %.4f s for %d passes\n", sum / PASSES,
           (double)(finish - start) / CLOCKS_PER_SEC, PASSES);
    return (double)(finish - start) / CLOCKS_PER_SEC;
}

static double time_rows(int *const a[DISTRICTS])
{
    long sum = 0;
    clock_t start = clock();
    for (int p = 0; p < PASSES; p++) sum += total_rows(a);
    clock_t finish = clock();
    printf("row pointers total %ld, %.4f s for %d passes\n", sum / PASSES,
           (double)(finish - start) / CLOCKS_PER_SEC, PASSES);
    return (double)(finish - start) / CLOCKS_PER_SEC;
}

int main(void)
{
    static int rain[DISTRICTS][MONTHS];
    int flat[DISTRICTS * MONTHS];
    int *rows[DISTRICTS];
    double t2d, tflat, trows;

    for (int i = 0; i < DISTRICTS; i++) {
        rows[i] = malloc(MONTHS * sizeof *rows[i]);
        if (rows[i] == NULL) {
            fprintf(stderr, "out of memory\n");
            return 1;
        }
        for (int j = 0; j < MONTHS; j++) {
            rain[i][j] = 40 + ((i * 37 + j * 53) % 220);
            flat[i * MONTHS + j] = rain[i][j];
            rows[i][j] = rain[i][j];
        }
    }

    long district0_2d = 0;
    long district0_flat = 0;
    long district0_rows = 0;
    for (int j = 0; j < MONTHS; j++) {
        district0_2d += rain[0][j];
        district0_flat += flat[0 * MONTHS + j];
        district0_rows += rows[0][j];
    }
    printf("same district 0 total in all layouts: %ld %ld %ld\n",
           district0_2d, district0_flat, district0_rows);
    printf("maximum via flat index i * 12 + j: %d\n", maximum_flat(flat));
    printf("extra space for row pointers: %zu bytes\n", sizeof rows);

    t2d = time_2d(rain);
    tflat = time_flat(flat);
    trows = time_rows(rows);
    printf("flat %.2fx 2-D, row pointers %.2fx 2-D (all access O(1))\n",
           t2d / tflat, trows / t2d);

    for (int i = 0; i < DISTRICTS; i++) free(rows[i]);
    return 0;
}
