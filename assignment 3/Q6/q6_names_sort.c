#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 30
#define MAX_NAMES 500

static void sort_names(char names[][NAME_LEN], int n, long long *comparisons, long long *copies)
{
    *comparisons = 0;
    *copies = 0;
    for (int i = 1; i < n; i++) {
        char key[NAME_LEN];
        int j = i - 1;
        strcpy(key, names[i]);
        *copies += strlen(key) + 1;
        while (j >= 0) {
            (*comparisons)++;
            if (strcmp(names[j], key) <= 0) break;
            strcpy(names[j + 1], names[j]);
            *copies += strlen(names[j]) + 1;
            j--;
        }
        strcpy(names[j + 1], key);
        *copies += strlen(key) + 1;
    }
}

static void sort_indices(const char names[][NAME_LEN], int index[], int n,
                         long long *comparisons, long long *moves)
{
    *comparisons = 0;
    *moves = 0;
    for (int i = 1; i < n; i++) {
        int key = index[i];
        int j = i - 1;
        while (j >= 0) {
            (*comparisons)++;
            if (strcmp(names[index[j]], names[key]) <= 0) break;
            index[j + 1] = index[j];
            (*moves)++;
            j--;
        }
        index[j + 1] = key;
        (*moves)++;
    }
}

static int binary_search_names(const char names[][NAME_LEN], int n, const char *key,
                               long long *comparisons)
{
    int low = 0;
    int high = n - 1;
    *comparisons = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int order;
        (*comparisons)++;
        order = strcmp(names[mid], key);
        if (order == 0) return mid;
        if (order < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

static int read_names(FILE *fp, char names[][NAME_LEN], int capacity, int *truncated)
{
    int n = 0;
    *truncated = 0;
    while (n < capacity && fgets(names[n], NAME_LEN, fp) != NULL) {
        char *newline = strchr(names[n], '\n');
        if (newline != NULL) {
            *newline = '\0';
        } else {
            int ch;
            while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
            }
            (*truncated)++;
        }
        n++;
    }
    return n;
}

static void directory_demo(void)
{
    const char *source[] = {"Alice", "Bob", "Charlie", "Alice", "Diana", "Eve", "bob", "Aaron"};
    char names[MAX_NAMES][NAME_LEN];
    int index[MAX_NAMES];
    FILE *fp = tmpfile();
    long long name_cmp, name_copies, index_cmp, index_moves, search_cmp;
    int n, truncated;
    int pos;

    if (fp == NULL) {
        fprintf(stderr, "tmpfile failed\n");
        return;
    }
    for (size_t i = 0; i < sizeof source / sizeof source[0]; i++) fprintf(fp, "%s\n", source[i]);
    rewind(fp);
    n = read_names(fp, names, MAX_NAMES, &truncated);
    fclose(fp);

    for (int i = 0; i < n; i++) index[i] = i;
    sort_indices(names, index, n, &index_cmp, &index_moves);
    sort_names(names, n, &name_cmp, &name_copies);

    printf("sorted directory (strcmp/strcpy sort):\n");
    for (int i = 0; i < n; i++) printf("%s%s", i == 0 ? "  " : ", ", names[i]);
    printf("\nstrcmp calls = %lld, character copies = %lld\n", name_cmp, name_copies);
    printf("index sort: strcmp calls = %lld, index moves = %lld, extra ints = %d\n",
           index_cmp, index_moves, n);

    printf("duplicates found by adjacent comparison:");
    for (int i = 1; i < n; i++) {
        if (strcmp(names[i - 1], names[i]) == 0) printf(" %s", names[i]);
    }
    printf("\nstrcmp case sensitivity: \"apple\" < \"Apple\" is %s\n",
           strcmp("apple", "Apple") < 0 ? "yes" : "no");

    pos = binary_search_names(names, n, "Diana", &search_cmp);
    printf("search \"Diana\" -> index %d, strcmp calls %lld\n", pos, search_cmp);
    pos = binary_search_names(names, n, "Zoe", &search_cmp);
    printf("search \"Zoe\"   -> index %d, strcmp calls %lld\n", pos, search_cmp);
    printf("truncated lines while reading = %d\n", truncated);
}

static void length_tests(void)
{
    char s29[30];
    char s30[31];
    char buffer[NAME_LEN];
    FILE *fp = tmpfile();
    int n, truncated;

    if (fp == NULL) {
        fprintf(stderr, "tmpfile failed\n");
        return;
    }
    memset(s29, 'A', 29);
    s29[29] = '\0';
    memset(s30, 'B', 30);
    s30[30] = '\0';
    fprintf(fp, "%s\n%s\n", s29, s30);
    rewind(fp);
    n = read_names(fp, &buffer, 1, &truncated);
    printf("\nlength 29 line stored as %zu characters\n", strlen(buffer));
    if (n > 0) {
        n = read_names(fp, &buffer, 1, &truncated);
    }
    printf("length 30 line stored as %zu characters, truncated flag = %d\n",
           strlen(buffer), truncated);
    printf("names[500][30] holds at most 29 characters plus the null character\n");
    fclose(fp);
}

static void make_name(char *buf, size_t size, int i)
{
    snprintf(buf, size, "Student%06d", (i * 7919) % 1000000);
}

static void benchmark(int n)
{
    char (*names)[NAME_LEN] = malloc((size_t)n * sizeof *names);
    int *index = malloc((size_t)n * sizeof *index);
    long long name_cmp, name_copies, index_cmp, index_moves, search_cmp;
    char key[NAME_LEN];
    int pos;

    if (names == NULL || index == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        make_name(names[i], NAME_LEN, i);
        index[i] = i;
    }

    sort_indices(names, index, n, &index_cmp, &index_moves);
    sort_names(names, n, &name_cmp, &name_copies);
    make_name(key, sizeof key, n / 2);
    pos = binary_search_names(names, n, key, &search_cmp);

    printf("n = %-6d copy-sort strcmp %-12lld copies %-14lld | index-sort strcmp %-12lld moves %-12lld | search present %d (%lld strcmp)\n",
           n, name_cmp, name_copies, index_cmp, index_moves, pos, search_cmp);
    free(names);
    free(index);
}

int main(void)
{
    directory_demo();
    length_tests();
    printf("\nmeasurement on generated names\n");
    benchmark(100);
    benchmark(500);
    benchmark(5000);
    return 0;
}
