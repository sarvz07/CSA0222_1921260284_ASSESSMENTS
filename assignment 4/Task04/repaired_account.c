#include <stdio.h>

struct account { int number; float balance; };

void addInterest(struct account *a, float rate) {
    a->balance = a->balance + a->balance * rate / 100;
}

int main(void) {
    struct account list[3] = {{101, 5000}, {102, 12000}, {103, 800}};
    for (int i = 0; i < 3; i++) addInterest(&list[i], 4);
    for (int i = 0; i < 3; i++) printf("%d %.2f\n", list[i].number, list[i].balance);
    return 0;
}
