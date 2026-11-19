#include <stdio.h>
#include <string.h>
#include "bank.h"

int register_donor(Donor *donors, size_t capacity, size_t *size,
                   int id, const char *group, int age)
{
    Donor *d;

    if (donors == NULL || size == NULL || group == NULL || *size >= capacity) return 0;
    d = &donors[*size];
    d->id = id;
    snprintf(d->group, sizeof d->group, "%s", group);
    d->age = age;
    (*size)++;
    return 1;
}

int is_eligible(const Donor *donor, const char *group, int min_age, int max_age)
{
    if (donor == NULL || group == NULL) return 0;
    return strcmp(donor->group, group) == 0 &&
           donor->age >= min_age && donor->age <= max_age;
}

int eligible_summary_out(const Donor *donors, size_t count, const char *group,
                         int min_age, int max_age, int *out_count, int *out_youngest_id)
{
    size_t i;
    size_t youngest = 0;
    int found = 0;
    int total = 0;

    if (donors == NULL || out_count == NULL || out_youngest_id == NULL) return 0;
    for (i = 0; i < count; i++) {
        if (!is_eligible(&donors[i], group, min_age, max_age)) continue;
        if (!found || donors[i].age < donors[youngest].age) {
            youngest = i;
            found = 1;
        }
        total++;
    }
    *out_count = total;
    *out_youngest_id = found ? donors[youngest].id : 0;
    return 1;
}

EligibleSummary eligible_summary_struct(const Donor *donors, size_t count,
                                        const char *group, int min_age, int max_age)
{
    EligibleSummary result = {0, 0};

    if (!eligible_summary_out(donors, count, group, min_age, max_age,
                              &result.count, &result.youngest_id)) {
        result.count = 0;
        result.youngest_id = 0;
    }
    return result;
}

int update_stock(Stock *stock, size_t count, int group_code, int delta)
{
    size_t index;

    if (stock == NULL || group_code < 1 || (size_t)group_code > count) return 0;
    index = (size_t)group_code - 1;
    if (delta < 0 && stock[index].units < -delta) return 0;
    stock[index].units += delta;
    return 1;
}

int save_donors(FILE *fp, const Donor *donors, size_t count)
{
    size_t i;

    if (fp == NULL || donors == NULL) return 0;
    for (i = 0; i < count; i++) {
        if (fprintf(fp, "%d %s %d\n", donors[i].id, donors[i].group, donors[i].age) < 0)
            return 0;
    }
    return 1;
}

size_t load_donors(FILE *fp, Donor *donors, size_t capacity)
{
    char line[128];
    size_t count = 0;

    if (fp == NULL || donors == NULL) return 0;
    while (count < capacity && fgets(line, sizeof line, fp) != NULL) {
        Donor d;
        if (sscanf(line, "%d %3s %d", &d.id, d.group, &d.age) != 3) continue;
        donors[count++] = d;
    }
    return count;
}
