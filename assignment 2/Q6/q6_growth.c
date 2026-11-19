#include <stdio.h>

int simulate(double principal, double rate, double payment)
{
    double balance = principal;
    int month = 0;
    int max_months = 100000;
    while (balance > 0.000001 && month < max_months) {
        double interest = balance * rate;
        double principal_paid;
        month++;
        if (payment <= interest) {
            return -1;
        }
        if (balance + interest <= payment) {
            balance = 0.0;
        } else {
            principal_paid = payment - interest;
            balance -= principal_paid;
        }
    }
    if (balance > 0.000001) return -2;
    return month;
}

int main(void)
{
    double principal = 10000.0;
    double rate = 0.02;
    double payments[] = {2500.0, 2000.0, 1500.0, 1000.0, 500.0, 300.0, 250.0, 210.0, 205.0, 201.0, 200.0};
    int n = 11;
    for (int i = 0; i < n; i++) {
        int months = simulate(principal, rate, payments[i]);
        if (months == -1) {
            printf("payment %.2f: non-terminating (payment <= monthly interest)\n", payments[i]);
        } else if (months == -2) {
            printf("payment %.2f: exceeded safety iterations\n", payments[i]);
        } else {
            printf("payment %.2f: months=%d\n", payments[i], months);
        }
    }
    return 0;
}
