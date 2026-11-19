#include <stdio.h>

struct account { int number; float balance; };

void addInterest(struct account a, float rate) {
    printf("inside addInterest: &a = %p\n", (void *)&a);
    a.balance = a.balance + a.balance * rate / 100;
}

int main(void) {
    struct account list[3] = {{101, 5000}, {102, 12000}, {103, 800}};
    struct account *p = list;
    printf("in main: &list[0] = %p, p = %p\n", (void *)&list[0], (void *)p);
    for (int i = 0; i < 3; i++) addInterest(list[i], 4);
#ifdef SHOW_BROKEN_STATEMENT
    printf("%d %f\n", p.number, p.balance);
#else
    printf("in main: list[0] = %d %.2f\n", list[0].number, list[0].balance);
#endif
    for (int i = 0; i < 3; i++) printf("%d %.2f\n", list[i].number, list[i].balance);
    return 0;
}
