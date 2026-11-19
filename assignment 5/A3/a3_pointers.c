#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NGROUPS 8

static const char *const group_names[NGROUPS] = {
    "O+", "O-", "A+", "A-", "B+", "B-", "AB+", "AB-"
};

static int group_index(const char *name)
{
    int i;

    if (name == NULL) return -1;
    for (i = 0; i < NGROUPS; i++) {
        if (strcmp(group_names[i], name) == 0) return i;
    }
    return -1;
}

static int *lowest_stock(int *stock, size_t count)
{
    size_t i;
    int *low;

    if (stock == NULL || count == 0) return NULL;
    low = stock;
    for (i = 1; i < count; i++) {
        if (stock[i] < *low) low = &stock[i];
    }
    return low;
}

static int demo(int *stock)
{
    int *p = stock;
    int *q;
    int *low;
    size_t i;

    puts("pointer expressions with int stock[8] and int *p = stock:");
    printf("p        = %p, stock    = %p\n", (void *)p, (void *)stock);
    printf("p + 3    = %p, &stock[3] = %p\n", (void *)(p + 3), (void *)&stock[3]);
    printf("*(p + 3) = %d, p[3]      = %d, stock[3] = %d\n",
           *(p + 3), p[3], stock[3]);
    q = p;
    printf("*q++     = %d\n", *q++);
    printf("after q++: q = %p, *q = %d\n", (void *)q, *q);
    puts("memory diagram (each cell is one int):");
    for (i = 0; i < NGROUPS; i++) {
        printf("  %-4s stock[%zu] @ p+%zu = %d\n",
               group_names[i], i, i, stock[i]);
    }
    low = lowest_stock(stock, NGROUPS);
    if (low == NULL) return 1;
    printf("lowest stock: %s = %d at offset %td\n",
           group_names[low - stock], *low, low - stock);
    *low += 1;
    printf("after *low += 1: %s = %d\n", group_names[low - stock], *low);
    puts("pointer return: O(1) extra space, caller updates through returned address;");
    puts("index return: same O(n) scan, caller uses stock[index], safer after realloc");
    puts("but one extra lookup; a pointer to a local variable would dangle.");
    return 0;
}

int main(int argc, char **argv)
{
    int stock[NGROUPS] = {40, 25, 12, 8, 30, 18, 22, 15};
    int *low;
    int index;
    int amount;
    long parsed;
    char *end;

    if (argc == 1) return demo(stock);

    if (strcmp(argv[1], "lowest") == 0 && argc == 2) {
        low = lowest_stock(stock, NGROUPS);
        if (low == NULL) return 1;
        printf("lowest: %s %d\n", group_names[low - stock], *low);
        return 0;
    }
    if (strcmp(argv[1], "find") == 0 && argc == 3) {
        index = group_index(argv[2]);
        if (index < 0) {
            printf("invalid blood group: %s\n", argv[2]);
            return 1;
        }
        printf("find %s: %d units\n", group_names[index], stock[index]);
        return 0;
    }
    if (strcmp(argv[1], "issue") == 0 && argc == 4) {
        index = group_index(argv[2]);
        if (index < 0) {
            printf("invalid blood group: %s\n", argv[2]);
            return 1;
        }
        errno = 0;
        parsed = strtol(argv[3], &end, 10);
        if (errno != 0 || end == argv[3] || *end != '\0' || parsed < 1 || parsed > 1000000) {
            printf("invalid amount: %s\n", argv[3]);
            return 1;
        }
        amount = (int)parsed;
        if (stock[index] < amount) {
            printf("insufficient stock: %s has %d, requested %d\n",
                   group_names[index], stock[index], amount);
            return 1;
        }
        stock[index] -= amount;
        printf("issued %d of %s, remaining %d\n", amount, group_names[index], stock[index]);
        return 0;
    }

    puts("usage: a3_pointers");
    puts("       a3_pointers lowest");
    puts("       a3_pointers find GROUP");
    puts("       a3_pointers issue GROUP UNITS");
    puts("valid groups: O+ O- A+ A- B+ B- AB+ AB-");
    printf("invalid arguments: argc=%d\n", argc);
    return 1;
}
