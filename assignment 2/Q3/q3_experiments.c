#include <stdio.h>

enum { LIGHT = 0, FAN = 1, AC = 2, HEATER = 3, PUMP = 4 };

unsigned char switch_on(unsigned char reg, int dev)
{
    return (unsigned char)(reg | (unsigned char)(1u << dev));
}

unsigned char switch_off(unsigned char reg, int dev)
{
    return (unsigned char)(reg & (unsigned char)~(1u << dev));
}

unsigned char toggle(unsigned char reg, int dev)
{
    return (unsigned char)(reg ^ (unsigned char)(1u << dev));
}

int is_on(unsigned char reg, int dev)
{
    return (reg & (unsigned char)(1u << dev)) != 0;
}

void show(unsigned char reg)
{
    for (int bit = 7; bit >= 0; bit--) putchar(((reg >> bit) & 1u) ? '1' : '0');
    putchar('\n');
}

int count_on(unsigned char reg)
{
    int count = 0;
    for (int bit = 0; bit < 8; bit++) {
        if (reg & (unsigned char)(1u << bit)) count++;
    }
    return count;
}

int main(void)
{
    unsigned char reg = 0;
    printf("initial: ");
    show(reg);
    reg = switch_on(reg, LIGHT);
    printf("after switch_on(LIGHT): ");
    show(reg);
    reg = switch_on(reg, AC);
    printf("after switch_on(AC):    ");
    show(reg);
    reg = toggle(reg, FAN);
    printf("after toggle(FAN):      ");
    show(reg);
    reg = switch_off(reg, LIGHT);
    printf("after switch_off(LIGHT):");
    show(reg);
    printf("AC is %s, PUMP is %s\n", is_on(reg, AC) ? "ON" : "OFF",
           is_on(reg, PUMP) ? "ON" : "OFF");
    printf("devices ON: %d\n", count_on(reg));

    {
        unsigned char r = 5;
        int dev = 1;
        unsigned int a = r & 1u << dev;
        unsigned int b = (r & 1u) << dev;
        printf("precedence demo r=5 dev=1: r&1<<dev=%u (r&1)<<dev=%u\n", a, b);
    }
    {
        signed char s = (signed char)0x80;
        unsigned char shifted = (unsigned char)(s >> 1);
        printf("signed char 0x80 shifted right as implementation-defined/extended sign: 0x%02X\n", (unsigned)shifted);
    }
    return 0;
}
