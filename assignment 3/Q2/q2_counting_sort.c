#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static unsigned int rng_state = 98765u;

static unsigned int next_random(void)
{
    rng_state = rng_state * 1103515245u + 12345u;
    return (rng_state >> 8) & 0x7fffffffu;
}

static void counting_sort(const int in[], int out[], int n, int k)
{
    int *count = calloc((size_t)k, sizeof *count);
    if (count == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) count[in[i]]++;
    for (int i = 1; i < k; i++) count[i] += count[i - 1];
    for (int i = n - 1; i >= 0; i--) out[--count[in[i]]] = in[i];
    free(count);
}

static long long insertion_sort(const int in[], int out[], int n)
{
    long long comparisons = 0;
    for (int i = 0; i < n; i++) out[i] = in[i];
    for (int i = 1; i < n; i++) {
        int key = out[i];
        int j = i - 1;
        while (j >= 0) {
            comparisons++;
            if (out[j] <= key) break;
            out[j + 1] = out[j];
            j--;
        }
        out[j + 1] = key;
    }
    return comparisons;
}

static int is_sorted(const int a[], int n)
{
    for (int i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) return 0;
    }
    return 1;
}

static void measure(int n, int k)
{
    int *in = malloc((size_t)n * sizeof *in);
    int *out = malloc((size_t)n * sizeof *out);
    clock_t start, finish;
    double counting_time;
    double insertion_time = 0.0;
    long long comparisons = 0;

    if (in == NULL || out == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) in[i] = (int)(next_random() % (unsigned int)k);

    start = clock();
    counting_sort(in, out, n, k);
    finish = clock();
    counting_time = (double)(finish - start) / CLOCKS_PER_SEC;
    if (!is_sorted(out, n)) fprintf(stderr, "counting sort failed\n");

    if (n <= 10000) {
        start = clock();
        comparisons = insertion_sort(in, out, n);
        finish = clock();
        insertion_time = (double)(finish - start) / CLOCKS_PER_SEC;
        if (!is_sorted(out, n)) fprintf(stderr, "insertion sort failed\n");
    }

    printf("n = %-7d k = %-8d counting %8.4f s   extra ints %-8d",
           n, k, counting_time, k);
    if (n <= 10000) {
        printf("   insertion %8.4f s   cmp %lld", insertion_time, comparisons);
    } else {
        printf("   insertion omitted (quadratic)");
    }
    putchar('\n');
    free(in);
    free(out);
}

int main(void)
{
    measure(1000, 1000);
    measure(1000, 10000);
    measure(10000, 10000);
    measure(10000, 100000);
    measure(100000, 100000);
    measure(100000, 1000000);
    return 0;
}
