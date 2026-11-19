#include <stdio.h>

int main(void)
{
    struct Bill {
        double price;
        int qty;
        int senior;
    };
    struct Bill bills[] = {
        {100.0, 10, 1},
        {250.0, 3, 0},
        {150.0, 8, 1}
    };
    int n = 3;

    printf("Original (buggy) calculation:\n");
    for (int i = 0; i < n; i++) {
        double price = bills[i].price;
        int qty = bills[i].qty;
        int senior = bills[i].senior;
        double total = price * qty - price * qty * 15 / 100 * senior + price * qty * 5 / 100;
        double net = total > 1000 ? total - 50 : total;
        printf("Bill %d: price=%.2f qty=%d senior=%d -> total=%.2f net=%.2f\n",
               i + 1, price, qty, senior, total, net);
    }

    printf("\nCorrected calculation:\n");
    for (int i = 0; i < n; i++) {
        double price = bills[i].price;
        int qty = bills[i].qty;
        int senior = bills[i].senior;
        double gross = price * (double)qty;
        double discount = senior ? gross * 0.15 : 0.0;
        double gst = gross * 0.05;
        double total = gross - discount + gst;
        double net = total > 1000.0 ? total - 50.0 : total;
        printf("Bill %d: price=%.2f qty=%d senior=%d -> gross=%.2f discount=%.2f gst=%.2f total=%.2f net=%.2f\n",
               i + 1, price, qty, senior, gross, discount, gst, total, net);
    }

    return 0;
}
