#include <stdio.h>
#include <stdlib.h>

static int table_size_for(int n)
{
    int size = 1;
    while (size < 2 * n) size *= 2;
    return size;
}

static int hash_search(const int table[], int size, int key, long *probes)
{
    unsigned int mask = (unsigned int)size - 1u;
    unsigned int home = (unsigned int)key & mask;
    for (int i = 0; i < size; i++) {
        unsigned int slot = (home + (unsigned int)i) & mask;
        (*probes)++;
        if (table[slot] == key) return (int)slot;
        if (table[slot] == -1) return -1;
    }
    return -1;
}

static void build_table(const int a[], int n, int table[], int size)
{
    unsigned int mask = (unsigned int)size - 1u;
    for (int i = 0; i < size; i++) table[i] = -1;
    for (int i = 0; i < n; i++) {
        unsigned int home = (unsigned int)a[i] & mask;
        for (int j = 0; j < size; j++) {
            unsigned int slot = (home + (unsigned int)j) & mask;
            if (table[slot] == -1) {
                table[slot] = a[i];
                break;
            }
        }
    }
}

static int interpolation_search(const int a[], int n, int key, long *probes)
{
    int low = 0;
    int high = n - 1;
    while (low <= high) {
        (*probes)++;
        if (key < a[low] || key > a[high]) return -1;
        if (low == high) return a[low] == key ? low : -1;
        long long span = (long long)a[high] - a[low];
        long long offset = (long long)key - a[low];
        int pos = low + (int)(offset * (high - low) / span);
        if (pos < low) pos = low;
        if (pos > high) pos = high;
        if (a[pos] == key) return pos;
        if (a[pos] < key) low = pos + 1;
        else high = pos - 1;
    }
    return -1;
}

static void measure(int n)
{
    int *a = malloc((size_t)n * sizeof *a);
    int size = table_size_for(n);
    int *table = malloc((size_t)size * sizeof *table);
    int keys[4];
    const char *label[4] = {"beginning", "middle", "end", "missing"};
    long hash_probes[4];
    long interp_probes[4];

    if (a == NULL || table == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) a[i] = 1000 + i * 7;
    build_table(a, n, table, size);
    keys[0] = a[0];
    keys[1] = a[n / 2];
    keys[2] = a[n - 1];
    keys[3] = 1000 + n * 7 + 1;

    printf("\nn = %d, hash table size = %d (extra space %d ints)\n", n, size, size);
    printf("%-10s %-16s %-18s\n", "position", "hash probes", "interpolation probes");
    for (int k = 0; k < 4; k++) {
        hash_probes[k] = 0;
        interp_probes[k] = 0;
        hash_search(table, size, keys[k], &hash_probes[k]);
        interpolation_search(a, n, keys[k], &interp_probes[k]);
        printf("%-10s %-16ld %-18ld\n", label[k], hash_probes[k], interp_probes[k]);
    }
    free(a);
    free(table);
}

int main(void)
{
    measure(1000);
    measure(10000);
    measure(100000);
    return 0;
}
