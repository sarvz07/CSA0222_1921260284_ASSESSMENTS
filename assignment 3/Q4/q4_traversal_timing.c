#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIDE 2000
#define PASSES 3

static long long row_wise(const int *a)
{
    long long sum = 0;
    for (int i = 0; i < SIDE; i++) {
        for (int j = 0; j < SIDE; j++) sum += a[i * SIDE + j];
    }
    return sum;
}

static long long column_wise(const int *a)
{
    long long sum = 0;
    for (int j = 0; j < SIDE; j++) {
        for (int i = 0; i < SIDE; i++) sum += a[i * SIDE + j];
    }
    return sum;
}

static double time_passes(long long (*traverse)(const int *), const int *a, long long *sum)
{
    clock_t start = clock();
    for (int p = 0; p < PASSES; p++) *sum = traverse(a);
    clock_t finish = clock();
    return (double)(finish - start) / CLOCKS_PER_SEC / PASSES;
}

int main(void)
{
    int *rain = malloc((size_t)SIDE * SIDE * sizeof *rain);
    long long row_sum = 0;
    long long column_sum = 0;
    double row_time;
    double column_time;

    if (rain == NULL) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }
    for (int i = 0; i < SIDE; i++) {
        for (int j = 0; j < SIDE; j++) rain[i * SIDE + j] = (i * 13 + j * 7) % 100;
    }

    row_time = time_passes(row_wise, rain, &row_sum);
    column_time = time_passes(column_wise, rain, &column_sum);

    printf("array %d x %d, %zu bytes, %d passes each\n", SIDE, SIDE,
           (size_t)SIDE * SIDE * sizeof *rain, PASSES);
    printf("row-wise    sum = %lld, %.4f s per pass\n", row_sum, row_time);
    printf("column-wise sum = %lld, %.4f s per pass\n", column_sum, column_time);
    printf("same sum: %s, row-wise faster: %s\n",
           row_sum == column_sum ? "yes" : "no", row_time < column_time ? "yes" : "no");
    free(rain);
    return 0;
}
