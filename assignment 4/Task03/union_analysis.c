#include <stdio.h>

struct all_fields {
    float celsius;
    int percent;
    char status[8];
};

union sensor_value {
    float celsius;
    int percent;
    char status[8];
};

struct tagged_reading {
    int tag;
    union sensor_value value;
};

int main(void)
{
    struct all_fields all = {31.5f, 68, "OK"};
    union sensor_value value;

    value.celsius = 31.5f;
    printf("sizeof struct with all fields = %zu\n", sizeof all);
    printf("sizeof union = %zu\n", sizeof value);
    printf("sizeof tagged reading = %zu\n", sizeof(struct tagged_reading));
    printf("all-field packet: %.1f C, %d %%, %s\n",
           all.celsius, all.percent, all.status);
    printf("stored 31.5f as celsius, read as percent = %d (0x%08X)\n",
           value.percent, (unsigned int)value.percent);
    return 0;
}
