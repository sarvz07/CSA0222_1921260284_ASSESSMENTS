#include <stdio.h>
#include "bank.h"

int main(void)
{
    Donor donors[16];
    Stock stock[8] = {{1, 40}, {2, 25}, {3, 12}, {4, 8},
                      {5, 30}, {6, 18}, {7, 22}, {8, 15}};
    size_t size = 0;
    size_t loaded_count;
    Donor loaded[16];
    EligibleSummary by_struct;
    int by_out_count = 0;
    int by_out_youngest = 0;
    FILE *fp;
    size_t i;

    puts("structure chart:");
    puts("main -> register_donor, is_eligible, eligible_summary_out,");
    puts("       eligible_summary_struct, update_stock, save_donors, load_donors");
    puts("eligible_summary_out -> is_eligible");
    puts("eligible_summary_struct -> eligible_summary_out -> is_eligible");

    puts("interfaces: scalar = by value, Donor * / DonorList * = address of mutable state,");
    puts("const Donor * = read-only array, const char * = read-only text, FILE * = open stream");

    register_donor(donors, 16, &size, 7001, "O+", 24);
    register_donor(donors, 16, &size, 7002, "O+", 19);
    register_donor(donors, 16, &size, 7003, "O+", 61);
    register_donor(donors, 16, &size, 7004, "A+", 33);
    register_donor(donors, 16, &size, 7005, "O+", 17);
    printf("registered %zu donors\n", size);

    eligible_summary_out(donors, size, "O+", 18, 65, &by_out_count, &by_out_youngest);
    by_struct = eligible_summary_struct(donors, size, "O+", 18, 65);
    printf("pointer outputs: count=%d youngest_id=%d\n", by_out_count, by_out_youngest);
    printf("struct return  : count=%d youngest_id=%d\n",
           by_struct.count, by_struct.youngest_id);
    printf("both designs agree: %s\n",
           (by_out_count == by_struct.count &&
            by_out_youngest == by_struct.youngest_id) ? "yes" : "no");

    printf("update stock group 3 by -4: %s, units=%d\n",
           update_stock(stock, 8, 3, -4) ? "ok" : "rejected", stock[2].units);
    printf("update stock group 3 by -100: %s, units=%d\n",
           update_stock(stock, 8, 3, -100) ? "ok" : "rejected", stock[2].units);

    fp = tmpfile();
    if (fp == NULL) { perror("tmpfile"); return 1; }
    if (!save_donors(fp, donors, size)) { perror("save_donors"); fclose(fp); return 1; }
    rewind(fp);
    loaded_count = load_donors(fp, loaded, 16);
    printf("saved and loaded %zu donors", loaded_count);
    if (loaded_count > 0) printf(", first=%d last=%d", loaded[0].id, loaded[loaded_count - 1].id);
    printf("\n");
    fclose(fp);

    printf("stock after updates:");
    for (i = 0; i < 8; i++) printf(" %d:%d", stock[i].group_code, stock[i].units);
    printf("\n");
    return 0;
}
