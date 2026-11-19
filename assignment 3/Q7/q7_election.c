#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CANDIDATES 12

typedef struct {
    int candidate;
    int votes;
} Result;

static unsigned int rng_state = 24680u;

static unsigned int next_random(void)
{
    rng_state = rng_state * 1103515245u + 12345u;
    return (rng_state >> 8) & 0x7fffffffu;
}

static long long sort_comparisons;

static int compare_int(const void *p, const void *q)
{
    int x = *(const int *)p;
    int y = *(const int *)q;
    sort_comparisons++;
    return (x > y) - (x < y);
}

static void count_votes(const int votes[], int v, int candidates, int counts[], long long *invalid)
{
    for (int i = 0; i < candidates; i++) counts[i] = 0;
    *invalid = 0;
    for (int i = 0; i < v; i++) {
        if (votes[i] >= 1 && votes[i] <= candidates) counts[votes[i] - 1]++;
        else (*invalid)++;
    }
}

static void sort_results(Result results[], int candidates)
{
    for (int i = 1; i < candidates; i++) {
        Result key = results[i];
        int j = i - 1;
        while (j >= 0 && (results[j].votes < key.votes ||
               (results[j].votes == key.votes && results[j].candidate > key.candidate))) {
            results[j + 1] = results[j];
            j--;
        }
        results[j + 1] = key;
    }
}

static void report(const char *title, const int votes[], int v, int candidates)
{
    int counts[MAX_CANDIDATES];
    Result results[MAX_CANDIDATES];
    long long invalid = 0;
    int tied = 0;

    count_votes(votes, v, candidates, counts, &invalid);
    for (int i = 0; i < candidates; i++) {
        results[i].candidate = i + 1;
        results[i].votes = counts[i];
    }
    sort_results(results, candidates);

    printf("%s: valid = %d, invalid = %lld\n", title, v - (int)invalid, invalid);
    printf("descending order:");
    for (int i = 0; i < candidates; i++) {
        printf(" candidate %d = %d", results[i].candidate, results[i].votes);
    }
    putchar('\n');
    for (int i = 1; i < candidates && results[i].votes == results[0].votes; i++) tied = 1;
    if (tied) {
        printf("tie for first place:");
        for (int i = 0; i < candidates && results[i].votes == results[0].votes; i++) {
            printf(" candidate %d", results[i].candidate);
        }
        putchar('\n');
    } else {
        printf("winner = candidate %d with %d votes\n", results[0].candidate, results[0].votes);
    }
}

static void hand_checks(void)
{
    int votes[] = {1, 2, 2, 3, 0, 4, 2, 1, -1, 2};
    int tie_votes[] = {1, 2, 1, 2, 3, 0, 9};
    int candidates = 3;
    int counts[MAX_CANDIDATES];
    long long invalid = 0;

    count_votes(votes, (int)(sizeof votes / sizeof votes[0]), candidates, counts, &invalid);
    printf("hand check counts: candidate 1 = %d, candidate 2 = %d, candidate 3 = %d, invalid = %lld\n",
           counts[0], counts[1], counts[2], invalid);
    report("mixed valid and invalid", votes, (int)(sizeof votes / sizeof votes[0]), candidates);
    report("tie case", tie_votes, (int)(sizeof tie_votes / sizeof tie_votes[0]), candidates);
}

static void generate_votes(int votes[], int n, int candidates)
{
    for (int i = 0; i < n; i++) {
        int value = (int)(next_random() % (unsigned int)(candidates + 2)) - 1;
        votes[i] = value;
    }
}

static long long search_names_method(const int votes[], int n, int candidates, int counts[])
{
    long long comparisons = 0;
    for (int i = 0; i < candidates; i++) counts[i] = 0;
    for (int i = 0; i < n; i++) {
        for (int c = 0; c < candidates; c++) {
            comparisons++;
            if (votes[i] == c + 1) {
                counts[c]++;
                break;
            }
        }
    }
    return comparisons;
}

static int counts_match(const int a[], const int b[], const int c[], int candidates)
{
    for (int i = 0; i < candidates; i++) {
        if (a[i] != b[i] || a[i] != c[i]) return 0;
    }
    return 1;
}

static long long sort_runs_method(const int votes[], int n, int candidates, int counts[])
{
    int *sorted = malloc((size_t)n * sizeof *sorted);
    long long comparisons;
    int i = 0;

    if (sorted == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (int j = 0; j < n; j++) sorted[j] = votes[j];
    sort_comparisons = 0;
    qsort(sorted, (size_t)n, sizeof *sorted, compare_int);
    comparisons = sort_comparisons;
    for (int j = 0; j < candidates; j++) counts[j] = 0;
    while (i < n) {
        int value = sorted[i];
        int run = 0;
        while (i < n && sorted[i] == value) {
            run++;
            i++;
        }
        if (value >= 1 && value <= candidates) counts[value - 1] = run;
    }
    free(sorted);
    return comparisons;
}

static void measure(int n, int candidates)
{
    int *votes = malloc((size_t)n * sizeof *votes);
    int counts_a[MAX_CANDIDATES];
    int counts_b[MAX_CANDIDATES];
    int counts_c[MAX_CANDIDATES];
    long long invalid = 0;
    long long search_comparisons;
    long long run_comparisons;
    clock_t start, finish;
    double count_time, search_time, run_time;

    if (votes == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    generate_votes(votes, n, candidates);

    start = clock();
    count_votes(votes, n, candidates, counts_a, &invalid);
    finish = clock();
    count_time = (double)(finish - start) / CLOCKS_PER_SEC;

    start = clock();
    search_comparisons = search_names_method(votes, n, candidates, counts_b);
    finish = clock();
    search_time = (double)(finish - start) / CLOCKS_PER_SEC;

    start = clock();
    run_comparisons = sort_runs_method(votes, n, candidates, counts_c);
    finish = clock();
    run_time = (double)(finish - start) / CLOCKS_PER_SEC;

    printf("votes = %-7d invalid = %-7lld count array %.4f s | linear search %lld comparisons %.4f s | sort runs %lld comparisons %.4f s | counts match %s\n",
           n, invalid, count_time, search_comparisons, search_time,
           run_comparisons, run_time,
           counts_match(counts_a, counts_b, counts_c, candidates) ? "yes" : "no");
    free(votes);
}

int main(void)
{
    hand_checks();
    printf("\nmeasurement with %d candidates\n", MAX_CANDIDATES);
    measure(5000, MAX_CANDIDATES);
    measure(500000, MAX_CANDIDATES);
    return 0;
}
