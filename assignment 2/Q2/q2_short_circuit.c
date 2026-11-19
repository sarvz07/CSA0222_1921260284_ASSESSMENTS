#include <stdio.h>

static int verify_calls;

int slow_verify(int passenger_id)
{
    verify_calls++;
    return passenger_id % 10 == 0;
}

int main(void)
{
    int n = 1000;
    int senior[1000];
    int eligible[1000];
    int disability[1000];
    int i;
    int granted;
    long calls_a = 0;
    long calls_b = 0;
    long calls_c = 0;

    for (i = 0; i < n; i++) {
        senior[i] = (i % 3 == 0);
        eligible[i] = (i % 4 == 0);
        disability[i] = (i % 20 == 0);
    }
    (void)disability;

    verify_calls = 0;
    for (i = 0; i < n; i++) {
        granted = (senior[i] && eligible[i]) || slow_verify(i);
        if (granted) { }
    }
    calls_a = verify_calls;

    verify_calls = 0;
    for (i = 0; i < n; i++) {
        granted = slow_verify(i) || (senior[i] && eligible[i]);
        if (granted) { }
    }
    calls_b = verify_calls;

    verify_calls = 0;
    for (i = 0; i < n; i++) {
        granted = (senior[i] & eligible[i]) || slow_verify(i);
        if (granted) { }
    }
    calls_c = verify_calls;

    printf("1000 passengers:\n");
    printf("Order (senior&&eligible) || slow_verify: calls=%ld\n", calls_a);
    printf("Order slow_verify || (senior&&eligible): calls=%ld\n", calls_b);
    printf("Using & instead of && for first pair: calls=%ld\n", calls_c);

    {
        int x = 0;
        int y = 0;
        int r;
        verify_calls = 0;
        r = (x != 0) && slow_verify(1);
        printf("x!=0 is false, && second evaluated? calls=%d result=%d\n", verify_calls, r);
        verify_calls = 0;
        r = (x == 0) || slow_verify(2);
        printf("x==0 is true, || second evaluated? calls=%d result=%d\n", verify_calls, r);
        verify_calls = 0;
        r = (x != 0) & (slow_verify(3) != 0);
        printf("using & with false left, second evaluated? calls=%d result=%d\n", verify_calls, r);
        if (y) { }
    }

    printf("Changed condition (audit log): slow_verify must always run, so place it where it is always evaluated or call explicitly.\n");
    verify_calls = 0;
    for (i = 0; i < n; i++) {
        int verified = slow_verify(i);
        granted = (senior[i] && eligible[i]) || verified;
        if (granted) { }
    }
    printf("Audit-required explicit call: calls=%d\n", verify_calls);

    return 0;
}
