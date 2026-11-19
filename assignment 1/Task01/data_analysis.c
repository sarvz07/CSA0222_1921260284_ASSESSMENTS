#include <stdio.h>
#include <string.h>
#include <limits.h>

#define METER_MAX 65535u

typedef struct {
    unsigned int household_no;
    unsigned short previous;
    unsigned short current;
    char category;
    unsigned int units;
    long fixed_paise;
    long energy_paise;
    long total_paise;
} Bill;

static unsigned int units_from_ints(unsigned short previous, unsigned short current)
{
    return (unsigned short)(current - previous);
}

static unsigned int units_from_text(const char *previous, const char *current)
{
    unsigned int p = 0;
    unsigned int c = 0;
    const char *s;

    for (s = previous; *s != '\0'; s++) {
        p = p * 10u + (unsigned int)(*s - '0');
    }
    for (s = current; *s != '\0'; s++) {
        c = c * 10u + (unsigned int)(*s - '0');
    }
    return c >= p ? c - p : (METER_MAX + 1u - p) + c;
}

int main(void)
{
    Bill b;
    unsigned int units_text;

    b.household_no = 1042u;
    b.previous = 1540u;
    b.current = 1870u;
    b.category = 'R';
    b.units = units_from_ints(b.previous, b.current);
    b.fixed_paise = 5000L;
    b.energy_paise = (long)b.units * 225L;
    b.total_paise = b.fixed_paise + b.energy_paise;

    printf("household %u, category %c\n", b.household_no, b.category);
    printf("%u -> %u = %u units\n", (unsigned)b.previous, (unsigned)b.current, b.units);
    printf("fixed %ld paise, energy %ld paise, total %ld paise\n",
           b.fixed_paise, b.energy_paise, b.total_paise);
    printf("sizeof(Bill) = %zu bytes, sizeof(unsigned short) = %zu bytes\n",
           sizeof b, sizeof b.previous);

    units_text = units_from_text("01540", "01870");
    printf("text readings \"01540\" and \"01870\" -> %u units\n", units_text);
    printf("digits scanned as text: %zu + %zu; bytes as integers: %zu + %zu\n",
           strlen("01540"), strlen("01870"), sizeof b.previous, sizeof b.current);
    printf("bytes for one five-digit text reading: %zu\n", sizeof "01540");
    return 0;
}
