#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TARGET_INDEX 49999
#define SMALL_N 10

typedef struct {
    int id;
    char group[4];
    int age;
} Donor;

typedef struct {
    int group_code;
    int units;
} Stock;

static const char *const group_names[] = {
    "O+", "O-", "A+", "A-", "B+", "B-", "AB+", "AB-"
};

static void make_donor(Donor *d, int i)
{
    d->id = 7001 + i;
    snprintf(d->group, sizeof d->group, "%s", group_names[i % 8]);
    d->age = 18 + i % 50;
}

static void make_stock(Stock *s, int i)
{
    s->group_code = i % 8 + 1;
    s->units = i % 100;
}

static int write_text(FILE *fp, const Donor *donors, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (fprintf(fp, "%d %s %d\n",
                    donors[i].id, donors[i].group, donors[i].age) < 0) return 0;
    }
    return 1;
}

static int read_text(FILE *fp, Donor *donors, int n)
{
    char line[128];
    int count = 0;

    rewind(fp);
    while (count < n && fgets(line, sizeof line, fp) != NULL) {
        Donor d;
        if (sscanf(line, "%d %3s %d", &d.id, d.group, &d.age) != 3) continue;
        donors[count++] = d;
    }
    return count;
}

static int donor_equal(const Donor *a, const Donor *b)
{
    return a->id == b->id && strcmp(a->group, b->group) == 0 && a->age == b->age;
}

