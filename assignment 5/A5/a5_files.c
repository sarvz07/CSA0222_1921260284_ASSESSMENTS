#include <stdio.h>
#include <string.h>

#define NSTOCK 5
#define NDONORS 8

typedef struct {
    int id;
    char group[4];
    int age;
} Donor;

typedef struct {
    int group_code;
    int units;
} Stock;

static int write_donor_text(FILE *fp, const Donor *donors, size_t count)
{
    size_t i;
    for (i = 0; i < count; i++) {
        if (fprintf(fp, "%d %s %d\n", donors[i].id, donors[i].group, donors[i].age) < 0)
            return 0;
    }
    return 1;
}

static size_t read_donor_text(FILE *fp, Donor *donors, size_t capacity)
{
    char line[128];
    size_t count = 0;
    while (count < capacity && fgets(line, sizeof line, fp) != NULL) {
        Donor d;
        if (sscanf(line, "%d %3s %d", &d.id, d.group, &d.age) != 3) continue;
        donors[count++] = d;
    }
    return count;
}

static int write_stock_text(FILE *fp, const Stock *stock, size_t count)
{
    size_t i;
    for (i = 0; i < count; i++) {
        if (fprintf(fp, "%d %d\n", stock[i].group_code, stock[i].units) < 0) return 0;
    }
    return 1;
}

static size_t read_stock_text(FILE *fp, Stock *stock, size_t capacity)
{
    size_t count = 0;
    while (count < capacity && fscanf(fp, "%d %d",
                                      &stock[count].group_code,
                                      &stock[count].units) == 2) {
        count++;
    }
    return count;
}

static long file_size(FILE *fp)
{
    long size;

    if (fseek(fp, 0, SEEK_END) != 0) return -1;
    size = ftell(fp);
    rewind(fp);
    return size;
}

static int update_units(FILE *fp, int index, int units)
{
    Stock s;
    long offset;

    if (fp == NULL || index < 0) return 0;
    offset = (long)index * (long)sizeof s;
    if (fseek(fp, offset, SEEK_SET) != 0) return 0;
    if (fread(&s, sizeof s, 1, fp) != 1) return 0;
    s.units = units;
    if (fseek(fp, offset, SEEK_SET) != 0) return 0;
    if (fwrite(&s, sizeof s, 1, fp) != 1) return 0;
    return 1;
}

static int update_units_text(FILE *fp, int index, int units, size_t count, size_t *written)
{
    Stock all[16];
    size_t got;

    if (fp == NULL || index < 0 || (size_t)index >= count || count > 16) return 0;
    rewind(fp);
    got = read_stock_text(fp, all, count);
    if (got != count) return 0;
    all[index].units = units;
    rewind(fp);
    if (!write_stock_text(fp, all, count)) return 0;
    *written = count;
    return 1;
}

int main(void)
{
    static const char *const groups[] = {"O+", "A+", "B-", "AB+", "O-"};
    Donor donors[NDONORS];
    Donor reread[NDONORS];
    Stock stock[NSTOCK] = {{1, 40}, {2, 25}, {3, 12}, {4, 8}, {5, 30}};
    Stock bin_read[NSTOCK];
    Stock text_read[NSTOCK];
    FILE *donor_fp;
    FILE *bin_fp;
    FILE *text_fp;
    size_t size_before;
    size_t written = 0;
    size_t i;
    int match = 1;

    for (i = 0; i < NDONORS; i++) {
        donors[i].id = 7001 + (int)i;
        snprintf(donors[i].group, sizeof donors[i].group, "%s", groups[i % 5]);
        donors[i].age = 20 + (int)i;
    }

    donor_fp = tmpfile();
    bin_fp = tmpfile();
    text_fp = tmpfile();
    if (donor_fp == NULL || bin_fp == NULL || text_fp == NULL) {
        perror("tmpfile");
        if (donor_fp != NULL) fclose(donor_fp);
        if (bin_fp != NULL) fclose(bin_fp);
        if (text_fp != NULL) fclose(text_fp);
        return 1;
    }

    write_donor_text(donor_fp, donors, NDONORS);
    fwrite(stock, sizeof stock[0], NSTOCK, bin_fp);
    write_stock_text(text_fp, stock, NSTOCK);
    printf("donor text bytes=%ld, stock binary bytes=%ld, stock text bytes=%ld\n",
           ftell(donor_fp), ftell(bin_fp), ftell(text_fp));
    printf("fixed-size binary record=%zu bytes; text line is variable length\n",
           sizeof(Stock));

    rewind(donor_fp);
    if (read_donor_text(donor_fp, reread, NDONORS) != NDONORS) match = 0;
    for (i = 0; i < NDONORS; i++) {
        if (reread[i].id != donors[i].id ||
            strcmp(reread[i].group, donors[i].group) != 0 ||
            reread[i].age != donors[i].age) match = 0;
    }
    printf("donor text readback identical: %s\n", match ? "yes" : "no");

    rewind(bin_fp);
    if (fread(bin_read, sizeof stock[0], NSTOCK, bin_fp) != NSTOCK) match = 0;
    for (i = 0; i < NSTOCK; i++) {
        if (bin_read[i].group_code != stock[i].group_code ||
            bin_read[i].units != stock[i].units) match = 0;
    }
    printf("stock binary readback identical: %s\n", match ? "yes" : "no");

    rewind(text_fp);
    if (read_stock_text(text_fp, text_read, NSTOCK) != NSTOCK) match = 0;
    for (i = 0; i < NSTOCK; i++) {
        if (text_read[i].group_code != stock[i].group_code ||
            text_read[i].units != stock[i].units) match = 0;
    }
    printf("stock text readback identical: %s\n", match ? "yes" : "no");

    size_before = (size_t)file_size(bin_fp);
    printf("binary update record 3: %s\n", update_units(bin_fp, 3, 20) ? "done" : "failed");
    printf("binary file bytes before=%zu after=%ld\n", size_before, file_size(bin_fp));
    printf("text update record 3: %s\n",
           update_units_text(text_fp, 3, 20, NSTOCK, &written) ? "done" : "failed");
    printf("binary records touched=1, text records rewritten=%zu\n", written);

    rewind(bin_fp);
    printf("binary after update:");
    for (i = 0; i < NSTOCK; i++) {
        if (fread(&stock[i], sizeof stock[0], 1, bin_fp) != 1) break;
        printf(" %d:%d", stock[i].group_code, stock[i].units);
    }
    printf("\n");
    puts("text donors: human readable, variable length, append friendly, O(n) to rewrite");
    puts("binary stock: fixed-size records, fseek gives O(1) update, not portable across ABIs");
    puts("in-place text update is hard because a changed value changes the byte offset");

    fclose(donor_fp);
    fclose(bin_fp);
    fclose(text_fp);
    return 0;
}
