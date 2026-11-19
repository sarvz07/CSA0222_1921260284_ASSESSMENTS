#ifndef BANK_H
#define BANK_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    int id;
    char group[4];
    int age;
} Donor;

typedef struct {
    int group_code;
    int units;
} Stock;

typedef struct {
    int count;
    int youngest_id;
} EligibleSummary;

int register_donor(Donor *donors, size_t capacity, size_t *size,
                   int id, const char *group, int age);
int is_eligible(const Donor *donor, const char *group, int min_age, int max_age);
int eligible_summary_out(const Donor *donors, size_t count, const char *group,
                         int min_age, int max_age, int *out_count, int *out_youngest_id);
EligibleSummary eligible_summary_struct(const Donor *donors, size_t count,
                                        const char *group, int min_age, int max_age);
int update_stock(Stock *stock, size_t count, int group_code, int delta);
int save_donors(FILE *fp, const Donor *donors, size_t count);
size_t load_donors(FILE *fp, Donor *donors, size_t capacity);

#endif
