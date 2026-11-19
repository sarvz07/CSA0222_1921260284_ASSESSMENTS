#include <stdio.h>
#include <stdlib.h>

static int linear_search(const int a[], int n, int key, long *comparisons)
{
    for (int i = 0; i < n; i++) {
        (*comparisons)++;
        if (a[i] == key) return i;
    }
    return -1;
}

static int binary_search(const int a[], int n, int key, long *comparisons)
{
    int low = 0;
    int high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        (*comparisons)++;
        if (a[mid] == key) return mid;
        if (a[mid] < key) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

static int worst_binary(int n)
{
    int probes = 1;
    while ((1 << probes) < n + 1) probes++;
    return probes;
}

static int *make_catalogue(int n)
{
    int *a = malloc((size_t)n * sizeof *a);
    if (a == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) a[i] = 1000 + i * 7;
    return a;
}

static void trial_catalogue(void)
{
    int accession[] = {1021, 1048, 1103, 1190, 1222, 1305, 1377, 1402, 1460, 1533};
    int n = (int)(sizeof accession / sizeof accession[0]);
    int keys[] = {1021, 1305, 1533, 1400};
    printf("trial catalogue (n = %d), predicted binary worst = %d\n", n, worst_binary(n));
    printf("%-8s %-16s %-16s\n", "key", "linear cmp", "binary cmp");
    for (int k = 0; k < 4; k++) {
        long linear_cmp = 0;
        long binary_cmp = 0;
        int lp = linear_search(accession, n, keys[k], &linear_cmp);
        int bp = binary_search(accession, n, keys[k], &binary_cmp);
        printf("%-8d %-16ld %-16ld (index %d / %d)\n", keys[k], linear_cmp, binary_cmp, lp, bp);
    }
    printf("the missing book takes as many probes as the deepest successful search\n");
}

static void measure(int n)
{
    int *a = make_catalogue(n);
    int keys[4];
    const char *label[4] = {"beginning", "middle", "end", "missing"};
    long linear_cmp[4];
    long binary_cmp[4];
    int linear_pos[4];
    int binary_pos[4];

    keys[0] = a[0];
    keys[1] = a[n / 2];
    keys[2] = a[n - 1];
    keys[3] = 1000 + n * 7 + 1;

    for (int k = 0; k < 4; k++) {
        linear_cmp[k] = 0;
        binary_cmp[k] = 0;
        linear_pos[k] = linear_search(a, n, keys[k], &linear_cmp[k]);
        binary_pos[k] = binary_search(a, n, keys[k], &binary_cmp[k]);
    }

    printf("\nn = %d (predicted: linear worst %d, binary worst %d)\n", n, n, worst_binary(n));
    printf("%-10s %-12s %-10s %-12s %-10s\n", "position", "linear cmp", "found", "binary cmp", "found");
    for (int k = 0; k < 4; k++) {
        printf("%-10s %-12ld %-10d %-12ld %-10d\n", label[k], linear_cmp[k],
               linear_pos[k], binary_cmp[k], binary_pos[k]);
    }
    free(a);
}

int main(void)
{
    trial_catalogue();
    measure(1000);
    measure(10000);
    measure(100000);
    return 0;
}
