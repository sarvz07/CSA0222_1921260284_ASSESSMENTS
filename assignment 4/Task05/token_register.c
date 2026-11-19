#include <stdio.h>

static int token = 1000;

int next_token(void)
{
    return ++token;
}

int main(void)
{
    int values[3];

    for (register int i = 0; i < 3; i++) {
        values[i] = next_token();
    }
    for (register int i = 0; i < 3; i++) {
        printf("%d ", values[i]);
    }
    printf("\n");
    printf("single printf: %d %d %d\n", next_token(), next_token(), next_token());
    printf("final token = %d\n", token);
    return 0;
}
