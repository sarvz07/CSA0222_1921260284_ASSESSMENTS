#include <stdio.h>

unsigned long checks = 0;

int is_prime(unsigned long n)
{
    unsigned long i = 2;
    checks = 0;
    if (n < 2) return 0;
    if (n == 2 || n == 3) return 1;
    if (n % 2 == 0) {
        checks = 1;
        return 0;
    }
    i = 3;
    do {
        checks++;
        if (n % i == 0) return 0;
        i += 2;
    } while (i * i <= n);
    return 1;
}

int main(void)
{
    unsigned long tickets[] = {2, 3, 97, 100, 7919, 1000003};
    int count = 6;
    for (int k = 0; k < count; k++) {
        int p = is_prime(tickets[k]);
        printf("%8lu  %-9s  divisions = %lu\n", tickets[k],
               p ? "prime" : "not prime", checks);
    }
    return 0;
}
