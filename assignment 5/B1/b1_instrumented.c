#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long long calls;

static int digit_sum(int n)
{
    int total = 0;
    while (n > 0) {
        total += n % 10;
        n /= 10;
    }
    return total;
}

static int digital_root(int n)
{
    calls++;
    if (n < 10) return n;
    return digital_root(digit_sum(n));
}

static unsigned long long power_linear(unsigned long long base, int exp)
{
    calls++;
    if (exp == 0) return 1;
    return base * power_linear(base, exp - 1);
}

static unsigned long long power_fast(unsigned long long base, int exp)
{
    unsigned long long half;
    unsigned long long result;

    calls++;
    if (exp == 0) return 1;
    half = power_fast(base, exp / 2);
    result = half * half;
    if (exp % 2 != 0) result *= base;
    return result;
}

static unsigned long long power_fast_twice(unsigned long long base, int exp)
{
    unsigned long long result;

    calls++;
    if (exp == 0) return 1;
    result = power_fast_twice(base, exp / 2) * power_fast_twice(base, exp / 2);
    if (exp % 2 != 0) result *= base;
    return result;
}

static unsigned long long power_iterative(unsigned long long base, int exp)
{
    unsigned long long result = 1;

    calls++;
    while (exp > 0) {
        if (exp % 2 != 0) result *= base;
        base *= base;
        exp /= 2;
    }
    return result;
}

int main(int argc, char **argv)
{
    static const int exponents[] = {10, 100, 1000, 10000};
    static const int roots[] = {0, 1, 9, 9875, 123456789};
    unsigned long long value;
    size_t i;
    int n;

    if (argc == 2 && strcmp(argv[1], "--stack-probe") == 0) {
        puts("stack probe is off by default; use --stack-probe N to attempt depth N");
        puts("the linear recursive version can overflow the stack for large N");
        return 0;
    }
    if (argc == 3 && strcmp(argv[1], "--stack-probe") == 0) {
        char *end;
        long requested = strtol(argv[2], &end, 10);
        if (end == argv[2] || *end != '\0' || requested < 1 || requested > 100000000L) {
            printf("invalid probe depth: %s\n", argv[2]);
            return 1;
        }
        printf("warning: attempting linear recursion depth %ld (may overflow stack)\n", requested);
        calls = 0;
        value = power_linear(1, (int)requested);
        printf("probe result=%llu calls=%llu\n", value, calls);
        return 0;
    }
    if (argc != 1) {
        printf("unexpected arguments: argc=%d\n", argc);
        return 1;
    }

    puts("digital root tests (0 verifies the base case, 1 verifies identity):");
    for (i = 0; i < sizeof roots / sizeof roots[0]; i++) {
        calls = 0;
        n = digital_root(roots[i]);
        printf("digital_root(%d)=%d calls=%llu\n", roots[i], n, calls);
    }

    puts("power tests (0 exponent, 1 exponent, ordinary, and chosen value):");
    calls = 0;
    value = power_linear(0, 0);
    printf("0^0=%llu calls=%llu\n", value, calls);
    calls = 0;
    value = power_linear(5, 0);
    printf("5^0=%llu calls=%llu\n", value, calls);
    calls = 0;
    value = power_linear(7, 1);
    printf("7^1=%llu calls=%llu\n", value, calls);
    calls = 0;
    value = power_fast(2, 10);
    printf("2^10=%llu calls=%llu\n", value, calls);
    calls = 0;
    value = power_fast(2, 30);
    printf("2^30=%llu calls=%llu\n", value, calls);

    puts("call counts by exponent (unsigned arithmetic, base=1 for large n):");
    for (i = 0; i < sizeof exponents / sizeof exponents[0]; i++) {
        n = exponents[i];
        calls = 0;
        power_linear(1, n);
        printf("n=%-6d linear=%-7llu predicted=%-7d", n, calls, n + 1);
        calls = 0;
        power_fast(1, n);
        printf(" fast=%-4llu", calls);
        calls = 0;
        power_iterative(1, n);
        printf(" iterative=%llu\n", calls);
    }

    calls = 0;
    power_fast_twice(1, 10);
    printf("double-recursive halving at n=10: %llu calls\n", calls);
    puts("linear T(n)=T(n-1)+1; halving T(n)=T(n/2)+1; two halves T(n)=1+2T(n/2)");

    calls = 0;
    value = power_iterative(1, 1000000);
    printf("iterative n=1000000: result=%llu calls=%llu stack O(1)\n", value, calls);
    puts("code matches the design; only instrumentation was added for counting.");
    return 0;
}
