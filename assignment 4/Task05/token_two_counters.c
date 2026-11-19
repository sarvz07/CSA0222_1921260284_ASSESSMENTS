#include <stdio.h>

int auto_token(int *counter)
{
    return ++*counter;
}

int static_veg_token(void)
{
    static int token = 1000;
    return ++token;
}

int static_nonveg_token(void)
{
    static int token = 2000;
    return ++token;
}

int main(void)
{
    int veg = 1000;
    int nonveg = 2000;
    int av1 = auto_token(&veg);
    int av2 = auto_token(&veg);
    int an1 = auto_token(&nonveg);
    int an2 = auto_token(&nonveg);
    int sv1 = static_veg_token();
    int sv2 = static_veg_token();
    int sn1 = static_nonveg_token();
    int sn2 = static_nonveg_token();

    printf("auto pointer veg: %d %d\n", av1, av2);
    printf("auto pointer nonveg: %d %d\n", an1, an2);
    printf("static local veg: %d %d\n", sv1, sv2);
    printf("static local nonveg: %d %d\n", sn1, sn2);
    return 0;
}
