#include <stdio.h>

int main(void)
{
    unsigned int house = 0;
    int previous = 0;
    int current = 0;
    char category = '\0';
    int count;
    long paise = 16250L;
    long big = 4294967297L;

    printf("Enter house category previous current: ");
    count = scanf("%u %c %d %d", &house, &category, &previous, &current);
    if (count != 4) {
        printf("Invalid input: scanf returned %d\n", count);
        return 1;
    }
    if (previous < 0 || previous > 65535 || current < 0 || current > 65535) {
        printf("Reading out of range 0..65535\n");
        return 1;
    }

    printf("house=%u, category=%c, previous=%d, current=%d\n",
           house, category, previous, current);
    printf("formatted bill: Rs %ld.%02ld\n", paise / 100, paise % 100);
    printf("correct %%ld value          : %ld\n", big);
    printf("same bytes read as int (%%d): %d\n", *(int *)(void *)&big);
    printf("sizes: int=%zu, long=%zu, long long=%zu, double=%zu\n",
           sizeof(int), sizeof(long), sizeof(long long), sizeof(double));
    return 0;
}
