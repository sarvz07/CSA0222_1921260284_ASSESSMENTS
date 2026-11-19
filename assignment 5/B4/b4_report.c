#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HOSPITALS 25
#define DONORS_PER_HOSPITAL 2000
#define TOTAL_DONORS (HOSPITALS * DONORS_PER_HOSPITAL)
#define MEMORY_BUDGET (16u * 1024u * 1024u)

typedef struct {
    int id;
    char group[4];
    int age;
    int days_since;
    int hospital;
} Donor;

typedef struct {
    Donor *data;
    size_t size;
    size_t capacity;
} DonorList;

static const char *const group_names[] = {
    "O+", "O-", "A+", "A-", "B+", "B-", "AB+", "AB-"
};

static int add_donor(DonorList *list, int id, const char *group,
                     int age, int days_since, int hospital)
{
    Donor *p;

    if (list->size == list->capacity) {
        size_t next = list->capacity == 0 ? 2 : list->capacity * 2;
        if (next < list->capacity || next > SIZE_MAX / sizeof *p) return 0;
        p = realloc(list->data, next * sizeof *p);
        if (p == NULL) return 0;
        list->data = p;
        list->capacity = next;
    }
    list->data[list->size].id = id;
    snprintf(list->data[list->size].group, 4, "%s", group);
    list->data[list->size].age = age;
    list->data[list->size].days_since = days_since;
    list->data[list->size].hospital = hospital;
    list->size++;
    return 1;
}

static int write_donor_file(FILE *fp)
{
    int hospital;
    int i;

    for (hospital = 0; hospital < HOSPITALS; hospital++) {
        for (i = 0; i < DONORS_PER_HOSPITAL; i++) {
            int id = hospital * 100000 + 7001 + i;
            if (fprintf(fp, "%d %s %d %d %d\n", id, group_names[id % 8],
                        18 + id % 50, id % 60, hospital + 1) < 0) return 0;
        }
    }
    return 1;
}

static size_t load_donor_file(FILE *fp, DonorList *list, size_t limit)
{
    char line[160];
    Donor d;

    rewind(fp);
    while (list->size < limit &&
           fgets(line, sizeof line, fp) != NULL) {
        if (sscanf(line, "%d %3s %d %d %d",
                   &d.id, d.group, &d.age, &d.days_since, &d.hospital) != 5) continue;
        if (!add_donor(list, d.id, d.group, d.age, d.days_since, d.hospital)) break;
    }
    return list->size;
}

static int eligible(const Donor *d)
{
    return d->age >= 18 && d->age <= 64 && d->days_since >= 30;
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

int main(int argc, char **argv)
{
    DonorList list = {NULL, 0, 0};
    size_t limit = TOTAL_DONORS;
    size_t per_group[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    size_t total_eligible = 0;
    size_t bytes;
    FILE *fp;
    clock_t start;
    double load_time;
    double scan_time;
    size_t i;
    int youngest_id = 0;
    int youngest_age = 0;

    if (argc == 2) {
        char *end;
        long requested = strtol(argv[1], &end, 10);
        if (end == argv[1] || *end != '\0' || requested < 1 ||
            requested > TOTAL_DONORS) {
            printf("invalid donor limit: %s\n", argv[1]);
            return 1;
        }
        limit = (size_t)requested;
    } else if (argc != 1) {
        printf("unexpected arguments: argc=%d\n", argc);
        puts("usage: b4_report [donor_limit]");
        return 1;
    }

    fp = tmpfile();
    if (fp == NULL) { perror("tmpfile"); return 1; }
    if (!write_donor_file(fp)) { perror("write_donor_file"); fclose(fp); return 1; }
    printf("scale: %d hospitals x %d donors = %d donors (100x baseline of 500)\n",
           HOSPITALS, DONORS_PER_HOSPITAL, TOTAL_DONORS);
    printf("source file bytes=%ld, loading at most %zu donors\n", file_bytes(fp), limit);

    start = clock();
    load_donor_file(fp, &list, limit);
    load_time = seconds_since(start);
    bytes = list.capacity * sizeof *list.data;
    printf("loaded=%zu capacity=%zu donor_bytes=%zu overhead=%zu total_bytes=%zu time=%.6f s\n",
           list.size, list.capacity, bytes, sizeof list, bytes + sizeof list, load_time);

    start = clock();
    for (i = 0; i < list.size; i++) {
        const Donor *d = &list.data[i];
        int g;
        if (!eligible(d)) continue;
        total_eligible++;
        for (g = 0; g < 8; g++) {
            if (strcmp(d->group, group_names[g]) == 0) {
                per_group[g]++;
                break;
            }
        }
        if (youngest_id == 0 || d->age < youngest_age) {
            youngest_age = d->age;
            youngest_id = d->id;
        }
    }
    scan_time = seconds_since(start);
    printf("eligible next week=%zu scan_time=%.6f s youngest_id=%d age=%d\n",
           total_eligible, scan_time, youngest_id, youngest_age);
    printf("eligible by group:");
    for (i = 0; i < 8; i++) printf(" %s=%zu", group_names[i], per_group[i]);
    printf("\n");

    if (list.size == (size_t)TOTAL_DONORS && bytes <= MEMORY_BUDGET) {
        puts("decision: keep-all-in-memory");
        printf("justification: %zu bytes under %u byte budget, O(1) lookup after load\n",
               bytes + sizeof list, MEMORY_BUDGET);
    } else {
        puts("decision: read-in-batches");
        printf("justification: only %zu of %d donors fit the working set\n",
               list.size, TOTAL_DONORS);
    }
    puts("index alternative: O(log n) fseek reads per lookup, O(1) memory vs loaded O(n)");
    puts("nightly full report scans all records once, so a full in-memory scan is simplest");

    free(list.data);
    list.data = NULL;
    list.size = 0;
    list.capacity = 0;
    printf("freed: data=%p size=%zu capacity=%zu\n",
           (void *)list.data, list.size, list.capacity);
    fclose(fp);
    return 0;
}
