#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static long long qsort_comparisons;

static int compare_descending(const void *p, const void *q)
{
    int x = *(const int *)p;
    int y = *(const int *)q;
    qsort_comparisons++;
    return (y > x) - (y < x);
}

static int merge_count(const int a[], int na, const int b[], int nb, int out[], long long *comparisons)
{
    int i = 0;
    int j = 0;
    int k = 0;
    *comparisons = 0;
    while (i < na && j < nb) {
        (*comparisons)++;
        if (a[i] >= b[j]) out[k++] = a[i++];
        else out[k++] = b[j++];
    }
    while (i < na) out[k++] = a[i++];
    while (j < nb) out[k++] = b[j++];
    return k;
}

static void inplace_merge(int a[], int na, const int b[], int nb, long long *comparisons, long long *moves)
{
    int used = na;
    *comparisons = 0;
    *moves = 0;
    for (int j = 0; j < nb; j++) {
        int value = b[j];
        int i = used - 1;
        while (i >= 0) {
            (*comparisons)++;
            if (a[i] >= value) break;
            a[i + 1] = a[i];
            (*moves)++;
            i--;
        }
        a[i + 1] = value;
        (*moves)++;
        used++;
    }
}

static int is_descending(const int a[], int n)
{
    for (int i = 1; i < n; i++) {
        if (a[i - 1] < a[i]) return 0;
    }
    return 1;
}

static void print_result(const char *title, const int a[], int na, const int b[], int nb)
{
    int out[64];
    long long comparisons = 0;
    int n = merge_count(a, na, b, nb, out, &comparisons);
    printf("%s (%d + %d, score comparisons %lld): ", title, na, nb, comparisons);
    for (int i = 0; i < n; i++) printf("%d ", out[i]);
    printf("[ordered %s]\n", is_descending(out, n) ? "yes" : "no");
}

static void edge_cases(void)
{
    int empty[1] = {0};
    int equal_a[4] = {77, 77, 77, 77};
    int equal_b[3] = {77, 77, 77};
    int campus_a[4] = {98, 91, 85, 72};
    int campus_b[3] = {95, 91, 80};
    print_result("empty first list", empty, 0, campus_b, 3);
    print_result("empty second list", campus_a, 4, empty, 0);
    print_result("all equal scores", equal_a, 4, equal_b, 3);
    print_result("different lengths", campus_a, 4, campus_b, 3);
}

static void large_compare(int n)
{
    int *a = malloc((size_t)n * sizeof *a);
    int *b = malloc((size_t)n * sizeof *b);
    int *merged = malloc((size_t)(2 * n) * sizeof *merged);
    int *joined = malloc((size_t)(2 * n) * sizeof *joined);
    long long merge_cmp = 0;
    clock_t start, finish;
    double merge_time;
    double sort_time;

    if (a == NULL || b == NULL || merged == NULL || joined == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        a[i] = 2 * n - 2 * i;
        b[i] = 2 * n - 2 * i - 1;
    }

    start = clock();
    merge_count(a, n, b, n, merged, &merge_cmp);
    finish = clock();
    merge_time = (double)(finish - start) / CLOCKS_PER_SEC;

    for (int i = 0; i < n; i++) {
        joined[i] = a[i];
        joined[n + i] = b[i];
    }
    qsort_comparisons = 0;
    start = clock();
    qsort(joined, (size_t)(2 * n), sizeof *joined, compare_descending);
    finish = clock();
    sort_time = (double)(finish - start) / CLOCKS_PER_SEC;

    printf("m = n = %-6d merge cmp %-10lld join+qsort cmp %-14lld merge %.4f s  join+qsort %.4f s  ordered %s/%s\n",
           n, merge_cmp, qsort_comparisons, merge_time, sort_time,
           is_descending(merged, 2 * n) ? "yes" : "no",
           is_descending(joined, 2 * n) ? "yes" : "no");
    free(a);
    free(b);
    free(merged);
    free(joined);
}

static int matches(const int a[], const int b[], int n)
{
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

static void inplace_demo(int n)
{
    int *a = malloc((size_t)(2 * n) * sizeof *a);
    int *b = malloc((size_t)n * sizeof *b);
    int *merged = malloc((size_t)(2 * n) * sizeof *merged);
    long long merge_cmp = 0;
    long long comparisons = 0;
    long long moves = 0;

    if (a == NULL || b == NULL || merged == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        a[i] = 2 * n - 2 * i;
        b[i] = 2 * n - 2 * i - 1;
    }
    merge_count(a, n, b, n, merged, &merge_cmp);
    inplace_merge(a, n, b, n, &comparisons, &moves);
    printf("in-place merge at n = %d: score comparisons %lld, element moves %lld, matches third-array merge %s\n",
           n, comparisons, moves, matches(merged, a, 2 * n) ? "yes" : "no");
    free(a);
    free(b);
    free(merged);
}

int main(void)
{
    edge_cases();
    large_compare(1000);
    large_compare(10000);
    inplace_demo(200);
    return 0;
}
