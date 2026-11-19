#include <stdio.h>

#define MAX_DEPTH 64

static int dr_calls;
static int lin_calls;
static int fast_calls;
static char frames[MAX_DEPTH][48];

static int digit_sum(int n)
{
    int total = 0;
    while (n > 0) {
        total += n % 10;
        n /= 10;
    }
    return total;
}

static void print_deepest(int depth)
{
    int i;
    printf("deepest stack:");
    for (i = 0; i <= depth; i++) printf(" -> %s", frames[i]);
    printf("\n");
}

static int digital_root_at(int n, int depth, int *deepest)
{
    int next;

    if (depth >= MAX_DEPTH - 1) return n % 10;
    snprintf(frames[depth], sizeof frames[0], "digital_root(%d)", n);
    if (depth + 1 > *deepest) *deepest = depth + 1;
    printf("%*sdigital_root(%d)\n", 2 * depth, "", n);
    dr_calls++;
    if (n < 10) {
        print_deepest(depth);
        return n;
    }
    next = digit_sum(n);
    return digital_root_at(next, depth + 1, deepest);
}

static unsigned long long power_linear_at(unsigned long long base, int exp,
                                          int depth, int *deepest)
{
    if (depth >= MAX_DEPTH - 1) return 1;
    snprintf(frames[depth], sizeof frames[0], "power_linear(%llu,%d)", base, exp);
    if (depth + 1 > *deepest) *deepest = depth + 1;
    printf("%*spower_linear(%llu, %d)\n", 2 * depth, "", base, exp);
    lin_calls++;
    if (exp == 0) {
        print_deepest(depth);
        return 1;
    }
    return base * power_linear_at(base, exp - 1, depth + 1, deepest);
}

static unsigned long long power_fast_at(unsigned long long base, int exp,
                                        int depth, int *deepest)
{
    unsigned long long half;
    unsigned long long result;

    if (depth >= MAX_DEPTH - 1) return 1;
    snprintf(frames[depth], sizeof frames[0], "power_fast(%llu,%d)", base, exp);
    if (depth + 1 > *deepest) *deepest = depth + 1;
    printf("%*spower_fast(%llu, %d)\n", 2 * depth, "", base, exp);
    fast_calls++;
    if (exp == 0) {
        print_deepest(depth);
        return 1;
    }
    half = power_fast_at(base, exp / 2, depth + 1, deepest);
    result = half * half;
    if (exp % 2 != 0) result *= base;
    return result;
}

static int digital_root_loop(int n)
{
    while (n >= 10) n = digit_sum(n);
    return n;
}

static int digital_root_formula(int n)
{
    return n == 0 ? 0 : 1 + (n - 1) % 9;
}

static unsigned long long power_loop(unsigned long long base, int exp)
{
    unsigned long long result = 1;
    int e = exp;
    while (e > 0) {
        if (e % 2 != 0) result *= base;
        base *= base;
        e /= 2;
    }
    return result;
}

int main(void)
{
    int deepest = 0;
    int root;
    unsigned long long linear_value;
    unsigned long long fast_value;

    puts("digital root trace: n = 9875");
    dr_calls = 0;
    root = digital_root_at(9875, 0, &deepest);
    printf("result=%d calls=%d max_depth=%d\n", root, dr_calls, deepest);
    printf("loop=%d formula=%d\n",
           digital_root_loop(9875), digital_root_formula(9875));

    puts("digital root base cases: 0 and 1");
    dr_calls = 0;
    deepest = 0;
    root = digital_root_at(0, 0, &deepest);
    printf("digital_root(0)=%d calls=%d\n", root, dr_calls);
    dr_calls = 0;
    deepest = 0;
    root = digital_root_at(1, 0, &deepest);
    printf("digital_root(1)=%d calls=%d formula(1)=%d\n",
           root, dr_calls, digital_root_formula(1));

    puts("power trace: 2^10, linear");
    lin_calls = 0;
    deepest = 0;
    linear_value = power_linear_at(2, 10, 0, &deepest);
    printf("result=%llu calls=%d max_depth=%d\n", linear_value, lin_calls, deepest);

    puts("power trace: 2^10, halving");
    fast_calls = 0;
    deepest = 0;
    fast_value = power_fast_at(2, 10, 0, &deepest);
    printf("result=%llu calls=%d max_depth=%d\n", fast_value, fast_calls, deepest);

    puts("recurrences: linear C(n)=C(n-1)+1, C(0)=1 => n+1 calls, O(n) stack");
    puts("halving C(n)=C(n/2)+1, C(0)=1 => floor(log2 n)+2 calls, O(log n) stack");
    puts("digital root: one call per digit-sum reduction, stack depth = digits reduced");

    printf("tests: 0^0=%llu 5^0=%llu 7^1=%llu 2^10=%llu loop2^10=%llu\n",
           power_loop(0, 0), power_loop(5, 0), power_loop(7, 1),
           power_loop(2, 10), power_loop(2, 10));
    printf("tests: root(9)=%d root(123456789)=%d root(10)=%d\n",
           digital_root_loop(9), digital_root_loop(123456789), digital_root_loop(10));
    puts("risks: no base case means unbounded recursion and stack overflow;");
    puts("replace recursion by a loop when depth is large or no structure is gained.");
    return 0;
}
