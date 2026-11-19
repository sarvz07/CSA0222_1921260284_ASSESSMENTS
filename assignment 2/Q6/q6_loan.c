#include <stdio.h>

int main(void)
{
    double principal = 10000.0;
    double rate = 0.02;
    double payment = 2000.0;
    double balance = principal;
    double total_interest = 0.0;
    int month = 0;
    int max_months = 360;

    printf("Month  Payment   Interest   PrincipalPaid  Balance\n");
    while (balance > 0.000001 && month < max_months) {
        double interest = balance * rate;
        double principal_paid;
        double pay;
        month++;
        if (payment <= interest) {
            printf("Repayment %.2f is not greater than monthly interest %.2f at month %d. Loan will not terminate.\n",
                   payment, interest, month);
            return 0;
        }
        if (balance + interest <= payment) {
            pay = balance + interest;
            principal_paid = balance;
            balance = 0.0;
        } else {
            pay = payment;
            principal_paid = payment - interest;
            balance -= principal_paid;
        }
        total_interest += interest;
        printf("%5d  %8.2f  %8.2f   %13.2f  %8.2f\n",
               month, pay, interest, principal_paid, balance);
    }

    if (balance > 0.000001) {
        printf("Safety limit reached after %d months. Remaining balance: %.2f\n", month, balance);
    } else {
        printf("Months: %d\n", month);
        printf("Total interest: %.2f\n", total_interest);
    }
    return 0;
}