static double seconds_since(clock_t start)
{
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

static long file_bytes(FILE *fp)
{
    long size;

    if (fseek(fp, 0, SEEK_END) != 0) return -1;
    size = ftell(fp);
    rewind(fp);
    return size;
}

static void small_correctness(void)
{
    Donor donors[SMALL_N];
    Donor back[SMALL_N];
    Stock stocks[SMALL_N];
    Stock back_stock[SMALL_N];
    FILE *text_fp = tmpfile();
    FILE *bin_fp = tmpfile();
    int i;
    int match = 1;

    if (text_fp == NULL || bin_fp == NULL) {
        perror("tmpfile");
        if (text_fp != NULL) fclose(text_fp);
        if (bin_fp != NULL) fclose(bin_fp);
        exit(1);
    }
    for (i = 0; i < SMALL_N; i++) {
        make_donor(&donors[i], i);
        make_stock(&stocks[i], i);
    }
    if (!write_text(text_fp, donors, SMALL_N)) match = 0;
    if (fwrite(stocks, sizeof stocks[0], SMALL_N, bin_fp) != SMALL_N) match = 0;
    if (read_text(text_fp, back, SMALL_N) != SMALL_N) match = 0;
    rewind(bin_fp);
    if (fread(back_stock, sizeof back_stock[0], SMALL_N, bin_fp) != SMALL_N) match = 0;
    for (i = 0; i < SMALL_N; i++) {
        if (!donor_equal(&donors[i], &back[i])) match = 0;
        if (back_stock[i].group_code != stocks[i].group_code ||
            back_stock[i].units != stocks[i].units) match = 0;
    }
    printf("10 donors: text and binary readback identical: %s\n", match ? "yes" : "no");
    printf("10 donors: text bytes=%ld binary bytes=%ld\n",
           file_bytes(text_fp), file_bytes(bin_fp));
    fclose(text_fp);
    fclose(bin_fp);
}

static void benchmark(int n)
{
    Donor *donors;
    Donor *back;
    Stock *stocks;
    Stock *back_stock;
    FILE *text_fp;
    FILE *bin_fp;
    clock_t start;
    double write_text_time;
    double write_bin_time;
    double read_text_time;
    double read_bin_time;
    long text_bytes;
    long bin_bytes;
    int match = 1;
    int i;

    donors = malloc((size_t)n * sizeof *donors);
    back = malloc((size_t)n * sizeof *back);
    stocks = malloc((size_t)n * sizeof *stocks);
    back_stock = malloc((size_t)n * sizeof *back_stock);
    text_fp = tmpfile();
    bin_fp = tmpfile();
    if (donors == NULL || back == NULL || stocks == NULL ||
        back_stock == NULL || text_fp == NULL || bin_fp == NULL) {
        perror("allocation or tmpfile");
        free(donors);
        free(back);
        free(stocks);
        free(back_stock);
        if (text_fp != NULL) fclose(text_fp);
        if (bin_fp != NULL) fclose(bin_fp);
        exit(1);
    }
    for (i = 0; i < n; i++) {
        make_donor(&donors[i], i);
        make_stock(&stocks[i], i);
    }

    start = clock();
    write_text(text_fp, donors, n);
    write_text_time = seconds_since(start);
    start = clock();
    fwrite(stocks, sizeof stocks[0], (size_t)n, bin_fp);
    write_bin_time = seconds_since(start);
    text_bytes = file_bytes(text_fp);
    bin_bytes = file_bytes(bin_fp);

    start = clock();
    if (read_text(text_fp, back, n) != n) match = 0;
    read_text_time = seconds_since(start);
    start = clock();
    rewind(bin_fp);
    if (fread(back_stock, sizeof back_stock[0], (size_t)n, bin_fp) != (size_t)n) match = 0;
    read_bin_time = seconds_since(start);
    for (i = 0; i < n; i++) {
        if (!donor_equal(&donors[i], &back[i])) match = 0;
        if (back_stock[i].group_code != stocks[i].group_code ||
            back_stock[i].units != stocks[i].units) match = 0;
    }

    printf("n=%-7d text_bytes=%-8ld binary_bytes=%-8ld write(text/binary)=%.6f/%.6f "
           "read(text/binary)=%.6f/%.6f match=%s\n",
           n, text_bytes, bin_bytes, write_text_time, write_bin_time,
           read_text_time, read_bin_time, match ? "yes" : "no");

    if (n == 100000) {
        Stock one;
        long offset = (long)TARGET_INDEX * (long)sizeof one;
        long before = file_bytes(text_fp);
        long after;

        printf("updating record %d (age %d -> 68, units %d -> 77)\n",
               TARGET_INDEX + 1, donors[TARGET_INDEX].age, stocks[TARGET_INDEX].units);
        start = clock();
        if (fseek(bin_fp, offset, SEEK_SET) == 0 &&
            fread(&one, sizeof one, 1, bin_fp) == 1) {
            one.units = 77;
            if (fseek(bin_fp, offset, SEEK_SET) == 0) {
                fwrite(&one, sizeof one, 1, bin_fp);
            }
        }
        printf("binary in-place update: %.6f s, records touched=1\n", seconds_since(start));

        start = clock();
        donors[TARGET_INDEX].age = 68;
        write_text(text_fp, donors, n);
        printf("text full rewrite: %.6f s, records rewritten=%d\n",
               seconds_since(start), n);
        after = file_bytes(text_fp);
        printf("text bytes before=%ld after=%ld, binary bytes=%ld\n",
               before, after, file_bytes(bin_fp));
        rewind(bin_fp);
        if (fseek(bin_fp, offset, SEEK_SET) == 0 &&
            fread(&one, sizeof one, 1, bin_fp) == 1) {
            printf("binary record %d after update: group=%d units=%d\n",
                   TARGET_INDEX + 1, one.group_code, one.units);
        }
        printf("complexity per update: binary O(1), text rewrite O(n)\n");
    }

    fclose(text_fp);
    fclose(bin_fp);
    free(donors);
    free(back);
    free(stocks);
    free(back_stock);
}

int main(void)
{
    static const int sizes[] = {1000, 10000, 100000};
    size_t i;

    small_correctness();
    for (i = 0; i < sizeof sizes / sizeof sizes[0]; i++) benchmark(sizes[i]);
    puts("binary records are fixed size and ABI dependent; text is portable and human readable");
    puts("recommendation: text donors for inspection, binary stock for fast in-place updates");
    puts("one fwrite for the whole array replaces n fwrite calls: O(n) calls become 1");
    return 0;
}
