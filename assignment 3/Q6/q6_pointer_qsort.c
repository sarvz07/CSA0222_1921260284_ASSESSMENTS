#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAMES 500
#define NAME_LEN 30

static long long qsort_comparisons;

static int compare_strings(const void *p, const void *q)
{
    const char *a = *(const char *const *)p;
    const char *b = *(const char *const *)q;
    qsort_comparisons++;
    return strcmp(a, b);
}

static int compare_case_insensitive(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static int compare_ignore_case(const void *p, const void *q)
{
    const char *a = *(const char *const *)p;
    const char *b = *(const char *const *)q;
    qsort_comparisons++;
    return compare_case_insensitive(a, b);
}

static void make_name(char *buf, size_t size, int i)
{
    snprintf(buf, size, "Resident%04d", (i * 3571) % 10000);
}

static void print_list(char *list[], int n, const char *title)
{
    printf("%s:", title);
    for (int i = 0; i < n && i < 12; i++) printf(" %s", list[i]);
    if (n > 12) printf(" ...");
    putchar('\n');
}

int main(void)
{
    char *list[MAX_NAMES];
    static char storage[MAX_NAMES][NAME_LEN];
    char *small_list[] = {"apple", "Apple", "banana", "Banana", "apple"};
    char *small_ptrs[5];
    int n = 500;

    for (int i = 0; i < 5; i++) small_ptrs[i] = small_list[i];
    qsort_comparisons = 0;
    qsort(small_ptrs, 5, sizeof *small_ptrs, compare_strings);
    print_list(small_ptrs, 5, "strcmp order");
    printf("strcmp comparisons = %lld\n", qsort_comparisons);

    qsort_comparisons = 0;
    qsort(small_ptrs, 5, sizeof *small_ptrs, compare_ignore_case);
    print_list(small_ptrs, 5, "case-insensitive order");
    printf("case-insensitive comparisons = %lld\n", qsort_comparisons);
    printf("\"apple\" vs \"Apple\": strcmp = %d, case-insensitive = %d\n",
           strcmp("apple", "Apple"), compare_case_insensitive("apple", "Apple"));

    for (int i = 0; i < n; i++) {
        make_name(storage[i], NAME_LEN, i);
        list[i] = storage[i];
    }
    qsort_comparisons = 0;
    qsort(list, (size_t)n, sizeof *list, compare_strings);
    printf("\n%d pointers sorted with qsort: %lld strcmp calls, each swap is %zu bytes, extra space %zu bytes\n",
           n, qsort_comparisons, sizeof list[0], sizeof list);
    print_list(list, n, "first entries");
    printf("duplicates adjacent:");
    for (int i = 1; i < n; i++) {
        if (strcmp(list[i - 1], list[i]) == 0) printf(" %s", list[i]);
    }
    putchar('\n');
    return 0;
}
