#include <stdio.h>

#define MAX_ITEMS 6
#define MAX_COMBO_PARTS 3
#define MAX_SALES 10

struct date {
    int day;
    int month;
    int year;
};

struct prep_time {
    int hour;
    int minute;
};

struct packaged {
    float price;
    struct date expiry;
};

struct cooked {
    float price;
    struct prep_time prepared;
};

struct combo {
    float price;
    int parts[MAX_COMBO_PARTS];
    int part_count;
};

enum item_kind { KIND_PACKAGED, KIND_COOKED, KIND_COMBO };

struct item {
    int id;
    char name[32];
    int stock;
    enum item_kind kind;
    union {
        struct packaged packaged;
        struct cooked cooked;
        struct combo combo;
    } detail;
};

struct sale {
    int item_id;
    int quantity;
    float revenue;
    struct date date;
};

static int is_leap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

static int day_number(struct date d)
{
    static const int before[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    return before[d.month - 1] + d.day + (d.month > 2 && is_leap(d.year) ? 1 : 0);
}

static int days_until(struct date from, struct date to)
{
    int days = 0;
    int year = from.year;
    while (year < to.year) {
        days += is_leap(year) ? 366 : 365;
        year++;
    }
    return days + day_number(to) - day_number(from);
}

static int find_index(const struct item *items, int count, int id)
{
    for (int i = 0; i < count; i++) {
        if (items[i].id == id) {
            return i;
        }
    }
    return -1;
}

static float item_price(const struct item *item)
{
    if (item->kind == KIND_PACKAGED) {
        return item->detail.packaged.price;
    }
    if (item->kind == KIND_COOKED) {
        return item->detail.cooked.price;
    }
    return item->detail.combo.price;
}

static void warn_expiry(const struct item *items, int count, struct date today)
{
    printf("Expiry warnings for %02d/%02d/%04d:\n", today.day, today.month, today.year);
    for (int i = 0; i < count; i++) {
        if (items[i].kind != KIND_PACKAGED) {
            continue;
        }
        int left = days_until(today, items[i].detail.packaged.expiry);
        if (left <= 2) {
            printf("  %s expires in %d day(s) on %02d/%02d/%04d\n",
                   items[i].name, left,
                   items[i].detail.packaged.expiry.day,
                   items[i].detail.packaged.expiry.month,
                   items[i].detail.packaged.expiry.year);
        }
    }
}

static void record_sale(struct item *items, int item_count, struct sale *sales,
                        int *sale_count, int id, int quantity, struct date today)
{
    int index = find_index(items, item_count, id);
    if (index < 0) {
        printf("REJECTED: unknown item id %d\n", id);
        return;
    }
    struct item *item = &items[index];
    if (*sale_count >= MAX_SALES) {
        printf("REJECTED: sales log is full\n");
        return;
    }
    if (quantity <= 0) {
        printf("REJECTED: invalid quantity %d for %s\n", quantity, item->name);
        return;
    }
    if (item->stock < quantity) {
        printf("REJECTED: %s x%d, only %d in stock\n", item->name, quantity, item->stock);
        return;
    }
    if (item->kind == KIND_COMBO) {
        for (int i = 0; i < item->detail.combo.part_count; i++) {
            int part_index = find_index(items, item_count, item->detail.combo.parts[i]);
            if (part_index < 0 || items[part_index].stock < quantity) {
                printf("REJECTED: combo %s needs %d of every part, stock unavailable\n",
                       item->name, quantity);
                return;
            }
        }
        for (int i = 0; i < item->detail.combo.part_count; i++) {
            int part_index = find_index(items, item_count, item->detail.combo.parts[i]);
            items[part_index].stock -= quantity;
        }
    } else {
        item->stock -= quantity;
    }
    float price = item_price(item);
    float revenue = price * quantity;
    sales[*sale_count].item_id = id;
    sales[*sale_count].quantity = quantity;
    sales[*sale_count].revenue = revenue;
    sales[*sale_count].date = today;
    (*sale_count)++;
    printf("SOLD: %s x%d, revenue %.2f\n", item->name, quantity, revenue);
}

static void sort_sales(struct sale *sales, int count)
{
    for (int i = 0; i < count - 1; i++) {
        int best = i;
        for (int j = i + 1; j < count; j++) {
            if (sales[j].revenue > sales[best].revenue) {
                best = j;
            }
        }
        if (best != i) {
            struct sale temp = sales[i];
            sales[i] = sales[best];
            sales[best] = temp;
        }
    }
}

static void print_summary(const struct sale *sales, int count,
                          const struct item *items, int item_count)
{
    printf("Day's sales by revenue:\n");
    for (int i = 0; i < count; i++) {
        int index = find_index(items, item_count, sales[i].item_id);
        printf("  %-16s %2d x %.2f = %.2f\n",
               items[index].name, sales[i].quantity,
               item_price(&items[index]), sales[i].revenue);
    }
}

static void print_stock(const struct item *items, int count)
{
    printf("Stock after sales:\n");
    for (int i = 0; i < count; i++) {
        printf("  %-16s %d\n", items[i].name, items[i].stock);
    }
}

int main(void)
{
    struct item items[MAX_ITEMS] = {
        {1, "Biscuit", 10, KIND_PACKAGED, {.packaged = {20.0f, {20, 10, 2026}}}},
        {2, "Juice", 5, KIND_PACKAGED, {.packaged = {30.0f, {13, 10, 2026}}}},
        {3, "Samosa", 8, KIND_COOKED, {.cooked = {15.0f, {9, 30}}}},
        {4, "Sandwich", 4, KIND_PACKAGED, {.packaged = {40.0f, {14, 10, 2026}}}},
        {5, "Tea", 20, KIND_COOKED, {.cooked = {10.0f, {10, 15}}}},
        {6, "Breakfast Combo", 6, KIND_COMBO, {.combo = {70.0f, {1, 4, 0}, 2}}}
    };
    struct sale sales[MAX_SALES];
    int sale_count = 0;
    struct date today = {12, 10, 2026};

    warn_expiry(items, MAX_ITEMS, today);
    record_sale(items, MAX_ITEMS, sales, &sale_count, 1, 2, today);
    record_sale(items, MAX_ITEMS, sales, &sale_count, 2, 99, today);
    record_sale(items, MAX_ITEMS, sales, &sale_count, 6, 1, today);
    record_sale(items, MAX_ITEMS, sales, &sale_count, 5, 3, today);
    sort_sales(sales, sale_count);
    print_summary(sales, sale_count, items, MAX_ITEMS);
    print_stock(items, MAX_ITEMS);
    return 0;
}
