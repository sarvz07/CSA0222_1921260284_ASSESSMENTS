#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    char line[128];
    char category = '\0';
    char extra[32];
    unsigned int house = 0;
    long previous = 0;
    long current = 0;
    int converted;

    printf("Enter house category previous current: ");
    if (fgets(line, sizeof line, stdin) == NULL) {
        printf("No input\n");
        return 1;
    }
    converted = sscanf(line, " %u %c %ld %ld %s",
                       &house, &category, &previous, &current, extra);
    if (converted != 4) {
        printf("Invalid input: sscanf returned %d\n", converted);
        return 1;
    }
    if (previous < 0 || previous > 65535 || current < 0 || current > 65535) {
        printf("Reading out of range 0..65535\n");
        return 1;
    }
    printf("fgets+sscanf: house=%u, category=%c, previous=%ld, current=%ld\n",
           house, category, previous, current);
    printf("line length = %zu bytes, buffer = %zu bytes\n", strlen(line), sizeof line);
    return 0;
}
