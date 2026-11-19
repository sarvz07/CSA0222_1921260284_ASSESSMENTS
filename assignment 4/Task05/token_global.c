#include "token_global.h"

int veg_counter = 1000;
int nonveg_counter = 2000;

int next_veg_token(void)
{
    return ++veg_counter;
}

int next_nonveg_token(void)
{
    return ++nonveg_counter;
}
