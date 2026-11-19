#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef long long Count;

static unsigned int rng_state = 12345u;

static unsigned int next_random(void)
{
    rng_state = rng_state * 1103515245u + 12345u;
    return (rng_state >> 8) & 0x7fffffffu;
}

static void fill(int a[], int n, int order)
{
    for (int i = 0; i < n; i++) a[i] = i + 1;
    if (order == 1) {
        for (int i = 0; i < 10 && i < n / 2; i++) {
            int t = a[i];
            a[i] = a[n - 1 - i];
            a[n - 1 - i] = t;
        }
    } else if (order == 2) {
        for (int i = 0; i < n; i++) a[i] = n - i;
    } else if (order == 3) {
        for (int i = n - 1; i > 0; i--) {
            int j = (int)(next_random() % (unsigned int)(i + 1));
            int t = a[i];
            a[i] = a[j];
            a[j] = t;
        }
    }
}

static void bubble_sort(int a[], int n, Count *comparisons, Count *swaps)
{
    *comparisons = 0;
    *swaps = 0;
    for (int i = 0; i < n - 1; i++) {
        int exchanged = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            (*comparisons)++;
            if (a[j] > a[j + 1]) {
                int t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
                (*swaps)++;
                exchanged = 1;
            }
        }
        if (!exchanged) break;
    }
}

static void selection_sort(int a[], int n, Count *comparisons, Count *swaps)
{
    *comparisons = 0;
    *swaps = 0;
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) {
            (*comparisons)++;
            if (a[j] < a[best]) best = j;
        }
        if (best != i) {
            int t = a[i];
            a[i] = a[best];
            a[best] = t;
            (*swaps)++;
        }
    }
}

static void insertion_sort(int a[], int n, Count *comparisons, Count *shifts)
{
    *comparisons = 0;
    *shifts = 0;
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0) {
            (*comparisons)++;
            if (a[j] <= key) break;
            a[j + 1] = a[j];
            (*shifts)++;
            j--;
        }
        a[j + 1] = key;
    }
}

static int is_sorted(const int a[], int n)
{
    for (int i = 1; i < n; i++) {
        if (a[i - 1] > a[i]) return 0;
    }
    return 1;
}

static void measure(int n, int order, const char *name)
{
    int *source = malloc((size_t)n * sizeof *source);
    int *a = malloc((size_t)n * sizeof *a);
    Count bc, bs, sc, ss, ic, ish;
    clock_t start, finish;

    if (source == NULL || a == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    fill(source, n, order);

    for (int i = 0; i < n; i++) a[i] = source[i];
    start = clock();
    bubble_sort(a, n, &bc, &bs);
    finish = clock();
    if (!is_sorted(a, n)) fprintf(stderr, "bubble sort failed\n");

    for (int i = 0; i < n; i++) a[i] = source[i];
    selection_sort(a, n, &sc, &ss);
    if (!is_sorted(a, n)) fprintf(stderr, "selection sort failed\n");

    for (int i = 0; i < n; i++) a[i] = source[i];
    insertion_sort(a, n, &ic, &ish);
    if (!is_sorted(a, n)) fprintf(stderr, "insertion sort failed\n");

    printf("%-6d %-22s %14lld %12lld %15lld %14lld %16lld %14lld %10.3f\n",
           n, name, bc, bs, sc, ss, ic, ish,
           (double)(finish - start) / CLOCKS_PER_SEC);
    free(source);
    free(a);
}

int main(void)
{
    const char *names[4] = {"sorted", "nearly sorted (20 late)", "reverse", "random"};
    int sizes[3] = {100, 1000, 10000};

    printf("%-6s %-22s %14s %12s %15s %14s %16s %14s %10s\n",
           "n", "order", "bubble cmp", "bubble swap", "select cmp", "select swap",
           "insert cmp", "insert shift", "bubble s");
    for (int s = 0; s < 3; s++) {
        for (int order = 0; order < 4; order++) measure(sizes[s], order, names[order]);
    }
    return 0;
}
