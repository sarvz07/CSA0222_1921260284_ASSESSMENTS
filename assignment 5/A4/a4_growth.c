#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GROUP_LEN 4
#define TEST_DONORS 100

typedef struct {
    int id;
    char group[GROUP_LEN];
} Donor;

typedef struct {
    Donor *data;
    size_t size;
    size_t capacity;
} DonorList;

typedef struct {
    size_t reallocs;
    size_t copies;
} GrowthStats;

static int grow(DonorList *list, size_t new_capacity, size_t limit, GrowthStats *stats)
{
    Donor *tmp;

    if (new_capacity < list->capacity) return 0;
    if (new_capacity > SIZE_MAX / sizeof *tmp) return 0;
    if (new_capacity > limit) {
        fprintf(stderr, "grow: capacity %zu rejected, list unchanged\n", new_capacity);
        return 0;
    }
    tmp = realloc(list->data, new_capacity * sizeof *tmp);
    if (tmp == NULL) {
        fprintf(stderr, "grow: realloc failed, list unchanged\n");
        return 0;
    }
    list->data = tmp;
    list->capacity = new_capacity;
    if (stats != NULL) {
        stats->reallocs++;
        stats->copies += list->size;
    }
    return 1;
}

static int add_donor(DonorList *list, int id, const char *group,
                     size_t step, size_t limit, GrowthStats *stats)
{
    if (list->size == list->capacity) {
        size_t next = list->capacity == 0 ? (step == 0 ? 2 : step)
                                          : (step == 0 ? list->capacity * 2
                                                       : list->capacity + step);
        if (!grow(list, next, limit, stats)) return 0;
    }
    list->data[list->size].id = id;
    snprintf(list->data[list->size].group, GROUP_LEN, "%s", group);
    list->size++;
    return 1;
}

static void print_list(const DonorList *list)
{
    size_t i;
    printf("list: size=%zu capacity=%zu data:", list->size, list->capacity);
    for (i = 0; i < list->size; i++) {
        printf(" %d(%s)", list->data[i].id, list->data[i].group);
    }
    printf("\n");
}

int main(void)
{
    static const char *const groups[] = {"O+", "A+", "B-", "AB+", "O-"};
    DonorList doubling = {NULL, 0, 0};
    DonorList fixed = {NULL, 0, 0};
    DonorList limited = {NULL, 0, 0};
    GrowthStats double_stats = {0, 0};
    GrowthStats fixed_stats = {0, 0};
    int i;

    for (i = 0; i < 5; i++) {
        add_donor(&doubling, 7001 + i, groups[i], 0, SIZE_MAX, &double_stats);
        printf("doubling: added %d, size=%zu, capacity=%zu\n",
               doubling.data[i].id, doubling.size, doubling.capacity);
    }
    print_list(&doubling);

    add_donor(&limited, 8001, "O+", 0, 2, NULL);
    add_donor(&limited, 8002, "A+", 0, 2, NULL);
    printf("third add with capacity limit 2: %s\n",
           add_donor(&limited, 8003, "B-", 0, 2, NULL) ? "accepted" : "rejected");
    print_list(&limited);
    printf("old data preserved: %d %s\n", limited.data[0].id, limited.data[0].group);

    for (i = 0; i < TEST_DONORS; i++) {
        if (!add_donor(&doubling, 7100 + i, groups[i % 5], 0, SIZE_MAX, &double_stats)) break;
        if (!add_donor(&fixed, 8100 + i, groups[i % 5], 10, SIZE_MAX, &fixed_stats)) break;
    }
    printf("n=%d doubling: reallocs=%zu copies=%zu capacity=%zu unused=%zu\n",
           TEST_DONORS + 5, double_stats.reallocs, double_stats.copies,
           doubling.capacity, doubling.capacity - doubling.size);
    printf("n=%d fixed +10: reallocs=%zu copies=%zu capacity=%zu unused=%zu\n",
           TEST_DONORS, fixed_stats.reallocs, fixed_stats.copies,
           fixed.capacity, fixed.capacity - fixed.size);
    puts("doubling copies O(n); fixed step copies O(n*n); unused < n vs < step");

    puts("leak: overwrite the only pointer to a block without freeing it; keep temp and free once");
    puts("dangling: use data after free; set pointer to NULL and never dereference after free");
    puts("double free: free the same pointer twice; free once then set pointer to NULL");

    free(fixed.data);
    free(limited.data);
    free(doubling.data);
    printf("freed all lists, bytes released=%zu\n",
           (doubling.capacity + fixed.capacity + limited.capacity) * sizeof(Donor));
    return 0;
}
